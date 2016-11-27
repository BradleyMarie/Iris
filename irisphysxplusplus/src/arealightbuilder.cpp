/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    arealightbuilder.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ AreaLightBuilder type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

//
// Functions
//

void
AreaLightBuilder::AttachLightToGeometry(
    _In_ SIZE_T GeometryIndex,
    _In_ UINT32 FaceIndex,
    _In_ SIZE_T LightIndex
    )
{
    ISTATUS Status = PhysxAreaLightBuilderAttachLightToGeometry(Data,
                                                                GeometryIndex,
                                                                FaceIndex,
                                                                LightIndex);

    if (Status == ISTATUS_SUCCESS)
    {
        return;
    }

    switch (Status)
    {
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("GeometryIndex");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("FaceIndex");
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("LightIndex");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_00:
            throw std::runtime_error("Mapping Already Exists");
            break;
        default:
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
    }
}

std::tuple<std::vector<Geometry>, std::vector<Light>>
AreaLightBuilder::Build(
    void
    )
{
    std::vector<Geometry> GeometryList;
    std::vector<Light> LightList;
    PPHYSX_GEOMETRY *IrisGeometry;
    PPHYSX_LIGHT *IrisLights;
    SIZE_T NumberOfGeometry;
    SIZE_T NumberOfLights;

    ISTATUS Status = PhysxAreaLightBuilderBuildLightsAndGeometry(Data,
                                                                 &IrisGeometry,
                                                                 &NumberOfGeometry,
                                                                 &IrisLights,
                                                                 &NumberOfLights);
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        default:
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
    }
    
    try
    {
        for (SIZE_T Index = 0; Index < NumberOfGeometry; Index++) 
        {
            GeometryList.emplace_back(Geometry(IrisGeometry[Index], false));
            IrisGeometry[Index] = nullptr;
        }

        free(IrisGeometry);
        IrisGeometry = nullptr;

        for (SIZE_T Index = 0; Index < NumberOfLights; Index++)
        {
            LightList.emplace_back(Light(IrisLights[Index], false));
            IrisLights[Index] = nullptr;
        }

        free(IrisLights);
        IrisLights = nullptr;
    }
    catch (std::bad_alloc e) 
    {
        if (IrisGeometry)
        {
            for (SIZE_T Index = 0; Index < NumberOfGeometry; Index++)
            {
                PhysxGeometryRelease(IrisGeometry[Index]);
            }
        }

        free(IrisGeometry);

        if (IrisLights)
        {
            for (SIZE_T Index = 0; Index < NumberOfLights; Index++)
            {
                PhysxLightRelease(IrisLights[Index]);
            }
        }

        free(IrisLights);

        throw e;
    }

    return make_tuple(GeometryList, LightList);
}

} // namespace IrisPhysx
