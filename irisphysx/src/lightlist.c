/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    lightlist.c

Abstract:

    This file contains the definitions for the PHYSX_LIGHT_LIST type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_LIGHT_LIST {
    POINTER_LIST Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightListAllocate(
    _Out_ PPHYSX_LIGHT_LIST *LightList
    )
{
    PPHYSX_LIGHT_LIST Result;
    ISTATUS Status;

    if (LightList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Result = malloc(sizeof(PHYSX_LIGHT_LIST));

    if (Result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = PointerListInitialize(&Result->Data);

    if (Status != ISTATUS_SUCCESS)
    {
        free(Result);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    *LightList = Result;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightListAddLight(
    _In_ PPHYSX_LIGHT_LIST LightList,
    _In_ PPHYSX_LIGHT Light
    )
{
    PPHYSX_LIGHT CurrentLight;
    SIZE_T Index;
    SIZE_T Size;
    ISTATUS Status;

    if (LightList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Size = PointerListGetSize(&LightList->Data);

    for (Index = 0; Index < Size; Index++)
    {
        CurrentLight = PointerListRetrieveAtIndex(&LightList->Data, 
                                                  Index);

        if (CurrentLight == Light)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
    }

    Status = PointerListAddPointer(&LightList->Data, Light);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    PhysxLightRetain(Light);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightListGetLight(
    _In_ PCPHYSX_LIGHT_LIST LightList,
    _In_ SIZE_T Index,
    _Out_ PCPHYSX_LIGHT *Light
    )
{
    SIZE_T Size;

    if (LightList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Size = PointerListGetSize(&LightList->Data);

    if (Index >= Size)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *Light = (PCPHYSX_LIGHT) PointerListRetrieveAtIndex(&LightList->Data,
                                                        Index);

    return ISTATUS_SUCCESS;
}

ISTATUS
PhysxLightListGetSize(
    _In_ PCPHYSX_LIGHT_LIST LightList,
    _Out_ PSIZE_T Size
    )
{
    if (LightList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Size == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *Size = PointerListGetSize(&LightList->Data);

    return ISTATUS_SUCCESS;
}

ISTATUS
PhysxLightListClear(
    _In_ PPHYSX_LIGHT_LIST LightList
    )
{
    if (LightList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    PointerListClear(&LightList->Data);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightListGetRandomLight(
    _In_ PCPHYSX_LIGHT_LIST LightList,
    _In_ PRANDOM_REFERENCE Rng,
    _Out_ PCPHYSX_LIGHT *Light
    )
{
    SIZE_T Index;
    SIZE_T Size;
    ISTATUS Status;

    if (LightList == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Size = PointerListGetSize(&LightList->Data);

    Status = RandomReferenceGenerateIndex(Rng,
                                          0,
                                          Size,
                                          &Index);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Light = (PCPHYSX_LIGHT) PointerListRetrieveAtIndex(&LightList->Data,
                                                        Index);

    return ISTATUS_SUCCESS;
}

VOID
PhysxLightListFree(
    _In_opt_ _Post_invalid_ PPHYSX_LIGHT_LIST LightList
    )
{
    PPHYSX_LIGHT CurrentLight;
    SIZE_T Index;
    SIZE_T Size;

    if (LightList == NULL)
    {
        return;
    }

    Size = PointerListGetSize(&LightList->Data);

    for (Index = 0; Index < Size; Index++)
    {
        CurrentLight = PointerListRetrieveAtIndex(&LightList->Data, 
                                                  Index);

        PhysxLightRelease(CurrentLight);
    }

    PointerListDestroy(&LightList->Data);
    free(LightList);
}
