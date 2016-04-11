/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisplusplus_vector.h

Abstract:

    This file contains the definitions for the 
    Iris++ vector type.

--*/

#include <irisplusplus.h>

#ifndef _VECTOR_IRIS_PLUS_PLUS_
#define _VECTOR_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

enum class VectorAxis : VECTOR_AXIS {
    X = VECTOR_X_AXIS,
    Y = VECTOR_Y_AXIS,
    Z = VECTOR_Z_AXIS
};

class Vector final {
public:
    Vector(
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        )
    : Data(VectorCreate(X, Y, Z))
    { }

    Vector(
        _In_ const VECTOR3 & IrisVector
        )
    : Data(IrisVector)
    { }

    Vector 
    operator-() const
    {
        VECTOR3 Negated;

        Negated = VectorNegate(Data);

        return Vector(Negated);
    }

    VECTOR3 
    AsVECTOR3(
        void
        ) const
    {
        return Data;
    }

    _Ret_
    PCVECTOR3 
    AsPCVECTOR3(
        void
        ) const
    {
        return &Data;
    }

    FLOAT 
    Length(
        void
        ) const
    {
        return VectorLength(Data);
    }

    VectorAxis 
    DominantAxis(
        void
        )
    {
        return VectorAxis(VectorDominantAxis(Data));
    }

    VectorAxis 
    DiminishedAxis(
        void
        )
    {
        return VectorAxis(VectorDiminishedAxis(Data));
    }

    static 
    Vector 
    Normalize(
        _In_ const Vector & ToNormalize
        )
    {
        VECTOR3 Normalized;

        Normalized = VectorNormalize(ToNormalize.Data, NULL, NULL);

        return Vector(Normalized);
    }

    static 
    std::tuple<Vector, FLOAT, FLOAT> 
    NormalizeWithLength(
        _In_ const Vector & ToNormalize
        )
    {
        VECTOR3 Normalized;
        FLOAT OldLengthSquared;
        FLOAT OldLength;

        Normalized = VectorNormalize(ToNormalize.Data, &OldLengthSquared, &OldLength);

        return std::make_tuple(Vector(Normalized), OldLengthSquared, OldLength);
    }

    static 
    FLOAT 
    DotProduct(
        _In_ const Vector & Operand0, 
        _In_ const Vector & Operand1
        )
    {
        return VectorDotProduct(Operand0.Data, Operand1.Data);
    }

    static 
    Vector 
    CrossProduct(
        _In_ const Vector & Operand0, 
        _In_ const Vector & Operand1
        )
    {
        VECTOR3 Product;

        Product = VectorCrossProduct(Operand0.Data, Operand1.Data);

        return Vector(Product);
    }

    static 
    Vector 
    Fma(
        _In_ const Vector & Addend0, 
        _In_ const Vector & Addend1, 
        _In_ FLOAT Scalar
        )
    {
        VECTOR3 Sum;

        Sum = VectorAddScaled(Addend0.Data, Addend1.Data, Scalar);

        return Vector(Sum);
    }

    static 
    Vector 
    Reflect(
        _In_ const Vector & Vec, 
        _In_ const Vector & Normal
        )
    {
        VECTOR3 Reflected;

        Reflected = VectorReflect(Vec.Data, Normal.Data);

        return Vector(Reflected);
    }

    static 
    Vector 
    HalfAngle(
        _In_ const Vector & Vector0, 
        _In_ const Vector & Vector1
        )
    {
        VECTOR3 HalfAngle;

        HalfAngle = VectorHalfAngle(Vector0.Data, Vector1.Data);

        return Vector(HalfAngle);
    }

    static
    Vector
    TransposedMultiply(
        _In_ const Matrix & Multiplicand0,
        _In_ const Vector & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        VECTOR3 IrisMultiplicand1;
        VECTOR3 IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

        IrisResult = VectorMatrixTransposedMultiply(IrisMultiplicand0,
                                                    IrisMultiplicand1);

        return Vector(IrisResult);
    }

    static
    Vector
    TransposedMultiply(
        _In_ const MatrixReference & Multiplicand0,
        _In_ const Vector & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        VECTOR3 IrisMultiplicand1;
        VECTOR3 IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

        IrisResult = VectorMatrixTransposedMultiply(IrisMultiplicand0,
                                                    IrisMultiplicand1);

        return Vector(IrisResult);
    }

