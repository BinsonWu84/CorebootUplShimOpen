/** @file

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ShimLayer.h"

STATIC UINT32  mTopOfLowerUsableDram = 0;

GUID gGraphicsInfoHobGuid                   = { 0x39f62cce, 0x6825, 0x4669, { 0xbb, 0x56, 0x54, 0x1a, 0xba, 0x75, 0x3a, 0x07 }};
GUID gGraphicsDeviceInfoHobGuid             = { 0xe5cb2ac9, 0xd35d, 0x4430, { 0x93, 0x6e, 0x1d, 0xe3, 0x32, 0x47, 0x8d, 0xe7 }};
GUID gUniversalPayloadSmbiosTableGuid       = { 0x590a0d26, 0x06e5, 0x4d20, { 0x8a, 0x82, 0x59, 0xea, 0x1b, 0x34, 0x98, 0x2d }};
GUID gUniversalPayloadAcpiTableGuid         = { 0x9f9a9506, 0x5597, 0x4515, { 0xba, 0xb6, 0x8b, 0xcd, 0xe7, 0x84, 0xba, 0x87 }};
GUID gUniversalPayloadExtraDataGuid         = { 0x15a5baf6, 0x1c91, 0x467d, { 0x9d, 0xfb, 0x31, 0x9d, 0x17, 0x8d, 0x4b, 0xb4 }};
GUID gUniversalPayloadSerialPortInfoGuid    = { 0xaa7e190d, 0xbe21, 0x4409, { 0x8e, 0x67, 0xa2, 0xcd, 0x0f, 0x61, 0xe1, 0x70 }};

/**
  Allocates one or more pages of type BootServicesData.

  Allocates the number of pages of MemoryType and returns a pointer to the
  allocated buffer.  The buffer returned is aligned on a 4KB boundary.
  If Pages is 0, then NULL is returned.
  If there is not enough memory availble to satisfy the request, then NULL
  is returned.

  @param   Pages                 The number of 4 KB pages to allocate.
  @return  A pointer to the allocated buffer or NULL if allocation fails.
**/
VOID *
AllocatePages (
  IN UINTN  Pages
  )
{
  HOB_POINTERS            Hob;
  ADDRESS                 Offset;
  HOB_HANDOFF_INFO_TABLE  *HobTable;

  Hob.Raw  = GetHobList ();
  HobTable = Hob.HandoffInformationTable;

  if (Pages == 0) {
    return NULL;
  }

  // Make sure allocation address is page alligned.
  Offset = HobTable->FreeMemoryTop & PAGE_MASK;
  if (Offset != 0) {
    HobTable->FreeMemoryTop -= Offset;
  }

  //
  // Check available memory for the allocation
  //
  if (HobTable->FreeMemoryTop - ((Pages * PAGE_SIZE) + sizeof (HOB_MEMORY_ALLOCATION)) < HobTable->FreeMemoryBottom) {
    return NULL;
  }

  HobTable->FreeMemoryTop -= Pages * PAGE_SIZE;
  BuildMemoryAllocationHob (HobTable->FreeMemoryTop, Pages * PAGE_SIZE, BootServicesData);

  return (VOID *)(UINTN)HobTable->FreeMemoryTop;
}

