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

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitialize(
    _Out_ PINVERTIBLE_MATRIX Matrix,
    _In_ FLOAT M00,
    _In_ FLOAT M01,
    _In_ FLOAT M02,
    _In_ FLOAT M03,
    _In_ FLOAT M10,
    _In_ FLOAT M11,
    _In_ FLOAT M12,
    _In_ FLOAT M13,
    _In_ FLOAT M20,
    _In_ FLOAT M21,
    _In_ FLOAT M22,
    _In_ FLOAT M23,
    _In_ FLOAT M30,
    _In_ FLOAT M31,
    _In_ FLOAT M32,
    _In_ FLOAT M33
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeIdentity(
    _Out_ PINVERTIBLE_MATRIX Matrix
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeTranslation(
    _Out_ PINVERTIBLE_MATRIX Matrix,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeScalar(
    _Out_ PINVERTIBLE_MATRIX Matrix,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeRotation(
    _Out_ PINVERTIBLE_MATRIX Matrix,
    _In_ FLOAT Theta,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeFrustum(
    _Out_ PINVERTIBLE_MATRIX Matrix,
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeOrothographic(
    _Out_ PINVERTIBLE_MATRIX Matrix,
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixMultiply(
    _In_ PINVERTIBLE_MATRIX Multiplicand0,
    _In_ PINVERTIBLE_MATRIX Multiplicand1,
    _Out_ PINVERTIBLE_MATRIX Product
    );

#endif // _MATRIX_IRIS_