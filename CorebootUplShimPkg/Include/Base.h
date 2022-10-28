/** @file

  Copyright (c) 2010, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2017 - 2022, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __SH_BASE_H__
#define __SH_BASE_H__

///
/// Datum is passed to the function.
///
#define IN

///
/// Datum is returned from the function.
///
#define OUT

///
/// Passing the datum to the function is optional, and a NULL
/// is passed if the value is not supplied.
///
#define OPTIONAL

#define STATIC  static
#define CONST   const
#define VOID    void
#define NULL    ((VOID *) 0)
#define TRUE    ((unsigned char)(1==1))
#define FALSE   ((unsigned char)(0==1))

#define ASCII_RSIZE_MAX 1000000

typedef unsigned long long UINT64;
typedef long long INT64;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned short UINT16;
typedef unsigned short CHAR16;
typedef short INT16;
typedef unsigned char BOOLEAN;
typedef unsigned char UINT8;
typedef char CHAR8;
typedef signed char INT8;
typedef UINT32 UINTN;
typedef INT32 intn;
typedef UINT64 ADDRESS;

///
/// Function return status for EFI API.
///
typedef UINTN RETURN_STATUS;

///
/// Maximum values for common UEFI Data Types
///
#define MAX_INT8    ((INT8)0x7F)
#define MAX_UINT8   ((UINT8)0xFF)
#define MAX_INT16   ((INT16)0x7FFF)
#define MAX_UINT16  ((UINT16)0xFFFF)
#define MAX_INT32   ((INT32)0x7FFFFFFF)
#define MAX_UINT32  ((UINT32)0xFFFFFFFF)
#define MAX_INT64   ((INT64)0x7FFFFFFFFFFFFFFFULL)
#define MAX_UINT64  ((UINT64)0xFFFFFFFFFFFFFFFFULL)

typedef struct {
    UINT32    Data1;
    UINT16    Data2;
    UINT16    Data3;
    UINT8     Data4[8];
} GUID;

///
/// Describes the format and size of the data inside the HOB.
/// All HOBs must contain this generic HOB header.
///
typedef struct {
  ///
  /// Identifies the HOB data structure type.
  ///
  UINT16          HobType;
  ///
  /// The length in bytes of the HOB.
  ///
  UINT16          HobLength;
  ///
  /// This field must always be set to zero.
  ///
  unsigned int    Reserved;
} HOB_GENERIC_HEADER;

///
/// Allows writers of executable content in the HOB producer phase to
/// maintain and manage HOBs with specific GUID.
///
typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_GUID_EXTENSION.
  ///
  HOB_GENERIC_HEADER    Header;
  ///
  /// A GUID that defines the contents of this HOB.
  ///
  GUID                  Name;
  //
  // Guid specific data goes here
  //
} HOB_GUID_TYPE;

typedef unsigned int RESOURCE_TYPE;
#define RESOURCE_ATTRIBUTE_UNCACHEABLE              0x00000400
#define RESOURCE_ATTRIBUTE_UNCACHED_EXPORTED        0x00020000
#define RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE        0x00000800
#define RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE  0x00001000
#define RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE     0x00002000
#define RESOURCE_ATTRIBUTE_WRITE_PROTECTABLE        0x00200000
#define RESOURCE_ATTRIBUTE_EXECUTION_PROTECTABLE    0x00400000
#define RESOURCE_ATTRIBUTE_PERSISTABLE              0x01000000
#define RESOURCE_ATTRIBUTE_MORE_RELIABLE            0x02000000
#define RESOURCE_ATTRIBUTE_READ_PROTECTABLE         0x00100000
#define RESOURCE_ATTRIBUTE_READ_ONLY_PROTECTABLE    0x00080000
#define RESOURCE_SYSTEM_MEMORY                      0x00000000
#define RESOURCE_MEMORY_RESERVED                    0x00000005

typedef enum {
  ReservedMemoryType,
  LoaderCode,
  LoaderData,
  BootServicesCode,
  BootServicesData,
  RuntimeServicesCode,
  RuntimeServicesData,
  ConventionalMemory,
  UnusableMemory,
  ACPIReclaimMemory,
  ACPIMemoryNVS,
  MemoryMappedIO,
  MemoryMappedIOPortSpace,
  PalCode,
  PersistentMemory,
  MaxMemoryType
} MEMORY_TYPE;

typedef struct {
  ///
  /// A GUID that defines the memory allocation region's type and purpose, as well as
  /// other fields within the memory allocation HOB. This GUID is used to define the
  /// additional data within the HOB that may be present for the memory allocation HOB.
  /// Type GUID is defined in InstallProtocolInterface() in the UEFI 2.0
  /// specification.
  ///
  GUID                Name;

  ///
  /// The base address of memory allocated by this HOB. Type
  /// address is defined in AllocatePages() in the UEFI 2.0
  /// specification.
  ///
  ADDRESS    MemoryBaseAddress;

  ///
  /// The length in bytes of memory allocated by this HOB.
  ///
  UINT64                  MemoryLength;

  ///
  /// Defines the type of memory allocated by this HOB. The memory type definition
  /// follows the MEMORY_TYPE definition. Type MEMORY_TYPE is defined
  /// in AllocatePages() in the UEFI 2.0 specification.
  ///
  MEMORY_TYPE         MemoryType;

  ///
  /// Padding for Itanium processor family
  ///
  UINT8                   Reserved[4];
} HOB_MEMORY_ALLOCATION_HEADER;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_MEMORY_ALLOCATION.
  ///
  HOB_GENERIC_HEADER              Header;
  ///
  /// An instance of the HOB_MEMORY_ALLOCATION_HEADER that describes the
  /// various attributes of the logical memory allocation.
  ///
  HOB_MEMORY_ALLOCATION_HEADER    AllocDescriptor;
  //
  // Additional data pertaining to the "Name" Guid memory
  // may go here.
  //
} HOB_MEMORY_ALLOCATION;

#define MAX_BIT      0x80000000
//
// Return the maximum of two operands.
// This macro returns the maximum of two operand specified by a and b.
// Both a and b must be the same numerical types, signed or unsigned.
//
#define MAX(a, b)                       \
  (((a) > (b)) ? (a) : (b))

//
// Return the minimum of two operands.
// This macro returns the minimal of two operand specified by a and b.
// Both a and b must be the same numerical types, signed or unsigned.
//
#define MIN(a, b)                       \
  (((a) < (b)) ? (a) : (b))

///
/// Set the upper bit to indicate EFI Error.
///
#define ENCODE_ERROR(a)              ((RETURN_STATUS)(MAX_BIT | (a)))
#define ENCODE_WARNING(a)            ((RETURN_STATUS)(a))
#define RETURN_ERROR(a)              (((intn)(RETURN_STATUS)(a)) < 0)
#define RETURN_ABORTED               ENCODE_ERROR (21)
#define RETURN_UNSUPPORTED           ENCODE_ERROR (3)
#define RETURN_NOT_FOUND             ENCODE_ERROR (14)
#define RETURN_SUCCESS               0
#define RETURN_INVALID_PARAMETER     ENCODE_ERROR (2)
#define SUCCESS                      RETURN_SUCCESS
#define ABORTED                      RETURN_ABORTED
#define UNSUPPORTED                  RETURN_UNSUPPORTED
#define NOT_FOUND                    RETURN_NOT_FOUND
#define INVALID_PARAMETER            RETURN_INVALID_PARAMETER 

#define ERROR(a)                     RETURN_ERROR(a)

#define  BIT0     0x00000001
#define  BIT1     0x00000002
#define  BIT2     0x00000004
#define  BIT3     0x00000008
#define  BIT4     0x00000010
#define  BIT5     0x00000020
#define  BIT6     0x00000040
#define  BIT7     0x00000080
#define  BIT8     0x00000100
#define  BIT9     0x00000200
#define  BIT10    0x00000400
#define  BIT11    0x00000800
#define  BIT12    0x00001000
#define  BIT13    0x00002000
#define  BIT14    0x00004000
#define  BIT15    0x00008000
#define  BIT16    0x00010000
#define  BIT17    0x00020000
#define  BIT18    0x00040000
#define  BIT19    0x00080000
#define  BIT20    0x00100000
#define  BIT21    0x00200000
#define  BIT22    0x00400000
#define  BIT23    0x00800000
#define  BIT24    0x01000000
#define  BIT25    0x02000000
#define  BIT26    0x04000000
#define  BIT27    0x08000000
#define  BIT28    0x10000000
#define  BIT29    0x20000000
#define  BIT30    0x40000000
#define  BIT31    0x80000000
#define  BIT32    0x0000000100000000ULL
#define  BIT33    0x0000000200000000ULL
#define  BIT34    0x0000000400000000ULL
#define  BIT35    0x0000000800000000ULL
#define  BIT36    0x0000001000000000ULL
#define  BIT37    0x0000002000000000ULL
#define  BIT38    0x0000004000000000ULL
#define  BIT39    0x0000008000000000ULL
#define  BIT40    0x0000010000000000ULL
#define  BIT41    0x0000020000000000ULL
#define  BIT42    0x0000040000000000ULL
#define  BIT43    0x0000080000000000ULL
#define  BIT44    0x0000100000000000ULL
#define  BIT45    0x0000200000000000ULL
#define  BIT46    0x0000400000000000ULL
#define  BIT47    0x0000800000000000ULL
#define  BIT48    0x0001000000000000ULL
#define  BIT49    0x0002000000000000ULL
#define  BIT50    0x0004000000000000ULL
#define  BIT51    0x0008000000000000ULL
#define  BIT52    0x0010000000000000ULL
#define  BIT53    0x0020000000000000ULL
#define  BIT54    0x0040000000000000ULL
#define  BIT55    0x0080000000000000ULL
#define  BIT56    0x0100000000000000ULL
#define  BIT57    0x0200000000000000ULL
#define  BIT58    0x0400000000000000ULL
#define  BIT59    0x0800000000000000ULL
#define  BIT60    0x1000000000000000ULL
#define  BIT61    0x2000000000000000ULL
#define  BIT62    0x4000000000000000ULL
#define  BIT63    0x8000000000000000ULL

//
// The EFI memory allocation functions work in units of PAGEs that are
// 4KB. This should in no way be confused with the page size of the processor.
// An PAGE is just the quanta of memory in EFI.
//
#define SIZE_4KB                      0x00001000
#define SIZE_64KB                     0x00010000
#define SIZE_512KB                    0x00080000
#define SIZE_1MB                      0x00100000
#define PAGE_SIZE                     SIZE_4KB
#define PAGE_MASK                     0xFFF
#define PAGE_SHIFT                    12
#define HOB_TYPE_END_OF_HOB_LIST      0xFFFF
#define HOB_TYPE_UNUSED               0xFFFE
#define HOB_TYPE_MEMORY_ALLOCATION    0x0002
#define HOB_TYPE_RESOURCE_DESCRIPTOR  0x0003
#define HOB_TYPE_GUID_EXTENSION       0x0004
/**
  Macro that converts a size, in bytes, to a number of PAGESs.

  @param  Size      A size in bytes.  This parameter is assumed to be type UINTN.
                    Passing in a parameter that is larger than UINTN may produce
                    unexpected results.

  @return  The number of PAGESs associated with the number of bytes specified
           by Size.

**/
#define SIZE_TO_PAGES(Size)  (((Size) >> PAGE_SHIFT) + (((Size) & PAGE_MASK) ? 1 : 0))

