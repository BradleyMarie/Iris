/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisplusplus_vector.h

Abstract:

    This file contains the definitions for the 
    Iris++ vector type.

--*/

#include <iristoolkit.h>

#ifndef _VECTOR_IRIS_PLUS_PLUS_
#define _VECTOR_IRIS_PLUS_PLUS_

//
// Types
//

enum class VectorAxis : VECTOR_AXIS {
    X = VECTOR_X_AXIS,
    Y = VECTOR_Y_AXIS,
    Z = VECTOR_Z_AXIS
};

class Vector
{
public:
    Vector(FLOAT X, FLOAT Y, FLOAT Z)
    {
        VectorInitialize(&Data, X, Y, Z);
    }

    Vector(const VECTOR3 & IrisVector)
    : Data(IrisVector)
    { }

    Vector operator-() const
    {
        VECTOR3 Negated;

        VectorNegate(&Data, &Negated);

        return Vector(Negated);
    }

    VECTOR3 AsVECTOR3() const
    {
        return Data;
    }

    PVECTOR3 AsPVECTOR3()
    {
        return &Data;
    }

    PCVECTOR3 AsPCVECTOR3() const
    {
        return &Data;
    }

    FLOAT Length() const
    {
        return VectorLength(&Data);
    }

    VectorAxis DominantAxis()
    {
        return VectorAxis(VectorDominantAxis(&Data));
    }

    VectorAxis DiminishedAxis()
    {
        return VectorAxis(VectorDiminishedAxis(&Data));
    }

    static Vector Normalize(const Vector & ToNormalize)
    {
        VECTOR3 Normalized;

        VectorNormalize(&ToNormalize.Data, &Normalized);

        return Vector(Normalized);
    }

    static std::pair<Vector, FLOAT> NormalizeWithLength(const Vector & ToNormalize)
    {
        VECTOR3 Normalized;
        FLOAT OldLength;

        VectorNormalizeWithLength(&ToNormalize.Data, &OldLength, &Normalized);

        return std::make_pair(Vector(Normalized), OldLength);
    }

    static FLOAT DotProduct(const Vector & Operand0, const Vector & Operand1)
    {
        return VectorDotProduct(&Operand0.Data, &Operand1.Data);
    }

    static Vector CrossProduct(const Vector & Operand0, const Vector & Operand1)
    {
        VECTOR3 Product;

        VectorCrossProduct(&Operand0.Data, &Operand1.Data, &Product);

        return Vector(Product);
    }

    static Vector Fma(const Vector & Addend0, const Vector & Addend1, FLOAT Scalar)
    {
        VECTOR3 Sum;

        VectorAddScaled(&Addend0.Data, &Addend1.Data, Scalar, &Sum);

        return Vector(Sum);
    }

    static Vector Reflect(const Vector & Vec, const Vector & Normal)
    {
        VECTOR3 Reflected;

        VectorReflect(&Vec.Data, &Normal.Data, &Reflected);

        return Vector(Reflected);
    }

    static Vector HalfAngle(const Vector & Vector0, const Vector & Vector1)
    {
        VECTOR3 HalfAngle;

        VectorHalfAngle(&Vector0.Data, &Vector1.Data, &HalfAngle);

        return Vector(HalfAngle);
    }

    FLOAT & X()
    {
        return Data.X;
    }

    FLOAT & Y()
    {
        return Data.Y;
    }

    FLOAT & Z()
    {
        return Data.Z;
    }

    FLOAT X() const
    {
        return Data.X;
    }

    FLOAT Y() const
    {
        return Data.Y;
    }

    FLOAT Z() const
    {
        return Data.Z;
    }
    
    friend Vector operator+(const Vector & Addend0, const Vector & Addednd1);
    friend Vector operator-(const Vector & Minuend, const Vector & Subtrahend);
    friend Vector operator*(const Vector & ToScale, FLOAT Scalar);

private:
    VECTOR3 Data;
};

//
// Functions
//

static inline Vector operator+(const Vector & Addend0, const Vector & Addend1)
{
    VECTOR3 Sum;

    VectorAdd(&Addend0.Data, &Addend1.Data, &Sum);

    return Vector(Sum);
}

static inline Vector operator-(const Vector & Minuend, const Vector & Subtrahend)
{
    VECTOR3 Difference;

    VectorSubtract(&Minuend.Data, &Subtrahend.Data, &Difference);

    return Vector(Difference);
}

static inline Vector operator*(const Vector & ToScale, FLOAT Scalar)
{
    VECTOR3 Scaled;

    VectorScale(&ToScale.Data, Scalar, &Scaled);

    return Vector(Scaled);
}

#endif // _VECTOR_IRIS_PLUS_PLUS_