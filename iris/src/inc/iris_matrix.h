/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_matrix.h

Abstract:

    This file contains the internal definitions for the matrix types.

--*/

#include <irisp.h>
    
#ifndef _MATRIX_IRIS_INTERNAL_
#define _MATRIX_IRIS_INTERNAL_

//
// Types
//

typedef struct _INVERTIBLE_MATRIX INVERTIBLE_MATRIX, *PINVERTIBLE_MATRIX;

typedef struct _MATRIX {
    FLOAT M[4][4];
    struct _MATRIX *Inverse;
    PINVERTIBLE_MATRIX InvertibleMatrix;
} MATRIX, *PMATRIX;

typedef CONST MATRIX *PCMATRIX;

struct _INVERTIBLE_MATRIX {
    MATRIX Matrix;
    MATRIX Inverse;
    SIZE_T ReferenceCount;
};

typedef CONST INVERTIBLE_MATRIX *PCINVERTIBLE_MATRIX;

#endif // _MATRIX_IRIS_INTERNAL_