/**
  Macro that converts a number of PAGEs to a size in bytes.

  @param  Pages     The number of PAGES.  This parameter is assumed to be
                    type UINTN.  Passing in a parameter that is larger than
                    UINTN may produce unexpected results.

  @return  The number of bytes associated with the number of PAGEs specified
           by Pages.

**/
#define PAGES_TO_SIZE(Pages)  ((Pages) << PAGE_SHIFT)

/**
  Rounds a value up to the next boundary using a specified alignment.

  This function rounds Value up to the next boundary using the specified Alignment.
  This aligned value is returned.

  @param   Value      The value to round up.
  @param   Alignment  The alignment boundary used to return the aligned value.

  @return  A value up to the next boundary.

**/
#define ALIGN_VALUE(Value, Alignment) ((Value) + (((Alignment) - (Value)) & ((Alignment) - 1)))

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_MEMORY_ALLOCATION.
  ///
  HOB_GENERIC_HEADER              Header;
  ///
  /// An instance of the HOB_MEMORY_ALLOCATION_HEADER that describes the
  /// various attributes of the logical memory allocation.
  ///
  HOB_MEMORY_ALLOCATION_HEADER    AllocDescriptor;
} HOB_MEMORY_ALLOCATION_BSP_STORE;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_MEMORY_ALLOCATION.
  ///
  HOB_GENERIC_HEADER              Header;
  ///
  /// An instance of the HOB_MEMORY_ALLOCATION_HEADER that describes the
  /// various attributes of the logical memory allocation.
  ///
  HOB_MEMORY_ALLOCATION_HEADER    AllocDescriptor;
} HOB_MEMORY_ALLOCATION_STACK;

