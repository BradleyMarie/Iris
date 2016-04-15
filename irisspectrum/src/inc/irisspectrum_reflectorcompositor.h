/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrum_reflectorcompositor.h

Abstract:

    This file contains the internal definitions for the REFLECTOR_COMPOSITOR type.

--*/

#ifndef _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_INTERNAL_
#define _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_INTERNAL_

#include <irisspectrump.h>

//
// Types
//

typedef struct _ATTENUATED_REFLECTOR {
    REFLECTOR ReflectorHeader;
    PCREFLECTOR Reflector;
    FLOAT Attenuation;
} ATTENUATED_REFLECTOR, *PATTENUATED_REFLECTOR;

typedef CONST ATTENUATED_REFLECTOR *PCATTENUATED_REFLECTOR;

//
// Variables
//

extern CONST REFLECTOR_VTABLE AttenuatedReflectorVTable;

#endif // _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_INTERNAL_