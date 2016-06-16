/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_math.h

Abstract:

    This file contains the internal definitions for the math routines.

--*/

#include <irisp.h>
    
#ifndef _MATH_IRIS_INTERNAL_
#define _MATH_IRIS_INTERNAL_

//
// Functions
//

SFORCEINLINE
VOID
Float4x4DivideRow(
    _Inout_ FLOAT Matrix[4][4],
    _In_range_(0, 3) SIZE_T Row,
    _In_ FLOAT Divisor
    )
{
    FLOAT Scalar;

    ASSERT(Matrix != NULL);
    ASSERT(0 <= Row);
    ASSERT(Row <= 3);
    ASSERT(IsFiniteFloat(Divisor) != FALSE);
    ASSERT(IsNotZeroFloat(Divisor) != FALSE);

    Scalar = (FLOAT) 1.0 / Divisor;

    Matrix[Row][0] *= Scalar;
    Matrix[Row][1] *= Scalar;
    Matrix[Row][2] *= Scalar;
    Matrix[Row][3] *= Scalar;
}

SFORCEINLINE
VOID
Float4x4ScaledSubtractRow(
    _Inout_ FLOAT Matrix[4][4],
    _In_range_(0, 3) SIZE_T ConstantRow,
    _In_range_(0, 3) SIZE_T ModifiedRow,
    _In_ FLOAT Scalar
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(0 <= ConstantRow);
    ASSERT(ConstantRow <= 3);
    ASSERT(0 <= ModifiedRow);
    ASSERT(ModifiedRow <= 3);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);

    Matrix[ModifiedRow][0] -= Matrix[ConstantRow][0] * Scalar;
    Matrix[ModifiedRow][1] -= Matrix[ConstantRow][1] * Scalar;
    Matrix[ModifiedRow][2] -= Matrix[ConstantRow][2] * Scalar;
    Matrix[ModifiedRow][3] -= Matrix[ConstantRow][3] * Scalar;
}

