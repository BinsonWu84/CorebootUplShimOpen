/** @file
  Parse Library.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PARSE_LIB_H__
#define __PARSE_LIB_H__

#include <Base.h>
#include <BaseLib.h>
#include <CorebootRelated.h>
#include <Graphics.h>
#include <UniversalPayload.h>
#include <SerialPort.h>

typedef RETURN_STATUS \
(*BL_MEM_INFO_CALLBACK) (
  MEMORY_MAP_ENTRY  *MemoryMapEntry,
  VOID              *Param
  );

/**
  Convert a packed value from cbuint64 to a UINT64 value.

  @param  val      The pointer to packed data.

  @return          the UNIT64 value after conversion.

**/
UINT64
cb_unpack64 (
  IN struct cbuint64  val
  );

/**
  Find coreboot record with given Tag.

  @param  Tag                The tag id to be found

  @retval NULL              The Tag is not found.
  @retval Others            The pointer to the record found.

**/
VOID *
FindCbTag (
  IN  UINT32  Tag
  );

/**
  Acquire the memory information from the coreboot table in memory.

  @param  MemInfoCallback     The callback routine
  @param  Params              Pointer to the callback routine parameter

  @retval RETURN_SUCCESS     Successfully find out the memory information.
  @retval RETURN_NOT_FOUND   Failed to find the memory information.

**/
RETURN_STATUS
ParseMemoryInfo (
  IN  BL_MEM_INFO_CALLBACK  MemInfoCallback,
  IN  VOID                  *Params
  );

/**
  Find the video frame buffer information

  @param  GfxInfo             Pointer to the PEI_GRAPHICS_INFO_HOB structure

  @retval RETURN_SUCCESS     Successfully find the video frame buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the video frame buffer information .

**/
RETURN_STATUS
ParseGfxInfo (
  OUT PEI_GRAPHICS_INFO_HOB  *GfxInfo
  );

/**
  Find the video frame buffer device information

  @param  GfxDeviceInfo      Pointer to the PEI_GRAPHICS_DEVICE_INFO_HOB structure

  @retval RETURN_SUCCESS     Successfully find the video frame buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the video frame buffer information.

**/
RETURN_STATUS
ParseGfxDeviceInfo (
  OUT PEI_GRAPHICS_DEVICE_INFO_HOB  *GfxDeviceInfo
  );

/**
  Acquire SMBIOS table from coreboot.

  @param  SmbiosTable               Pointer to the SMBIOS table info.

  @retval RETURN_SUCCESS            Successfully find out the tables.
  @retval RETURN_NOT_FOUND          Failed to find the tables.

**/
RETURN_STATUS
ParseSmbiosTable (
  OUT UNIVERSAL_PAYLOAD_SMBIOS_TABLE  *SmbiosTable
  );

/**
  Acquire ACPI table from coreboot.

  @param  AcpiTableHob              Pointer to the ACPI table info.

  @retval RETURN_SUCCESS            Successfully find out the tables.
  @retval RETURN_NOT_FOUND          Failed to find the tables.

**/
RETURN_STATUS
ParseAcpiTableInfo (
  OUT UNIVERSAL_PAYLOAD_ACPI_TABLE  *AcpiTableHob
  );

/**
  Find the serial port information

  @param  SerialPortInfo     Pointer to serial port info structure

  @retval RETURN_SUCCESS     Successfully find the serial port information.
  @retval RETURN_NOT_FOUND   Failed to find the serial port information .

**/
RETURN_STATUS
ParseSerialInfo (
  OUT SERIAL_PORT_INFO  *SerialPortInfo
  );

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
ParseCbMemTable (
  IN  UINT32  TableId,
  OUT VOID    **MemTable,
  OUT UINT32  *MemTableSize
  );

/**
  Find the given table with TableId from the given coreboot memory Root.

  @param  Root               The coreboot memory table to be searched in
  @param  TableId            Table id to be found
  @param  MemTable           To save the base address of the memory table found
  @param  MemTableSize       To save the size of memory table found

  @retval RETURN_SUCCESS            Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND          Failed to find the memory table.

**/
RETURN_STATUS
FindCbMemTable (
  IN  struct cbmem_root  *Root,
  IN  UINT32             TableId,
  OUT VOID               **MemTable,
  OUT UINT32             *MemTableSize
  );

/**
  Set bootloader parameter address.

  @retval NULL            Failed to find the GUID HOB.
  @retval others          GUIDed HOB data pointer.

**/
RETURN_STATUS
SetBootloaderParameter (
  IN  UINTN ParameterAddr
  );

#endif // __PARSE_LIB_H__