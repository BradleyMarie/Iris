/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisplusplus.c

Abstract:

    This file contains tests for irisplusplus.

--*/

#include <iristest.h>
#include <vector>

#define _Alignof(_Type_) alignof(_Type_)
#include <iriscommon_triangle.h>

using namespace Iris;

//
// Test Matrix
//

TEST(MatrixPlusPlusValueConstructorAndInverse)
{
    FLOAT Contents[4][4];
    FLOAT InverseContents[4][4];

    Matrix Matrix0((FLOAT) 1.0,
                   (FLOAT) 0.0,
                   (FLOAT) 0.0,
                   (FLOAT) 1.0,
                   (FLOAT) 0.0,
                   (FLOAT) 1.0,
                   (FLOAT) 0.0,
                   (FLOAT) 1.0,
                   (FLOAT) 0.0,
                   (FLOAT) 0.0,
                   (FLOAT) 1.0,
                   (FLOAT) 1.0,
                   (FLOAT) 0.0,
                   (FLOAT) 0.0,
                   (FLOAT) 0.0,
                   (FLOAT) 1.0);

    Matrix0.ReadContents(Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);

    Matrix Inverse = Matrix0.Inverse();

    Inverse.ReadContents(InverseContents);

    CHECK_EQUAL((FLOAT) 1.0, InverseContents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[0][2]);
    CHECK_EQUAL((FLOAT) -1.0, InverseContents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, InverseContents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[1][2]);
    CHECK_EQUAL((FLOAT) -1.0, InverseContents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, InverseContents[2][2]);
    CHECK_EQUAL((FLOAT) -1.0, InverseContents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, InverseContents[3][3]);
}

TEST(MatrixPlusPlusTranslation)
{
    FLOAT Contents[4][4];

    Matrix Matrix0 = Matrix::Translation((FLOAT) 1.0,
                                         (FLOAT) 2.0,
                                         (FLOAT) 3.0);

    Matrix0.ReadContents(Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}

TEST(MatrixPlusPlusScalar)
{
    FLOAT Contents[4][4];

    Matrix Matrix0 = Matrix::Scalar((FLOAT) 1.0,
                                    (FLOAT) 2.0,
                                    (FLOAT) 3.0);

    Matrix0.ReadContents(Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}

TEST(MatrixPlusPlusFrustum)
{

}

TEST(MatrixPlusPlusOrtho)
{

}

TEST(MatrixPlusPlusMultiply)
{
    FLOAT Contents[4][4];

    Matrix Matrix0 = Matrix::Translation((FLOAT) 1.0,
                                         (FLOAT) 2.0,
                                         (FLOAT) 3.0);

    Matrix Matrix1 = Matrix::Scalar((FLOAT) 2.0,
                                    (FLOAT) 2.0,
                                    (FLOAT) 2.0);
                             
    Matrix Matrix2 = Matrix0 * Matrix1;

    Matrix2.ReadContents(Contents);

    CHECK_EQUAL((FLOAT) 2.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}

TEST(MatrixPlusPlusMultiplyWithIdentityAndOverwrite)
{
    FLOAT Contents[4][4];

    Matrix Matrix0 = Matrix::Translation((FLOAT) 1.0,
                                         (FLOAT) 2.0,
                                         (FLOAT) 3.0);
    
    Matrix0 = Matrix0 * Matrix::Identity();

    Matrix0.ReadContents(Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}

//
// Test Vector
//

TEST(VectorPlusPlusInitialize)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    CHECK_EQUAL((FLOAT) 1.0, Vector0.X());
    CHECK_EQUAL((FLOAT) 2.0, Vector0.Y());
    CHECK_EQUAL((FLOAT) 3.0, Vector0.Z());
}

TEST(VectorPlusPlusDotProduct)
{
    Vector Vector0((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 2.0);
    Vector Vector1((FLOAT) 4.0, (FLOAT) 3.0, (FLOAT) 2.0);

    CHECK_EQUAL((FLOAT) 5.0, Vector::DotProduct(Vector0, Vector0));
    CHECK_EQUAL((FLOAT) 7.0, Vector::DotProduct(Vector0, Vector1));
}

TEST(VectorPlusPlusLength)
{
    Vector Vector0((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Vector Vector1((FLOAT) 0.0, (FLOAT) 2.0, (FLOAT) 0.0);

    CHECK_CLOSE((FLOAT) 1.0, Vector0.Length(), (FLOAT) 0.00001 );
    CHECK_CLOSE((FLOAT) 2.0, Vector1.Length(), (FLOAT) 0.00001 );
}

TEST(VectorPlusPlusScale)
{
    Vector Vector0((FLOAT) -1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Vector Vector1((FLOAT) -2.0, (FLOAT) 2.0, (FLOAT) 2.0);
    
    CHECK_EQUAL(Vector1, Vector0 * (FLOAT) 2.0);
}

TEST(VectorPlusPlusSubtract)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 3.0, (FLOAT)-1.0);
    Vector Vector1((FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 1.0);
    Vector Vector2((FLOAT)-1.0, (FLOAT) 1.0, (FLOAT)-2.0);

    CHECK_EQUAL(Vector2, Vector0 - Vector1);
}

TEST(VectorPlusPlusAdd)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 3.0, (FLOAT)-1.0);
    Vector Vector1((FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 1.0);
    Vector Vector2((FLOAT) 3.0, (FLOAT) 5.0, (FLOAT) 0.0);

    CHECK_EQUAL(Vector2, Vector0 + Vector1);
}

TEST(VectorPlusPlusFma)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 3.0, (FLOAT)-1.0);
    Vector Vector1((FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 1.0);
    Vector Vector2((FLOAT) 5.0, (FLOAT) 7.0, (FLOAT) 1.0);

    CHECK_EQUAL(Vector2, Vector::Fma(Vector0, Vector1, (FLOAT) 2.0));
}

TEST(VectorPlusPlusDiminishedAxis)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);
    CHECK_EQUAL(VectorAxis::X, Vector0.DiminishedAxis());
}

TEST(VectorPlusPlusDominantAxis)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);
    CHECK_EQUAL(VectorAxis::Z, Vector0.DominantAxis());
}

