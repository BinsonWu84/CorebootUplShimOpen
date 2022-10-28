/** @file

  Copyright (c) 2010, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "HobLib.h"

VOID  *mHobList;

/**
  Returns the pointer to the HOB list.

  This function returns the pointer to first HOB in the list.

  @return The pointer to the HOB list.

**/
VOID *
GetHobList (
  VOID
  )
{
  return mHobList;
}

/**
  Build a Handoff Information Table HOB

  This function initialize a HOB region from MemoryBegin to
  MemoryTop. And FreeMemoryBottom and FreeMemoryTop should
  be inside the HOB region.

  @param[in] MemoryBottom       Total memory start address
  @param[in] MemoryTop          Total memory end address.
  @param[in] FreeMemoryBottom   Free memory start address
  @param[in] FreeMemoryTop      Free memory end address.

  @return   The pointer to the handoff HOB table.

**/
HOB_HANDOFF_INFO_TABLE *
HobConstructor (
  IN VOID  *MemoryBottom,
  IN VOID  *MemoryTop,
  IN VOID  *FreeMemoryBottom,
  IN VOID  *FreeMemoryTop
  )
{
  HOB_HANDOFF_INFO_TABLE  *Hob;
  HOB_GENERIC_HEADER      *HobEnd;

  Hob    = FreeMemoryBottom;
  HobEnd = (HOB_GENERIC_HEADER *)(Hob+1);

  Hob->Header.HobType   = HOB_TYPE_HANDOFF;
  Hob->Header.HobLength = sizeof (HOB_HANDOFF_INFO_TABLE);
  Hob->Header.Reserved  = 0;

  HobEnd->HobType   = HOB_TYPE_END_OF_HOB_LIST;
  HobEnd->HobLength = sizeof (HOB_GENERIC_HEADER);
  HobEnd->Reserved  = 0;

  Hob->Version  = HOB_HANDOFF_TABLE_VERSION;
  Hob->BootMode = BOOT_WITH_FULL_CONFIGURATION;

  Hob->MemoryTop        = (ADDRESS)(UINTN)MemoryTop;
  Hob->MemoryBottom     = (ADDRESS)(UINTN)MemoryBottom;
  Hob->FreeMemoryTop    = (ADDRESS)(UINTN)FreeMemoryTop;
  Hob->FreeMemoryBottom = (ADDRESS)(UINTN)(HobEnd+1);
  Hob->EndOfHobList     = (ADDRESS)(UINTN)HobEnd;

  mHobList = Hob;
  return Hob;
}

/**
  Add a new HOB to the HOB List.

  @param HobType            Type of the new HOB.
  @param HobLength          Length of the new HOB to allocate.

  @return  NULL if there is no space to create a hob.
  @return  The address point to the new created hob.

**/
VOID *
CreateHob (
  IN  UINT16  HobType,
  IN  UINT16  HobLength
  )
{
  HOB_HANDOFF_INFO_TABLE  *HandOffHob;
  HOB_GENERIC_HEADER      *HobEnd;
  ADDRESS                 FreeMemory;
  VOID                    *Hob;

  HandOffHob = GetHobList ();

  HobLength = (UINT16)((HobLength + 0x7) & (~0x7));

  FreeMemory = HandOffHob->FreeMemoryTop - HandOffHob->FreeMemoryBottom;

  if (FreeMemory < HobLength) {
    return NULL;
  }

  Hob                                    = (VOID *)(UINTN)HandOffHob->EndOfHobList;
  ((HOB_GENERIC_HEADER *)Hob)->HobType   = HobType;
  ((HOB_GENERIC_HEADER *)Hob)->HobLength = HobLength;
  ((HOB_GENERIC_HEADER *)Hob)->Reserved  = 0;

  HobEnd                   = (HOB_GENERIC_HEADER *)((UINTN)Hob + HobLength);
  HandOffHob->EndOfHobList = (ADDRESS)(UINTN)HobEnd;

  HobEnd->HobType   = HOB_TYPE_END_OF_HOB_LIST;
  HobEnd->HobLength = sizeof (HOB_GENERIC_HEADER);
  HobEnd->Reserved  = 0;
  HobEnd++;
  HandOffHob->FreeMemoryBottom = (ADDRESS)(UINTN)HobEnd;

  return Hob;
}

