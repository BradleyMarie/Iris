/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    irisphysx_scene.h

Abstract:

    This file contains the definitions for the SPECTRUM_SCENE type.

--*/

#ifndef _SPECTRUM_SCENE_IRIS_PHYSX_INTERNAL_
#define _SPECTRUM_SCENE_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_SCENE {
    PSCENE Scene;
    POINTER_LIST Lights;
    SIZE_T ReferenceCount;
};

//
// Functions
//

SFORCEINLINE
VOID 
SpectrumSceneGetLights(
    _In_ PCSPECTRUM_SCENE SpectrumScene,
    _Outptr_result_buffer_(NumberOfLights) PCLIGHT *Lights[],
    _Out_ PSIZE_T NumberOfLights
    )
{
	PVOID **PointerArray;

    ASSERT(SpectrumScene != NULL);
    ASSERT(Lights != NULL);
    ASSERT(NumberOfLights != NULL);
    
	PointerArray = (PVOID **) Lights;

	PointerListGetData(&SpectrumScene->Lights,
		               PointerArray,
					   NumberOfLights);
}

#endif // _SPECTRUM_SCENE_IRIS_PHYSX_INTERNAL_