/**
  Acquire the coreboot memory table with the given table id

  @param  TableId            Table id to be searched
  @param  MemTable           Pointer to the base address of the memory table
  @param  MemTableSize       Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
ParseCbmemInfo (
  IN  UINT32  TableId,
  OUT VOID    **MemTable,
  OUT UINT32  *MemTableSize
  )
{
  RETURN_STATUS           Status;
  CB_MEMORY               *Rec;
  struct cb_memory_range  *Range;
  UINT64                  Start;
  UINT64                  Size;
  UINTN                   Index;
  struct cbmem_root       *CbMemLgRoot;
  VOID                    *CbMemSmRoot;
  VOID                    *CbMemSmRootTable;
  UINT32                   SmRootTableSize;
  struct imd_root_pointer *SmRootPointer;

  if (MemTable == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  *MemTable = NULL;
  Status    = RETURN_NOT_FOUND;

  //
  // Get the coreboot memory table
  //
  Rec = (CB_MEMORY *)FindCbTag (CB_TAG_MEMORY);
  if (Rec == NULL) {
    return Status;
  }

  for (Index = 0; Index < MEM_RANGE_COUNT (Rec); Index++) {
    Range = MEM_RANGE_PTR (Rec, Index);
    Start = cb_unpack64 (Range->start);
    Size  = cb_unpack64 (Range->size);

    if ((Range->type == CB_MEM_TABLE) && (Start > 0x1000)) {
      CbMemLgRoot = (struct  cbmem_root *)(UINTN)(Start + Size - DYN_CBMEM_ALIGN_SIZE);
      Status      = FindCbMemTable (CbMemLgRoot, TableId, MemTable, MemTableSize);
      if (!ERROR (Status)) {
        break;
      } else {
        /* Try to locate small root table and find the target CBMEM entry in small root table */
        Status        = FindCbMemTable (CbMemLgRoot, CBMEM_ID_IMD_SMALL, &CbMemSmRootTable, &SmRootTableSize);
        SmRootPointer = (struct imd_root_pointer *)(UINTN)((UINTN) CbMemSmRootTable + SmRootTableSize - sizeof (struct imd_root_pointer));
        CbMemSmRoot   = (struct cbmem_root *)(UINTN)(SmRootPointer->root_offset + (UINTN) SmRootPointer);
        if (!ERROR (Status)) {
          Status = FindCbMemTable ((struct cbmem_root *) CbMemSmRoot, TableId, MemTable, MemTableSize);
          if (!ERROR (Status)) {
            break;
          }
        }
      }
    }
  }

  return Status;
}

/**
   Callback function to find TOLUD (Top of Lower Usable DRAM)

   Estimate where TOLUD (Top of Lower Usable DRAM) resides. The exact position
   would require platform specific code.

   @param MemoryMapEntry         Memory map entry info got from bootloader.
   @param Params                 Not used for now.

  @retval SUCCESS            Successfully updated mTopOfLowerUsableDram.
**/
RETURN_STATUS
FindToludCallback (
  IN MEMORY_MAP_ENTRY  *MemoryMapEntry,
  IN VOID              *Params
  )
{
  //
  // This code assumes that the memory map on this x86 machine below 4GiB is continous
  // until TOLUD. In addition it assumes that the bootloader provided memory tables have
  // no "holes" and thus the first memory range not covered by e820 marks the end of
  // usable DRAM. In addition it's assumed that every reserved memory region touching
  // usable RAM is also covering DRAM, everything else that is marked reserved thus must be
  // MMIO not detectable by bootloader/OS
  //

  //
  // Skip memory types not RAM or reserved
  //
  if ((MemoryMapEntry->Type == E820_UNUSABLE) || (MemoryMapEntry->Type == E820_DISABLED) ||
      (MemoryMapEntry->Type == E820_PMEM))
  {
    return SUCCESS;
  }

  //
  // Skip resources above 4GiB
  //
  if ((MemoryMapEntry->Base + MemoryMapEntry->Size) > 0x100000000ULL) {
    return SUCCESS;
  }

  if ((MemoryMapEntry->Type == E820_RAM) || (MemoryMapEntry->Type == E820_ACPI) ||
      (MemoryMapEntry->Type == E820_NVS))
  {
    //
    // It's usable DRAM. Update TOLUD.
    //
    if (mTopOfLowerUsableDram < (MemoryMapEntry->Base + MemoryMapEntry->Size)) {
      mTopOfLowerUsableDram = (UINT32)(MemoryMapEntry->Base + MemoryMapEntry->Size);
    }
  } else {
    //
    // It might be 'reserved DRAM' or 'MMIO'.
    //
    // If it touches usable DRAM at Base assume it's DRAM as well,
    // as it could be bootloader installed tables, TSEG, GTT, ...
    //
    if (mTopOfLowerUsableDram == MemoryMapEntry->Base) {
      mTopOfLowerUsableDram = (UINT32)(MemoryMapEntry->Base + MemoryMapEntry->Size);
    }
  }

  return SUCCESS;
}

