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

struct _MATRIX {
    MATRIX_REFERENCE MatrixReference;
    PINVERTIBLE_MATRIX InvertibleMatrix;    
};

#endif // _MATRIX_IRIS_INTERNAL_