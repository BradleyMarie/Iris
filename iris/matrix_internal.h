/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    matrix_internal.h

Abstract:

    The 4x4 transformation matrix used by Iris.

--*/
    
#ifndef _IRIS_MATRIX_INTERNAL_
#define _IRIS_MATRIX_INTERNAL_

#include <tgmath.h>

//
// Types
//

typedef struct _INVERTIBLE_MATRIX INVERTIBLE_MATRIX, *PINVERTIBLE_MATRIX;
typedef const INVERTIBLE_MATRIX *PCINVERTIBLE_MATRIX;

struct _MATRIX {
    float_t m[4][4];
    struct _MATRIX *inverse;
    PINVERTIBLE_MATRIX invertible_matrix;
};

#endif // _IRIS_MATRIX_INTERNAL_