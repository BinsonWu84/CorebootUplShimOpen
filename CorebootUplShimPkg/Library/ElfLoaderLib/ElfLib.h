/** @file
  ELF library

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef ELF_LIB_H_
#define ELF_LIB_H_

#include <Base.h>
#include <Library/ElfLoaderLib.h>

#define  ELF_CLASS32  1
#define  ELF_CLASS64  2

#define  ELF_PT_LOAD  1

typedef struct {
  UINT32    PtType;
  UINTN     Offset;
  UINTN     Length;
  UINTN     MemLen;
  UINTN     MemAddr;
  UINTN     Alignment;
} SEGMENT_INFO;

/**
  Parse the ELF image info.

  @param[in]  ImageBase      Memory address of an image.
  @param[out] ElfCt          The EFL image context pointer.

  @retval INVALID_PARAMETER   Input parameters are not valid.
  @retval UNSUPPORTED         Unsupported binary type.
  @retval LOAD_ERROR          ELF binary loading error.
  @retval SUCCESS             ELF binary is loaded successfully.
**/
RETURN_STATUS
ParseElfImage (
  IN  VOID               *ImageBase,
  OUT ELF_IMAGE_CONTEXT  *ElfCt
  );

/**
  Load the ELF segments to specified address in ELF header.

  This function loads ELF image segments into memory address specified
  in ELF program header.

  @param[in]  ElfCt               ELF image context pointer.

  @retval INVALID_PARAMETER   Input parameters are not valid.
  @retval UNSUPPORTED         Unsupported binary type.
  @retval LOAD_ERROR          ELF binary loading error.
  @retval SUCCESS             ELF binary is loaded successfully.
**/
RETURN_STATUS
LoadElfImage (
  IN  ELF_IMAGE_CONTEXT  *ElfCt
  );

/**
  Get a ELF section name from its index.

  @param[in]  ElfCt               ELF image context pointer.
  @param[in]  SectionIndex        ELF section index.
  @param[out] SectionName         The pointer to the section name.

  @retval INVALID_PARAMETER   ElfCt or SecName is NULL.
  @retval NOT_FOUND           Could not find the section.
  @retval SUCCESS             Section name was filled successfully.
**/
RETURN_STATUS
GetElfSectionName (
  IN  ELF_IMAGE_CONTEXT  *ElfCt,
  IN  UINT32             SectionIndex,
  OUT CHAR8              **SectionName
  );

/**
  Get the offset and size of x-th ELF section.

  @param[in]  ElfCt               ELF image context pointer.
  @param[in]  Index               ELF section index.
  @param[out] Offset              Return the offset of the specific section.
  @param[out] Size                Return the size of the specific section.

  @retval INVALID_PARAMETER   ImageBase, Offset or Size is NULL.
  @retval INVALID_PARAMETER   EiClass doesn't equal to ELFCLASS32 or ELFCLASS64.
  @retval NOT_FOUND           Could not find the section.
  @retval SUCCESS             Offset and Size are returned.
**/
RETURN_STATUS
GetElfSectionPos (
  IN  ELF_IMAGE_CONTEXT  *ElfCt,
  IN  UINT32             Index,
  OUT UINTN              *Offset,
  OUT UINTN              *Size
  );

#endif /* ELF_LIB_H_ */
