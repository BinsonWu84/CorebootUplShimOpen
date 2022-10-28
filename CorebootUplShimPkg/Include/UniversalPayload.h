/** @file
  Universal Payload general definitions.

Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
    - Universal Payload Specification 0.75 (https://universalpayload.github.io/documentation/)
**/

#ifndef __UNIVERSAL_PAYLOAD_H__
#define __UNIVERSAL_PAYLOAD_H__

/**
  Main entry point to Universal Payload.

  @param HobList  Pointer to the beginning of the HOB List from boot loader.
**/
typedef  VOID ( *UNIVERSAL_PAYLOAD_ENTRY)(VOID *HobList);

#define UNIVERSAL_PAYLOAD_IDENTIFIER                    SIGNATURE_32('P', 'L', 'D', 'H')
#define UNIVERSAL_PAYLOAD_INFO_SEC_NAME                 ".upld_info"
#define UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX         ".upld."
#define UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX_LENGTH  (sizeof (UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX) - 1)

#pragma pack(1)

typedef struct {
  UINT32    Identifier;
  UINT32    HeaderLength;
  UINT16    SpecRevision;
  UINT8     Reserved[2];
  UINT32    Revision;
  UINT32    Attribute;
  UINT32    Capability;
  CHAR8     ProducerId[16];
  CHAR8     ImageId[16];
} UNIVERSAL_PAYLOAD_INFO_HEADER;

typedef struct {
  UINT8     Revision;
  UINT8     Reserved;
  UINT16    Length;
} UNIVERSAL_PAYLOAD_GENERIC_HEADER;

typedef struct {
  UNIVERSAL_PAYLOAD_GENERIC_HEADER    Header;
  ADDRESS                             SmBiosEntryPoint;
} UNIVERSAL_PAYLOAD_SMBIOS_TABLE;

typedef struct {
  UNIVERSAL_PAYLOAD_GENERIC_HEADER    Header;
  ADDRESS                             Rsdp;
} UNIVERSAL_PAYLOAD_ACPI_TABLE;

typedef struct {
  UNIVERSAL_PAYLOAD_GENERIC_HEADER    Header;
  BOOLEAN                             UseMmio;
  UINT8                               RegisterStride;
  UINT32                              BaudRate;
  ADDRESS                             RegisterBase;
} UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO;

typedef struct {
  //
  // Base and Limit are the device address instead of host address when
  // Translation is not zero
  //
  UINT64    Base;
  UINT64    Limit;
  //
  // According to UEFI 2.7, Device Address = Host Address + Translation,
  // so Translation = Device Address - Host Address.
  // On platforms where Translation is not zero, the subtraction is probably to
  // be performed with UINT64 wrap-around semantics, for we may translate an
  // above-4G host address into a below-4G device address for legacy PCIe device
  // compatibility.
  //
  // NOTE: The alignment of Translation is required to be larger than any BAR
  // alignment in the same root bridge, so that the same alignment can be
  // applied to both device address and host address, which simplifies the
  // situation and makes the current resource allocation code in generic PCI
  // host bridge driver still work.
  //
  UINT64    Translation;
} UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE;

