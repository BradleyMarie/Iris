/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    bounding_box_test.cc

Abstract:

    Unit tests for bounding_box.h

--*/

extern "C" {
#include "iris_advanced/bounding_box.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(BoundingBoxTest, BoundingBoxCreate)
{
    POINT3 corner0 = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    POINT3 corner1 = PointCreate((float_t) 4.0, (float_t) 5.0, (float_t) 6.0);
    BOUNDING_BOX box = BoundingBoxCreate(corner0, corner1);

    EXPECT_EQ(corner0, box.corners[0]);
    EXPECT_EQ(corner1, box.corners[1]);
}

TEST(BoundingBoxTest, BoundingBoxValidateBadFloats)
{
    POINT3 corner0 = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    POINT3 corner1 = PointCreate((float_t) 4.0, (float_t) 5.0, (float_t) 6.0);
    BOUNDING_BOX box = BoundingBoxCreate(corner0, corner1);
    EXPECT_TRUE(BoundingBoxValidate(box));

    box.corners[0].x = (float_t) INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[0].x = (float_t) -INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[0].x = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[0].x = (float_t) 0.0;

    box.corners[0].y = (float_t) INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[0].y = (float_t) -INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[0].y = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[0].y = (float_t) 0.0;
    
    box.corners[0].z = (float_t) INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[0].z = (float_t) -INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[0].z = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[0].z = (float_t) 0.0;

    box.corners[1].x = (float_t) INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[1].x = (float_t) -INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[1].x = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[1].x = (float_t) 0.0;

    box.corners[1].y = (float_t) INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[1].y = (float_t) -INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[1].y = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[1].y = (float_t) 0.0;
    
    box.corners[1].z = (float_t) INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[1].z = (float_t) -INFINITY;
    EXPECT_FALSE(BoundingBoxValidate(box));

    box.corners[1].z = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[1].z = (float_t) 0.0;
}

TEST(BoundingBoxTest, BoundingBoxValidateBadBounds)
{
    POINT3 corner0 = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    POINT3 corner1 = PointCreate((float_t) 4.0, (float_t) 5.0, (float_t) 6.0);
    BOUNDING_BOX box = BoundingBoxCreate(corner0, corner1);
    EXPECT_TRUE(BoundingBoxValidate(box));

    box.corners[1].x = (float_t) 0.0;
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[1].x = (float_t) 4.0;

    box.corners[1].y = (float_t) 0.0;
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[1].y = (float_t) 5.0;

    box.corners[1].z = (float_t) 0.0;
    EXPECT_FALSE(BoundingBoxValidate(box));
    box.corners[1].z = (float_t) 6.0;
}

TEST(BoundingBoxTest, BoundingBoxEnvelop)
{
    POINT3 point0 = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    BOUNDING_BOX box0 = BoundingBoxCreate(point0, point0);
    
    POINT3 point1 = PointCreate((float_t) -1.0, (float_t) -1.0, (float_t) -1.0);
    BOUNDING_BOX box1 = BoundingBoxEnvelop(box0, point1);
    EXPECT_EQ(box1.corners[1], point0);
    EXPECT_EQ(box1.corners[0], point1);
    
    POINT3 point2 = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    BOUNDING_BOX box2 = BoundingBoxEnvelop(box1, point2);
    EXPECT_EQ(box2.corners[1], point2);
    EXPECT_EQ(box2.corners[0], point1);
}

TEST(BoundingBoxTest, BoundingBoxUnion)
{
    POINT3 point0 = PointCreate((float_t) -1.0, (float_t) -1.0, (float_t) -1.0);
    POINT3 point1 = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    BOUNDING_BOX box0 = BoundingBoxCreate(point0, point1);

    POINT3 point2 = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    BOUNDING_BOX box1 = BoundingBoxCreate(point1, point2);

    BOUNDING_BOX box2 = BoundingBoxUnion(box0, box1);
    EXPECT_EQ(box2.corners[0], point0);
    EXPECT_EQ(box2.corners[1], point2);
}

TEST(BoundingBoxTest, BoundingBoxIntersectionEmpty)
{
    POINT3 point0 = PointCreate((float_t) -1.0, (float_t) -1.0, (float_t) -1.0);
    POINT3 point1 = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    BOUNDING_BOX box0 = BoundingBoxCreate(point0, point1);

    POINT3 point2 = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    BOUNDING_BOX box1 = BoundingBoxCreate(point1, point2);

    BOUNDING_BOX box2 = BoundingBoxIntersection(box0, box1);
    EXPECT_EQ(box2.corners[0], point1);
    EXPECT_EQ(box2.corners[1], point1);
}

TEST(BoundingBoxTest, BoundingBoxIntersectionOnePoint)
{
    POINT3 point0 = PointCreate((float_t) -1.0, (float_t) -1.0, (float_t) -1.0);
    POINT3 point1 = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    BOUNDING_BOX box0 = BoundingBoxCreate(point0, point1);

    POINT3 point2 = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    BOUNDING_BOX box1 = BoundingBoxCreate(point1, point2);

    BOUNDING_BOX box2 = BoundingBoxIntersection(box0, box1);
    EXPECT_EQ(box2.corners[0], point1);
    EXPECT_EQ(box2.corners[1], point1);
}

TEST(BoundingBoxTest, BoundingBoxIntersection)
{
    POINT3 point0 = PointCreate((float_t) -1.0, (float_t) -1.0, (float_t) -1.0);
    POINT3 point1 = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    BOUNDING_BOX box0 = BoundingBoxCreate(point0, point1);

    POINT3 point2 = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    POINT3 point3 = PointCreate((float_t) 2.0, (float_t) 2.0, (float_t) 2.0);
    BOUNDING_BOX box1 = BoundingBoxCreate(point2, point3);

    BOUNDING_BOX box2 = BoundingBoxIntersection(box0, box1);
    EXPECT_EQ(box2.corners[0], point2);
    EXPECT_EQ(box2.corners[1], point1);
}

TEST(BoundingBoxTest, BoundingBoxTransform)
{
    POINT3 point0 = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    POINT3 point1 = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    BOUNDING_BOX box = BoundingBoxCreate(point0, point1);

    PMATRIX scalar;
    ISTATUS status = MatrixAllocateScalar((float_t) 1.0,
                                          (float_t) 2.0,
                                          (float_t) 3.0,
                                          &scalar);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    BOUNDING_BOX scaled = BoundingBoxTransform(scalar, box);
    POINT3 bound0 = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    EXPECT_EQ(scaled.corners[0], point0);
    EXPECT_EQ(scaled.corners[1], bound0);

    PMATRIX translation;
    status = MatrixAllocateTranslation((float_t) 1.0,
                                       (float_t) 2.0,
                                       (float_t) 3.0,
                                       &translation);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    BOUNDING_BOX translated = BoundingBoxTransform(translation, box);
    POINT3 bound1 = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    POINT3 bound2 = PointCreate((float_t) 2.0, (float_t) 3.0, (float_t) 4.0);
    EXPECT_EQ(translated.corners[0], bound1);
    EXPECT_EQ(translated.corners[1], bound2);
}

TEST(BoundingBoxTest, BoundingBoxOverlaps)
{
    POINT3 points[4] = {
        PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0),
        PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0),
        PointCreate((float_t) 2.0, (float_t) 2.0, (float_t) 2.0),
        PointCreate((float_t) 3.0, (float_t) 3.0, (float_t) 3.0),
    };

    for (uint32_t i = 0; i < 64; i++)
    {
        uint32_t lower_index = i / 8;
        POINT3 lower_upper = PointCreate(
            points[1 + ((lower_index >> 0) & 1u)].x,
            points[1 + ((lower_index >> 1) & 1u)].y,
            points[1 + ((lower_index >> 2) & 1u)].z);
        BOUNDING_BOX lower_box = BoundingBoxCreate(points[0], lower_upper);

        uint32_t upper_index = i % 8;
        POINT3 upper_lower = PointCreate(
            points[2 - ((upper_index >> 0) & 1u)].x,
            points[2 - ((upper_index >> 1) & 1u)].y,
            points[2 - ((upper_index >> 2) & 1u)].z);
        BOUNDING_BOX upper_box = BoundingBoxCreate(upper_lower, points[3]);

        if (i == 63)
        {
            EXPECT_TRUE(BoundingBoxOverlaps(lower_box, upper_box));
            EXPECT_TRUE(BoundingBoxOverlaps(upper_box, lower_box));
        }
        else
        {
            EXPECT_FALSE(BoundingBoxOverlaps(lower_box, upper_box));
            EXPECT_FALSE(BoundingBoxOverlaps(upper_box, lower_box));
        }
    }
}