/**
   Callback function to build resource descriptor HOB

   This function build a HOB based on the memory map entry info.
   Only add RESOURCE_SYSTEM_MEMORY.

   @param MemoryMapEntry         Memory map entry info got from bootloader.
   @param Params                 Not used for now.

  @retval RETURN_SUCCESS        Successfully build a HOB.
**/
RETURN_STATUS
MemInfoCallback (
  IN MEMORY_MAP_ENTRY  *MemoryMapEntry,
  IN VOID              *Params
  )
{
  ADDRESS                  Base;
  RESOURCE_TYPE            Type;
  UINT64                   Size;
  RESOURCE_ATTRIBUTE_TYPE  Attribue;

  //
  // Skip everything not known to be usable DRAM.
  // It will be added later.
  //
  if ((MemoryMapEntry->Type != E820_RAM) && (MemoryMapEntry->Type != E820_ACPI) &&
      (MemoryMapEntry->Type != E820_NVS))
  {
    return RETURN_SUCCESS;
  }

  Type = RESOURCE_SYSTEM_MEMORY;
  Base = MemoryMapEntry->Base;
  Size = MemoryMapEntry->Size;

  Attribue = RESOURCE_ATTRIBUTE_PRESENT |
             RESOURCE_ATTRIBUTE_INITIALIZED |
             RESOURCE_ATTRIBUTE_TESTED |
             RESOURCE_ATTRIBUTE_UNCACHEABLE |
             RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
             RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;

  BuildResourceDescriptorHob (Type, Attribue, (ADDRESS)Base, Size);

  if (MemoryMapEntry->Type == E820_ACPI) {
    BuildMemoryAllocationHob (Base, Size, ACPIReclaimMemory);
  } else if (MemoryMapEntry->Type == E820_NVS) {
    BuildMemoryAllocationHob (Base, Size, ACPIMemoryNVS);
  }

  return RETURN_SUCCESS;
}

/**
   Callback function to build resource descriptor HOB

   This function build a HOB based on the memory map entry info.
   It creates only RESOURCE_MEMORY_MAPPED_IO and RESOURCE_MEMORY_RESERVED
   resources.

   @param MemoryMapEntry         Memory map entry info got from bootloader.
   @param Params                 A pointer to ACPI_BOARD_INFO.

  @retval SUCCESS            Successfully build a HOB.
  @retval INVALID_PARAMETER  Invalid parameter provided.
**/
RETURN_STATUS
MemInfoCallbackMmio (
  IN MEMORY_MAP_ENTRY  *MemoryMapEntry,
  IN VOID              *Params
  )
{
  ADDRESS                  Base;
  RESOURCE_TYPE            Type;
  UINT64                   Size;
  RESOURCE_ATTRIBUTE_TYPE  Attribue;

  //
  // Skip types already handled in MemInfoCallback
  //
  if ((MemoryMapEntry->Type == E820_RAM) || (MemoryMapEntry->Type == E820_ACPI)) {
    return SUCCESS;
  }

  if (MemoryMapEntry->Base < mTopOfLowerUsableDram) {
    //
    // It's in DRAM and thus must be reserved
    //
    Type = RESOURCE_MEMORY_RESERVED;
  } else if ((MemoryMapEntry->Base < 0x100000000ULL) && (MemoryMapEntry->Base >= mTopOfLowerUsableDram)) {
    //
    // It's not in DRAM, must be MMIO
    //
    Type = RESOURCE_MEMORY_MAPPED_IO;
  } else {
    Type = RESOURCE_MEMORY_RESERVED;
  }

  Base = MemoryMapEntry->Base;
  Size = MemoryMapEntry->Size;

  Attribue = RESOURCE_ATTRIBUTE_PRESENT |
             RESOURCE_ATTRIBUTE_INITIALIZED |
             RESOURCE_ATTRIBUTE_TESTED |
             RESOURCE_ATTRIBUTE_UNCACHEABLE |
             RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
             RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;

  BuildResourceDescriptorHob (Type, Attribue, (ADDRESS)Base, Size);

  if ((MemoryMapEntry->Type == E820_UNUSABLE) ||
      (MemoryMapEntry->Type == E820_DISABLED))
  {
    BuildMemoryAllocationHob (Base, Size, UnusableMemory);
  } else if (MemoryMapEntry->Type == E820_PMEM) {
    BuildMemoryAllocationHob (Base, Size, PersistentMemory);
  }

  return SUCCESS;
}

