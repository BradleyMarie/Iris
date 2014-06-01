/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    point.c

Abstract:

    This file contains tests for the POINT3 type.

--*/

#include <iristest.h>

TEST(PointInitialize)
{
    POINT3 Point1, Point2;

    PointInitialize(&Point1, (FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    Point2.X = (FLOAT) 1.0;
    Point2.Y = (FLOAT) 2.0;
    Point2.Z = (FLOAT) 3.0;

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointInitializeScaled)
{
    POINT3 Point1, Point2;

    PointInitializeScaled(&Point1,
                          (FLOAT) 1.0,
                          (FLOAT) 2.0,
                          (FLOAT) 3.0,
                          (FLOAT) 2.0);

    PointInitialize(&Point2, (FLOAT) 0.5, (FLOAT) 1.0, (FLOAT) 1.5);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointSubtract)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector1, Vector2;

    PointInitialize(&Point1, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    PointInitialize(&Point2, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    PointSubtract(&Point2, &Point1, &Vector1);

    VectorInitialize(&Vector2, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    CHECK_EQUAL(Vector2, Vector1);
}

TEST(PointVectorSubtract)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    PointInitialize(&Point1, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    PointInitialize(&Point2, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    VectorInitialize(&Vector, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    PointVectorSubtract(&Point1, &Vector, &Point1);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointVectorAdd)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    PointInitialize(&Point1, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    PointInitialize(&Point2, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    VectorInitialize(&Vector, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    PointVectorAdd(&Point1, &Vector, &Point1);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointVectorAddScaled)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    PointInitialize(&Point1, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    PointInitialize(&Point2, (FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 2.0);
    VectorInitialize(&Vector, (FLOAT) 4.0, (FLOAT) 4.0, (FLOAT) 4.0);

    PointVectorAddScaled(&Point1, &Vector, (FLOAT) 0.5, &Point1);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointVectorSubtractScaled)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    PointInitialize(&Point1, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    PointInitialize(&Point2, (FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 2.0);
    VectorInitialize(&Vector, (FLOAT) -4.0, (FLOAT) -4.0, (FLOAT) -4.0);

    PointVectorSubtractScaled(&Point1, &Vector, (FLOAT) 0.5, &Point1);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointMatrixMultiply)
{
    POINT3 Point1, Point2;
    PMATRIX Matrix;

    Matrix = MatrixAllocateTranslation((FLOAT) 1.0,
                                       (FLOAT) 2.0, 
                                       (FLOAT) 3.0);

    PointInitialize(&Point1, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    PointInitialize(&Point2, (FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    PointMatrixMultiply(Matrix, &Point1, &Point1);

    CHECK_EQUAL(Point2, Point1);
}