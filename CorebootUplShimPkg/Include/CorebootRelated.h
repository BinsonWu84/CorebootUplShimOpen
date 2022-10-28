/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __COREBOOT_RELATED_H__
#define __COREBOOT_RELATED_H__

#define FMAP_STRLEN             32
#define CBMEM_ID_IMD_SMALL      0x53a11439
#define CBMEM_ID_FMAP           0x464d4150
#define CBMEM_ID_CBFS_RO_MCACHE 0x524d5346
#define CBMEM_ID_FSP_RUNTIME    0x52505346
#define MCACHE_MAGIC_FILE       0x454c4946
#define MCACHE_MAGIC_FULL       0x4c4c5546
#define MCACHE_MAGIC_END        0x444e4524
#define CBFS_METADATA_MAX_SIZE  256
#define CBFS_UNIVERSAL_PAYLOAD  "img/UniversalPayload"
#define CBFS_MCACHE_ALIGNMENT   sizeof (UINT32)
#define CBFS_ALIGNMENT 64

struct imd_root_pointer {
  UINT32    magic;
  INT32     root_offset;
};

#pragma pack(1)
struct cbfs_file {
  char magic[8];
  UINT32 len;
  UINT32 type;
  UINT32 attributes_offset;
  UINT32 offset;
  char filename[0];
} ;

struct fmap_area {
  UINT32 offset;            /* offset relative to base */
  UINT32 size;              /* size in bytes */
  UINT8  name[FMAP_STRLEN]; /* descriptive name */
  UINT16 flags;             /* flags for this area */
};

struct fmap {
  UINT8  signature[8];      /* "__FMAP__" (0x5F5F464D41505F5F) */
  UINT8  ver_major;         /* major version */
  UINT8  ver_minor;         /* minor version */
  UINT64 base;              /* address of the firmware binary */
  UINT32 size;              /* size of firmware binary in bytes */
  UINT8  name[FMAP_STRLEN]; /* name of this firmware binary */
  UINT16 nareas;            /* number of areas */
  struct fmap_area areas[];
};

struct cbfs_payload_segment {
  UINT32 type;
  UINT32 compression;
  UINT32 offset;
  UINT64 load_addr;
  UINT32 len;
  UINT32 mem_len;
};

struct cbfs_payload {
  struct cbfs_payload_segment segments;
};
#pragma pack()

union cbfs_mdata {
  struct cbfs_file h;
  UINT8 raw[CBFS_METADATA_MAX_SIZE];
};

union mcache_entry {
  union cbfs_mdata file;
  struct {  /* These fields exactly overlap file.h.magic */
    UINT32 magic;
    UINT32 offset;
  };
};

enum cbfs_payload_segment_type {
  PAYLOAD_SEGMENT_CODE   = 0x434F4445,  /* BE: 'CODE' */
  PAYLOAD_SEGMENT_DATA   = 0x44415441,  /* BE: 'DATA' */
  PAYLOAD_SEGMENT_BSS    = 0x42535320,  /* BE: 'BSS ' */
  PAYLOAD_SEGMENT_PARAMS = 0x50415241,  /* BE: 'PARA' */
  PAYLOAD_SEGMENT_ENTRY  = 0x454E5452,  /* BE: 'ENTR' */
};

#define DYN_CBMEM_ALIGN_SIZE  (4096)

struct cbmem_entry {
  UINT32    magic;
  UINT32    start;
  UINT32    size;
  UINT32    id;
};

struct cbmem_root {
  UINT32                max_entries;
  UINT32                num_entries;
  UINT32                locked;
  UINT32                size;
  struct cbmem_entry    entries[0];
};

struct cbuint64 {
  UINT32    lo;
  UINT32    hi;
};

struct cb_record {
  UINT32    tag;
  UINT32    size;
};

#define CB_TAG_UNUSED  0x0000
#define CB_TAG_MEMORY  0x0001

struct cb_memory_range {
  struct cbuint64    start;
  struct cbuint64    size;
  UINT32             type;
};

#define CB_MEM_RAM          1
#define CB_MEM_RESERVED     2
#define CB_MEM_ACPI         3
#define CB_MEM_NVS          4
#define CB_MEM_UNUSABLE     5
#define CB_MEM_VENDOR_RSVD  6
#define CB_MEM_TABLE        16

struct cb_memory {
  UINT32                    tag;
  UINT32                    size;
  struct cb_memory_range    map[0];
};

