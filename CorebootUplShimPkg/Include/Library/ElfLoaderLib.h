/** @file
  This library can parse and load ELF format binary in memory
  and extract those required information.

  Copyright (c) 2014 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ELF_LOADER_LIB_H__
#define __ELF_LOADER_LIB_H__

typedef struct {
  RETURN_STATUS    ParseStatus;            ///< Return the status after ParseElfImage().
  UINT8            *FileBase;              ///< The source location in memory.
  UINTN            FileSize;               ///< The size including sections that don't require loading.
  UINT8            *PreferredImageAddress; ///< The preferred image to be loaded. No relocation is needed if loaded to this address.
  BOOLEAN          ReloadRequired;         ///< The image needs a new memory location for running.
  UINT8            *ImageAddress;          ///< The destination memory address set by caller.
  UINTN            ImageSize;              ///< The memory size for loading and execution.
  UINT32           EiClass;
  UINT32           ShNum;
  UINT32           PhNum;
  UINTN            ShStrOff;
  UINTN            ShStrLen;
  UINTN            EntryPoint;           ///< Return the actual entry point after LoadElfImage().
} ELF_IMAGE_CONTEXT;

/**
  Parse the ELF image info.

  On return, all fields in ElfCt are updated except ImageAddress.

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
  Load the ELF image to Context.ImageAddress.

  Context should be initialized by ParseElfImage().
  Caller should set Context.ImageAddress to a proper value, either pointing to
  a new allocated memory whose size equal to Context.ImageSize, or pointing
  to Context.PreferredImageAddress.

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

#endif // __ELF_LOADER_LIB_H__