typedef unsigned int BOOT_MODE;

typedef unsigned int RESOURCE_ATTRIBUTE_TYPE;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_HANDOFF.
  ///
  HOB_GENERIC_HEADER    Header;
  ///
  /// The version number pertaining to the PHIT HOB definition.
  /// This value is four bytes in length to provide an 8-byte aligned entry
  /// when it is combined with the 4-byte BootMode.
  ///
  unsigned int  Version;
  ///
  /// The system boot mode as determined during the HOB producer phase.
  ///
  BOOT_MODE     BootMode;
  ///
  /// The highest address location of memory that is allocated for use by the HOB producer
  /// phase. This address must be 4-KB aligned to meet page restrictions of UEFI.
  ///
  ADDRESS       MemoryTop;
  ///
  /// The lowest address location of memory that is allocated for use by the HOB producer phase.
  ///
  ADDRESS       MemoryBottom;
  ///
  /// The highest address location of free memory that is currently available
  /// for use by the HOB producer phase.
  ///
  ADDRESS       FreeMemoryTop;
  ///
  /// The lowest address location of free memory that is available for use by the HOB producer phase.
  ///
  ADDRESS       FreeMemoryBottom;
  ///
  /// The end of the HOB list.
  ///
  ADDRESS       EndOfHobList;
} HOB_HANDOFF_INFO_TABLE;