/**
  Builds a HOB that describes a chunk of system memory.

  This function builds a HOB that describes a chunk of system memory.
  If there is no additional space for HOB creation, then ASSERT().

  @param  ResourceType        The type of resource described by this HOB.
  @param  ResourceAttribute   The resource attributes of the memory described by this HOB.
  @param  PhysicalStart       The 64 bit physical address of memory described by this HOB.
  @param  NumberOfBytes       The length of the memory described by this HOB in bytes.

**/
VOID
BuildResourceDescriptorHob (
  IN RESOURCE_TYPE            ResourceType,
  IN RESOURCE_ATTRIBUTE_TYPE  ResourceAttribute,
  IN ADDRESS         PhysicalStart,
  IN UINT64                       NumberOfBytes
  )
{
  HOB_RESOURCE_DESCRIPTOR  *Hob;

  Hob = CreateHob (HOB_TYPE_RESOURCE_DESCRIPTOR, sizeof (HOB_RESOURCE_DESCRIPTOR));

  Hob->ResourceType      = ResourceType;
  Hob->ResourceAttribute = ResourceAttribute;
  Hob->PhysicalStart     = PhysicalStart;
  Hob->ResourceLength    = NumberOfBytes;
}

/**
  Returns the next instance of a HOB type from the starting HOB.

  This function searches the first instance of a HOB type from the starting HOB pointer.
  If there does not exist such HOB type from the starting HOB pointer, it will return NULL.
  In contrast with macro GET_NEXT_HOB(), this function does not skip the starting HOB pointer
  unconditionally: it returns HobStart back if HobStart itself meets the requirement;
  caller is required to use GET_NEXT_HOB() if it wishes to skip current HobStart.
  If HobStart is NULL, then ASSERT().

  @param  Type          The HOB type to return.
  @param  HobStart      The starting HOB pointer to search from.

  @return The next instance of a HOB type from the starting HOB.

**/
VOID *
GetNextHob (
  IN UINT16      Type,
  IN const VOID  *HobStart
  )
{
  HOB_POINTERS  Hob;

  Hob.Raw = (UINT8 *)HobStart;
  //
  // Parse the HOB list until end of list or matching type is found.
  //
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == Type) {
      return Hob.Raw;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  return NULL;
}

/**
  Returns the first instance of a HOB type among the whole HOB list.

  This function searches the first instance of a HOB type among the whole HOB list.
  If there does not exist such HOB type in the HOB list, it will return NULL.

  @param  Type          The HOB type to return.

  @return The next instance of a HOB type from the starting HOB.

**/
VOID *
GetFirstHob (
  IN UINT16  Type
  )
{
  VOID  *HobList;

  HobList = GetHobList ();
  return GetNextHob (Type, HobList);
}

/**
  This function searches the first instance of a HOB from the starting HOB pointer.
  Such HOB should satisfy two conditions:
  its HOB type is HOB_TYPE_GUID_EXTENSION and its GUID Name equals to the input Guid.
  If there does not exist such HOB from the starting HOB pointer, it will return NULL.
  Caller is required to apply GET_GUID_HOB_DATA () and GET_GUID_HOB_DATA_SIZE ()
  to extract the data section and its size info respectively.
  In contrast with macro GET_NEXT_HOB(), this function does not skip the starting HOB pointer
  unconditionally: it returns HobStart back if HobStart itself meets the requirement;
  caller is required to use GET_NEXT_HOB() if it wishes to skip current HobStart.
  If Guid is NULL, then ASSERT().
  If HobStart is NULL, then ASSERT().

  @param  Guid          The GUID to match with in the HOB list.
  @param  HobStart      A pointer to a Guid.

  @return The next instance of the matched GUID HOB from the starting HOB.

**/
VOID *
GetNextGuidHob (
  IN const GUID   *Guid,
  IN const VOID   *HobStart
  )
{
  HOB_POINTERS  GuidHob;

  GuidHob.Raw = (UINT8 *)HobStart;
  while ((GuidHob.Raw = GetNextHob (HOB_TYPE_GUID_EXTENSION, GuidHob.Raw)) != NULL) {

    if (CompareGuid (Guid, &GuidHob.Guid->Name)) {
      break;
    }

    GuidHob.Raw = GET_NEXT_HOB (GuidHob);
  }

  return GuidHob.Raw;
}

