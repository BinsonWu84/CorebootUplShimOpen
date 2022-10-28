/** @file
  The firmware related definitions in PI.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
  PI Version 1.6.

**/


#ifndef __PI_FIRMWARE_H__
#define __PI_FIRMWARE_H__

#pragma pack(1)
///
/// Used to verify the integrity of the file.
///
typedef union {
  struct {
    ///
    /// The IntegrityCheck.Checksum.Header field is an 8-bit checksum of the file
    /// header. The State and IntegrityCheck.Checksum.File fields are assumed
    /// to be zero and the checksum is calculated such that the entire header sums to zero.
    ///
    UINT8   Header;
    ///
    /// If the FFS_ATTRIB_CHECKSUM (see definition below) bit of the Attributes
    /// field is set to one, the IntegrityCheck.Checksum.File field is an 8-bit
    /// checksum of the file data.
    /// If the FFS_ATTRIB_CHECKSUM bit of the Attributes field is cleared to zero,
    /// the IntegrityCheck.Checksum.File field must be initialized with a value of
    /// 0xAA. The IntegrityCheck.Checksum.File field is valid any time the
    /// FILE_DATA_VALID bit is set in the State field.
    ///
    UINT8   File;
  } Checksum;
  ///
  /// This is the full 16 bits of the IntegrityCheck field.
  ///
  UINT16    Checksum16;
} FFS_INTEGRITY_CHECK;

typedef UINT8 FV_FILETYPE;
typedef UINT8 FFS_FILE_ATTRIBUTES;
typedef UINT8 FFS_FILE_STATE;

///
/// Each file begins with the header that describe the
/// contents and state of the files.
///
typedef struct {
  ///
  /// This GUID is the file name. It is used to uniquely identify the file.
  ///
  GUID                Name;
  ///
  /// Used to verify the integrity of the file.
  ///
  FFS_INTEGRITY_CHECK IntegrityCheck;
  ///
  /// Identifies the type of file.
  ///
  FV_FILETYPE         Type;
  ///
  /// Declares various file attribute bits.
  ///
  FFS_FILE_ATTRIBUTES Attributes;
  ///
  /// The length of the file in bytes, including the FFS header.
  ///
  UINT8               Size[3];
  ///
  /// Used to track the state of the file throughout the life of the file from creation to deletion.
  ///
  FFS_FILE_STATE      State;
} FFS_FILE_HEADER;

///
/// The argument passed as the FfsFileHeaderPtr parameter to the
/// FFS_FILE_SIZE() function-like macro below must not have side effects:
/// FfsFileHeaderPtr is evaluated multiple times.
///
#define FFS_FILE_SIZE(FfsFileHeaderPtr) ((UINT32) ( \
    (((FFS_FILE_HEADER *) (UINTN) (FfsFileHeaderPtr))->Size[0]      ) | \
    (((FFS_FILE_HEADER *) (UINTN) (FfsFileHeaderPtr))->Size[1] <<  8) | \
    (((FFS_FILE_HEADER *) (UINTN) (FfsFileHeaderPtr))->Size[2] << 16)))

#define FFS_FILE2_SIZE(FfsFileHeaderPtr) \
    ((UINT32) (((FFS_FILE_HEADER2 *) (UINTN) FfsFileHeaderPtr)->ExtendedSize))

typedef UINT8 SECTION_TYPE;

#pragma pack()


///
/// type of EFI FVB attribute
///
typedef UINT32  FVB_ATTRIBUTES_2;

typedef struct {
  ///
  /// The number of sequential blocks which are of the same size.
  ///
  UINT32 NumBlocks;
  ///
  /// The size of the blocks.
  ///
  UINT32 Length;
} FV_BLOCK_MAP_ENTRY;

///
/// Describes the features and layout of the firmware volume.
///
typedef struct {
  ///
  /// The first 16 bytes are reserved to allow for the reset vector of
  /// processors whose reset vector is at address 0.
  ///
  UINT8                     ZeroVector[16];
  ///
  /// Declares the file system with which the firmware volume is formatted.
  ///
  GUID                      FileSystemGuid;
  ///
  /// Length in bytes of the complete firmware volume, including the header.
  ///
  UINT64                    FvLength;
  ///
  /// Set to FVH_SIGNATURE
  ///
  UINT32                    Signature;
  ///
  /// Declares capabilities and power-on defaults for the firmware volume.
  ///
  FVB_ATTRIBUTES_2          Attributes;
  ///
  /// Length in bytes of the complete firmware volume header.
  ///
  UINT16                    HeaderLength;
  ///
  /// A 16-bit checksum of the firmware volume header. A valid header sums to zero.
  ///
  UINT16                    Checksum;
  ///
  /// Offset, relative to the start of the header, of the extended header
  /// (FIRMWARE_VOLUME_EXT_HEADER) or zero if there is no extended header.
  ///
  UINT16                    ExtHeaderOffset;
  ///
  /// This field must always be set to zero.
  ///
  UINT8                     Reserved[1];
  ///
  /// Set to 2. Future versions of this specification may define new header fields and will
  /// increment the Revision field accordingly.
  ///
  UINT8                     Revision;
  ///
  /// An array of run-length encoded FvBlockMapEntry structures. The array is
  /// terminated with an entry of {0,0}.
  ///
  FV_BLOCK_MAP_ENTRY        BlockMap[1];
} FIRMWARE_VOLUME_HEADER;

/// If this bit is set, then the PCI Root Bridge does not
/// support separate windows for Non-prefetchable and Prefetchable
/// memory. A PCI bus driver needs to include requests for Prefetchable
/// memory in the Non-prefetchable memory pool.
///
#define PCI_HOST_BRIDGE_COMBINE_MEM_PMEM  1

///
/// If this bit is set, then the PCI Root Bridge supports
/// 64 bit memory windows.  If this bit is not set,
/// the PCI bus driver needs to include requests for 64 bit
/// memory address in the corresponding 32 bit memory pool.
///
#define PCI_HOST_BRIDGE_MEM64_DECODE      2

#endif // __PI_FIRMWARE_H__