typedef struct {
  ///
  /// Type of the memory region.
  /// Type MEMORY_TYPE is defined in the
  /// AllocatePages() function description.
  ///
  unsigned int  Type;
  ///
  /// Physical address of the first byte in the memory region. PhysicalStart must be
  /// aligned on a 4 KiB boundary, and must not be above 0xfffffffffffff000. Type
  /// address is defined in the AllocatePages() function description
  ///
  ADDRESS       PhysicalStart;
  ///
  /// Virtual address of the first byte in the memory region.
  /// VirtualStart must be aligned on a 4 KiB boundary,
  /// and must not be above 0xfffffffffffff000.
  ///
  ADDRESS       VirtualStart;
  ///
  /// NumberOfPagesNumber of 4 KiB pages in the memory region.
  /// NumberOfPages must not be 0, and must not be any value
  /// that would represent a memory page with a start address,
  /// either physical or virtual, above 0xfffffffffffff000.
  ///
  UINT64        NumberOfPages;
  ///
  /// Attributes of the memory region that describe the bit mask of capabilities
  /// for that memory region, and not necessarily the current settings for that
  /// memory region.
  ///
  UINT64        Attribute;
} MEMORY_DESCRIPTOR;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_MEMORY_ALLOCATION.
  ///
  HOB_GENERIC_HEADER              Header;
  ///
  /// An instance of the HOB_MEMORY_ALLOCATION_HEADER that describes the
  /// various attributes of the logical memory allocation.
  ///
  HOB_MEMORY_ALLOCATION_HEADER    MemoryAllocationHeader;
  ///
  /// The GUID specifying the values of the firmware file system name
  /// that contains the HOB consumer phase component.
  ///
  GUID                            ModuleName;
  ///
  /// The address of the memory-mapped firmware volume
  /// that contains the HOB consumer phase firmware file.
  ///
  ADDRESS                         EntryPoint;
} HOB_MEMORY_ALLOCATION_MODULE;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_RESOURCE_DESCRIPTOR.
  ///
  HOB_GENERIC_HEADER         Header;
  ///
  /// A GUID representing the owner of the resource. This GUID is used by HOB
  /// consumer phase components to correlate device ownership of a resource.
  ///
  GUID                       Owner;
  ///
  /// The resource type enumeration as defined by RESOURCE_TYPE.
  ///
  RESOURCE_TYPE              ResourceType;
  ///
  /// Resource attributes as defined by RESOURCE_ATTRIBUTE_TYPE.
  ///
  RESOURCE_ATTRIBUTE_TYPE    ResourceAttribute;
  ///
  /// The physical start address of the resource region.
  ///
  ADDRESS                    PhysicalStart;
  ///
  /// The number of bytes of the resource region.
  ///
  UINT64                     ResourceLength;
} HOB_RESOURCE_DESCRIPTOR;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_FV.
  ///
  HOB_GENERIC_HEADER    Header;
  ///
  /// The physical memory-mapped base address of the firmware volume.
  ///
  ADDRESS               BaseAddress;
  ///
  /// The length in bytes of the firmware volume.
  ///
  UINT64                Length;
} HOB_FIRMWARE_VOLUME;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_FV2.
  ///
  HOB_GENERIC_HEADER    Header;
  ///
  /// The physical memory-mapped base address of the firmware volume.
  ///
  ADDRESS               BaseAddress;
  ///
  /// The length in bytes of the firmware volume.
  ///
  UINT64                Length;
  ///
  /// The name of the firmware volume.
  ///
  GUID                  FvName;
  ///
  /// The name of the firmware file that contained this firmware volume.
  ///
  GUID                  FileName;
} HOB_FIRMWARE_VOLUME2;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_FV3.
  ///
  HOB_GENERIC_HEADER    Header;
  ///
  /// The physical memory-mapped base address of the firmware volume.
  ///
  ADDRESS               BaseAddress;
  ///
  /// The length in bytes of the firmware volume.
  ///
  UINT64                Length;
  ///
  /// The authentication status.
  ///
  unsigned int          AuthenticationStatus;
  ///
  /// TRUE if the FV was extracted as a file within another firmware volume.
  /// FALSE otherwise.
  ///
  unsigned char         ExtractedFv;
  ///
  /// The name of the firmware volume.
  /// Valid only if IsExtractedFv is TRUE.
  ///
  GUID                  FvName;
  ///
  /// The name of the firmware file that contained this firmware volume.
  /// Valid only if IsExtractedFv is TRUE.
  ///
  GUID                  FileName;
} HOB_FIRMWARE_VOLUME3;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_CPU.
  ///
  HOB_GENERIC_HEADER    Header;
  ///
  /// Identifies the maximum physical memory addressability of the processor.
  ///
  UINT8                 SizeOfMemorySpace;
  ///
  /// Identifies the maximum physical I/O addressability of the processor.
  ///
  UINT8                 SizeOfIoSpace;
  ///
  /// This field will always be set to zero.
  ///
  UINT8                 Reserved[6];
} HOB_CPU;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = HOB_TYPE_MEMORY_POOL.
  ///
  HOB_GENERIC_HEADER    Header;
} HOB_MEMORY_POOL;

