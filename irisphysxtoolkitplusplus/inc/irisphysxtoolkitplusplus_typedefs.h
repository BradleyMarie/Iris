/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus_typedefs.h

Abstract:

    This file contains the definition some typedefs.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _TYPEDEFS_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
#define _TYPEDEFS_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_

namespace IrisPhysxToolkit {

//
// Types
//

typedef std::function<IrisAdvanced::Color3(IrisSpectrum::SpectrumReference)> ToneMappingRoutine;

typedef std::function<void(std::vector<IrisAdvanced::RandomReference> &,
                           std::vector<IrisPhysx::ProcessHitRoutine> &,
                           std::vector<ToneMappingRoutine> &,
                           SIZE_T NumberOfThreads)> CreateStateRoutine;

} // namespace IrisPhysxToolkit

#endif // _TYPEDEFS_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_