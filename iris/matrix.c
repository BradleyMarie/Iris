/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    matrix.c

Abstract:

    The reference counted 4x4 transformation matrix used by Iris.

--*/

#include "iris/matrix.h"

#include <assert.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>

#include "iris/matrix_internal.h"
#include "iris/vector.h"

//
// Types
//

struct _INVERTIBLE_MATRIX {
    MATRIX matrix;
    MATRIX inverse;
    atomic_uintmax_t reference_count;
};

//
// Static Functions
//

static
ISTATUS
Float4x4InverseInitialize(
    _In_ float_t m00,
    _In_ float_t m01,
    _In_ float_t m02,
    _In_ float_t m03,
    _In_ float_t m10,
    _In_ float_t m11,
    _In_ float_t m12,
    _In_ float_t m13,
    _In_ float_t m20,
    _In_ float_t m21,
    _In_ float_t m22,
    _In_ float_t m23,
    _In_ float_t m30,
    _In_ float_t m31,
    _In_ float_t m32,
    _In_ float_t m33,
    _Out_writes_(4) float_t inverse[4][4]
    )
{
    assert(isfinite(m00));
    assert(isfinite(m01));
    assert(isfinite(m02));
    assert(isfinite(m03));
    assert(isfinite(m10));
    assert(isfinite(m11));
    assert(isfinite(m12));
    assert(isfinite(m13));
    assert(isfinite(m20));
    assert(isfinite(m21));
    assert(isfinite(m22));
    assert(isfinite(m23));
    assert(isfinite(m30));
    assert(isfinite(m31));
    assert(isfinite(m32));
    assert(isfinite(m33));
    assert(inverse != NULL);

    inverse[0][0] = m00;
    inverse[0][1] = m01;
    inverse[0][2] = m02;
    inverse[0][3] = m03;
    inverse[1][0] = m10;
    inverse[1][1] = m11;
    inverse[1][2] = m12;
    inverse[1][3] = m13;
    inverse[2][0] = m20;
    inverse[2][1] = m21;
    inverse[2][2] = m22;
    inverse[2][3] = m23;
    inverse[3][0] = m30;
    inverse[3][1] = m31;
    inverse[3][2] = m32;
    inverse[3][3] = m33;

    size_t column_index[] = { 0, 0, 0, 0 };
    size_t row_index[] = { 0, 0, 0, 0 };
    size_t pivot[] = { 0, 0, 0, 0 };

    for (size_t i = 0; i < 4; i++)
    {
        float_t best_candidate = (float_t)0.0;
        size_t best_column = 0;
        size_t best_row = 0;

        for (size_t j = 0; j < 4; j++)
        {
            if (pivot[j] == 1)
            {
                continue;
            }

            for (size_t k = 0; k < 4; k++)
            {
                if (pivot[k] == 1)
                {
                    continue;
                }

                if (pivot[k] > 1)
                {
                    return ISTATUS_ARITHMETIC_ERROR;
                }

                float_t new_candidate = fabs(inverse[j][k]);
                if (new_candidate >= best_candidate)
                {
                    best_candidate = new_candidate;
                    best_row = j;
                    best_column = k;
                }
            }
        }

        pivot[best_column] += 1;

        if (best_row != best_column)
        {
            for (size_t k = 0; k < 4; k++)
            {
                float_t tmp = inverse[best_row][k];
                inverse[best_row][k] = inverse[best_column][k];
                inverse[best_column][k] = tmp;
            }
        }

        if (inverse[best_column][best_column] == (float_t)0.0)
        {
            return ISTATUS_ARITHMETIC_ERROR;
        }

        column_index[i] = best_column;
        row_index[i] = best_row;

        float_t divisor = inverse[best_column][best_column];
        inverse[best_column][best_column] = (float_t)1.0;
        for (size_t j = 0; j < 4; j++)
        {
            inverse[best_column][j] /= divisor;
        }

        for (size_t j = 0; j < 4; j++)
        {
            if (j == best_column)
            {
                continue;
            }

            float_t scalar = -inverse[j][best_column];
            inverse[j][best_column] = (float_t)0.0;
            for (size_t k = 0; k < 4; k++)
            {
                inverse[j][k] += scalar * inverse[best_column][k];
            }
        }
    }

    for (size_t jj = 0; jj < 4; jj++)
    {
        size_t j = 3 - jj;
        if (row_index[j] == column_index[j])
        {
            continue;
        }

        for (size_t k = 0; k < 4; k++)
        {
            float_t tmp = inverse[k][row_index[j]];
            inverse[k][row_index[j]] = inverse[k][column_index[j]];
            inverse[k][column_index[j]] = tmp;
        }
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
InvertibleMatrixInitialize(
    _In_ float_t m00,
    _In_ float_t m01,
    _In_ float_t m02,
    _In_ float_t m03,
    _In_ float_t m10,
    _In_ float_t m11,
    _In_ float_t m12,
    _In_ float_t m13,
    _In_ float_t m20,
    _In_ float_t m21,
    _In_ float_t m22,
    _In_ float_t m23,
    _In_ float_t m30,
    _In_ float_t m31,
    _In_ float_t m32,
    _In_ float_t m33,
    _Out_ PINVERTIBLE_MATRIX matrix
    )
{
    assert(isfinite(m00));
    assert(isfinite(m01));
    assert(isfinite(m02));
    assert(isfinite(m03));
    assert(isfinite(m10));
    assert(isfinite(m11));
    assert(isfinite(m12));
    assert(isfinite(m13));
    assert(isfinite(m20));
    assert(isfinite(m21));
    assert(isfinite(m22));
    assert(isfinite(m23));
    assert(isfinite(m30));
    assert(isfinite(m31));
    assert(isfinite(m32));
    assert(isfinite(m33));
    assert(matrix != NULL);

    ISTATUS status = Float4x4InverseInitialize(m00, m01, m02, m03,
                                               m10, m11, m12, m13,
                                               m20, m21, m22, m23,
                                               m30, m31, m32, m33,
                                               matrix->inverse.m);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }
    
    matrix->inverse.inverse = &matrix->matrix;
    matrix->inverse.invertible_matrix = matrix;

    matrix->matrix.m[0][0] = m00;
    matrix->matrix.m[0][1] = m01;
    matrix->matrix.m[0][2] = m02;
    matrix->matrix.m[0][3] = m03;
    matrix->matrix.m[1][0] = m10;
    matrix->matrix.m[1][1] = m11;
    matrix->matrix.m[1][2] = m12;
    matrix->matrix.m[1][3] = m13;
    matrix->matrix.m[2][0] = m20;
    matrix->matrix.m[2][1] = m21;
    matrix->matrix.m[2][2] = m22;
    matrix->matrix.m[2][3] = m23;
    matrix->matrix.m[3][0] = m30;
    matrix->matrix.m[3][1] = m31;
    matrix->matrix.m[3][2] = m32;
    matrix->matrix.m[3][3] = m33;

    matrix->matrix.inverse = &matrix->inverse;
    matrix->matrix.invertible_matrix = matrix;

    matrix->reference_count = 1;

    return ISTATUS_SUCCESS;
}

static
void
InvertibleMatrixInitializeFromValues(
    _In_ float_t m00,
    _In_ float_t m01,
    _In_ float_t m02,
    _In_ float_t m03,
    _In_ float_t m10,
    _In_ float_t m11,
    _In_ float_t m12,
    _In_ float_t m13,
    _In_ float_t m20,
    _In_ float_t m21,
    _In_ float_t m22,
    _In_ float_t m23,
    _In_ float_t m30,
    _In_ float_t m31,
    _In_ float_t m32,
    _In_ float_t m33,
    _In_ float_t i00,
    _In_ float_t i01,
    _In_ float_t i02,
    _In_ float_t i03,
    _In_ float_t i10,
    _In_ float_t i11,
    _In_ float_t i12,
    _In_ float_t i13,
    _In_ float_t i20,
    _In_ float_t i21,
    _In_ float_t i22,
    _In_ float_t i23,
    _In_ float_t i30,
    _In_ float_t i31,
    _In_ float_t i32,
    _In_ float_t i33,
    _Out_ PINVERTIBLE_MATRIX matrix
    )
{
    assert(isfinite(m00));
    assert(isfinite(m01));
    assert(isfinite(m02));
    assert(isfinite(m03));
    assert(isfinite(m10));
    assert(isfinite(m11));
    assert(isfinite(m12));
    assert(isfinite(m13));
    assert(isfinite(m20));
    assert(isfinite(m21));
    assert(isfinite(m22));
    assert(isfinite(m23));
    assert(isfinite(m30));
    assert(isfinite(m31));
    assert(isfinite(m32));
    assert(isfinite(m33));
    assert(isfinite(i00));
    assert(isfinite(i01));
    assert(isfinite(i02));
    assert(isfinite(i03));
    assert(isfinite(i10));
    assert(isfinite(i11));
    assert(isfinite(i12));
    assert(isfinite(i13));
    assert(isfinite(i20));
    assert(isfinite(i21));
    assert(isfinite(i22));
    assert(isfinite(i23));
    assert(isfinite(i30));
    assert(isfinite(i31));
    assert(isfinite(i32));
    assert(isfinite(i33));
    assert(matrix != NULL);

    matrix->matrix.m[0][0] = m00;
    matrix->matrix.m[0][1] = m01;
    matrix->matrix.m[0][2] = m02;
    matrix->matrix.m[0][3] = m03;
    matrix->matrix.m[1][0] = m10;
    matrix->matrix.m[1][1] = m11;
    matrix->matrix.m[1][2] = m12;
    matrix->matrix.m[1][3] = m13;
    matrix->matrix.m[2][0] = m20;
    matrix->matrix.m[2][1] = m21;
    matrix->matrix.m[2][2] = m22;
    matrix->matrix.m[2][3] = m23;
    matrix->matrix.m[3][0] = m30;
    matrix->matrix.m[3][1] = m31;
    matrix->matrix.m[3][2] = m32;
    matrix->matrix.m[3][3] = m33;

    matrix->matrix.inverse = &matrix->inverse;
    matrix->matrix.invertible_matrix = matrix;
    
    matrix->inverse.m[0][0] = i00;
    matrix->inverse.m[0][1] = i01;
    matrix->inverse.m[0][2] = i02;
    matrix->inverse.m[0][3] = i03;
    matrix->inverse.m[1][0] = i10;
    matrix->inverse.m[1][1] = i11;
    matrix->inverse.m[1][2] = i12;
    matrix->inverse.m[1][3] = i13;
    matrix->inverse.m[2][0] = i20;
    matrix->inverse.m[2][1] = i21;
    matrix->inverse.m[2][2] = i22;
    matrix->inverse.m[2][3] = i23;
    matrix->inverse.m[3][0] = i30;
    matrix->inverse.m[3][1] = i31;
    matrix->inverse.m[3][2] = i32;
    matrix->inverse.m[3][3] = i33;

    matrix->inverse.inverse = &matrix->matrix;
    matrix->inverse.invertible_matrix = matrix;

    matrix->reference_count = 1;
}

static
ISTATUS
MatrixAllocateFromValues(
    _In_ float_t m00,
    _In_ float_t m01,
    _In_ float_t m02,
    _In_ float_t m03,
    _In_ float_t m10,
    _In_ float_t m11,
    _In_ float_t m12,
    _In_ float_t m13,
    _In_ float_t m20,
    _In_ float_t m21,
    _In_ float_t m22,
    _In_ float_t m23,
    _In_ float_t m30,
    _In_ float_t m31,
    _In_ float_t m32,
    _In_ float_t m33,
    _In_ float_t i00,
    _In_ float_t i01,
    _In_ float_t i02,
    _In_ float_t i03,
    _In_ float_t i10,
    _In_ float_t i11,
    _In_ float_t i12,
    _In_ float_t i13,
    _In_ float_t i20,
    _In_ float_t i21,
    _In_ float_t i22,
    _In_ float_t i23,
    _In_ float_t i30,
    _In_ float_t i31,
    _In_ float_t i32,
    _In_ float_t i33,
    _Out_ PMATRIX *matrix
    )
{   
    assert(isfinite(m00));
    assert(isfinite(m01));
    assert(isfinite(m02));
    assert(isfinite(m03));
    assert(isfinite(m10));
    assert(isfinite(m11));
    assert(isfinite(m12));
    assert(isfinite(m13));
    assert(isfinite(m20));
    assert(isfinite(m21));
    assert(isfinite(m22));
    assert(isfinite(m23));
    assert(isfinite(m30));
    assert(isfinite(m31));
    assert(isfinite(m32));
    assert(isfinite(m33));
    assert(isfinite(m00));
    assert(isfinite(i01));
    assert(isfinite(i02));
    assert(isfinite(i03));
    assert(isfinite(i10));
    assert(isfinite(i11));
    assert(isfinite(i12));
    assert(isfinite(i13));
    assert(isfinite(i20));
    assert(isfinite(i21));
    assert(isfinite(i22));
    assert(isfinite(i23));
    assert(isfinite(i30));
    assert(isfinite(i31));
    assert(isfinite(i32));
    assert(isfinite(i33));
    assert(matrix != NULL);

    PINVERTIBLE_MATRIX invertible_matrix = malloc(sizeof(INVERTIBLE_MATRIX));

    if (invertible_matrix == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    InvertibleMatrixInitializeFromValues(m00, m01, m02, m03,
                                         m10, m11, m12, m13,
                                         m20, m21, m22, m23,
                                         m30, m31, m32, m33,
                                         i00, i01, i02, i03,
                                         i10, i11, i12, i13,
                                         i20, i21, i22, i23,
                                         i30, i31, i32, i33,
                                         invertible_matrix);

    *matrix = &invertible_matrix->matrix;

    return ISTATUS_SUCCESS;
}

//
// Public Functions
//

ISTATUS
MatrixAllocate(
    _In_ float_t m00,
    _In_ float_t m01,
    _In_ float_t m02,
    _In_ float_t m03,
    _In_ float_t m10,
    _In_ float_t m11,
    _In_ float_t m12,
    _In_ float_t m13,
    _In_ float_t m20,
    _In_ float_t m21,
    _In_ float_t m22,
    _In_ float_t m23,
    _In_ float_t m30,
    _In_ float_t m31,
    _In_ float_t m32,
    _In_ float_t m33,
    _Out_ PMATRIX *matrix
    )
{
    if (!isfinite(m00))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(m01))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(m02))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(m03))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(m10))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(m11))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(m12))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (!isfinite(m13))
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (!isfinite(m20))
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (!isfinite(m21))
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    if (!isfinite(m22))
    {
        return ISTATUS_INVALID_ARGUMENT_10;
    }

    if (!isfinite(m23))
    {
        return ISTATUS_INVALID_ARGUMENT_11;
    }

    if (!isfinite(m30))
    {
        return ISTATUS_INVALID_ARGUMENT_12;
    }

    if (!isfinite(m31))
    {
        return ISTATUS_INVALID_ARGUMENT_13;
    }

    if (!isfinite(m32))
    {
        return ISTATUS_INVALID_ARGUMENT_14;
    }

    if (!isfinite(m33))
    {
        return ISTATUS_INVALID_ARGUMENT_15;
    }

    if (matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_16;
    }

    PINVERTIBLE_MATRIX invertible_matrix = malloc(sizeof(INVERTIBLE_MATRIX));

    if (invertible_matrix == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status = InvertibleMatrixInitialize(m00, m01, m02, m03,
                                                m10, m11, m12, m13,
                                                m20, m21, m22, m23,
                                                m30, m31, m32, m33,
                                                invertible_matrix);

    if (status != ISTATUS_SUCCESS)
    {
        free(invertible_matrix);
        return status;
    }

    *matrix = &invertible_matrix->matrix;

    return ISTATUS_SUCCESS;
}