/**
  This function searches the first instance of a HOB among the whole HOB list.
  Such HOB should satisfy two conditions:
  its HOB type is HOB_TYPE_GUID_EXTENSION and its GUID Name equals to the input Guid.
  If there does not exist such HOB from the starting HOB pointer, it will return NULL.
  Caller is required to apply GET_GUID_HOB_DATA () and GET_GUID_HOB_DATA_SIZE ()
  to extract the data section and its size info respectively.
  If Guid is NULL, then ASSERT().

  @param  Guid          The GUID to match with in the HOB list.

  @return The first instance of the matched GUID HOB among the whole HOB list.

**/
VOID *
GetFirstGuidHob (
  IN const GUID  *Guid
  )
{
  VOID  *HobList;

  HobList = GetHobList ();
  return GetNextGuidHob (Guid, HobList);
}

/**
  Builds a GUID HOB with a certain data length.

  This function builds a customized HOB tagged with a GUID for identification
  and returns the start address of GUID HOB data so that caller can fill the customized data.
  The HOB Header and Name field is already stripped.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.
  If Guid is NULL, then ASSERT().
  If there is no additional space for HOB creation, then ASSERT().
  If DataLength >= (0x10000 - sizeof (HOB_GUID_TYPE)), then ASSERT().

  @param  Guid          The GUID to tag the customized HOB.
  @param  DataLength    The size of the data payload for the GUID HOB.

  @return The start address of GUID HOB data.

**/
VOID *
BuildGuidHob (
  IN const GUID  *Guid,
  IN UINTN       DataLength
  )
{
  HOB_GUID_TYPE  *Hob;

  Hob = CreateHob (HOB_TYPE_GUID_EXTENSION, (UINT16)(sizeof (HOB_GUID_TYPE) + DataLength));
  CopyGuid (&Hob->Name, Guid);
  return Hob + 1;
}

/**
  Copies a data buffer to a newly-built HOB.

  This function builds a customized HOB tagged with a GUID for identification,
  copies the input data to the HOB data field and returns the start address of the GUID HOB data.
  The HOB Header and Name field is already stripped.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.
  If Guid is NULL, then ASSERT().
  If Data is NULL and DataLength > 0, then ASSERT().
  If there is no additional space for HOB creation, then ASSERT().
  If DataLength >= (0x10000 - sizeof (HOB_GUID_TYPE)), then ASSERT().

  @param  Guid          The GUID to tag the customized HOB.
  @param  Data          The data to be copied into the data field of the GUID HOB.
  @param  DataLength    The size of the data payload for the GUID HOB.

  @return The start address of GUID HOB data.

**/
VOID *
ShBuildGuidDataHob (
  IN const GUID  *Guid,
  IN VOID        *Data,
  IN UINTN       DataLength
  )
{
  VOID  *HobData;

  HobData = BuildGuidHob (Guid, DataLength);

  return CopyMem (HobData, Data, DataLength);
}

/**
  Builds a Firmware Volume HOB.

  This function builds a Firmware Volume HOB.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.
  If there is no additional space for HOB creation, then ASSERT().

  @param  BaseAddress   The base address of the Firmware Volume.
  @param  Length        The size of the Firmware Volume in bytes.

**/
VOID
ShBuildFvHob (
  IN ADDRESS  BaseAddress,
  IN UINT64   Length
  )
{
  HOB_FIRMWARE_VOLUME  *Hob;

  Hob = CreateHob (HOB_TYPE_FV, sizeof (HOB_FIRMWARE_VOLUME));

  Hob->BaseAddress = BaseAddress;
  Hob->Length      = Length;
}

/**
  Builds a HOB_TYPE_FV2 HOB.

  This function builds a HOB_TYPE_FV2 HOB.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.
  If there is no additional space for HOB creation, then ASSERT().

  @param  BaseAddress   The base address of the Firmware Volume.
  @param  Length        The size of the Firmware Volume in bytes.
  @param  FvName       The name of the Firmware Volume.
  @param  FileName      The name of the file.

**/
VOID
ShBuildFv2Hob (
  IN          ADDRESS  BaseAddress,
  IN          UINT64   Length,
  IN const    GUID     *FvName,
  IN const    GUID     *FileName
  )
{
  HOB_FIRMWARE_VOLUME2  *Hob;

  Hob = CreateHob (HOB_TYPE_FV2, sizeof (HOB_FIRMWARE_VOLUME2));

  Hob->BaseAddress = BaseAddress;
  Hob->Length      = Length;
  CopyGuid (&Hob->FvName, FvName);
  CopyGuid (&Hob->FileName, FileName);
}