SFORCEINLINE
VOID
Float4x4SwapRows(
    _Inout_ FLOAT Matrix[4][4],
    _In_range_(0, 3) SIZE_T Row0,
    _In_range_(0, 3) SIZE_T Row1
    )
{
    SIZE_T Index;
    FLOAT Temp;

    ASSERT(Matrix != NULL);
    ASSERT(0 <= Row0);
    ASSERT(Row0 <= 3);
    ASSERT(0 <= Row1);
    ASSERT(Row1 <= 3);

    for (Index = 0; Index < 4; Index++)
    {
        Temp = Matrix[Row0][Index];
        Matrix[Row0][Index] = Matrix[Row1][Index];
        Matrix[Row1][Index] = Temp;
    }
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
Float4x4InverseInitialize(
    _In_ FLOAT M00,
    _In_ FLOAT M01,
    _In_ FLOAT M02,
    _In_ FLOAT M03,
    _In_ FLOAT M10,
    _In_ FLOAT M11,
    _In_ FLOAT M12,
    _In_ FLOAT M13,
    _In_ FLOAT M20,
    _In_ FLOAT M21,
    _In_ FLOAT M22,
    _In_ FLOAT M23,
    _In_ FLOAT M30,
    _In_ FLOAT M31,
    _In_ FLOAT M32,
    _In_ FLOAT M33,
    _Out_writes_(4) FLOAT Inverse[4][4]
    )
{
    SIZE_T ColumnIndex;
    SIZE_T CurrentRow;
    FLOAT Matrix[4][4];
    SIZE_T RowIndex;

    ASSERT(IsFiniteFloat(M00) != FALSE);
    ASSERT(IsFiniteFloat(M01) != FALSE);
    ASSERT(IsFiniteFloat(M02) != FALSE);
    ASSERT(IsFiniteFloat(M03) != FALSE);
    ASSERT(IsFiniteFloat(M10) != FALSE);
    ASSERT(IsFiniteFloat(M11) != FALSE);
    ASSERT(IsFiniteFloat(M12) != FALSE);
    ASSERT(IsFiniteFloat(M13) != FALSE);
    ASSERT(IsFiniteFloat(M20) != FALSE);
    ASSERT(IsFiniteFloat(M21) != FALSE);
    ASSERT(IsFiniteFloat(M22) != FALSE);
    ASSERT(IsFiniteFloat(M23) != FALSE);
    ASSERT(IsFiniteFloat(M30) != FALSE);
    ASSERT(IsFiniteFloat(M31) != FALSE);
    ASSERT(IsFiniteFloat(M32) != FALSE);
    ASSERT(IsFiniteFloat(M33) != FALSE);
    ASSERT(Inverse != NULL);

    Inverse[0][0] = (FLOAT) 1.0;
    Inverse[0][1] = (FLOAT) 0.0;
    Inverse[0][2] = (FLOAT) 0.0;
    Inverse[0][3] = (FLOAT) 0.0;
    Inverse[1][0] = (FLOAT) 0.0;
    Inverse[1][1] = (FLOAT) 1.0;
    Inverse[1][2] = (FLOAT) 0.0;
    Inverse[1][3] = (FLOAT) 0.0;
    Inverse[2][0] = (FLOAT) 0.0;
    Inverse[2][1] = (FLOAT) 0.0;
    Inverse[2][2] = (FLOAT) 1.0;
    Inverse[2][3] = (FLOAT) 0.0;
    Inverse[3][0] = (FLOAT) 0.0;
    Inverse[3][1] = (FLOAT) 0.0;
    Inverse[3][2] = (FLOAT) 0.0;
    Inverse[3][3] = (FLOAT) 1.0;

    Matrix[0][0] = M00;
    Matrix[0][1] = M01;
    Matrix[0][2] = M02;
    Matrix[0][3] = M03;
    Matrix[1][0] = M10;
    Matrix[1][1] = M11;
    Matrix[1][2] = M12;
    Matrix[1][3] = M13;
    Matrix[2][0] = M20;
    Matrix[2][1] = M21;
    Matrix[2][2] = M22;
    Matrix[2][3] = M23;
    Matrix[3][0] = M30;
    Matrix[3][1] = M31;
    Matrix[3][2] = M32;
    Matrix[3][3] = M33;

    for (CurrentRow = 0, ColumnIndex = 0; 
         CurrentRow < 4 && ColumnIndex < 4; 
         CurrentRow++, ColumnIndex++)
    {
        for (; // No Initialization Needed
             ColumnIndex < 4;
             ColumnIndex++)
        {
            for (RowIndex = CurrentRow;
                 RowIndex < 4;
                 RowIndex++)
            {
                if (IsZeroFloat(Matrix[RowIndex][ColumnIndex]) == FALSE)
                {
                    break;
                }
            }

            if (RowIndex != 4 &&
                IsZeroFloat(Matrix[RowIndex][ColumnIndex]) == FALSE)
            {
                break;
            }
        }

        if (ColumnIndex == 4)
        {
            return ISTATUS_ARITHMETIC_ERROR;
        }

        if (RowIndex != CurrentRow)
        {
            Float4x4SwapRows(Matrix, RowIndex, CurrentRow);
            Float4x4SwapRows(Inverse, RowIndex, CurrentRow);
        }

        Float4x4DivideRow(Inverse, CurrentRow, Matrix[CurrentRow][ColumnIndex]);
        Float4x4DivideRow(Matrix, CurrentRow, Matrix[CurrentRow][ColumnIndex]);

        for (RowIndex = 0; RowIndex < 4; RowIndex++)
        {
            if (RowIndex == CurrentRow)
            {
                continue;
            }

            Float4x4ScaledSubtractRow(Inverse, 
                                      CurrentRow, 
                                      RowIndex,
                                      Matrix[RowIndex][ColumnIndex]);

            Float4x4ScaledSubtractRow(Matrix,
                                      CurrentRow,
                                      RowIndex,
                                      Matrix[RowIndex][ColumnIndex]);
        }
    }

    return ISTATUS_SUCCESS;
}

#endif // _MATH_IRIS_INTERNAL_