ISTATUS
MatrixAllocateTranslation(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PMATRIX *matrix
    )
{
    if (!isfinite(x))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(y))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(z))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ISTATUS status = MatrixAllocateFromValues(
        (float_t) 1.0, (float_t) 0.0, (float_t) 0.0, x,
        (float_t) 0.0, (float_t) 1.0, (float_t) 0.0, y,
        (float_t) 0.0, (float_t) 0.0, (float_t) 1.0, z, 
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        (float_t) 1.0, (float_t) 0.0, (float_t) 0.0, -x,
        (float_t) 0.0, (float_t) 1.0, (float_t) 0.0, -y,
        (float_t) 0.0, (float_t) 0.0, (float_t) 1.0, -z, 
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        matrix);

    return status;
}

ISTATUS
MatrixAllocateScalar(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PMATRIX *matrix
    )
{
    if (!isfinite(x) || x == (float_t) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(y) || y == (float_t) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(z) || z == (float_t) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ISTATUS status = MatrixAllocateFromValues(
        x, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, y, (float_t) 0.0, (float_t) 0.0, 
        (float_t) 0.0, (float_t) 0.0, z, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        (float_t) 1.0 / x, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 1.0 / y, (float_t) 0.0, (float_t) 0.0, 
        (float_t) 0.0, (float_t) 0.0, (float_t) 1.0 / z, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        matrix);

    return status;
}

ISTATUS
MatrixAllocateRotation(
    _In_ float_t theta,
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PMATRIX *matrix
    )
{
    if (!isfinite(theta))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(x))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(y))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(z))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (x == (float_t) 0.0 && y == (float_t) 0.0 && z == (float_t) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    VECTOR3 axis = VectorCreate(x, y, z);
    VECTOR3 normalized = VectorNormalize(axis, NULL, NULL);

#if FLT_EVAL_METHOD	== 0
    double expanded_theta = (double)theta;
#elif FLT_EVAL_METHOD == 1
    long double expanded_theta = (long double)theta;
#elif FLT_EVAL_METHOD == 2
    long double expanded_theta = (long double)theta;
#endif

    float_t sin_theta = sin(expanded_theta);
    float_t cos_theta = cos(expanded_theta);
    float_t ic = (float_t) 1.0 - cos_theta;

    float_t m00 = normalized.x * normalized.x * ic + cos_theta;
    float_t m01 = normalized.x * normalized.y * ic - normalized.z * sin_theta;
    float_t m02 = normalized.x * normalized.z * ic + normalized.y * sin_theta;

    float_t m10 = normalized.y * normalized.x * ic + normalized.z * sin_theta;
    float_t m11 = normalized.y * normalized.y * ic + cos_theta;
    float_t m12 = normalized.y * normalized.z * ic - normalized.x * sin_theta;
    
    float_t m20 = normalized.z * normalized.x * ic - normalized.y * sin_theta;
    float_t m21 = normalized.z * normalized.y * ic + normalized.x * sin_theta;
    float_t m22 = normalized.z * normalized.z * ic + cos_theta;

    ISTATUS status = MatrixAllocateFromValues(
        m00, m01, m02, (float_t) 0.0, 
        m10, m11, m12, (float_t) 0.0,
        m20, m21, m22, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        m00, m10, m20, (float_t) 0.0, 
        m01, m11, m21, (float_t) 0.0,
        m02, m12, m22, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        matrix);

    return status;
}

