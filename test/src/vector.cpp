/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    vector.c

Abstract:

    This file contains tests for the VECTOR3 type.

--*/

#include <iristest.h>

TEST(VectorInitialize)
{
    VECTOR3 Vector1, Vector2;

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    Vector2.X = (FLOAT) 1.0;
    Vector2.Y = (FLOAT) 2.0;
    Vector2.Z = (FLOAT) 3.0;

    CHECK_EQUAL(Vector2, Vector1);
}

TEST(VectorDotProduct)
{
    VECTOR3 Vector1, Vector2;

    VectorInitialize(&Vector1, (FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 2.0);
    VectorInitialize(&Vector2, (FLOAT) 4.0, (FLOAT) 3.0, (FLOAT) 2.0);

    CHECK_EQUAL((FLOAT) 5.0, VectorDotProduct(&Vector1, &Vector1));
    CHECK_EQUAL((FLOAT) 7.0, VectorDotProduct(&Vector1, &Vector2));
}

TEST(VectorLength)
{
    VECTOR3 Vector1, Vector2;

    VectorInitialize(&Vector1, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    VectorInitialize(&Vector2, (FLOAT) 0.0, (FLOAT) 2.0, (FLOAT) 0.0);

    CHECK_CLOSE((FLOAT) 1.0, VectorLength(&Vector1), (FLOAT) 0.00001 );
    CHECK_CLOSE((FLOAT) 2.0, VectorLength(&Vector2), (FLOAT) 0.00001 );
}

TEST(VectorScale)
{
    VECTOR3 Vector1, Vector2;

    VectorInitialize(&Vector1, (FLOAT) -1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    VectorInitialize(&Vector2, (FLOAT) -2.0, (FLOAT) 2.0, (FLOAT) 2.0);

    VectorScale(&Vector1, (FLOAT) 2.0, &Vector1);
    CHECK_EQUAL(Vector2, Vector1);
}

TEST(VectorDiminishedAxis)
{
    VECTOR3 Vector1;

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    CHECK_EQUAL(VECTOR_X_AXIS, VectorDiminishedAxis(&Vector1));
}

TEST(VectorDominantAxis)
{
    VECTOR3 Vector1;

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    CHECK_EQUAL(VECTOR_Z_AXIS, VectorDominantAxis(&Vector1));
}

TEST(VectorSubtract)
{
    VECTOR3 Vector1, Vector2, Vector3;

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 3.0, (FLOAT) -1.0);
    VectorInitialize(&Vector2, (FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 1.0);
    VectorInitialize(&Vector3, (FLOAT) -1.0, (FLOAT) 1.0, (FLOAT) -2.0);

    VectorSubtract(&Vector1, &Vector2, &Vector2);

    CHECK_EQUAL(Vector3, Vector2);
}

TEST(VectorAdd)
{
    VECTOR3 Vector1, Vector2, Vector3;

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 3.0, (FLOAT) -1.0);
    VectorInitialize(&Vector2, (FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 1.0);
    VectorInitialize(&Vector3, (FLOAT) 3.0, (FLOAT) 5.0, (FLOAT) 0.0);

    VectorAdd(&Vector1, &Vector2, &Vector2);

    CHECK_EQUAL(Vector3, Vector2);
}

TEST(VectorNormalize)
{
    VECTOR3 Vector1, Vector2;

    VectorInitialize(&Vector1, (FLOAT) 3.0, (FLOAT) 1.0, (FLOAT) 2.0);

    VectorInitialize(&Vector2, 
                     (FLOAT) 0.801784,
                     (FLOAT) 0.267261,
                     (FLOAT) 0.534522);

    VectorNormalize(&Vector1, &Vector1);

    CHECK_EQUAL(Vector2, Vector1);
}

TEST(VectorCrossProduct)
{
    VECTOR3 Vector1, Vector2, Vector3;

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);
    VectorInitialize(&Vector2, (FLOAT) 3.0, (FLOAT) 2.0, (FLOAT) 1.0);
    VectorInitialize(&Vector3, (FLOAT) -4.0, (FLOAT) 8.0, (FLOAT) -4.0);

    VectorCrossProduct(&Vector1, &Vector2, &Vector2);

    CHECK_EQUAL(Vector3, Vector2);
}

TEST(VectorMatrixMultiply)
{
    VECTOR3 Vector1, Vector2;
    PCMATRIX Matrix;

    Matrix = MatrixAllocateScalar((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    VectorInitialize(&Vector2, (FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    VectorMatrixMultiply(Matrix, &Vector1, &Vector1);

    CHECK_EQUAL(Vector2, Vector1);
}

TEST(VectorMatrixTransposedMultiply)
{
    VECTOR3 Vector1, Vector2;
    PCMATRIX Matrix;

    Matrix = MatrixAllocate((FLOAT) 1.0,
                            (FLOAT) 2.0,
                            (FLOAT) 3.0,
                            (FLOAT) 4.0,
                            (FLOAT) 4.0,
                            (FLOAT) 2.0,
                            (FLOAT) 3.0,
                            (FLOAT) 6.0,
                            (FLOAT) 2.0,
                            (FLOAT) 2.0,
                            (FLOAT) 1.0,
                            (FLOAT) 4.0,
                            (FLOAT) 2.0,
                            (FLOAT) 9.0,
                            (FLOAT) 6.0,
                            (FLOAT) 4.0);

    VectorInitialize(&Vector1, (FLOAT) 1.0, (FLOAT) 3.0, (FLOAT) 4.0);
    VectorInitialize(&Vector2, (FLOAT) 21.0, (FLOAT) 16.0, (FLOAT) 16.0);

    VectorMatrixTransposedMultiply(Matrix, &Vector1, &Vector1);

    CHECK_EQUAL(Vector2, Vector1);
}