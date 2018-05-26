/*++

Copyright (c) 2018 Brad Weinberger

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
#include "test/test_util.h"

TEST(BoundingBoxTest, BoundingBoxCreate)
{
    POINT3 corner0 = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    POINT3 corner1 = PointCreate((float_t) 4.0, (float_t) 5.0, (float_t) 6.0);
    BOUNDING_BOX box = BoundingBoxCreate(corner0, corner1);

    EXPECT_EQ(corner0, box.corners[0]);
    EXPECT_EQ(corner1, box.corners[1]);
}

TEST(BoundingBoxTest, BoundingBoxValidate)
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