ISTATUS
MatrixAllocateOrthographic(
    _In_ float_t left,
    _In_ float_t right,
    _In_ float_t bottom,
    _In_ float_t top,
    _In_ float_t near,
    _In_ float_t far,
    _Out_ PMATRIX *matrix
    )
{
    if (!isfinite(left))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(right))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(bottom))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(top))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(near))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(far))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (left == right)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (bottom == top)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
    }

    if (near == far)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
    }

    float_t tx = -(right + left) / (right - left);
    float_t ty = -(top + bottom) / (top - bottom);
    float_t tz = -(far + near) / (far - near);

    float_t sx = (float_t) 2.0 / (right - left);
    float_t sy = (float_t) 2.0 / (top - bottom);
    float_t sz = (float_t) -2.0 / (far - near);

    ISTATUS status = MatrixAllocate(
        sx, (float_t) 0.0, (float_t) 0.0, tx,
        (float_t) 0.0, sy, (float_t) 0.0, ty,
        (float_t) 0.0, (float_t) 0.0, sz, tz,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        matrix);

    return status;
}

ISTATUS
MatrixAllocateFrustum(
    _In_ float_t left,
    _In_ float_t right,
    _In_ float_t bottom,
    _In_ float_t top,
    _In_ float_t near,
    _In_ float_t far,
    _Out_ PMATRIX *matrix
    )
{
    if (!isfinite(left))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(right))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(bottom))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(top))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(near) || near <= (float_t) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(far) || far <= (float_t) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (left == right)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (bottom == top)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
    }

    if (near == far)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
    }

    float_t sx = ((float_t) 2.0 * near) / (right - left);
    float_t sy = ((float_t) 2.0 * near) / (top - bottom);

    float_t a = (right + left) / (right - left);
    float_t b = (top + bottom) / (top - bottom);

    float_t c = -(far + near) / (far - near);
    float_t d = (float_t) -2.0 * far * near / (far - near);

    ISTATUS status = MatrixAllocate(
        sx, (float_t) 0.0, a, (float_t) 0.0,
        (float_t) 0.0, sy, b, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, c, d,
        (float_t) 0.0, (float_t) 0.0, (float_t) -1.0, (float_t) 0.0,
        matrix);

    return status;
}

