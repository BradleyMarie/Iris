/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    matrix.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ matrix type.

--*/

#include <irisplusplus.h>

namespace Iris {

Matrix::Matrix(
    _In_ FLOAT M00,
    _In_ FLOAT M02,
    _In_ FLOAT M03,
    _In_ FLOAT M10,
    _In_ FLOAT M11,
    _In_ FLOAT M12,
    _In_ FLOAT M01,
    _In_ FLOAT M13,
    _In_ FLOAT M20,
    _In_ FLOAT M21,
    _In_ FLOAT M22,
    _In_ FLOAT M23,
    _In_ FLOAT M30,
    _In_ FLOAT M31,
    _In_ FLOAT M32,
    _In_ FLOAT M33
    )
{
    ISTATUS Status;
    
    Status = MatrixAllocate(M00,
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
                            &Data);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_ARITHMETIC_ERROR:
            throw std::invalid_argument("Not Invertible");
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("M00");
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("M01");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("M02");
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("M03");
            break;
        case ISTATUS_INVALID_ARGUMENT_04:
            throw std::invalid_argument("M10");
            break;
        case ISTATUS_INVALID_ARGUMENT_05:
            throw std::invalid_argument("M11");
            break;
        case ISTATUS_INVALID_ARGUMENT_06:
            throw std::invalid_argument("M12");
            break;
        case ISTATUS_INVALID_ARGUMENT_07:
            throw std::invalid_argument("M13");
            break;
        case ISTATUS_INVALID_ARGUMENT_08:
            throw std::invalid_argument("M20");
            break;
        case ISTATUS_INVALID_ARGUMENT_09:
            throw std::invalid_argument("M21");
            break;
        case ISTATUS_INVALID_ARGUMENT_10:
            throw std::invalid_argument("M22");
            break;
        case ISTATUS_INVALID_ARGUMENT_11:
            throw std::invalid_argument("M23");
            break;
        case ISTATUS_INVALID_ARGUMENT_12:
            throw std::invalid_argument("M30");
            break;
        case ISTATUS_INVALID_ARGUMENT_13:
            throw std::invalid_argument("M31");
            break;
        case ISTATUS_INVALID_ARGUMENT_14:
            throw std::invalid_argument("M32");
            break;
        case ISTATUS_INVALID_ARGUMENT_15:
            throw std::invalid_argument("M33");
            break;
        default:
            ASSERT(false);
    }
}

Matrix
Matrix::Translation(
    _In_ FLOAT X, 
    _In_ FLOAT Y, 
    _In_ FLOAT Z
    )
{
    PMATRIX IrisMatrix;
    ISTATUS Status;

    Status = MatrixAllocateTranslation(X, Y, Z, &IrisMatrix);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("X");
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Y");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Z");
            break;
        default:
            ASSERT(FALSE);
    }

    return Matrix(IrisMatrix);
}

Matrix
Matrix::Scalar(
    _In_ FLOAT X, 
    _In_ FLOAT Y, 
    _In_ FLOAT Z
    )
{
    PMATRIX IrisMatrix;
    ISTATUS Status;

    Status = MatrixAllocateScalar(X, Y, Z, &IrisMatrix);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("X");
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Y");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Z");
            break;
        default:
            ASSERT(FALSE);
    }

    return Matrix(IrisMatrix);
}

Matrix
Matrix::Rotation(
    _In_ FLOAT Theta, 
    _In_ FLOAT X, 
    _In_ FLOAT Y, 
    _In_ FLOAT Z
    )
{
    PMATRIX IrisMatrix;
    ISTATUS Status;

    Status = MatrixAllocateRotation(Theta, X, Y, Z, &IrisMatrix);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("Theta");
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("X");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Y");
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("Z");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_00:
            throw std::invalid_argument("All Axes Are Zero");
            break;
        default:
            ASSERT(FALSE);
    }

    return Matrix(IrisMatrix);
}

