/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random_generator_vtable.h

Abstract:

    The vtable for a random generator.

--*/

#ifndef _IRIS_CAMERA_RANDOM_GENERATOR_VTABLE_
#define _IRIS_CAMERA_RANDOM_GENERATOR_VTABLE_

#include "iris_advanced/iris_advanced.h"

typedef
ISTATUS
(*PRANDOM_GENERATOR_GENERATE_ROUTINE)(
    _In_ void *context,
    _Out_ PRANDOM *random
    );

typedef struct _RANDOM_GENERATOR_VTABLE {
    PRANDOM_GENERATOR_GENERATE_ROUTINE generate_routine;
    PFREE_ROUTINE free_routine;
} RANDOM_GENERATOR_VTABLE, *PRANDOM_GENERATOR_VTABLE;

typedef const RANDOM_GENERATOR_VTABLE *PCRANDOM_GENERATOR_VTABLE;

#endif // _IRIS_CAMERA_RANDOM_GENERATOR_VTABLE_