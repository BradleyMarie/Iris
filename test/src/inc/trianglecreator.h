/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    trianglecreator.h

Abstract:

    This file contains the prototypes for the triangle creation 
    routines used to simplify making the cornell box test.

--*/

#ifndef _IRIS_TEST_TRIANGLE_CREATOR_HEADER_
#define _IRIS_TEST_TRIANGLE_CREATOR_HEADER_

#include <iristest.h>

bool
CreateFlatQuad(
    PSCENE Scene,
    PMATRIX ModelToWorld,
    BOOL Premultiplied,
    float X0,
    float Y0,
    float Z0,
    float X1,
    float Y1,
    float Z1,
    float X2,
    float Y2,
    float Z2,
    float X3,
    float Y3,
    float Z3,
    PCTEXTURE FrontTexture,
    PCTEXTURE BackTexture
    );

#endif // _IRIS_TEST_TRIANGLE_CREATOR_HEADER_