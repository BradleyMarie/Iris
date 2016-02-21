/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_matrixreference.h

Abstract:

    This file contains the internal definitions for the matrix 
    reference type.

--*/

#include <irisp.h>
    
#ifndef _MATRIX_REFERENCE_IRIS_INTERNAL_
#define _MATRIX_REFERENCE_IRIS_INTERNAL_

//
// Types
//

struct _MATRIX_REFERENCE {
    FLOAT M[4][4];
    PCMATRIX_REFERENCE Inverse;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixReferencePairInitialize(
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
    _In_ FLOAT M33,
    _Out_ PMATRIX_REFERENCE Matrix,
    _Out_ PMATRIX_REFERENCE Inverse
    );

SFORCEINLINE
VOID
MatrixReferencePairInitializeFromValues(
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
    _In_ FLOAT M33,
    _In_ FLOAT I00,
    _In_ FLOAT I01,
    _In_ FLOAT I02,
    _In_ FLOAT I03,
    _In_ FLOAT I10,
    _In_ FLOAT I11,
    _In_ FLOAT I12,
    _In_ FLOAT I13,
    _In_ FLOAT I20,
    _In_ FLOAT I21,
    _In_ FLOAT I22,
    _In_ FLOAT I23,
    _In_ FLOAT I30,
    _In_ FLOAT I31,
    _In_ FLOAT I32,
    _In_ FLOAT I33,
    _Out_ PMATRIX_REFERENCE Matrix,
    _Out_ PMATRIX_REFERENCE Inverse
    )
{
    ASSERT(IsFiniteFloat(M00) != FALSE);
    ASSERT(IsFiniteFloat(M01) != FALSE);
    ASSERT(IsFiniteFloat(M02) != FALSE);
    ASSERT(IsFiniteFloat(M03) != FALSE);
    ASSERT(IsFiniteFloat(M10) != FALSE);
    ASSERT(IsFiniteFloat(M11) != FALSE);
    ASSERT(IsFiniteFloat(M12) != FALSE);
    ASSERT(IsFiniteFloat(M13) != FALSE);
    ASSERT(IsFiniteFloat(M20) != FALSE);
    ASSERT(IsFiniteFloat(M21) != FALSE);
    ASSERT(IsFiniteFloat(M22) != FALSE);
    ASSERT(IsFiniteFloat(M23) != FALSE);
    ASSERT(IsFiniteFloat(M30) != FALSE);
    ASSERT(IsFiniteFloat(M31) != FALSE);
    ASSERT(IsFiniteFloat(M32) != FALSE);
    ASSERT(IsFiniteFloat(M33) != FALSE);
    ASSERT(IsFiniteFloat(I00) != FALSE);
    ASSERT(IsFiniteFloat(I01) != FALSE);
    ASSERT(IsFiniteFloat(I02) != FALSE);
    ASSERT(IsFiniteFloat(I03) != FALSE);
    ASSERT(IsFiniteFloat(I10) != FALSE);
    ASSERT(IsFiniteFloat(I11) != FALSE);
    ASSERT(IsFiniteFloat(I12) != FALSE);
    ASSERT(IsFiniteFloat(I13) != FALSE);
    ASSERT(IsFiniteFloat(I20) != FALSE);
    ASSERT(IsFiniteFloat(I21) != FALSE);
    ASSERT(IsFiniteFloat(I22) != FALSE);
    ASSERT(IsFiniteFloat(I23) != FALSE);
    ASSERT(IsFiniteFloat(I30) != FALSE);
    ASSERT(IsFiniteFloat(I31) != FALSE);
    ASSERT(IsFiniteFloat(I32) != FALSE);
    ASSERT(IsFiniteFloat(I33) != FALSE);
    ASSERT(Inverse != NULL);
    ASSERT(Matrix != NULL);

    Matrix->M[0][0] = M00;
    Matrix->M[0][1] = M01;
    Matrix->M[0][2] = M02;
    Matrix->M[0][3] = M03;
    Matrix->M[1][0] = M10;
    Matrix->M[1][1] = M11;
    Matrix->M[1][2] = M12;
    Matrix->M[1][3] = M13;
    Matrix->M[2][0] = M20;
    Matrix->M[2][1] = M21;
    Matrix->M[2][2] = M22;
    Matrix->M[2][3] = M23;
    Matrix->M[3][0] = M30;
    Matrix->M[3][1] = M31;
    Matrix->M[3][2] = M32;
    Matrix->M[3][3] = M33;

    Matrix->Inverse = Inverse;
    
    Inverse->M[0][0] = I00;
    Inverse->M[0][1] = I01;
    Inverse->M[0][2] = I02;
    Inverse->M[0][3] = I03;
    Inverse->M[1][0] = I10;
    Inverse->M[1][1] = I11;
    Inverse->M[1][2] = I12;
    Inverse->M[1][3] = I13;
    Inverse->M[2][0] = I20;
    Inverse->M[2][1] = I21;
    Inverse->M[2][2] = I22;
    Inverse->M[2][3] = I23;
    Inverse->M[3][0] = I30;
    Inverse->M[3][1] = I31;
    Inverse->M[3][2] = I32;
    Inverse->M[3][3] = I33;

    Inverse->Inverse = Matrix;
}

#endif // _MATRIX_REFERENCE_IRIS_INTERNAL_