/**
  It will build HOBs based on information from bootloaders.

  @retval SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required HOBs.
**/
RETURN_STATUS
BuildHobFromBl (
  VOID
  )
{
  RETURN_STATUS                       Status;
  PEI_GRAPHICS_INFO_HOB               GfxInfo;
  PEI_GRAPHICS_INFO_HOB               *NewGfxInfo;
  PEI_GRAPHICS_DEVICE_INFO_HOB        GfxDeviceInfo;
  PEI_GRAPHICS_DEVICE_INFO_HOB        *NewGfxDeviceInfo;
  UNIVERSAL_PAYLOAD_SMBIOS_TABLE      *SmBiosTableHob;
  UNIVERSAL_PAYLOAD_ACPI_TABLE        *AcpiTableHob;

  //
  // First find TOLUD
  //
  Status = ParseMemoryInfo (FindToludCallback, NULL);
  if (ERROR (Status)) {
    return Status;
  }

  //
  // Parse memory info and build memory HOBs for Usable RAM
  //
  Status = ParseMemoryInfo (MemInfoCallback, NULL);
  if (ERROR (Status)) {
    return Status;
  }

  //
  // Create guid hob for frame buffer information
  //
  Status = ParseGfxInfo (&GfxInfo);
  if (!ERROR (Status)) {
    NewGfxInfo = BuildGuidHob (&gGraphicsInfoHobGuid, sizeof (GfxInfo));
    CopyMem (NewGfxInfo, &GfxInfo, sizeof (GfxInfo));
  }

  Status = ParseGfxDeviceInfo (&GfxDeviceInfo);
  if (!ERROR (Status)) {
    NewGfxDeviceInfo = BuildGuidHob (&gGraphicsDeviceInfoHobGuid, sizeof (GfxDeviceInfo));
    CopyMem (NewGfxDeviceInfo, &GfxDeviceInfo, sizeof (GfxDeviceInfo));
  }

  //
  // Creat SmBios table Hob
  //
  SmBiosTableHob = BuildGuidHob (&gUniversalPayloadSmbiosTableGuid, sizeof (UNIVERSAL_PAYLOAD_SMBIOS_TABLE));
  SmBiosTableHob->Header.Revision = UNIVERSAL_PAYLOAD_SMBIOS_TABLE_REVISION;
  SmBiosTableHob->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_SMBIOS_TABLE);
  Status = ParseSmbiosTable (SmBiosTableHob);

  //
  // Creat ACPI table Hob
  //
  AcpiTableHob = BuildGuidHob (&gUniversalPayloadAcpiTableGuid, sizeof (UNIVERSAL_PAYLOAD_ACPI_TABLE));
  AcpiTableHob->Header.Revision = UNIVERSAL_PAYLOAD_ACPI_TABLE_REVISION;
  AcpiTableHob->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_ACPI_TABLE);
  Status = ParseAcpiTableInfo (AcpiTableHob);

  //
  // Parse memory info and build memory HOBs for reserved DRAM and MMIO
  //
  Status = ParseMemoryInfo (MemInfoCallbackMmio, NULL);
  if (ERROR (Status)) {
    return Status;
  }

  return SUCCESS;
}