///
/// Payload PCI Root Bridge Information HOB
///
typedef struct {
  UINT32                                        Segment;               ///< Segment number.
  UINT64                                        Supports;              ///< Supported attributes.
                                                                       ///< Refer to PCI_ATTRIBUTE_xxx used by GetAttributes()
                                                                       ///< and SetAttributes() in PCI_ROOT_BRIDGE_IO_PROTOCOL.
  UINT64                                        Attributes;            ///< Initial attributes.
                                                                       ///< Refer to PCI_ATTRIBUTE_xxx used by GetAttributes()
                                                                       ///< and SetAttributes() in PCI_ROOT_BRIDGE_IO_PROTOCOL.
  BOOLEAN                                       DmaAbove4G;            ///< DMA above 4GB memory.
                                                                       ///< Set to TRUE when root bridge supports DMA above 4GB memory.
  BOOLEAN                                       NoExtendedConfigSpace; ///< When FALSE, the root bridge supports
                                                                       ///< Extended (4096-byte) Configuration Space.
                                                                       ///< When TRUE, the root bridge supports
                                                                       ///< 256-byte Configuration Space only.
  UINT64                                        AllocationAttributes;  ///< Allocation attributes.
                                                                       ///< Refer to PCI_HOST_BRIDGE_COMBINE_MEM_PMEM and
                                                                       ///< PCI_HOST_BRIDGE_MEM64_DECODE used by GetAllocAttributes()
                                                                       ///< in PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL.
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE    Bus;                   ///< Bus aperture which can be used by the root bridge.
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE    Io;                    ///< IO aperture which can be used by the root bridge.
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE    Mem;                   ///< MMIO aperture below 4GB which can be used by the root bridge.
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE    MemAbove4G;            ///< MMIO aperture above 4GB which can be used by the root bridge.
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE    PMem;                  ///< Prefetchable MMIO aperture below 4GB which can be used by the root bridge.
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE    PMemAbove4G;           ///< Prefetchable MMIO aperture above 4GB which can be used by the root bridge.
  UINT32                                        HID;                   ///< PnP hardware ID of the root bridge. This value must match the corresponding
                                                                       ///< _HID in the ACPI name space.
  UINT32                                        UID;                   ///< Unique ID that is required by ACPI if two devices have the same _HID.
                                                                       ///< This value must also match the corresponding _UID/_HID pair in the ACPI name space.
} UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE;

typedef struct {
  UNIVERSAL_PAYLOAD_GENERIC_HEADER     Header;
  BOOLEAN                              ResourceAssigned;
  UINT8                                Count;
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE    RootBridge[0];
} UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES;

#define PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO          0x0001
#define PCI_ATTRIBUTE_ISA_IO                      0x0002
#define PCI_ATTRIBUTE_VGA_PALETTE_IO              0x0004
#define PCI_ATTRIBUTE_VGA_MEMORY                  0x0008
#define PCI_ATTRIBUTE_VGA_IO                      0x0010
#define PCI_ATTRIBUTE_IDE_PRIMARY_IO              0x0020
#define PCI_ATTRIBUTE_IDE_SECONDARY_IO            0x0040
#define PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE        0x0080
#define PCI_ATTRIBUTE_MEMORY_CACHED               0x0800
#define PCI_ATTRIBUTE_MEMORY_DISABLE              0x1000
#define PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE          0x8000
#define PCI_ATTRIBUTE_ISA_IO_16                   0x10000
#define PCI_ATTRIBUTE_VGA_PALETTE_IO_16           0x20000
#define PCI_ATTRIBUTE_VGA_IO_16                   0x40000

typedef struct {
  CHAR8    Identifier[16];
  ADDRESS  Base;
  UINT64   Size;
} UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY;

typedef struct {
  UNIVERSAL_PAYLOAD_GENERIC_HEADER      Header;
  UINT32                                Count;
  UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY    Entry[0];
} UNIVERSAL_PAYLOAD_EXTRA_DATA;

#pragma pack()

/**
  Returns the size of a structure of known type, up through and including a specified field.

  @param   TYPE     The name of the data structure that contains the field specified by Field.
  @param   Field    The name of the field in the data structure.

  @return  size, in bytes.

**/
#define UNIVERSAL_PAYLOAD_SIZEOF_THROUGH_FIELD(TYPE, Field)  (OFFSET_OF(TYPE, Field) + sizeof (((TYPE *) 0)->Field))

#define UNIVERSAL_PAYLOAD_SMBIOS_TABLE_REVISION      1
#define UNIVERSAL_PAYLOAD_ACPI_TABLE_REVISION        1
#define UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES_REVISION  1
#define UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION  1
#define UNIVERSAL_PAYLOAD_EXTRA_DATA_REVISION        1

extern GUID gUniversalPayloadPciRootBridgeInfoGuid;
extern GUID gUniversalPayloadSmbios3TableGuid;
extern GUID gUniversalPayloadSmbiosTableGuid;
extern GUID gUniversalPayloadAcpiTableGuid;
extern GUID gUniversalPayloadExtraDataGuid;
extern GUID gUniversalPayloadSerialPortInfoGuid;

#endif // __UNIVERSAL_PAYLOAD_H__