Matrix
Matrix::Orthographic(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    )
{
    PMATRIX IrisMatrix;
    ISTATUS Status;

    Status = MatrixAllocateOrthographic(Left,
                                        Right,
                                        Bottom,
                                        Top,
                                        Near,
                                        Far,
                                        &IrisMatrix);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("Left");
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Right");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Bottom");
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("Top");
            break;
        case ISTATUS_INVALID_ARGUMENT_04:
            throw std::invalid_argument("Near");
            break;
        case ISTATUS_INVALID_ARGUMENT_05:
            throw std::invalid_argument("Far");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_00:
            throw std::invalid_argument("Left == Right");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_01:
            throw std::invalid_argument("Bottom == Top");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_02:
            throw std::invalid_argument("Near == Far");
            break;
        default:
            ASSERT(FALSE);
    }

    return Matrix(IrisMatrix);
}

Matrix
Matrix::Frustum(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    )
{
    PMATRIX IrisMatrix;
    ISTATUS Status;

    Status = MatrixAllocateFrustum(Left,
                                   Right,
                                   Top,
                                   Bottom,
                                   Near,
                                   Far,
                                   &IrisMatrix);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("Left");
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Right");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Bottom");
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("Top");
            break;
        case ISTATUS_INVALID_ARGUMENT_04:
            throw std::invalid_argument("Near");
            break;
        case ISTATUS_INVALID_ARGUMENT_05:
            throw std::invalid_argument("Far");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_00:
            throw std::invalid_argument("Left == Right");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_01:
            throw std::invalid_argument("Bottom == Top");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_02:
            throw std::invalid_argument("Near == Far");
            break;
        default:
            ASSERT(FALSE);
    }

    return Matrix(IrisMatrix);
}

void
Matrix::ReadContents(
    _Out_writes_(4) FLOAT Contents[4][4]
    )
{
    if (Contents == nullptr)
    {
        throw std::invalid_argument("Contents");
    }

    if (Data == nullptr)
    {
        Contents[0][0] = (FLOAT) 1.0;
        Contents[0][1] = (FLOAT) 0.0;
        Contents[0][2] = (FLOAT) 0.0;
        Contents[0][3] = (FLOAT) 0.0;
        Contents[1][0] = (FLOAT) 0.0;
        Contents[1][1] = (FLOAT) 1.0;
        Contents[1][2] = (FLOAT) 0.0;
        Contents[1][3] = (FLOAT) 0.0;
        Contents[2][0] = (FLOAT) 0.0;
        Contents[2][1] = (FLOAT) 0.0;
        Contents[2][2] = (FLOAT) 1.0;
        Contents[2][3] = (FLOAT) 0.0;
        Contents[3][0] = (FLOAT) 0.0;
        Contents[3][1] = (FLOAT) 0.0;
        Contents[3][2] = (FLOAT) 0.0;
        Contents[3][3] = (FLOAT) 1.0;
    }
    else
    {
        MatrixReadContents(Data, Contents);
    }
} 

Matrix & 
Matrix::operator=(
    _In_ const Matrix & ToCopy
    )
{
    if (this != &ToCopy)
    {
        MatrixDereference(Data);
        Data = ToCopy.Data;
        MatrixReference(Data);
    }

    return *this;
}

Matrix
operator*(
    _In_ const Matrix & Multiplicand0,
    _In_ const Matrix & Multiplicand1
    )
{
    PMATRIX IrisMatrix;
    ISTATUS Status;

    if (Multiplicand0.Data == nullptr && 
        Multiplicand1.Data == nullptr)
    {
        return Matrix::Identity();
    }

    Status = MatrixAllocateProduct(Multiplicand0.Data,
                                   Multiplicand1.Data,
                                   &IrisMatrix);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_ARITHMETIC_ERROR:
            throw std::invalid_argument("Product Not Invertible");
            break;
    }

    return Matrix(IrisMatrix);
}

} // namespace Iris