/**
  This function will build some generic HOBs that doesn't depend on information from bootloaders.

**/
VOID
BuildGenericHob (
  VOID
  )
{
  UINT32                   RegEax;
  UINT8                    PhysicalAddressBits;
  RESOURCE_ATTRIBUTE_TYPE  ResourceAttribute;

  //Memory allocaion hob for the Shim Layer
  // BuildMemoryAllocationHob (PcdGet32 (PcdPayloadFdMemBase), PcdGet32 (PcdPayloadFdMemSize), BootServicesData);
  BuildMemoryAllocationHob (MEMBASE, MEMSIZE, BootServicesData);

  //
  // Build CPU memory space and IO space hob
  //
  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8)RegEax;
  } else {
    PhysicalAddressBits = 36;
  }

  ShBuildCpuHob (PhysicalAddressBits, 16);

  //
  // Report Local APIC range, cause sbl HOB to be NULL, comment now
  //
  ResourceAttribute = (
                       RESOURCE_ATTRIBUTE_PRESENT |
                       RESOURCE_ATTRIBUTE_INITIALIZED |
                       RESOURCE_ATTRIBUTE_UNCACHEABLE |
                       RESOURCE_ATTRIBUTE_TESTED
                       );
  BuildResourceDescriptorHob (RESOURCE_MEMORY_MAPPED_IO, ResourceAttribute, 0xFEC80000, SIZE_512KB);
  BuildMemoryAllocationHob (0xFEC80000, SIZE_512KB, MemoryMappedIO);
}

RETURN_STATUS
ConvertCbmemToHob (
  VOID
  )
{
  UINTN                               MemBase;
  UINTN                               HobMemBase;
  UINTN                               HobMemTop;
  RETURN_STATUS                       Status;
  SERIAL_PORT_INFO                    SerialPortInfo;
  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *UniversalSerialPort;

  MemBase    = MEMBASE;
  HobMemBase = ALIGN_VALUE (MemBase + MEMSIZE, SIZE_1MB);
  HobMemTop  = HobMemBase + UEFI_REGION_SIZE;
  HobConstructor ((VOID *)MemBase, (VOID *)HobMemTop, (VOID *)HobMemBase, (VOID *)HobMemTop);

  Status = ParseSerialInfo (&SerialPortInfo);
  if (!ERROR (Status)) {
    UniversalSerialPort = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));
    UniversalSerialPort->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
    UniversalSerialPort->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);
    UniversalSerialPort->UseMmio         = (SerialPortInfo.Type == 1) ? FALSE : TRUE;
    UniversalSerialPort->RegisterBase    = SerialPortInfo.BaseAddr;
    UniversalSerialPort->BaudRate        = SerialPortInfo.Baud;
    UniversalSerialPort->RegisterStride  = (UINT8)SerialPortInfo.RegWidth;
  }

  // ProcessLibraryConstructorList ();
  Status = BuildHobFromBl ();
  if (ERROR (Status)) {
    return Status;
  }

  BuildGenericHob ();
  return SUCCESS;
}