TEST(VectorPlusPlusNormalize)
{
    Vector Vector0((FLOAT) 3.0, (FLOAT) 1.0, (FLOAT) 2.0);
    Vector Vector1((FLOAT) 0.801784,(FLOAT) 0.267261, (FLOAT) 0.534522);

    CHECK_EQUAL(Vector1, Vector::Normalize(Vector0));
}

TEST(VectorPlusPlusNormalizeWithLength)
{
    Vector Vector0((FLOAT) 2.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Vector Vector1((FLOAT) 1.0, (FLOAT) 0.0, (FLOAT) 0.0);

    auto Normalized = Vector::NormalizeWithLength(Vector0);

    CHECK_EQUAL(Vector1, std::get<0>(Normalized));
    CHECK_EQUAL((FLOAT) 4.0, std::get<1>(Normalized));
    CHECK_EQUAL((FLOAT) 2.0, std::get<2>(Normalized));
}

TEST(VectorPlusPlusCrossProduct)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);
    Vector Vector1((FLOAT) 3.0, (FLOAT) 2.0, (FLOAT) 1.0);
    Vector Vector2((FLOAT)-4.0, (FLOAT) 8.0, (FLOAT)-4.0);

    CHECK_EQUAL(Vector2, Vector::CrossProduct(Vector0, Vector1));
}