/* Helpful macros */

#define MEM_RANGE_COUNT(_rec) \
  (((_rec)->size - sizeof(*(_rec))) / sizeof((_rec)->map[0]))

#define MEM_RANGE_PTR(_rec, _idx) \
  (VOID *)(((UINT8 *) (_rec)) + sizeof(*(_rec)) \
    + (sizeof((_rec)->map[0]) * (_idx)))

typedef struct cb_memory CB_MEMORY;

#pragma pack(1)
typedef struct {
  UINT64    Base;
  UINT64    Size;
  UINT8     Type;
  UINT8     Flag;
  UINT8     Reserved[6];
} MEMORY_MAP_ENTRY;

typedef struct {
  UINT8               Revision;
  UINT8               Reserved0[3];
  UINT32              Count;
  MEMORY_MAP_ENTRY    Entry[0];
} MEMORY_MAP_INFO;
#pragma pack()

struct cb_header {
  UINT32    signature;
  UINT32    header_bytes;
  UINT32    header_checksum;
  UINT32    table_bytes;
  UINT32    table_checksum;
  UINT32    table_entries;
};

struct cb_framebuffer {
  UINT32    tag;
  UINT32    size;

  UINT64    physical_address;
  UINT32    x_resolution;
  UINT32    y_resolution;
  UINT32    bytes_per_line;
  UINT8     bits_per_pixel;
  UINT8     red_mask_pos;
  UINT8     red_mask_size;
  UINT8     green_mask_pos;
  UINT8     green_mask_size;
  UINT8     blue_mask_pos;
  UINT8     blue_mask_size;
  UINT8     reserved_mask_pos;
  UINT8     reserved_mask_size;
};

struct cb_forward {
  UINT32    tag;
  UINT32    size;
  UINT64    forward;
};

struct cb_serial {
  UINT32    tag;
  UINT32    size;
  #define CB_SERIAL_TYPE_IO_MAPPED      1
  #define CB_SERIAL_TYPE_MEMORY_MAPPED  2
  UINT32    type;
  UINT32    baseaddr;
  UINT32    baud;
  UINT32    regwidth;

  // Crystal or input frequency to the chip containing the UART.
  // Provide the board specific details to allow the payload to
  // initialize the chip containing the UART and make independent
  // decisions as to which dividers to select and their values
  // to eventually arrive at the desired console baud-rate.
  UINT32    input_hertz;

  // UART PCI address: bus, device, function
  // 1 << 31 - Valid bit, PCI UART in use
  // Bus << 20
  // Device << 15
  // Function << 12
  UINT32    uart_pci_addr;
};

struct imd_entry {
  UINT32    magic;
  UINT32    start_offset;
  UINT32    size;
  UINT32    id;
};

struct imd_root {
  UINT32              max_entries;
  UINT32              num_entries;
  UINT32              flags;
  UINT32              entry_align;
  UINT32              max_offset;
  struct imd_entry    entries[0];
};

//
// Cooreboot Tag
//
#define CB_TAG_SERIAL       0x000f
#define CB_TAG_FORWARD      0x0011
#define CB_TAG_FRAMEBUFFER  0x0012

/**
  Returns a 16-bit signature built from 2 ASCII characters.

  This macro returns a 16-bit value built from the two ASCII characters specified
  by A and B.

  @param  A    The first ASCII character.
  @param  B    The second ASCII character.

  @return A 16-bit value built from the two ASCII characters specified by A and B.

**/
#define SIGNATURE_16(A, B)        ((A) | (B << 8))

/**
  Returns a 32-bit signature built from 4 ASCII characters.

  This macro returns a 32-bit value built from the four ASCII characters specified
  by A, B, C, and D.

  @param  A    The first ASCII character.
  @param  B    The second ASCII character.
  @param  C    The third ASCII character.
  @param  D    The fourth ASCII character.

  @return A 32-bit value built from the two ASCII characters specified by A, B,
          C and D.

**/
#define SIGNATURE_32(A, B, C, D)  (SIGNATURE_16 (A, B) | (SIGNATURE_16 (C, D) << 16))

#define CB_HEADER_SIGNATURE  0x4F49424C
#define IMD_ENTRY_MAGIC      (~0xC0389481)
#define CBMEM_ENTRY_MAGIC    (~0xC0389479)

#endif