ISTATUS
MatrixAllocateProduct(
    _In_opt_ PMATRIX multiplicand0,
    _In_opt_ PMATRIX multiplicand1,
    _Out_ PMATRIX *product
    )
{
    if (product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (multiplicand0 == NULL && 
        multiplicand1 == NULL)
    {
        *product = NULL;
        return ISTATUS_SUCCESS;
    }

    if (multiplicand0 == NULL)
    {
        MatrixRetain(multiplicand1);
        *product = multiplicand1;
        return ISTATUS_SUCCESS;
    }

    if (multiplicand1 == NULL)
    {
        MatrixRetain(multiplicand0);
        *product = multiplicand0;
        return ISTATUS_SUCCESS;
    }

    float_t m00 = multiplicand0->m[0][0] * multiplicand1->m[0][0] + 
                  multiplicand0->m[0][1] * multiplicand1->m[1][0] +
                  multiplicand0->m[0][2] * multiplicand1->m[2][0] +
                  multiplicand0->m[0][3] * multiplicand1->m[3][0];

    float_t m01 = multiplicand0->m[0][0] * multiplicand1->m[0][1] + 
                  multiplicand0->m[0][1] * multiplicand1->m[1][1] +
                  multiplicand0->m[0][2] * multiplicand1->m[2][1] +
                  multiplicand0->m[0][3] * multiplicand1->m[3][1];

    float_t m02 = multiplicand0->m[0][0] * multiplicand1->m[0][2] + 
                  multiplicand0->m[0][1] * multiplicand1->m[1][2] +
                  multiplicand0->m[0][2] * multiplicand1->m[2][2] +
                  multiplicand0->m[0][3] * multiplicand1->m[3][2];

    float_t m03 = multiplicand0->m[0][0] * multiplicand1->m[0][3] + 
                  multiplicand0->m[0][1] * multiplicand1->m[1][3] +
                  multiplicand0->m[0][2] * multiplicand1->m[2][3] +
                  multiplicand0->m[0][3] * multiplicand1->m[3][3];

    float_t m10 = multiplicand0->m[1][0] * multiplicand1->m[0][0] + 
                  multiplicand0->m[1][1] * multiplicand1->m[1][0] +
                  multiplicand0->m[1][2] * multiplicand1->m[2][0] +
                  multiplicand0->m[1][3] * multiplicand1->m[3][0];

    float_t m11 = multiplicand0->m[1][0] * multiplicand1->m[0][1] + 
                  multiplicand0->m[1][1] * multiplicand1->m[1][1] +
                  multiplicand0->m[1][2] * multiplicand1->m[2][1] +
                  multiplicand0->m[1][3] * multiplicand1->m[3][1];

    float_t m12 = multiplicand0->m[1][0] * multiplicand1->m[0][2] + 
                  multiplicand0->m[1][1] * multiplicand1->m[1][2] +
                  multiplicand0->m[1][2] * multiplicand1->m[2][2] +
                  multiplicand0->m[1][3] * multiplicand1->m[3][2];

    float_t m13 = multiplicand0->m[1][0] * multiplicand1->m[0][3] + 
                  multiplicand0->m[1][1] * multiplicand1->m[1][3] +
                  multiplicand0->m[1][2] * multiplicand1->m[2][3] +
                  multiplicand0->m[1][3] * multiplicand1->m[3][3];

    float_t m20 = multiplicand0->m[2][0] * multiplicand1->m[0][0] + 
                  multiplicand0->m[2][1] * multiplicand1->m[1][0] +
                  multiplicand0->m[2][2] * multiplicand1->m[2][0] +
                  multiplicand0->m[2][3] * multiplicand1->m[3][0];

    float_t m21 = multiplicand0->m[2][0] * multiplicand1->m[0][1] + 
                  multiplicand0->m[2][1] * multiplicand1->m[1][1] +
                  multiplicand0->m[2][2] * multiplicand1->m[2][1] +
                  multiplicand0->m[2][3] * multiplicand1->m[3][1];

    float_t m22 = multiplicand0->m[2][0] * multiplicand1->m[0][2] + 
                  multiplicand0->m[2][1] * multiplicand1->m[1][2] +
                  multiplicand0->m[2][2] * multiplicand1->m[2][2] +
                  multiplicand0->m[2][3] * multiplicand1->m[3][2];

    float_t m23 = multiplicand0->m[2][0] * multiplicand1->m[0][3] + 
                  multiplicand0->m[2][1] * multiplicand1->m[1][3] +
                  multiplicand0->m[2][2] * multiplicand1->m[2][3] +
                  multiplicand0->m[2][3] * multiplicand1->m[3][3];

    float_t m30 = multiplicand0->m[3][0] * multiplicand1->m[0][0] + 
                  multiplicand0->m[3][1] * multiplicand1->m[1][0] +
                  multiplicand0->m[3][2] * multiplicand1->m[2][0] +
                  multiplicand0->m[3][3] * multiplicand1->m[3][0];

    float_t m31 = multiplicand0->m[3][0] * multiplicand1->m[0][1] + 
                  multiplicand0->m[3][1] * multiplicand1->m[1][1] +
                  multiplicand0->m[3][2] * multiplicand1->m[2][1] +
                  multiplicand0->m[3][3] * multiplicand1->m[3][1];

    float_t m32 = multiplicand0->m[3][0] * multiplicand1->m[0][2] + 
                  multiplicand0->m[3][1] * multiplicand1->m[1][2] +
                  multiplicand0->m[3][2] * multiplicand1->m[2][2] +
                  multiplicand0->m[3][3] * multiplicand1->m[3][2];

    float_t m33 = multiplicand0->m[3][0] * multiplicand1->m[0][3] + 
                  multiplicand0->m[3][1] * multiplicand1->m[1][3] +
                  multiplicand0->m[3][2] * multiplicand1->m[2][3] +
                  multiplicand0->m[3][3] * multiplicand1->m[3][3];

    float_t i00 =
        multiplicand1->inverse->m[0][0] * multiplicand0->inverse->m[0][0] +
        multiplicand1->inverse->m[0][1] * multiplicand0->inverse->m[1][0] +
        multiplicand1->inverse->m[0][2] * multiplicand0->inverse->m[2][0] +
        multiplicand1->inverse->m[0][3] * multiplicand0->inverse->m[3][0];

    float_t i01 =
        multiplicand1->inverse->m[0][0] * multiplicand0->inverse->m[0][1] +
        multiplicand1->inverse->m[0][1] * multiplicand0->inverse->m[1][1] +
        multiplicand1->inverse->m[0][2] * multiplicand0->inverse->m[2][1] +
        multiplicand1->inverse->m[0][3] * multiplicand0->inverse->m[3][1];

    float_t i02 =
        multiplicand1->inverse->m[0][0] * multiplicand0->inverse->m[0][2] +
        multiplicand1->inverse->m[0][1] * multiplicand0->inverse->m[1][2] +
        multiplicand1->inverse->m[0][2] * multiplicand0->inverse->m[2][2] +
        multiplicand1->inverse->m[0][3] * multiplicand0->inverse->m[3][2];

    float_t i03 =
        multiplicand1->inverse->m[0][0] * multiplicand0->inverse->m[0][3] +
        multiplicand1->inverse->m[0][1] * multiplicand0->inverse->m[1][3] +
        multiplicand1->inverse->m[0][2] * multiplicand0->inverse->m[2][3] +
        multiplicand1->inverse->m[0][3] * multiplicand0->inverse->m[3][3];

    float_t i10 =
        multiplicand1->inverse->m[1][0] * multiplicand0->inverse->m[0][0] +
        multiplicand1->inverse->m[1][1] * multiplicand0->inverse->m[1][0] +
        multiplicand1->inverse->m[1][2] * multiplicand0->inverse->m[2][0] +
        multiplicand1->inverse->m[1][3] * multiplicand0->inverse->m[3][0];

    float_t i11 =
        multiplicand1->inverse->m[1][0] * multiplicand0->inverse->m[0][1] +
        multiplicand1->inverse->m[1][1] * multiplicand0->inverse->m[1][1] +
        multiplicand1->inverse->m[1][2] * multiplicand0->inverse->m[2][1] +
        multiplicand1->inverse->m[1][3] * multiplicand0->inverse->m[3][1];

    float_t i12 =
        multiplicand1->inverse->m[1][0] * multiplicand0->inverse->m[0][2] +
        multiplicand1->inverse->m[1][1] * multiplicand0->inverse->m[1][2] +
        multiplicand1->inverse->m[1][2] * multiplicand0->inverse->m[2][2] +
        multiplicand1->inverse->m[1][3] * multiplicand0->inverse->m[3][2];

    float_t i13 =
        multiplicand1->inverse->m[1][0] * multiplicand0->inverse->m[0][3] +
        multiplicand1->inverse->m[1][1] * multiplicand0->inverse->m[1][3] +
        multiplicand1->inverse->m[1][2] * multiplicand0->inverse->m[2][3] +
        multiplicand1->inverse->m[1][3] * multiplicand0->inverse->m[3][3];

    float_t i20 =
        multiplicand1->inverse->m[2][0] * multiplicand0->inverse->m[0][0] +
        multiplicand1->inverse->m[2][1] * multiplicand0->inverse->m[1][0] +
        multiplicand1->inverse->m[2][2] * multiplicand0->inverse->m[2][0] +
        multiplicand1->inverse->m[2][3] * multiplicand0->inverse->m[3][0];

    float_t i21 =
        multiplicand1->inverse->m[2][0] * multiplicand0->inverse->m[0][1] +
        multiplicand1->inverse->m[2][1] * multiplicand0->inverse->m[1][1] +
        multiplicand1->inverse->m[2][2] * multiplicand0->inverse->m[2][1] +
        multiplicand1->inverse->m[2][3] * multiplicand0->inverse->m[3][1];

    float_t i22 =
        multiplicand1->inverse->m[2][0] * multiplicand0->inverse->m[0][2] +
        multiplicand1->inverse->m[2][1] * multiplicand0->inverse->m[1][2] +
        multiplicand1->inverse->m[2][2] * multiplicand0->inverse->m[2][2] +
        multiplicand1->inverse->m[2][3] * multiplicand0->inverse->m[3][2];

    float_t i23 =
        multiplicand1->inverse->m[2][0] * multiplicand0->inverse->m[0][3] +
        multiplicand1->inverse->m[2][1] * multiplicand0->inverse->m[1][3] +
        multiplicand1->inverse->m[2][2] * multiplicand0->inverse->m[2][3] +
        multiplicand1->inverse->m[2][3] * multiplicand0->inverse->m[3][3];

    float_t i30 =
        multiplicand1->inverse->m[3][0] * multiplicand0->inverse->m[0][0] +
        multiplicand1->inverse->m[3][1] * multiplicand0->inverse->m[1][0] +
        multiplicand1->inverse->m[3][2] * multiplicand0->inverse->m[2][0] +
        multiplicand1->inverse->m[3][3] * multiplicand0->inverse->m[3][0];

    float_t i31 =
        multiplicand1->inverse->m[3][0] * multiplicand0->inverse->m[0][1] +
        multiplicand1->inverse->m[3][1] * multiplicand0->inverse->m[1][1] +
        multiplicand1->inverse->m[3][2] * multiplicand0->inverse->m[2][1] +
        multiplicand1->inverse->m[3][3] * multiplicand0->inverse->m[3][1];

    float_t i32 =
        multiplicand1->inverse->m[3][0] * multiplicand0->inverse->m[0][2] +
        multiplicand1->inverse->m[3][1] * multiplicand0->inverse->m[1][2] +
        multiplicand1->inverse->m[3][2] * multiplicand0->inverse->m[2][2] +
        multiplicand1->inverse->m[3][3] * multiplicand0->inverse->m[3][2];

    float_t i33 =
        multiplicand1->inverse->m[3][0] * multiplicand0->inverse->m[0][3] +
        multiplicand1->inverse->m[3][1] * multiplicand0->inverse->m[1][3] +
        multiplicand1->inverse->m[3][2] * multiplicand0->inverse->m[2][3] +
        multiplicand1->inverse->m[3][3] * multiplicand0->inverse->m[3][3];

    ISTATUS status = MatrixAllocateFromValues(m00, m01, m02, m03, 
                                              m10, m11, m12, m13,
                                              m20, m21, m22, m23,
                                              m30, m31, m32, m33,
                                              i00, i01, i02, i03, 
                                              i10, i11, i12, i13,
                                              i20, i21, i22, i23,
                                              i30, i31, i32, i33,
                                              product);

    return status;
}

_Ret_opt_
PMATRIX
MatrixGetInverse(
    _In_opt_ PMATRIX matrix
    )
{
    if (matrix == NULL)
    {
        return NULL;
    }
    
    MatrixRetain(matrix);

    return matrix->inverse;
}

_Ret_opt_
PCMATRIX
MatrixGetConstantInverse(
    _In_opt_ PCMATRIX matrix
    )
{
    if (matrix == NULL)
    {
        return NULL;
    }
    
    return matrix->inverse;
}

ISTATUS
MatrixReadContents(
    _In_opt_ PCMATRIX matrix,
    _Out_writes_(4) float_t contents[4][4]
    )
{
    if (contents == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (matrix == NULL)
    {
        contents[0][0] = (float_t) 1.0;
        contents[0][1] = (float_t) 0.0;
        contents[0][2] = (float_t) 0.0;
        contents[0][3] = (float_t) 0.0;
        contents[1][0] = (float_t) 0.0;
        contents[1][1] = (float_t) 1.0;
        contents[1][2] = (float_t) 0.0;
        contents[1][3] = (float_t) 0.0;
        contents[2][0] = (float_t) 0.0;
        contents[2][1] = (float_t) 0.0;
        contents[2][2] = (float_t) 1.0;
        contents[2][3] = (float_t) 0.0;
        contents[3][0] = (float_t) 0.0;
        contents[3][1] = (float_t) 0.0;
        contents[3][2] = (float_t) 0.0;
        contents[3][3] = (float_t) 1.0;
    }
    else
    {
        contents[0][0] = matrix->m[0][0];
        contents[0][1] = matrix->m[0][1];
        contents[0][2] = matrix->m[0][2];
        contents[0][3] = matrix->m[0][3];
        contents[1][0] = matrix->m[1][0];
        contents[1][1] = matrix->m[1][1];
        contents[1][2] = matrix->m[1][2];
        contents[1][3] = matrix->m[1][3];
        contents[2][0] = matrix->m[2][0];
        contents[2][1] = matrix->m[2][1];
        contents[2][2] = matrix->m[2][2];
        contents[2][3] = matrix->m[2][3];
        contents[3][0] = matrix->m[3][0];
        contents[3][1] = matrix->m[3][1];
        contents[3][2] = matrix->m[3][2];
        contents[3][3] = matrix->m[3][3];
    }
    
    return ISTATUS_SUCCESS;
}

void
MatrixRetain(
    _In_opt_ PMATRIX matrix
    )
{
    if (matrix == NULL)
    {
        return;
    }

    atomic_fetch_add(&matrix->invertible_matrix->reference_count, 1);
}

void
MatrixRelease(
    _In_opt_ _Post_invalid_ PMATRIX matrix
    )
{
    if (matrix == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&matrix->invertible_matrix->reference_count, 1) == 1)
    {
        free(matrix->invertible_matrix);
    }
}