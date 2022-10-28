/** @file
  Graphics mode related definitions.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
  This HOB is introduced in in PI Version 1.4.

**/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

typedef enum {
  ///
  /// A pixel is 32-bits and byte zero represents red, byte one represents green,
  /// byte two represents blue, and byte three is reserved. This is the definition
  /// for the physical frame buffer. The byte values for the red, green, and blue
  /// components represent the color intensity. This color intensity value range
  /// from a minimum intensity of 0 to maximum intensity of 255.
  ///
  PixelRedGreenBlueReserved8BitPerColor,
  ///
  /// A pixel is 32-bits and byte zero represents blue, byte one represents green,
  /// byte two represents red, and byte three is reserved. This is the definition
  /// for the physical frame buffer. The byte values for the red, green, and blue
  /// components represent the color intensity. This color intensity value range
  /// from a minimum intensity of 0 to maximum intensity of 255.
  ///
  PixelBlueGreenRedReserved8BitPerColor,
  ///
  /// The Pixel definition of the physical frame buffer.
  ///
  PixelBitMask,
  ///
  /// This mode does not support a physical frame buffer.
  ///
  PixelBltOnly,
  ///
  /// Valid GRAPHICS_PIXEL_FORMAT enum values are less than this value.
  ///
  PixelFormatMax
} GRAPHICS_PIXEL_FORMAT;

typedef struct {
  UINT32            RedMask;
  UINT32            GreenMask;
  UINT32            BlueMask;
  UINT32            ReservedMask;
} PIXEL_BITMASK;

typedef struct {
  ///
  /// The version of this data structure. A value of zero represents the
  /// GRAPHICS_OUTPUT_MODE_INFORMATION structure as defined in this specification.
  ///
  UINT32                     Version;
  ///
  /// The size of video screen in pixels in the X dimension.
  ///
  UINT32                     HorizontalResolution;
  ///
  /// The size of video screen in pixels in the Y dimension.
  ///
  UINT32                     VerticalResolution;
  ///
  /// Enumeration that defines the physical format of the pixel. A value of PixelBltOnly
  /// implies that a linear frame buffer is not available for this mode.
  ///
  GRAPHICS_PIXEL_FORMAT      PixelFormat;
  ///
  /// This bit-mask is only valid if PixelFormat is set to PixelPixelBitMask.
  /// A bit being set defines what bits are used for what purpose such as Red, Green, Blue, or Reserved.
  ///
  PIXEL_BITMASK              PixelInformation;
  ///
  /// Defines the number of pixel elements per video memory line.
  ///
  UINT32                     PixelsPerScanLine;
} GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  ADDRESS                           FrameBufferBase;
  UINT32                            FrameBufferSize;
  GRAPHICS_OUTPUT_MODE_INFORMATION  GraphicsMode;
} PEI_GRAPHICS_INFO_HOB;

typedef struct {
  UINT16                                VendorId;           ///< Ignore if the value is 0xFFFF.
  UINT16                                DeviceId;           ///< Ignore if the value is 0xFFFF.
  UINT16                                SubsystemVendorId;  ///< Ignore if the value is 0xFFFF.
  UINT16                                SubsystemId;        ///< Ignore if the value is 0xFFFF.
  UINT8                                 RevisionId;         ///< Ignore if the value is 0xFF.
  UINT8                                 BarIndex;           ///< Ignore if the value is 0xFF.
} PEI_GRAPHICS_DEVICE_INFO_HOB;

#endif // __GRAPHICS_H__