typedef struct {
  ///
  /// The HOB generic header where Header.HobType = HOB_TYPE_UCAPSULE.
  ///
  HOB_GENERIC_HEADER    Header;

  ///
  /// The physical memory-mapped base address of an UEFI capsule. This value is set to
  /// point to the base of the contiguous memory of the UEFI capsule.
  /// The length of the contiguous memory in bytes.
  ///
  ADDRESS               BaseAddress;
  UINT64                Length;
} HOB_UCAPSULE;

typedef union {
  HOB_GENERIC_HEADER                 *Header;
  HOB_HANDOFF_INFO_TABLE             *HandoffInformationTable;
  HOB_MEMORY_ALLOCATION              *MemoryAllocation;
  HOB_MEMORY_ALLOCATION_BSP_STORE    *MemoryAllocationBspStore;
  HOB_MEMORY_ALLOCATION_STACK        *MemoryAllocationStack;
  HOB_MEMORY_ALLOCATION_MODULE       *MemoryAllocationModule;
  HOB_RESOURCE_DESCRIPTOR            *ResourceDescriptor;
  HOB_GUID_TYPE                      *Guid;
  HOB_FIRMWARE_VOLUME                *FirmwareVolume;
  HOB_FIRMWARE_VOLUME2               *FirmwareVolume2;
  HOB_FIRMWARE_VOLUME3               *FirmwareVolume3;
  HOB_CPU                            *Cpu;
  HOB_MEMORY_POOL                    *Pool;
  HOB_UCAPSULE                       *Capsule;
  UINT8                              *Raw;
} HOB_POINTERS;
typedef
VOID
( *SWITCH_STACK_ENTRY_POINT)(
   VOID                      *Context1   ,
   VOID                      *Context2
  );

