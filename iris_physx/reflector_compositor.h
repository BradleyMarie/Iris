/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    reflector_compositor.h

Abstract:

    Enables the composition of reflectors into new reflectors.

    The lifetimes of any reflectors allocated by a reflector compositor match
    the lifetime of the compositor and will be freed automatically when the
    compositor goes out of scope.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_COMPOSITOR_
#define _IRIS_PHYSX_REFLECTOR_COMPOSITOR_

#include "iris_physx/reflector.h"

//
// Types
//

typedef struct _REFLECTOR_COMPOSITOR REFLECTOR_COMPOSITOR, *PREFLECTOR_COMPOSITOR;
typedef const REFLECTOR_COMPOSITOR *PCREFLECTOR_COMPOSITOR;

//
// Functions
//

ISTATUS
ReflectorCompositorAttenuateReflector(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR reflector,
    _In_ float_t attenuation,
    _Out_ PCREFLECTOR *attenuated_reflector
    );

ISTATUS
ReflectorCompositorAttenuatedAddReflectors(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR added_reflector,
    _In_ PCREFLECTOR attenuated_reflector,
    _In_ float_t attenuation,
    _Out_ PCREFLECTOR *result
    );

ISTATUS
ReflectorCompositorMultiplyReflectors(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR multiplicand0,
    _In_ PCREFLECTOR multiplicand1,
    _Out_ PCREFLECTOR *product
    );

ISTATUS
ReflectorCompositorAddReflectors(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR addend0,
    _In_ PCREFLECTOR addend1,
    _Out_ PCREFLECTOR *sum
    );

#endif // _IRIS_PHYSX_REFLECTOR_COMPOSITOR_