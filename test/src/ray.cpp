/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    ray.c

Abstract:

    This file contains tests for the RAY type.

--*/

#include <iristest.h>

TEST(RayInitialize)
{
    VECTOR3 Direction;
    POINT3 Origin;
    RAY Ray;

    Origin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Direction = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Ray = RayCreate(Origin, Direction);

    CHECK_EQUAL(Origin, Ray.Origin);
    CHECK_EQUAL(Direction, Ray.Direction);
}

TEST(RayEndpoint)
{
    POINT3 Origin, Endpoint;
    VECTOR3 Direction;
    RAY Ray;

    Origin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Direction = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Ray = RayCreate(Origin, Direction);

    Endpoint = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Origin = RayEndpoint(Ray, (FLOAT) 1.0);

    CHECK_EQUAL(Endpoint, Origin);
}

TEST(RayMatrixMultiply)
{
    PMATRIX Matrix;
    POINT3 Origin;
    VECTOR3 Direction;
    RAY Ray, Multiplied;

    Origin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Direction = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Ray = RayCreate(Origin, Direction);

    Origin = PointCreate((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Direction = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Multiplied = RayCreate(Origin, Direction);

    Matrix = MatrixAllocateTranslation((FLOAT) 1.0,
                                       (FLOAT) 1.0,
                                       (FLOAT) 1.0);

    Ray = RayMatrixMultiply(Matrix, Ray);

    CHECK_EQUAL(Multiplied, Ray);
}