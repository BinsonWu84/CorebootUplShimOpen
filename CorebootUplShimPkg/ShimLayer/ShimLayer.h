/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SHIMLAYER_H__
#define __SHIMLAYER_H__

#include <Base.h>
#include <BaseLib.h>
#include <HobLib.h>
#include <ParseLib.h>
#include <ShimLayer/PiFirmware.h>
#include <ShimLayer/DevicePath.h>
#include <ElfLibInternal.h>
#include <CorebootRelated.h>
#include <Graphics.h>
#include <UniversalPayload.h>
#include <SerialPort.h>

#define LEGACY_8259_MASK_REGISTER_MASTER  0x21
#define LEGACY_8259_MASK_REGISTER_SLAVE   0xA1
#define GET_OCCUPIED_SIZE(ActualSize, Alignment) \
  ((ActualSize) + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1)))
#define ALIGN_UP(x, a) (((x)+(a - 1))&~(a-1))
#define SWAP32(x) \
  ((unsigned int)( \
    (((unsigned int)(x) & 0x000000ffUL) << 24) | \
    (((unsigned int)(x) & 0x0000ff00UL) <<  8) | \
    (((unsigned int)(x) & 0x00ff0000UL) >>  8) | \
    (((unsigned int)(x) & 0xff000000UL) >> 24)))


#define E820_RAM       1
#define E820_RESERVED  2
#define E820_ACPI      3
#define E820_NVS       4
#define E820_UNUSABLE  5
#define E820_DISABLED  6
#define E820_PMEM      7
#define E820_UNDEFINED 8

RETURN_STATUS
LzmaUefiDecompressGetInfo (
  IN  CONST VOID  *Source,
  IN  UINT32      SourceSize,
  OUT UINT32      *DestinationSize,
  OUT UINT32      *ScratchSize
  );

/**
  Update the Stack Hob if the stack has been moved

  @param  BaseAddress   The 64 bit physical address of the Stack.
  @param  Length        The length of the stack in bytes.

**/
VOID
UpdateStackHob (
  IN ADDRESS        BaseAddress,
  IN UINT64         Length
  );

/**
  This function searchs a given section type within a valid FFS file.

  @param  FileHeader            A pointer to the file header that contains the set of sections to
                                be searched.
  @param  SearchType            The value of the section type to search.
  @param  SectionData           A pointer to the discovered section, if successful.

  @retval SUCCESS           The section was found.
  @retval NOT_FOUND         The section was not found.

**/
RETURN_STATUS
FileFindSection (
  IN FFS_FILE_HEADER        *FileHeader,
  IN SECTION_TYPE           SectionType,
  OUT VOID                  **SectionData
  );

/**
  This function searchs a given file type with a given Guid within a valid FV.
  If input Guid is NULL, will locate the first section having the given file type

  @param FvHeader        A pointer to firmware volume header that contains the set of files
                         to be searched.
  @param FileType        File type to be searched.
  @param Guid            Will ignore if it is NULL.
  @param FileHeader      A pointer to the discovered file, if successful.

  @retval SUCCESS    Successfully found FileType
  @retval NOT_FOUND  File type can't be found.
**/
RETURN_STATUS
FvFindFileByTypeGuid (
  IN  FIRMWARE_VOLUME_HEADER  *FvHeader,
  IN  FV_FILETYPE             FileType,
  IN  GUID                    *Guid           OPTIONAL,
  OUT FFS_FILE_HEADER         **FileHeader
  );

RETURN_STATUS
LzmaUefiDecompress (
  IN CONST VOID  *Source,
  IN UINTN       SourceSize,
  IN OUT VOID    *Destination,
  IN OUT VOID    *Scratch
  );

/**
  Auto-generated function that calls the library constructors for all of the module's
  dependent libraries.  This function must be called by the SEC Core once a stack has
  been established.

**/
VOID
ProcessLibraryConstructorList (
  VOID
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

#endif // __SHIMLAYER_H__