    static
    Vector
    InverseMultiply(
        _In_ const Matrix & Multiplicand0,
        _In_ const Vector & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        VECTOR3 IrisMultiplicand1;
        VECTOR3 IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

        IrisResult = VectorMatrixInverseMultiply(IrisMultiplicand0,
                                                 IrisMultiplicand1);

        return Vector(IrisResult);
    }

    static
    Vector
    InverseMultiply(
        _In_ const MatrixReference & Multiplicand0,
        _In_ const Vector & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        VECTOR3 IrisMultiplicand1;
        VECTOR3 IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

        IrisResult = VectorMatrixInverseMultiply(IrisMultiplicand0,
                                                 IrisMultiplicand1);

        return Vector(IrisResult);
    }

    static
    Vector
    InverseTransposedMultiply(
        _In_ const Matrix & Multiplicand0,
        _In_ const Vector & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        VECTOR3 IrisMultiplicand1;
        VECTOR3 IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

        IrisResult = VectorMatrixInverseTransposedMultiply(IrisMultiplicand0,
                                                           IrisMultiplicand1);

        return Vector(IrisResult);
    }

    static
    Vector
    InverseTransposedMultiply(
        _In_ const MatrixReference & Multiplicand0,
        _In_ const Vector & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        VECTOR3 IrisMultiplicand1;
        VECTOR3 IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

        IrisResult = VectorMatrixInverseTransposedMultiply(IrisMultiplicand0,
                                                           IrisMultiplicand1);

        return Vector(IrisResult);
    }

    FLOAT 
    X(
        void
        ) const
    {
        return Data.X;
    }

    FLOAT 
    Y(
        void
        ) const
    {
        return Data.Y;
    }

    FLOAT 
    Z(
        void
        ) const
    {
        return Data.Z;
    }

    bool 
    Validate(
        void
        ) const
    {
        BOOL Valid;

        Valid = VectorValidate(Data);

        return (Valid != FALSE) ? true : false;
    }

private:
    const VECTOR3 Data;
};

//
// Functions
//

static 
inline 
Vector 
operator+(
    _In_ const Vector & Addend0, 
    _In_ const Vector & Addend1
    )
{
    VECTOR3 IrisAddend0;
    VECTOR3 IrisAddend1;
    VECTOR3 IrisSum;

    IrisAddend0 = Addend0.AsVECTOR3();
    IrisAddend1 = Addend1.AsVECTOR3();

    IrisSum = VectorAdd(IrisAddend0,IrisAddend1);

    return Vector(IrisSum);
}

static 
inline 
Vector 
operator-(
    _In_ const Vector & Minuend, 
    _In_ const Vector & Subtrahend
    )
{
    VECTOR3 IrisDifference;
    VECTOR3 IrisMinuend;
    VECTOR3 IrisSubtrahend;

    IrisMinuend = Minuend.AsVECTOR3();
    IrisSubtrahend = Subtrahend.AsVECTOR3();

    IrisDifference = VectorSubtract(IrisMinuend, IrisSubtrahend);

    return Vector(IrisDifference);
}

static 
inline 
Vector 
operator*(
    _In_ const Vector & ToScale, 
    _In_ FLOAT Scalar
    )
{
    VECTOR3 IrisScaled;
    VECTOR3 IrisToScale;

    IrisToScale = ToScale.AsVECTOR3();

    IrisScaled = VectorScale(IrisToScale, Scalar);

    return Vector(IrisScaled);
}

static
inline
Vector
operator*(
    _In_ const Matrix & Multiplicand0,
    _In_ const Vector & Multiplicand1
    )
{
    PCMATRIX IrisMultiplicand0;
    VECTOR3 IrisMultiplicand1;
    VECTOR3 IrisResult;

    IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
    IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

    IrisResult = VectorMatrixMultiply(IrisMultiplicand0,
                                      IrisMultiplicand1);

    return Vector(IrisResult);
}

static
inline
Vector
operator*(
    _In_ const MatrixReference & Multiplicand0,
    _In_ const Vector & Multiplicand1
    )
{
    PCMATRIX IrisMultiplicand0;
    VECTOR3 IrisMultiplicand1;
    VECTOR3 IrisResult;

    IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
    IrisMultiplicand1 = Multiplicand1.AsVECTOR3();

    IrisResult = VectorMatrixMultiply(IrisMultiplicand0,
                                      IrisMultiplicand1);

    return Vector(IrisResult);
}

} // namespace Iris

#endif // _VECTOR_IRIS_PLUS_PLUS_