/**
  Builds a HOB_TYPE_FV3 HOB.

  This function builds a HOB_TYPE_FV3 HOB.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.

  If there is no additional space for HOB creation, then ASSERT().

  @param BaseAddress            The base address of the Firmware Volume.
  @param Length                 The size of the Firmware Volume in bytes.
  @param AuthenticationStatus   The authentication status.
  @param ExtractedFv            TRUE if the FV was extracted as a file within
                                another firmware volume. FALSE otherwise.
  @param FvName                 The name of the Firmware Volume.
                                Valid only if IsExtractedFv is TRUE.
  @param FileName               The name of the file.
                                Valid only if IsExtractedFv is TRUE.

**/
VOID
ShBuildFv3Hob (
  IN          ADDRESS  BaseAddress,
  IN          UINT64   Length,
  IN          UINT32   AuthenticationStatus,
  IN          BOOLEAN  ExtractedFv,
  IN const    GUID     *FvName  OPTIONAL,
  IN const    GUID     *FileName OPTIONAL
  )
{
  HOB_FIRMWARE_VOLUME3  *Hob;

  Hob = CreateHob (HOB_TYPE_FV3, sizeof (HOB_FIRMWARE_VOLUME3));

  Hob->BaseAddress          = BaseAddress;
  Hob->Length               = Length;
  Hob->AuthenticationStatus = AuthenticationStatus;
  Hob->ExtractedFv          = ExtractedFv;
  if (ExtractedFv) {
    CopyGuid (&Hob->FvName, FvName);
    CopyGuid (&Hob->FileName, FileName);
  }
}

/**
  Builds a HOB for the CPU.

  This function builds a HOB for the CPU.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.
  If there is no additional space for HOB creation, then ASSERT().

  @param  SizeOfMemorySpace   The maximum physical memory addressability of the processor.
  @param  SizeOfIoSpace       The maximum physical I/O addressability of the processor.

**/
VOID
ShBuildCpuHob (
  IN UINT8  SizeOfMemorySpace,
  IN UINT8  SizeOfIoSpace
  )
{
  HOB_CPU  *Hob;

  Hob = CreateHob (HOB_TYPE_CPU, sizeof (HOB_CPU));

  Hob->SizeOfMemorySpace = SizeOfMemorySpace;
  Hob->SizeOfIoSpace     = SizeOfIoSpace;

  //
  // Zero the reserved space to match HOB spec
  //
  ZeroMem (Hob->Reserved, sizeof (Hob->Reserved));
}

/**
  Builds a HOB for the memory allocation.

  This function builds a HOB for the memory allocation.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.
  If there is no additional space for HOB creation, then ASSERT().

  @param  BaseAddress   The 64 bit physical address of the memory.
  @param  Length        The length of the memory allocation in bytes.
  @param  MemoryType    Type of memory allocated by this HOB.

**/
VOID
BuildMemoryAllocationHob (
  IN ADDRESS      BaseAddress,
  IN UINT64       Length,
  IN MEMORY_TYPE  MemoryType
  )
{
  HOB_MEMORY_ALLOCATION  *Hob;

  Hob = CreateHob (HOB_TYPE_MEMORY_ALLOCATION, sizeof (HOB_MEMORY_ALLOCATION));

  ZeroMem (&(Hob->AllocDescriptor.Name), sizeof (GUID));
  Hob->AllocDescriptor.MemoryBaseAddress = BaseAddress;
  Hob->AllocDescriptor.MemoryLength      = Length;
  Hob->AllocDescriptor.MemoryType        = MemoryType;
  //
  // Zero the reserved space to match HOB spec
  //
  ZeroMem (Hob->AllocDescriptor.Reserved, sizeof (Hob->AllocDescriptor.Reserved));
}
