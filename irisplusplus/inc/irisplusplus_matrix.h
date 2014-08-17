/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_matrix.h

Abstract:

    This file contains the definitions for the 
    Iris++ matrix type.

--*/

#include <irisplusplus.h>

#ifndef _MATRIX_IRIS_PLUS_PLUS_
#define _MATRIX_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class Matrix
{
private: 
    Matrix(
        _In_ PMATRIX IrisMatrix
        )
        : Data(IrisMatrix)
    { }

public:
    Matrix(
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
        PMATRIX IrisMatrix;
        
        IrisMatrix = MatrixAllocate(M00,
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
                                    M33);

        if (IrisMatrix == NULL)
        {
            throw std::bad_alloc();   
        }

        Data = IrisMatrix;
    }

    static
    Matrix
    Translation(
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        )
    {
        PMATRIX IrisMatrix;

        IrisMatrix = MatrixAllocateTranslation(X, Y, Z);

        if (IrisMatrix == NULL)
        {
            throw std::bad_alloc();
        }

        return Matrix(IrisMatrix);
    }

    static
    Matrix
    Scalar(
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        )
    {
        PMATRIX IrisMatrix;

        IrisMatrix = MatrixAllocateScalar(X, Y, Z);

        if (IrisMatrix == NULL)
        {
            throw std::bad_alloc();
        }

        return Matrix(IrisMatrix);
    }

    static
    Matrix
    Rotation(
        _In_ FLOAT Theta, 
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        )
    {
        PMATRIX IrisMatrix;

        IrisMatrix = MatrixAllocateRotation(Theta, X, Y, Z);

        if (IrisMatrix == NULL)
        {
            throw std::bad_alloc();
        }

        return Matrix(IrisMatrix);
    }

    static
    Matrix
    Frustum(
        _In_ FLOAT Left,
        _In_ FLOAT Right,
        _In_ FLOAT Bottom,
        _In_ FLOAT Top,
        _In_ FLOAT Near,
        _In_ FLOAT Far
        )
    {
        PMATRIX IrisMatrix;

        IrisMatrix = MatrixAllocateFrustum(Left,
                                           Right,
                                           Top,
                                           Bottom,
                                           Near,
                                           Far);

        if (IrisMatrix == NULL)
        {
            throw std::bad_alloc();
        }

        return Matrix(IrisMatrix);
    }

    static
    Matrix
    Orthographic(
        _In_ FLOAT Left,
        _In_ FLOAT Right,
        _In_ FLOAT Bottom,
        _In_ FLOAT Top,
        _In_ FLOAT Near,
        _In_ FLOAT Far
        )
    {
        PMATRIX IrisMatrix;

        IrisMatrix = MatrixAllocateOrthographic(Left,
                                                Right,
                                                Top,
                                                Bottom,
                                                Near,
                                                Far);

        if (IrisMatrix == NULL)
        {
            throw std::bad_alloc();
        }

        return Matrix(IrisMatrix);
    }

    static 
    Matrix
    Identity(
        VOID
        )
    {
        return Matrix(NULL);
    }

    PMATRIX
    AsPMATRIX(
        VOID
        )
    {
        return Data;
    }

    PCMATRIX
    AsPCMATRIX(
        VOID
        ) const
    {
        return Data;
    }

    Matrix
    Inverse(
        VOID
        )
    {
        PMATRIX IrisMatrix;

        if (Data == NULL)
        {
            return Identity();
        }

        IrisMatrix = MatrixAllocateInverse(Data);

        if (IrisMatrix == NULL)
        {
            throw std::bad_alloc();
        }

        return Matrix(IrisMatrix);
    }

    VOID
    ReadContents(
        _Out_writes_(4) FLOAT Contents[4][4]
        )
    {
        if (Contents == NULL)
        {
            throw std::invalid_argument("Contents");
        }

        if (Data == NULL)
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

    Matrix(
        const Matrix & ToCopy
        )
        : Data(ToCopy.Data)
    {
        MatrixReference(Data);
    }    

    Matrix & 
    operator=(
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

    ~Matrix()
    {
        MatrixDereference(Data);
    }

    friend 
    Matrix 
    operator*(
        _In_ const Matrix & Multiplicand0, 
        _In_ const Matrix & Multiplicand1
        );

private:
    PMATRIX Data;
};

static
inline
Matrix
operator*(
    _In_ const Matrix & Multiplicand0,
    _In_ const Matrix & Multiplicand1
    )
{
    PMATRIX IrisMatrix;

    if (Multiplicand0.Data == NULL && 
        Multiplicand1.Data == NULL)
    {
        return Matrix::Identity();
    }

    IrisMatrix = MatrixAllocateProduct(Multiplicand0.Data,
                                       Multiplicand1.Data);

    if (IrisMatrix == NULL)
    {
        throw std::bad_alloc();
    }

    return Matrix(IrisMatrix);
}

} // namespace Iris

#endif // _MATRIX_IRIS_PLUS_PLUS_