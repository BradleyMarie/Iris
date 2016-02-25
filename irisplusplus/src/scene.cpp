/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    scene.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ scene type.

--*/

#include <irisplusplusp.h>

namespace Iris {

Scene & 
Scene::operator=(
    _In_ const Scene & ToCopy
    )
{
    if (this != &ToCopy)
    {
        SceneRelease(Data);
        Data = ToCopy.Data;
        SceneRetain(Data);
    }

    return *this;
}

} // namespace Iris