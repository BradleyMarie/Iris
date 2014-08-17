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

    Point1 = PointCreate((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    Point2.X = (FLOAT) 1.0;
    Point2.Y = (FLOAT) 2.0;
    Point2.Z = (FLOAT) 3.0;

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointSubtract)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector1, Vector2;

    Point1 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point2 = PointCreate((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Vector1 = PointSubtract(Point2, Point1);

    Vector2 = VectorCreate((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    CHECK_EQUAL(Vector2, Vector1);
}

TEST(PointVectorSubtract)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    Point1 = PointCreate((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Point2 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Vector = VectorCreate((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    Point1 = PointVectorSubtract(Point1, Vector);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointVectorAdd)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    Point1 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point2 = PointCreate((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Vector = VectorCreate((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    Point1 = PointVectorAdd(Point1, Vector);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointVectorAddScaled)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    Point1 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point2 = PointCreate((FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 2.0);
    Vector = VectorCreate((FLOAT) 4.0, (FLOAT) 4.0, (FLOAT) 4.0);

    Point1 = PointVectorAddScaled(Point1, Vector, (FLOAT) 0.5);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointVectorSubtractScaled)
{
    POINT3 Point1, Point2;
    VECTOR3 Vector;

    Point1 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point2 = PointCreate((FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 2.0);
    Vector = VectorCreate((FLOAT)-4.0, (FLOAT)-4.0, (FLOAT)-4.0);

    Point1 = PointVectorSubtractScaled(Point1, Vector, (FLOAT) 0.5);

    CHECK_EQUAL(Point2, Point1);
}

TEST(PointMatrixMultiply)
{
    POINT3 Point1, Point2;
    PMATRIX Matrix;
    ISTATUS Status;

    Status = MatrixAllocateTranslation((FLOAT) 1.0,
                                       (FLOAT) 2.0, 
                                       (FLOAT) 3.0,
                                       &Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Point1 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point2 = PointCreate((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    Point1 = PointMatrixMultiply(Matrix, Point1);

    CHECK_EQUAL(Point2, Point1);
}