TEST(VectorPlusPlusMatrixMultiply)
{
    Matrix Scalar = Matrix::Scalar((FLOAT) 1.0, 
                                   (FLOAT) 2.0, 
                                   (FLOAT) 3.0);

    Vector Vector0((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Vector Vector1((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    CHECK_EQUAL(Vector1, Scalar * Vector0);
}

TEST(VectorPlusPlusMatrixTransposedMultiply)
{
    Vector Vector0((FLOAT) 1.0, (FLOAT) 3.0, (FLOAT) 4.0);
    Vector Vector1((FLOAT) 21.0, (FLOAT) 16.0, (FLOAT) 16.0);
    
    Matrix Transform((FLOAT) 1.0,
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
    
    CHECK_EQUAL(Vector1, Vector::TransposedMultiply(Transform, Vector0));
}

//
// Test Point
//

TEST(PointPlusPlusInitialize)
{
    Point Point1((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    CHECK_EQUAL((FLOAT) 1.0, Point1.X());
    CHECK_EQUAL((FLOAT) 2.0, Point1.Y());
    CHECK_EQUAL((FLOAT) 3.0, Point1.Z());
}

TEST(PointPlusPlusSubtract)
{
    Point Point1((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point Point2((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Vector Vector1((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    CHECK_EQUAL(Vector1, Point2 - Point1);
}

TEST(PointPlusPlusVectorSubtract)
{
    Point Point1((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Point Point2((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Vector Vector1((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    CHECK_EQUAL(Point2, Point1 - Vector1);
}

TEST(PointPlusPlusVectorAdd)
{
    Point Point1((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point Point2((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);
    Vector Vector1((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0);

    CHECK_EQUAL(Point2, Point1 + Vector1);
}

TEST(PointPlusPlusVectorAddScaled)
{
    Point Point1((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point Point2((FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 2.0);
    Vector Vector1((FLOAT) 4.0, (FLOAT) 4.0, (FLOAT) 4.0);

    CHECK_EQUAL(Point2, Point::AddScaled(Point1, Vector1, (FLOAT) 0.5));
}

TEST(PointPlusPlusVectorSubtractScaled)
{
    Point Point1((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point Point2((FLOAT) 2.0, (FLOAT) 2.0, (FLOAT) 2.0);
    Vector Vector1((FLOAT)-4.0, (FLOAT)-4.0, (FLOAT)-4.0);

    CHECK_EQUAL(Point2, Point::SubtractScaled(Point1, Vector1, 0.5));
}

TEST(PointPlusPlusMatrixMultiply)
{
    Matrix Transform = Matrix::Translation((FLOAT) 1.0,
                                           (FLOAT) 2.0, 
                                           (FLOAT) 3.0);

    Point Point1((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Point Point2((FLOAT) 1.0, (FLOAT) 2.0, (FLOAT) 3.0);

    CHECK_EQUAL(Point2, Transform * Point1);
}

//
// Test Ray
//

TEST(RayPlusPlusInitialize)
{
    Point Origin((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Vector Direction((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Ray R(Origin, Direction);

    CHECK_EQUAL(Origin, R.Origin());
    CHECK_EQUAL(Direction, R.Direction());
}

TEST(RayPlusPlusEndpoint)
{
    Point Origin((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    Vector Direction((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    Ray R(Origin, Direction);

    Point Endpoint((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);

    CHECK_EQUAL(Endpoint, Ray::Endpoint(R, 1.0));
}

TEST(RayPlusPlusMatrixMultiply)
{
    Ray R(Point((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0),
          Vector((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0));

    Ray Multiplied(Point((FLOAT) 1.0, (FLOAT) 1.0, (FLOAT) 1.0),
                   Vector((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0));

    Matrix Transform = Matrix::Translation((FLOAT) 1.0,
                                           (FLOAT) 1.0,
                                           (FLOAT) 1.0);

    CHECK_EQUAL(Multiplied, Transform * R);
}

//
// Test Ray Tracing
// 

class ListScene final {
public:
    static
    ListScene
    Create(
        void
        )
    {
        return ListScene();
    }

    void
    Add(
        _In_ std::function<PHIT_LIST(Ray, HitAllocator)> ShapeFunc,
        _In_ Matrix MatrixRef,
        _In_ bool Premultiplied
        )
    {
        Shapes.push_back(ShapeFunc);
        Matrices.push_back(MatrixRef);
        IsPremultiplied.push_back(Premultiplied);
    }

    void
    Trace(
        _In_ HitTester Tester
        ) const
    {
        for (size_t Index = 0; Index < Shapes.size(); Index++)
        {
            Tester.Test(Shapes[Index], Matrices[Index], IsPremultiplied[Index]);
        }
    }

private:
    std::vector<std::function<PHIT_LIST(Ray, HitAllocator)>> Shapes;
    std::vector<Matrix> Matrices;
    std::vector<bool> IsPremultiplied;
};

class Triangle final {
public:
    static
    std::function<PHIT_LIST(Ray, HitAllocator)>
    Create(
        _In_ Point Vertex0,
        _In_ Point Vertex1,
        _In_ Point Vertex2
        )
    {
        VECTOR_AXIS Axis;
        TRIANGLE Data;
        
        ISTATUS Status = TriangleInitialize(&Data,
                                            Vertex0.AsPOINT3(),
                                            Vertex1.AsPOINT3(),
                                            Vertex2.AsPOINT3(),
                                            &Axis);
                                            
        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(ISTATUSToCString(Status));
        }
        
        std::function<PHIT_LIST(Ray, HitAllocator)> Result;

        switch (Axis)
        {
            case VECTOR_X_AXIS:
                Result = [=](Ray WorldRay, HitAllocator Allocator) -> PHIT_LIST {
                    PHIT_LIST Result;

                    ISTATUS Status = TriangleXDominantTestRay(&Data,
                                                              WorldRay.AsRAY(),
                                                              &Data,
                                                              Allocator.AsPHIT_ALLOCATOR(),
                                                              &Result);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        throw std::runtime_error("error");
                    }

                    return Result;
                };
                break;       
            case VECTOR_Y_AXIS:
                Result = [=](Ray WorldRay, HitAllocator Allocator) -> PHIT_LIST {
                    PHIT_LIST Result;

                    ISTATUS Status = TriangleYDominantTestRay(&Data,
                                                              WorldRay.AsRAY(),
                                                              &Data,
                                                              Allocator.AsPHIT_ALLOCATOR(),
                                                              &Result);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        throw std::runtime_error("error");
                    }

                    return Result;
                };
                break;                
            default: // VECTOR_Z_AXIS
                Result = [=](Ray WorldRay, HitAllocator Allocator) -> PHIT_LIST {
                    PHIT_LIST Result;

                    ISTATUS Status = TriangleZDominantTestRay(&Data,
                                                              WorldRay.AsRAY(),
                                                              &Data,
                                                              Allocator.AsPHIT_ALLOCATOR(),
                                                              &Result);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        throw std::runtime_error("error");
                    }

                    return Result;
                };
                break;
        }
        
        return Result;
    }
};

TEST(RayTracePlusPlusTestIdentityTriangle)
{
    ListScene Scene = ListScene::Create();

    auto TrianglePtr = Triangle::Create(Point(-1.0f, 1.0f, -1.0f),
                                        Point(1.0f, 1.0f, -1.0f),
                                        Point(-1.0f, -1.0f, -1.0f));

    Scene.Add(TrianglePtr, Matrix::Identity(), false);
    RayTracer Tracer = RayTracer::Create();

    std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T)> EvaluateFirstHit;

    bool FirstHitFound = false;

    std::function<void(HitTester, const Ray &)> TraceFunction = [&](HitTester Tester, const Ray & WorldRay) -> void
    {
        Scene.Trace(Tester);
    };

    EvaluateFirstHit = [&](PCVOID DataPtr, FLOAT Distance, INT32 FaceHit, PCVOID AdditionalData, SIZE_T AdditionalDataSize) -> bool
    {
        CHECK_EQUAL((FLOAT) 1.0, Distance);
        CHECK_EQUAL(0, FaceHit);
        CHECK_EQUAL(true, !!AdditionalData);
        CHECK_EQUAL(sizeof(BARYCENTRIC_COORDINATES), AdditionalDataSize);
        FirstHitFound = true;
        return true;
    };

    Tracer.TraceClosestHit(TraceFunction,
                           Ray(Point(-0.5f, 0.5f, 0.0f), Vector(0.0f, 0.0f, -1.0f)),
                           0.0f,
                           EvaluateFirstHit);

    CHECK_EQUAL(true, FirstHitFound);
}

TEST(RayTracePlusPlusTestTranslatedTriangle)
{
    ListScene Scene = ListScene::Create();

    auto TrianglePtr = Triangle::Create(Point(-1.0f, 1.0f, -1.0f),
                                        Point(1.0f, 1.0f, -1.0f),
                                        Point(-1.0f, -1.0f, -1.0f));

    Scene.Add(TrianglePtr, Matrix::Translation(0.0f, 0.0f, -1.0f), false);

    RayTracer Tracer = RayTracer::Create();

    std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T)> EvaluateFirstHit;

    bool FirstHitFound = false;

    std::function<void(HitTester, const Ray &)> TraceFunction = [&](HitTester Tester, const Ray & WorldRay) -> void
    {
        Scene.Trace(Tester);
    };

    EvaluateFirstHit = [&](PCVOID DataPtr, FLOAT Distance, INT32 FaceHit, PCVOID AdditionalData, SIZE_T AdditionalDataSize) -> bool
    {
        CHECK_EQUAL((FLOAT) 2.0, Distance);
        CHECK_EQUAL(0, FaceHit);
        CHECK_EQUAL(true, !!AdditionalData);
        CHECK_EQUAL(sizeof(BARYCENTRIC_COORDINATES), AdditionalDataSize);
        FirstHitFound = true;
        return true;
    };

    Tracer.TraceClosestHit(TraceFunction,
                           Ray(Point(-0.5f, 0.5f, 0.0f), Vector(0.0f, 0.0f, -1.0f)),
                           0.0f,
                           EvaluateFirstHit);

    CHECK_EQUAL(true, FirstHitFound);
}