RETURN_STATUS
LocateAndDecompressPayload (
  OUT VOID **Dest
  )
{
  RETURN_STATUS               Status;
  ADDRESS                     SourceAddress;
  UINT64                      ImageSize;
  ADDRESS                     CBFSAddress;
  VOID                        *FMapEntry;
  UINT32                      FMapEntrySize;
  struct fmap_area            *FMapArea;
  struct cbfs_payload_segment *FirstSegment;
  UINT32                      Index;
  UINTN                       DestSize, ScratchSize;
  VOID                        *MyDestAddress, *ScratchAddress;
  UINT32                      Alignment;
  UINTN                       CBFSEntrySize;
  union cbfs_mdata            *CBFSEntry;
  UINT64                      CBFSEntryAddrEnd;

  SourceAddress     = 0;
  CBFSAddress       = 0;
  CBFSEntryAddrEnd  = 0;

  Status = ParseCbmemInfo (CBMEM_ID_FMAP, &FMapEntry, &FMapEntrySize);
  if (ERROR (Status)) {
    return NOT_FOUND;
  }
  /*Locate fmap from CBMEM*/
  FMapArea = (struct fmap_area *)((UINTN)FMapEntry + sizeof (struct fmap));
  for (Index = 0; Index < ((struct fmap *)FMapEntry)->nareas; Index++) {
    if (AsciiStrCmp ((const CHAR8 *)FMapArea->name, "COREBOOT") == 0){
      CBFSAddress   = ((struct fmap *)FMapEntry)->base + FMapArea->offset;
      CBFSEntrySize = (UINTN)FMapArea->size;
      break;
    }
    FMapArea = (struct fmap_area *)((UINTN)FMapArea + sizeof (struct fmap_area));
  }
  if (!CBFSAddress) {
    return NOT_FOUND;
  }

  //
  // Parse payload address from CBFS.
  //
  CBFSEntry         = (union cbfs_mdata *)CBFSAddress;
  CBFSEntryAddrEnd  = (UINT64)(CBFSAddress + CBFSEntrySize);

  while ((UINT64)CBFSEntry < CBFSEntryAddrEnd) {
    if (AsciiStrCmp (CBFSEntry->h.filename, CBFS_UNIVERSAL_PAYLOAD) == 0) {
      FirstSegment  = &((struct cbfs_payload *)(UINTN)((UINT64)CBFSEntry + SWAP32 (CBFSEntry->h.offset)))->segments;
      SourceAddress = (UINTN)FirstSegment + SWAP32 (FirstSegment->offset);
      ImageSize     = SWAP32 (FirstSegment->len);
      Alignment     = (FirstSegment->load_addr)>>32;
      Alignment     = SWAP32 (Alignment);
      break;
    }
    CBFSEntry = (union cbfs_mdata *)((UINTN)CBFSEntry + ALIGN_UP (SWAP32 (CBFSEntry->h.offset) + SWAP32 (CBFSEntry->h.len), CBFS_ALIGNMENT));
  }

  if (!SourceAddress) {
    return NOT_FOUND;
  }

  Status = LzmaUefiDecompressGetInfo((VOID *)(UINTN)SourceAddress, ImageSize, &DestSize, &ScratchSize);
  if (ERROR (Status)) {
    return Status;
  }
  DestSize += Alignment;
  MyDestAddress  = AllocatePages(SIZE_TO_PAGES(DestSize));
  MyDestAddress  = (VOID *) ALIGN_VALUE ((UINTN) MyDestAddress, Alignment);
  ScratchAddress = AllocatePages(SIZE_TO_PAGES(ScratchSize));

  Status = LzmaUefiDecompress ((VOID *)(UINTN)SourceAddress, ImageSize, MyDestAddress, ScratchAddress);
  *Dest = MyDestAddress;
  if (ERROR (Status)) {
    return Status;
  }
  return SUCCESS;
}

