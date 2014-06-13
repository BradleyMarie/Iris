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

typedef struct _MATRIX MATRIX, *PMATRIX;
typedef CONST MATRIX *PCMATRIX;

//
// Function definitions
//

_Check_return_
_Ret_opt_
IRISAPI
PMATRIX
MatrixAllocate(
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

_Check_return_
_Ret_opt_
IRISAPI
PMATRIX
MatrixAllocateTranslation(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    );

_Check_return_
_Ret_opt_
IRISAPI
PMATRIX
MatrixAllocateScalar(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    );

_Check_return_
_Ret_opt_
IRISAPI
PMATRIX
MatrixAllocateRotation(
    _In_ FLOAT Theta,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    );

_Check_return_
_Ret_opt_
IRISAPI
PMATRIX
MatrixAllocateFrustum(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    );

_Check_return_
_Ret_opt_
IRISAPI
PMATRIX
MatrixAllocateOrothographic(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    );

_Check_return_
_When_(Multiplicand0 == NULL && Multiplicand1 == NULL, _Ret_null_)
_When_(Multiplicand0 != NULL || Multiplicand1 != NULL, _Ret_opt_)
IRISAPI
PMATRIX
MatrixAllocateProduct(
    _In_opt_ PMATRIX Multiplicand0,
    _In_opt_ PMATRIX Multiplicand1
    );

_Check_return_
_Ret_opt_
IRISAPI
PMATRIX
MatrixAllocateInverse(
    _In_opt_ PMATRIX Matrix
    );

IRISAPI
ISTATUS
MatrixReadContents(
    _In_ PCMATRIX Matrix,
    _Out_writes_(4) FLOAT Contents[4][4]
    );

IRISAPI
VOID
MatrixReference(
    _In_opt_ PMATRIX Matrix
    );

IRISAPI
VOID
MatrixDereference(
    _In_opt_ _Post_invalid_ PMATRIX Matrix
    );

#endif // _MATRIX_IRIS_