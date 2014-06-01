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

    PointInitialize(&Origin, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    VectorInitialize(&Direction, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    RayInitialize(&Ray, &Origin, &Direction);

    CHECK_EQUAL(Origin, Ray.Origin);
    CHECK_EQUAL(Direction, Ray.Direction);
}

TEST(RayEndpoint)
{
    POINT3 Origin, Endpoint;
    VECTOR3 Direction;
    RAY Ray;

    PointInitialize(&Origin, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    VectorInitialize(&Direction, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    RayInitialize(&Ray, &Origin, &Direction);

    PointInitialize(&Endpoint, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    RayEndpoint(&Ray, (FLOAT) 1.0, &Origin);

    CHECK_EQUAL(Endpoint, Origin);
}

TEST(RayMatrixMultiply)
{
    PMATRIX Matrix;
    POINT3 Origin;
    VECTOR3 Direction;
    RAY Ray, Multiplied;

    PointInitialize(&Origin, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    VectorInitialize(&Direction, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    RayInitialize(&Ray, &Origin, &Direction);

    PointInitialize(&Origin, (FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    VectorInitialize(&Direction, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    RayInitialize(&Multiplied, &Origin, &Direction);

    Matrix = MatrixAllocateTranslation((FLOAT) 1.0,
                                       (FLOAT) 1.0,
                                       (FLOAT) 1.0);

    RayMatrixMultiply(Matrix, &Ray, &Ray);

    CHECK_EQUAL(Multiplied, Ray);
}