RETURN_STATUS
LoadPayload (
  OUT    ADDRESS        *ImageAddressArg   OPTIONAL,
  OUT    UINT64         *ImageSizeArg,
  OUT    ADDRESS        *UniversalPayloadEntry
  )
{
  RETURN_STATUS                  Status;
  UINT32                         Index;
  UINT16                         ExtraDataIndex;
  CHAR8                          *SectionName;
  UINTN                          Offset;
  UINTN                          Size;
  UINTN                          Length;
  UINT32                         ExtraDataCount;
  ELF_IMAGE_CONTEXT              Context;
  UNIVERSAL_PAYLOAD_EXTRA_DATA   *ExtraData;
  VOID *Dest;

  Status = LocateAndDecompressPayload (&Dest);
  if (ERROR (Status)) {
    return Status;
  }
  Status = ParseElfImage (Dest, &Context);
  if (ERROR (Status)) {
    return Status;
  }

  //
  // Get UNIVERSAL_PAYLOAD_INFO_HEADER and number of additional PLD sections.
  //

  ExtraDataCount = 0;
  for (Index = 0; Index < Context.ShNum; Index++) {
    Status = GetElfSectionName (&Context, Index, &SectionName);
    if (ERROR (Status)) {
      continue;
    }

    if (AsciiStrnCmp (SectionName, UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX, UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX_LENGTH) == 0) {
      Status = GetElfSectionPos (&Context, Index, &Offset, &Size);
      if (!ERROR (Status)) {
        ExtraDataCount++;
      }
    }
  }

  //
  // Report the additional PLD sections through HOB.
  //
  Length    = sizeof (UNIVERSAL_PAYLOAD_EXTRA_DATA) + ExtraDataCount * sizeof (UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY);
  ExtraData = BuildGuidHob (
                &gUniversalPayloadExtraDataGuid,
                Length
                );
  ExtraData->Count           = ExtraDataCount;
  ExtraData->Header.Revision = UNIVERSAL_PAYLOAD_EXTRA_DATA_REVISION;
  ExtraData->Header.Length   = (UINT16)Length;
  if (ExtraDataCount != 0) {
    for (ExtraDataIndex = 0, Index = 0; Index < Context.ShNum; Index++) {
      Status = GetElfSectionName (&Context, Index, &SectionName);
      if (ERROR (Status)) {
        continue;
      }

      if (AsciiStrnCmp (SectionName, UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX, UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX_LENGTH) == 0) {
        Status = GetElfSectionPos (&Context, Index, &Offset, &Size);
        if (!ERROR (Status)) {
          AsciiStrCpyS (
            ExtraData->Entry[ExtraDataIndex].Identifier,
            sizeof (ExtraData->Entry[ExtraDataIndex].Identifier),
            SectionName + UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX_LENGTH
            );
          ExtraData->Entry[ExtraDataIndex].Base = (UINTN)(Context.FileBase + Offset);
          ExtraData->Entry[ExtraDataIndex].Size = Size;
          ExtraDataIndex++;
        }
      }
    }
  }
  if (Context.ReloadRequired || (Context.PreferredImageAddress != Context.FileBase)) {
    Context.ImageAddress = AllocatePages (SIZE_TO_PAGES (Context.ImageSize));
  } else {
    Context.ImageAddress = Context.FileBase;
  }
  //
  // Load ELF into the required base
  //
  Status = LoadElfImage (&Context);
  if (!ERROR (Status)) {
    *ImageAddressArg        = (UINTN)Context.ImageAddress;
    *UniversalPayloadEntry  = Context.EntryPoint;
    *ImageSizeArg           = Context.ImageSize;
  }
  return Status;
}

RETURN_STATUS
HandOffToPayload (
  IN  ADDRESS       UniversalPayloadEntry,
  IN  HOB_POINTERS  Hob
  )
{
  UINTN       HobList;

  HobList = (UINTN)(VOID *)Hob.Raw;
  typedef VOID ( *PayloadEntry) (UINTN);
  ((PayloadEntry) (UINTN) UniversalPayloadEntry) (HobList);

  return SUCCESS;
}

/**

  Entry point to the C language phase of Shim Layer before UEFI payload.

  @param[in]   BootloaderParameter    The starting address of bootloader parameter block.

  @retval      It will not return if SUCCESS, and return error when passing bootloader parameter.

**/
RETURN_STATUS
_ModuleEntryPoint (
  IN  UINTN  BootloaderParameter
  )
{
  RETURN_STATUS   Status;
  HOB_POINTERS    Hob;
  ADDRESS         ImageAddress;
  UINT64          ImageSize;
  ADDRESS         UniversalPayloadEntry;

  SetBootloaderParameter (BootloaderParameter);
  Status = ConvertCbmemToHob();
  if (ERROR (Status)) {
    return Status;
  }

  Status = LoadPayload (&ImageAddress, &ImageSize, &UniversalPayloadEntry);
  BuildMemoryAllocationHob (ImageAddress, ImageSize, BootServicesData);
  Hob.HandoffInformationTable = (HOB_HANDOFF_INFO_TABLE *)GetFirstHob (HOB_TYPE_HANDOFF);
  HandOffToPayload (UniversalPayloadEntry, Hob);

  return SUCCESS;
}
