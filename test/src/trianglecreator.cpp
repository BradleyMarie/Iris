/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    trianglecreator.cpp

Abstract:

    This file contains the definitions for the triangle creation 
    routines used to simplify making the cornell box test.

--*/

#include <iristest.h>

bool
CreateFlatQuad(
    PSCENE Scene,
    PCMATRIX ModelToWorld,
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
    )
{
    PDRAWING_SHAPE Triangle0;
    PDRAWING_SHAPE Triangle1;
    PNORMAL FrontNormal;
    PNORMAL BackNormal;
    ISTATUS Status;
    POINT3 P0;
    POINT3 P1;
    POINT3 P2;
    POINT3 P3;

    P0 = PointCreate(X0, Y0, Z0);
    P1 = PointCreate(X1, Y1, Z1);
    P2 = PointCreate(X2, Y2, Z2);
    P3 = PointCreate(X3, Y3, Z3);

    Triangle0 = FlatTriangleAllocate(&P0,
                                     &P1,
                                     &P2,
                                     FrontTexture,
                                     BackTexture,
                                     &FrontNormal,
                                     &BackNormal);

    if (Triangle0 == NULL)
    {
        return false;
    }

    Triangle1 = TriangleAllocate(&P0,
                                 &P2,
                                 &P3,
                                 FrontTexture,
                                 FrontNormal,
                                 BackTexture,
                                 BackNormal);

    if (Triangle1 == NULL)
    {
        DrawingShapeFree(Triangle0);
        NormalFree(FrontNormal);
        NormalFree(BackNormal);
        return false;
    }

    Status = SceneAddObject(Scene,
                            Triangle0,
                            ModelToWorld,
                            Premultiplied);

    if (Status != ISTATUS_SUCCESS)
    {
        DrawingShapeFree(Triangle0);
        DrawingShapeFree(Triangle1);
        NormalFree(FrontNormal);
        NormalFree(BackNormal);
        return false;
    }

    Status = SceneAddObject(Scene,
                            Triangle1,
                            ModelToWorld,
                            Premultiplied);

    if (Status != ISTATUS_SUCCESS)
    {
        DrawingShapeFree(Triangle0);
        DrawingShapeFree(Triangle1);
        NormalFree(FrontNormal);
        NormalFree(BackNormal);
        return false;
    }

    return true;
}