VOID
InternalSwitchStack (
     SWITCH_STACK_ENTRY_POINT  EntryPoint,
     VOID                      *Context1    ,
     VOID                      *Context2    ,
     VOID                      *NewStack
  );

typedef
intn
( *BASE_SORT_COMPARE)(
    CONST VOID                 *Buffer1,
    CONST VOID                 *Buffer2
  );

VOID
QuickSort (
  VOID                 *BufferToSort,
    CONST UINTN              Count,
    CONST UINTN              ElementSize,
    BASE_SORT_COMPARE  CompareFunction,
  VOID                    *BufferOneElement
  );

#define GET_GUID_HOB_DATA(HobStart) \
  (VOID *)(*(UINT8 **)&(HobStart) + sizeof (HOB_GUID_TYPE))

#define END_OF_HOB_LIST(HobStart)  (GET_HOB_TYPE (HobStart) == (UINT16)HOB_TYPE_END_OF_HOB_LIST)

#define GET_HOB_TYPE(HobStart) \
  ((*(HOB_GENERIC_HEADER **)&(HobStart))->HobType)

#define GET_HOB_LENGTH(HobStart) \
  ((*(HOB_GENERIC_HEADER **)&(HobStart))->HobLength)

#define GET_NEXT_HOB(HobStart) \
  (VOID *)(*(UINT8 **)&(HobStart) + GET_HOB_LENGTH (HobStart))

//
// Value of ResourceType in HOB_RESOURCE_DESCRIPTOR.
//
#define RESOURCE_SYSTEM_MEMORY          0x00000000
#define RESOURCE_MEMORY_MAPPED_IO       0x00000001
#define RESOURCE_IO                     0x00000002
#define RESOURCE_FIRMWARE_DEVICE        0x00000003
#define RESOURCE_MEMORY_MAPPED_IO_PORT  0x00000004
#define RESOURCE_MEMORY_RESERVED        0x00000005
#define RESOURCE_IO_RESERVED            0x00000006
#define RESOURCE_MAX_MEMORY_TYPE        0x00000007

//
// These types can be ORed together as needed.
//
// The following attributes are used to describe settings
//
#define RESOURCE_ATTRIBUTE_PRESENT                  0x00000001
#define RESOURCE_ATTRIBUTE_INITIALIZED              0x00000002
#define RESOURCE_ATTRIBUTE_TESTED                   0x00000004
#define RESOURCE_ATTRIBUTE_READ_PROTECTED           0x00000080

/**
  The macro that returns the byte offset of a field in a data structure.

  This function returns the offset, in bytes, of field specified by Field from the
  beginning of the  data structure specified by TYPE. If TYPE does not contain Field,
  the module will not compile.

  @param   TYPE     The name of the data structure that contains the field specified by Field.
  @param   Field    The name of the field in the data structure.

  @return  Offset, in bytes, of field.

**/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
#define OFFSET_OF(TYPE, Field) ((UINTN) __builtin_offsetof(TYPE, Field))
#endif

#ifndef OFFSET_OF
#define OFFSET_OF(TYPE, Field) ((UINTN) &(((TYPE *)0)->Field))
#endif

#define DBG_PORT_PRINT(Value) __asm__ __volatile__ ("outb %b0,%w1" : : "a" (Value), "d" ((UINT16)0x80))
#define DBG_PORT_PRINT_ADDR(Value)({DBG_PORT_PRINT (0xad); \
                                    DBG_PORT_PRINT ((Value & 0xFF)); \
                                    DBG_PORT_PRINT (0xad); \
                                    DBG_PORT_PRINT ((Value & 0xFF00) >> 8); \
                                    DBG_PORT_PRINT (0xad); \
                                    DBG_PORT_PRINT ((Value & 0xFF0000) >> 16); \
                                    DBG_PORT_PRINT (0xad); \
                                    DBG_PORT_PRINT ((Value & 0xFF000000) >> 24);})

#endif // __SH_BASE_H__