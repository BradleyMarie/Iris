/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    matrix.h

Abstract:

    The reference counted 4x4 transformation matrix used by Iris.

--*/
    
#ifndef _IRIS_MATRIX_
#define _IRIS_MATRIX_

#if __cplusplus 
#include <math.h>
#else
#include <tgmath.h>
#endif // __cplusplus

#include "common/status.h"

//
// Types
//

typedef struct _MATRIX MATRIX, *PMATRIX;
typedef const MATRIX *PCMATRIX;

//
// Function definitions
//

//IRISAPI
ISTATUS
MatrixAllocate(
    _In_ float_t m00,
    _In_ float_t m01,
    _In_ float_t m02,
    _In_ float_t m03,
    _In_ float_t m10,
    _In_ float_t m11,
    _In_ float_t m12,
    _In_ float_t m13,
    _In_ float_t m20,
    _In_ float_t m21,
    _In_ float_t m22,
    _In_ float_t m23,
    _In_ float_t m30,
    _In_ float_t m31,
    _In_ float_t m32,
    _In_ float_t m33,
    _Out_ PMATRIX *matrix
    );

//IRISAPI
ISTATUS
MatrixAllocateTranslation(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PMATRIX *matrix
    );

//IRISAPI
ISTATUS
MatrixAllocateScalar(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PMATRIX *matrix
    );

//IRISAPI
ISTATUS
MatrixAllocateRotation(
    _In_ float_t theta,
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PMATRIX *matrix
    );

//IRISAPI
ISTATUS
MatrixAllocateOrthographic(
    _In_ float_t left,
    _In_ float_t right,
    _In_ float_t bottom,
    _In_ float_t top,
    _In_ float_t near,
    _In_ float_t far,
    _Out_ PMATRIX *matrix
    );

//IRISAPI
ISTATUS
MatrixAllocateFrustum(
    _In_ float_t left,
    _In_ float_t right,
    _In_ float_t bottom,
    _In_ float_t top,
    _In_ float_t near,
    _In_ float_t far,
    _Out_ PMATRIX *matrix
    );

//IRISAPI
ISTATUS
MatrixAllocateProduct(
    _In_opt_ PMATRIX multiplicand0,
    _In_opt_ PMATRIX multiplicand1,
    _Out_ PMATRIX *product
    );

_Ret_opt_
//IRISAPI
PMATRIX
MatrixGetInverse(
    _In_opt_ PMATRIX matrix
    );

_Ret_opt_
//IRISAPI
PCMATRIX
MatrixGetConstantInverse(
    _In_opt_ PCMATRIX matrix
    );

//IRISAPI
ISTATUS
MatrixReadContents(
    _In_ PCMATRIX matrix,
    _Out_writes_(4) float_t contents[4][4]
    );

//IRISAPI
void
MatrixRetain(
    _In_opt_ PMATRIX matrix
    );

//IRISAPI
void
MatrixRelease(
    _In_opt_ _Post_invalid_ PMATRIX matrix
    );

#endif // _IRIS_MATRIX_
