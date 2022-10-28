/* LzFind.h -- Match finder for LZ algorithms
2017-06-10 : Igor Pavlov : Public domain */

#ifndef __LZ_FIND_H__
#define __LZ_FIND_H__

#include "7zTypes.h"

EXTERN_C_BEGIN

typedef UINT32 CLzRef;

typedef struct _CMatchFinder {
  Byte            *buffer;
  UINT32          pos;
  UINT32          posLimit;
  UINT32          streamPos;
  UINT32          lenLimit;

  UINT32          cyclicBufferPos;
  UINT32          cyclicBufferSize; /* it must be = (historySize + 1) */

  Byte            streamEndWasReached;
  Byte            btMode;
  Byte            bigHash;
  Byte            directInput;

  UINT32          matchMaxLen;
  CLzRef          *hash;
  CLzRef          *son;
  UINT32          hashMask;
  UINT32          cutValue;

  Byte            *bufferBase;
  ISeqInStream    *stream;

  UINT32          blockSize;
  UINT32          keepSizeBefore;
  UINT32          keepSizeAfter;

  UINT32          numHashBytes;
  size_t          directInputRem;
  UINT32          historySize;
  UINT32          fixedHashSize;
  UINT32          hashSizeSum;
  SRes            result;
  UINT32          crc[256];
  size_t          numRefs;

  UINT64          expectedDataSize;
} CMatchFinder;

#define Inline_MatchFinder_GetPointerToCurrentPos(p)  ((p)->buffer)

#define Inline_MatchFinder_GetNumAvailableBytes(p)  ((p)->streamPos - (p)->pos)

#define Inline_MatchFinder_IsFinishedOK(p) \
    ((p)->streamEndWasReached \
        && (p)->streamPos == (p)->pos \
        && (!(p)->directInput || (p)->directInputRem == 0))

int
MatchFinder_NeedMove (
  CMatchFinder  *p
  );

Byte *
MatchFinder_GetPointerToCurrentPos (
  CMatchFinder  *p
  );

VOID
MatchFinder_MoveBlock (
  CMatchFinder  *p
  );

VOID
MatchFinder_ReadIfRequired (
  CMatchFinder  *p
  );

VOID
MatchFinder_Construct (
  CMatchFinder  *p
  );

/* Conditions:
     historySize <= 3 GB
     keepAddBufferBefore + matchMaxLen + keepAddBufferAfter < 511MB
*/
int
MatchFinder_Create (
  CMatchFinder  *p,
  UINT32        historySize,
  UINT32        keepAddBufferBefore,
  UINT32        matchMaxLen,
  UINT32        keepAddBufferAfter,
  ISzAllocPtr   alloc
  );

VOID
MatchFinder_Free (
  CMatchFinder  *p,
  ISzAllocPtr   alloc
  );

VOID
MatchFinder_Normalize3 (
  UINT32  subValue,
  CLzRef  *items,
  size_t  numItems
  );

VOID
MatchFinder_ReduceOffsets (
  CMatchFinder  *p,
  UINT32        subValue
  );

UINT32 *
GetMatchesSpec1 (
  UINT32      lenLimit,
  UINT32      curMatch,
  UINT32      pos,
  CONST Byte  *buffer,
  CLzRef      *son,
  UINT32      _cyclicBufferPos,
  UINT32      _cyclicBufferSize,
  UINT32      _cutValue,
  UINT32      *distances,
  UINT32      maxLen
  );

/*
Conditions:
  Mf_GetNumAvailableBytes_Func must be called before each Mf_GetMatchLen_Func.
  Mf_GetPointerToCurrentPos_Func's result must be used only before any other function
*/

typedef VOID (*Mf_Init_Func)(
  VOID  *object
  );
typedef UINT32 (*Mf_GetNumAvailableBytes_Func)(
  VOID  *object
  );
typedef CONST Byte * (*Mf_GetPointerToCurrentPos_Func)(
  VOID  *object
  );
typedef UINT32 (*Mf_GetMatches_Func)(
  VOID    *object,
  UINT32  *distances
  );
typedef VOID (*Mf_Skip_Func)(
  VOID  *object,
  UINT32
  );

typedef struct _IMatchFinder {
  Mf_Init_Func                      Init;
  Mf_GetNumAvailableBytes_Func      GetNumAvailableBytes;
  Mf_GetPointerToCurrentPos_Func    GetPointerToCurrentPos;
  Mf_GetMatches_Func                GetMatches;
  Mf_Skip_Func                      Skip;
} IMatchFinder;

VOID
MatchFinder_CreateVTable (
  CMatchFinder  *p,
  IMatchFinder  *vTable
  );

VOID
MatchFinder_Init_LowHash (
  CMatchFinder  *p
  );

VOID
MatchFinder_Init_HighHash (
  CMatchFinder  *p
  );

VOID
MatchFinder_Init_3 (
  CMatchFinder  *p,
  int           readData
  );

VOID
MatchFinder_Init (
  CMatchFinder  *p
  );

UINT32
Bt3Zip_MatchFinder_GetMatches (
  CMatchFinder  *p,
  UINT32        *distances
  );

UINT32
Hc3Zip_MatchFinder_GetMatches (
  CMatchFinder  *p,
  UINT32        *distances
  );

VOID
Bt3Zip_MatchFinder_Skip (
  CMatchFinder  *p,
  UINT32        num
  );

VOID
Hc3Zip_MatchFinder_Skip (
  CMatchFinder  *p,
  UINT32        num
  );

EXTERN_C_END

#endif
