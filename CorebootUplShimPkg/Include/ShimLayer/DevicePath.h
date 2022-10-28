/** @file
  The device path protocol as defined in UEFI 2.0.

  The device path represents a programmatic path to a device,
  from a software point of view. The path must persist from boot to boot, so
  it can not contain things like PCI bus numbers that change from boot to boot.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DEVICE_PATH_PROTOCOL_H__
#define __DEVICE_PATH_PROTOCOL_H__

//
//  EISA ID Macro
//  EISA ID Definition 32-bits
//   bits[15:0] - three character compressed ASCII EISA ID.
//   bits[31:16] - binary number
//    Compressed ASCII is 5 bits per character 0b00001 = 'A' 0b11010 = 'Z'
//
#define PNP_EISA_ID_CONST         0x41d0
#define EISA_ID(_Name, _Num)      ((UINT32)((_Name) | (_Num) << 16))
#define EISA_PNP_ID(_PNPId)       (EISA_ID(PNP_EISA_ID_CONST, (_PNPId)))
#define PNP_ID(_PNPId)            (EISA_ID(PNP_EISA_ID_CONST, (_PNPId)))

#endif // __DEVICE_PATH_PROTOCOL_H__