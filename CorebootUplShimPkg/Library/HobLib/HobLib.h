/** @file
  Hob Library.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __HOB_LIB_H__
#define __HOB_LIB_H__

#include <Base.h>
#include <BaseLib.h>

//
// 0x21 - 0xf..f are reserved.
//
#define BOOT_WITH_FULL_CONFIGURATION                  0x00
#define BOOT_WITH_MINIMAL_CONFIGURATION               0x01
#define BOOT_ASSUMING_NO_CONFIGURATION_CHANGES        0x02
#define BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS 0x03
#define BOOT_WITH_DEFAULT_SETTINGS                    0x04
#define BOOT_ON_S4_RESUME                             0x05
#define BOOT_ON_S5_RESUME                             0x06
#define BOOT_WITH_MFG_MODE_SETTINGS                   0x07
#define BOOT_ON_S2_RESUME                             0x10
#define BOOT_ON_S3_RESUME                             0x11
#define BOOT_ON_FLASH_UPDATE                          0x12
#define BOOT_IN_RECOVERY_MODE                         0x20

//
// HobType of HOB_GENERIC_HEADER.
//
#define HOB_TYPE_HANDOFF              0x0001
#define HOB_TYPE_MEMORY_ALLOCATION    0x0002
#define HOB_TYPE_RESOURCE_DESCRIPTOR  0x0003
#define HOB_TYPE_GUID_EXTENSION       0x0004
#define HOB_TYPE_FV                   0x0005
#define HOB_TYPE_CPU                  0x0006
#define HOB_TYPE_MEMORY_POOL          0x0007
#define HOB_TYPE_FV2                  0x0009
#define HOB_TYPE_LOAD_PEIM_UNUSED     0x000A
#define HOB_TYPE_UCAPSULE             0x000B
#define HOB_TYPE_FV3                  0x000C
#define HOB_TYPE_UNUSED               0xFFFE
#define HOB_TYPE_END_OF_HOB_LIST      0xFFFF

///
/// Value of version  in HOB_HANDOFF_INFO_TABLE.
///
#define HOB_HANDOFF_TABLE_VERSION 0x0009

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
  );

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
  );

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
  );

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
  IN ADDRESS                  PhysicalStart,
  IN UINT64                   NumberOfBytes
  );

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
  IN CONST GUID  *Guid,
  IN UINTN       DataLength
  );

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
  );


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
  IN CONST VOID  *HobStart
  );

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
  );

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
  IN CONST GUID  *Guid,
  IN CONST VOID  *HobStart
  );

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
  IN CONST GUID  *Guid
  );

/**
  Returns the pointer to the HOB list.

  This function returns the pointer to first HOB in the list.

  @return The pointer to the HOB list.

**/
VOID *
GetHobList (
    VOID
  );

/**
  Returns the size of the data buffer from a HOB of type HOB_TYPE_GUID_EXTENSION.

  This macro returns the size, in bytes, of the data buffer in a HOB specified by HobStart.
  HobStart is assumed to be a HOB of type HOB_TYPE_GUID_EXTENSION.

  @param   GuidHob   A pointer to a HOB.

  @return  The size of the data buffer.
**/
#define GET_GUID_HOB_DATA_SIZE(HobStart) \
  (UINT16)(GET_HOB_LENGTH (HobStart) - sizeof (HOB_GUID_TYPE))

#endif // __HOB_LIB_H__
