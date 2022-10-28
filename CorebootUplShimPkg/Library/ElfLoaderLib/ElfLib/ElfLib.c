/** @file
  ELF library

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <BaseLib.h>
#include "ElfLibInternal.h"

/**
  Check if the ELF image is valid.

  @param[in]  ImageBase       Memory address of an image.

  @retval     TRUE if valid.

**/
BOOLEAN
IsElfFormat (
  IN  const UINT8  *ImageBase
  )
{
  Elf32_Ehdr  *Elf32Hdr;
  Elf64_Ehdr  *Elf64Hdr;

  Elf32Hdr = (Elf32_Ehdr *)ImageBase;

  //
  // Start with correct signature "\7fELF"
  //
  if ((Elf32Hdr->e_ident[EI_MAG0] != ELFMAG0) ||
      (Elf32Hdr->e_ident[EI_MAG1] != ELFMAG1) ||
      (Elf32Hdr->e_ident[EI_MAG1] != ELFMAG1) ||
      (Elf32Hdr->e_ident[EI_MAG2] != ELFMAG2)
      )
  {
    return FALSE;
  }

  //
  // Support little-endian only
  //
  if (Elf32Hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
    return FALSE;
  }

  //
  // Check 32/64-bit architecture
  //
  if (Elf32Hdr->e_ident[EI_CLASS] == ELFCLASS64) {
    Elf64Hdr = (Elf64_Ehdr *)Elf32Hdr;
    Elf32Hdr = NULL;
  } else if (Elf32Hdr->e_ident[EI_CLASS] == ELFCLASS32) {
    Elf64Hdr = NULL;
  } else {
    return FALSE;
  }

  if (Elf64Hdr != NULL) {
    //
    // Support intel architecture only for now
    //
    if (Elf64Hdr->e_machine != EM_X86_64) {
      return FALSE;
    }

    //
    //  Support ELF types: EXEC (Executable file), DYN (Shared object file)
    //
    if ((Elf64Hdr->e_type != ET_EXEC) && (Elf64Hdr->e_type != ET_DYN)) {
      return FALSE;
    }

    //
    // Support current ELF version only
    //
    if (Elf64Hdr->e_version != EV_CURRENT) {
      return FALSE;
    }
  } else {
    //
    // Support intel architecture only for now
    //
    if (Elf32Hdr->e_machine != EM_386) {
      return FALSE;
    }

    //
    //  Support ELF types: EXEC (Executable file), DYN (Shared object file)
    //
    if ((Elf32Hdr->e_type != ET_EXEC) && (Elf32Hdr->e_type != ET_DYN)) {
      return FALSE;
    }

    //
    // Support current ELF version only
    //
    if (Elf32Hdr->e_version != EV_CURRENT) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
  Calculate a ELF file size.

  @param[in]  ElfCt               ELF image context pointer.
  @param[out] FileSize            Return the file size.

  @retval INVALID_PARAMETER   ElfCt or SecPos is NULL.
  @retval NOT_FOUND           Could not find the section.
  @retval SUCCESS             Section posistion was filled successfully.
**/
RETURN_STATUS
CalculateElfFileSize (
  IN  ELF_IMAGE_CONTEXT  *ElfCt,
  OUT UINTN              *FileSize
  )
{
  RETURN_STATUS  Status;
  UINTN          FileSize1;
  UINTN          FileSize2;
  Elf32_Ehdr     *Elf32Hdr;
  Elf64_Ehdr     *Elf64Hdr;
  UINTN          Offset;
  UINTN          Size;

  if ((ElfCt == NULL) || (FileSize == NULL)) {
    return INVALID_PARAMETER;
  }

  // Use last section as end of file
  Status = GetElfSectionPos (ElfCt, ElfCt->ShNum - 1, &Offset, &Size);
  if (ERROR (Status)) {
    return UNSUPPORTED;
  }

  FileSize1 = Offset + Size;

  // Use end of section header as end of file
  FileSize2 = 0;
  if (ElfCt->EiClass == ELFCLASS32) {
    Elf32Hdr  = (Elf32_Ehdr *)ElfCt->FileBase;
    FileSize2 = Elf32Hdr->e_shoff + Elf32Hdr->e_shentsize * Elf32Hdr->e_shnum;
  } else if (ElfCt->EiClass == ELFCLASS64) {
    Elf64Hdr  = (Elf64_Ehdr *)ElfCt->FileBase;
    FileSize2 = (UINTN)(Elf64Hdr->e_shoff + Elf64Hdr->e_shentsize * Elf64Hdr->e_shnum);
  }

  *FileSize = MAX (FileSize1, FileSize2);

  return SUCCESS;
}

/**
  Get a ELF program segment loading info.

  @param[in]  ImageBase           Image base.
  @param[in]  EiClass             ELF class.
  @param[in]  Index               ELF segment index.
  @param[out] SegInfo             The pointer to the segment info.

  @retval INVALID_PARAMETER   ElfCt or SecPos is NULL.
  @retval NOT_FOUND           Could not find the section.
  @retval SUCCESS             Section posistion was filled successfully.
**/
RETURN_STATUS
GetElfSegmentInfo (
  IN  UINT8         *ImageBase,
  IN  UINT32        EiClass,
  IN  UINT32        Index,
  OUT SEGMENT_INFO  *SegInfo
  )
{
  Elf32_Phdr  *Elf32Phdr;
  Elf64_Phdr  *Elf64Phdr;

  if ((ImageBase == NULL) || (SegInfo == NULL)) {
    return INVALID_PARAMETER;
  }

  if (EiClass == ELFCLASS32) {
    Elf32Phdr = GetElf32SegmentByIndex (ImageBase, Index);
    if (Elf32Phdr != NULL) {
      SegInfo->PtType    = Elf32Phdr->p_type;
      SegInfo->Offset    = Elf32Phdr->p_offset;
      SegInfo->Length    = Elf32Phdr->p_filesz;
      SegInfo->MemLen    = Elf32Phdr->p_memsz;
      SegInfo->MemAddr   = Elf32Phdr->p_paddr;
      SegInfo->Alignment = Elf32Phdr->p_align;
      return SUCCESS;
    }
  } else if (EiClass == ELFCLASS64) {
    Elf64Phdr = GetElf64SegmentByIndex (ImageBase, Index);
    if (Elf64Phdr != NULL) {
      SegInfo->PtType    = Elf64Phdr->p_type;
      SegInfo->Offset    = (UINTN)Elf64Phdr->p_offset;
      SegInfo->Length    = (UINTN)Elf64Phdr->p_filesz;
      SegInfo->MemLen    = (UINTN)Elf64Phdr->p_memsz;
      SegInfo->MemAddr   = (UINTN)Elf64Phdr->p_paddr;
      SegInfo->Alignment = (UINTN)Elf64Phdr->p_align;
      return SUCCESS;
    }
  }

  return NOT_FOUND;
}

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
  )
{
  Elf32_Ehdr     *Elf32Hdr;
  Elf64_Ehdr     *Elf64Hdr;
  Elf32_Shdr     *Elf32Shdr;
  Elf64_Shdr     *Elf64Shdr;
  RETURN_STATUS  Status;
  UINT32         Index;
  SEGMENT_INFO   SegInfo;
  UINTN          End;
  UINTN          Base;

  if (ElfCt == NULL) {
    return INVALID_PARAMETER;
  }

  ZeroMem (ElfCt, sizeof (ELF_IMAGE_CONTEXT));

  if (ImageBase == NULL) {
    return (ElfCt->ParseStatus = INVALID_PARAMETER);
  }

  ElfCt->FileBase = (UINT8 *)ImageBase;
  if (!IsElfFormat (ElfCt->FileBase)) {
    return (ElfCt->ParseStatus = UNSUPPORTED);
  }

  Elf32Hdr       = (Elf32_Ehdr *)ElfCt->FileBase;
  ElfCt->EiClass = Elf32Hdr->e_ident[EI_CLASS];
  if (ElfCt->EiClass == ELFCLASS32) {
    if ((Elf32Hdr->e_type != ET_EXEC) && (Elf32Hdr->e_type != ET_DYN)) {
      return (ElfCt->ParseStatus = UNSUPPORTED);
    }

    Elf32Shdr = (Elf32_Shdr *)GetElf32SectionByIndex (ElfCt->FileBase, Elf32Hdr->e_shstrndx);
    if (Elf32Shdr == NULL) {
      return (ElfCt->ParseStatus = UNSUPPORTED);
    }

    ElfCt->EntryPoint = (UINTN)Elf32Hdr->e_entry;
    ElfCt->ShNum      = Elf32Hdr->e_shnum;
    ElfCt->PhNum      = Elf32Hdr->e_phnum;
    ElfCt->ShStrLen   = Elf32Shdr->sh_size;
    ElfCt->ShStrOff   = Elf32Shdr->sh_offset;
  } else {
    Elf64Hdr = (Elf64_Ehdr *)Elf32Hdr;
    if ((Elf64Hdr->e_type != ET_EXEC) && (Elf64Hdr->e_type != ET_DYN)) {
      return (ElfCt->ParseStatus = UNSUPPORTED);
    }

    Elf64Shdr = (Elf64_Shdr *)GetElf64SectionByIndex (ElfCt->FileBase, Elf64Hdr->e_shstrndx);
    if (Elf64Shdr == NULL) {
      return (ElfCt->ParseStatus = UNSUPPORTED);
    }

    ElfCt->EntryPoint = (UINTN)Elf64Hdr->e_entry;
    ElfCt->ShNum      = Elf64Hdr->e_shnum;
    ElfCt->PhNum      = Elf64Hdr->e_phnum;
    ElfCt->ShStrLen   = (UINT32)Elf64Shdr->sh_size;
    ElfCt->ShStrOff   = (UINT32)Elf64Shdr->sh_offset;
  }

  //
  // Get the preferred image base and required memory size when loaded to new location.
  //
  End                   = 0;
  Base                  = MAX_UINT32;
  ElfCt->ReloadRequired = FALSE;
  for (Index = 0; Index < ElfCt->PhNum; Index++) {
    Status = GetElfSegmentInfo (ElfCt->FileBase, ElfCt->EiClass, Index, &SegInfo);

    if (SegInfo.PtType != PT_LOAD) {
      continue;
    }

    if (SegInfo.MemLen != SegInfo.Length) {
      //
      // Not enough space to execute at current location.
      //
      ElfCt->ReloadRequired = TRUE;
    }

    if (Base > (SegInfo.MemAddr & ~(PAGE_SIZE - 1))) {
      Base = SegInfo.MemAddr & ~(PAGE_SIZE - 1);
    }

    if (End < ALIGN_VALUE (SegInfo.MemAddr + SegInfo.MemLen, PAGE_SIZE) - 1) {
      End = ALIGN_VALUE (SegInfo.MemAddr + SegInfo.MemLen, PAGE_SIZE) - 1;
    }
  }

  //
  // 0 - MAX_UINT32  + 1 equals to 0.
  //
  ElfCt->ImageSize             = End - Base + 1;
  ElfCt->PreferredImageAddress = (VOID *)Base;

  CalculateElfFileSize (ElfCt, &ElfCt->FileSize);
  return (ElfCt->ParseStatus = SUCCESS);
}

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
  )
{
  RETURN_STATUS  Status;

  if (ElfCt == NULL) {
    return INVALID_PARAMETER;
  }

  if (ERROR (ElfCt->ParseStatus)) {
    return ElfCt->ParseStatus;
  }

  if (ElfCt->ImageAddress == NULL) {
    return INVALID_PARAMETER;
  }

  Status = UNSUPPORTED;
  if (ElfCt->EiClass == ELFCLASS32) {
    Status = LoadElf32Image (ElfCt);
  } else if (ElfCt->EiClass == ELFCLASS64) {
    Status = LoadElf64Image (ElfCt);
  }

  return Status;
}

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
  )
{
  Elf32_Shdr  *Elf32Shdr;
  Elf64_Shdr  *Elf64Shdr;
  CHAR8       *Name;

  if ((ElfCt == NULL) || (SectionName == NULL)) {
    return INVALID_PARAMETER;
  }

  if (ERROR (ElfCt->ParseStatus)) {
    return ElfCt->ParseStatus;
  }

  Name = NULL;
  if (ElfCt->EiClass == ELFCLASS32) {
    Elf32Shdr = GetElf32SectionByIndex (ElfCt->FileBase, SectionIndex);
    if ((Elf32Shdr != NULL) && (Elf32Shdr->sh_name < ElfCt->ShStrLen)) {
      Name = (CHAR8 *)(ElfCt->FileBase + ElfCt->ShStrOff + Elf32Shdr->sh_name);
    }
  } else if (ElfCt->EiClass == ELFCLASS64) {
    Elf64Shdr = GetElf64SectionByIndex (ElfCt->FileBase, SectionIndex);
    if ((Elf64Shdr != NULL) && (Elf64Shdr->sh_name < ElfCt->ShStrLen)) {
      Name = (CHAR8 *)(ElfCt->FileBase + ElfCt->ShStrOff + Elf64Shdr->sh_name);
    }
  }

  if (Name == NULL) {
    return NOT_FOUND;
  }

  *SectionName = Name;
  return SUCCESS;
}

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
  )
{
  Elf32_Shdr  *Elf32Shdr;
  Elf64_Shdr  *Elf64Shdr;

  if ((ElfCt == NULL) || (Offset == NULL) || (Size == NULL)) {
    return INVALID_PARAMETER;
  }

  if (ERROR (ElfCt->ParseStatus)) {
    return ElfCt->ParseStatus;
  }

  if (ElfCt->EiClass == ELFCLASS32) {
    Elf32Shdr = GetElf32SectionByIndex (ElfCt->FileBase, Index);
    if (Elf32Shdr != NULL) {
      *Offset = (UINTN)Elf32Shdr->sh_offset;
      *Size   = (UINTN)Elf32Shdr->sh_size;
      return SUCCESS;
    }
  } else if (ElfCt->EiClass == ELFCLASS64) {
    Elf64Shdr = GetElf64SectionByIndex (ElfCt->FileBase, Index);
    if (Elf64Shdr != NULL) {
      *Offset = (UINTN)Elf64Shdr->sh_offset;
      *Size   = (UINTN)Elf64Shdr->sh_size;
      return SUCCESS;
    }
  }

  return NOT_FOUND;
}
