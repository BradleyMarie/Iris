/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    matrixreference.c

Abstract:

    This module implements the Iris matrix reference functions.

--*/

#include <irisp.h>

//
// Internal Functions
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
    )
{
    ISTATUS Status;
    
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
    ASSERT(Matrix != NULL);
    ASSERT(Inverse != NULL);

    Status = Float4x4InverseInitialize(M00,
                                       M01,
                                       M02,
                                       M03,
                                       M10,
                                       M11,
                                       M12,
                                       M13,
                                       M20,
                                       M21,
                                       M22,
                                       M23,
                                       M30,
                                       M31,
                                       M32,
                                       M33,
                                       Inverse->M);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Inverse->Inverse = Matrix;

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

    return ISTATUS_SUCCESS;
}

//
// Public Functions
//

_Ret_opt_
IRISAPI
PCMATRIX_REFERENCE
MatrixReferenceGetInverse(
    _In_opt_ PCMATRIX_REFERENCE MatrixReference
    )
{
    if (MatrixReference == NULL)
    {
        return NULL;
    }
    
    return MatrixReference->Inverse;
}

IRISAPI
ISTATUS
MatrixReferenceReadContents(
    _In_ PCMATRIX_REFERENCE MatrixReference,
    _Out_writes_(4) FLOAT Contents[4][4]
    )
{
    if (MatrixReference == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Contents == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Contents[0][0] = MatrixReference->M[0][0];
    Contents[0][1] = MatrixReference->M[0][1];
    Contents[0][2] = MatrixReference->M[0][2];
    Contents[0][3] = MatrixReference->M[0][3];
    Contents[1][0] = MatrixReference->M[1][0];
    Contents[1][1] = MatrixReference->M[1][1];
    Contents[1][2] = MatrixReference->M[1][2];
    Contents[1][3] = MatrixReference->M[1][3];
    Contents[2][0] = MatrixReference->M[2][0];
    Contents[2][1] = MatrixReference->M[2][1];
    Contents[2][2] = MatrixReference->M[2][2];
    Contents[2][3] = MatrixReference->M[2][3];
    Contents[3][0] = MatrixReference->M[3][0];
    Contents[3][1] = MatrixReference->M[3][1];
    Contents[3][2] = MatrixReference->M[3][2];
    Contents[3][3] = MatrixReference->M[3][3];

    return ISTATUS_SUCCESS;
}