/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_matrix.h

Abstract:

    This file contains the definitions for the matrix types.

--*/

#include <iris.h>
    
#ifndef _MATRIX_IRIS_
#define _MATRIX_IRIS_

//
// Types
//

typedef struct _MATRIX {
    FLOAT M[4][4];
    struct _MATRIX *Inverse;
} MATRIX, *PMATRIX;

typedef struct _INVERTIBLE_MATRIX {
    MATRIX Matrix;
    MATRIX Inverse;
} INVERTIBLE_MATRIX, *PINVERTIBLE_MATRIX;

//
// Function definitions
//

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitialize(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT M00,
    __in FLOAT M01,
    __in FLOAT M02,
    __in FLOAT M03,
    __in FLOAT M10,
    __in FLOAT M11,
    __in FLOAT M12,
    __in FLOAT M13,
    __in FLOAT M20,
    __in FLOAT M21,
    __in FLOAT M22,
    __in FLOAT M23,
    __in FLOAT M30,
    __in FLOAT M31,
    __in FLOAT M32,
    __in FLOAT M33
    );

VOID
MatrixInitializeIdentity(
    __out PINVERTIBLE_MATRIX Matrix
    );

VOID
MatrixInitializeTranslation(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    );

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeScalar(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    );

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeRotation(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Theta,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    );

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeFrustum(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Left,
    __in FLOAT Right,
    __in FLOAT Bottom,
    __in FLOAT Top,
    __in FLOAT Near,
    __in FLOAT Far
    );

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeOrothographic(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Left,
    __in FLOAT Right,
    __in FLOAT Bottom,
    __in FLOAT Top,
    __in FLOAT Near,
    __in FLOAT Far
    );

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixMultiply(
    __in PINVERTIBLE_MATRIX Multiplicand0,
    __in PINVERTIBLE_MATRIX Multiplicand1,
    __out PINVERTIBLE_MATRIX Product
    );

#endif // _MATRIX_IRIS_