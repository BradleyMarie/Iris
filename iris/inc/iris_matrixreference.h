/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iris_matrixreference.h

Abstract:

    This file contains the definitions for the matrix reference type.
    
--*/

#include <iris.h>
    
#ifndef _MATRIX_REFERENCE_IRIS_
#define _MATRIX_REFERENCE_IRIS_

//
// Types
//

typedef struct _MATRIX_REFERENCE MATRIX_REFERENCE, *PMATRIX_REFERENCE;
typedef CONST MATRIX_REFERENCE *PCMATRIX_REFERENCE;

//
// Function definitions
//

_Ret_opt_
IRISAPI
PCMATRIX_REFERENCE
MatrixReferenceGetInverse(
    _In_opt_ PCMATRIX_REFERENCE MatrixReference
    );

IRISAPI
ISTATUS
MatrixReferenceReadContents(
    _In_ PCMATRIX_REFERENCE MatrixReference,
    _Out_writes_(4) FLOAT Contents[4][4]
    );

#endif // _MATRIX_REFERENCE_IRIS_