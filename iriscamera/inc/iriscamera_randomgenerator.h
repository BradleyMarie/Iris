/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_randomgenerator.h

Abstract:

    This file contains the definitions for the RANDOM_GENERATOR type.

--*/

#ifndef _RANDOM_GENERATOR_IRIS_CAMERA_
#define _RANDOM_GENERATOR_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PRANDOM_GENERATOR_ALLOCATE_RANDOM_ROUTINE)(
    _In_ PCVOID Context,
    _Out_ PRANDOM *Rng
    );

typedef struct _RANDOM_GENERATOR_VTABLE {
    PRANDOM_GENERATOR_ALLOCATE_RANDOM_ROUTINE AllocateRandomRoutine;
    PFREE_ROUTINE FreeRoutine;
} RANDOM_GENERATOR_VTABLE, *PRANDOM_GENERATOR_VTABLE;

typedef CONST RANDOM_GENERATOR_VTABLE *PCRANDOM_GENERATOR_VTABLE;

typedef struct _RANDOM_GENERATOR RANDOM_GENERATOR, *PRANDOM_GENERATOR;
typedef CONST RANDOM_GENERATOR *PCRANDOM_GENERATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
RandomGeneratorAllocate(
    _In_ PCRANDOM_GENERATOR_VTABLE RandomGeneratorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM_GENERATOR *RngGenerator
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
RandomGeneratorAllocateRandom(
    _In_ PCRANDOM_GENERATOR RngGenerator,
    _Out_ PRANDOM *Rng 
    );

IRISCAMERAAPI
VOID
RandomGeneratorFree(
    _In_opt_ _Post_invalid_ PRANDOM_GENERATOR RngGenerator
    );

#endif // _RANDOM_GENERATOR_IRIS_CAMERA_