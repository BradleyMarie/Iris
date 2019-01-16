/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    cornell_box.h

Abstract:

    The parameters of the Cornell box scene transformed to be left-handed.

--*/

#include "test_util/cornell_box.h"

//
// Camera Data
//

const POINT3 cornell_box_camera_location = {
    (float_t)-278.0, (float_t)273.0, (float_t)-800.0
};

const VECTOR3 cornell_box_camera_up = {
    (float_t)0.0, (float_t)1.0, (float_t)0.0
};

const VECTOR3 cornell_box_camera_direction = {
    (float_t)0.0, (float_t)0.0, (float_t)1.0
};

float_t cornell_box_focal_length = (float_t)0.035;
float_t cornell_box_camera_width = (float_t)0.025;
float_t cornell_box_camera_height = (float_t)0.025;

//
// Scene Data
//

const POINT3 cornell_box_left_wall[4] = {
    {(float_t)-552.8, (float_t)0.0, (float_t)0.0},
    {(float_t)-549.6, (float_t)0.0, (float_t)559.2},
    {(float_t)-556.0, (float_t)548.8, (float_t)559.2},
    {(float_t)-556.0, (float_t)548.8, (float_t)0.0}
};

const POINT3 cornell_box_right_wall[4] = {
    {(float_t)0.0, (float_t)0.0, (float_t)559.2},
    {(float_t)0.0, (float_t)0.0, (float_t)0.0},
    {(float_t)0.0, (float_t)548.8, (float_t)0.0},
    {(float_t)0.0, (float_t)548.8, (float_t)559.2}
};

const POINT3 cornell_box_back_wall[4] = {
    {(float_t)-549.6, (float_t)0.0, (float_t)559.2},
    {(float_t)0.0, (float_t)0.0, (float_t)559.2},
    {(float_t)0.0, (float_t)548.8, (float_t)559.2},
    {(float_t)-556.0, (float_t)548.8, (float_t)559.2}
};

// Height lowered from 548.8
const POINT3 cornell_box_light[4] = {
    {(float_t)-343.0, (float_t)548.75, (float_t)227.0},
    {(float_t)-343.0, (float_t)548.75, (float_t)332.0},
    {(float_t)-213.0, (float_t)548.75, (float_t)332.0},
    {(float_t)-213.0, (float_t)548.75, (float_t)227.0}
};

const POINT3 cornell_box_ceiling[4] = {
    {(float_t)-556.0, (float_t)548.8, (float_t)0.0},
    {(float_t)-556.0, (float_t)548.8, (float_t)559.2},
    {(float_t)0.0, (float_t)548.8, (float_t)559.2},
    {(float_t)0.0, (float_t)548.8, (float_t)0.0}
};

const POINT3 cornell_box_floor[4] = {
    {(float_t)-552.8, (float_t)0.0, (float_t)0.0},
    {(float_t)0.0, (float_t)0.0, (float_t)0.0},
    {(float_t)0.0, (float_t)0.0, (float_t)559.2},
    {(float_t)-549.6, (float_t)0.0, (float_t)559.2}
};

const POINT3 cornell_box_short_box[5][4] = {
    {
        {(float_t)-130.0, (float_t)165.0, (float_t)65.0},
        {(float_t)-82.0, (float_t)165.0, (float_t)225.0},
        {(float_t)-240.0, (float_t)165.0, (float_t)272.0},
        {(float_t)-290.0, (float_t)165.0, (float_t)114.0}
    },
    {
        {(float_t)-290.0, (float_t)0.0, (float_t)114.0},
        {(float_t)-290.0, (float_t)165.0, (float_t)114.0},
        {(float_t)-240.0, (float_t)165.0, (float_t)272.0},
        {(float_t)-240.0, (float_t)0.0, (float_t)272.0}
    },
    {
        {(float_t)-130.0, (float_t)0.0, (float_t)65.0},
        {(float_t)-130.0, (float_t)165.0, (float_t)65.0},
        {(float_t)-290.0, (float_t)165.0, (float_t)114.0},
        {(float_t)-290.0, (float_t)0.0, (float_t)114.0}
    },
    {
        {(float_t)-82.0, (float_t)0.0, (float_t)225.0},
        {(float_t)-82.0, (float_t)165.0, (float_t)225.0},
        {(float_t)-130.0, (float_t)165.0, (float_t)65.0},
        {(float_t)-130.0, (float_t)0.0, (float_t)65.0}
    },
    {
        {(float_t)-240.0, (float_t)0.0, (float_t)272.0},
        {(float_t)-240.0, (float_t)165.0, (float_t)272.0},
        {(float_t)-82.0, (float_t)165.0, (float_t)225.0},
        {(float_t)-82.0, (float_t)0.0, (float_t)225.0}
    }
};

const POINT3 cornell_box_tall_box[5][4] = {
    {
        {(float_t)-423.0, (float_t)330.0, (float_t)247.0},
        {(float_t)-265.0, (float_t)330.0, (float_t)296.0},
        {(float_t)-314.0, (float_t)330.0, (float_t)456.0},
        {(float_t)-472.0, (float_t)330.0, (float_t)406.0}
    },
    {
        {(float_t)-423.0, (float_t)0.0, (float_t)247.0},
        {(float_t)-423.0, (float_t)330.0, (float_t)247.0},
        {(float_t)-472.0, (float_t)330.0, (float_t)406.0},
        {(float_t)-472.0, (float_t)0.0, (float_t)406.0}
    },
    {
        {(float_t)-472.0, (float_t)0.0, (float_t)406.0},
        {(float_t)-472.0, (float_t)330.0, (float_t)406.0},
        {(float_t)-314.0, (float_t)330.0, (float_t)456.0},
        {(float_t)-314.0, (float_t)0.0, (float_t)456.0}
    },
    {
        {(float_t)-314.0, (float_t)0.0, (float_t)456.0},
        {(float_t)-314.0, (float_t)330.0, (float_t)456.0},
        {(float_t)-265.0, (float_t)330.0, (float_t)296.0},
        {(float_t)-265.0, (float_t)0.0, (float_t)296.0}
    },
    {
        {(float_t)-265.0, (float_t)0.0, (float_t)296.0},
        {(float_t)-265.0, (float_t)330.0, (float_t)296.0},
        {(float_t)-423.0, (float_t)330.0, (float_t)247.0},
        {(float_t)-423.0, (float_t)0.0, (float_t)247.0}
    }
};

//
// Shading Data
//

const float_t light_spectrum_wavelengths[4] = {
    (float_t)400.0,
    (float_t)500.0,
    (float_t)600.0,
    (float_t)700.0
};

const float_t light_spectrum_samples[4] = {
    (float_t)0.0,
    (float_t)8.0,
    (float_t)15.6,
    (float_t)18.4
};

const float_t wall_reflection_wavelengths[76] = {
    (float_t)400.0,
    (float_t)404.0,
    (float_t)408.0,
    (float_t)412.0,
    (float_t)416.0,
    (float_t)420.0,
    (float_t)424.0,
    (float_t)428.0,
    (float_t)432.0,
    (float_t)436.0,
    (float_t)440.0,
    (float_t)444.0,
    (float_t)448.0,
    (float_t)452.0,
    (float_t)456.0,
    (float_t)460.0,
    (float_t)464.0,
    (float_t)468.0,
    (float_t)472.0,
    (float_t)476.0,
    (float_t)480.0,
    (float_t)484.0,
    (float_t)488.0,
    (float_t)492.0,
    (float_t)496.0,
    (float_t)500.0,
    (float_t)504.0,
    (float_t)508.0,
    (float_t)512.0,
    (float_t)516.0,
    (float_t)520.0,
    (float_t)524.0,
    (float_t)528.0,
    (float_t)532.0,
    (float_t)536.0,
    (float_t)540.0,
    (float_t)544.0,
    (float_t)548.0,
    (float_t)552.0,
    (float_t)556.0,
    (float_t)560.0,
    (float_t)564.0,
    (float_t)568.0,
    (float_t)572.0,
    (float_t)576.0,
    (float_t)580.0,
    (float_t)584.0,
    (float_t)588.0,
    (float_t)592.0,
    (float_t)596.0,
    (float_t)600.0,
    (float_t)604.0,
    (float_t)608.0,
    (float_t)612.0,
    (float_t)616.0,
    (float_t)620.0,
    (float_t)624.0,
    (float_t)628.0,
    (float_t)632.0,
    (float_t)636.0,
    (float_t)640.0,
    (float_t)644.0,
    (float_t)648.0,
    (float_t)652.0,
    (float_t)656.0,
    (float_t)660.0,
    (float_t)664.0,
    (float_t)668.0,
    (float_t)672.0,
    (float_t)676.0,
    (float_t)680.0,
    (float_t)684.0,
    (float_t)688.0,
    (float_t)692.0,
    (float_t)696.0,
    (float_t)700.0
};

const float_t red_wall_reflection_samples[76] = {
    (float_t)0.04,
    (float_t)0.046,
    (float_t)0.048,
    (float_t)0.053,
    (float_t)0.049,
    (float_t)0.05,
    (float_t)0.053,
    (float_t)0.055,
    (float_t)0.057,
    (float_t)0.056,
    (float_t)0.059,
    (float_t)0.057,
    (float_t)0.061,
    (float_t)0.061,
    (float_t)0.06,
    (float_t)0.062,
    (float_t)0.062,
    (float_t)0.062,
    (float_t)0.061,
    (float_t)0.062,
    (float_t)0.06,
    (float_t)0.059,
    (float_t)0.057,
    (float_t)0.058,
    (float_t)0.058,
    (float_t)0.058,
    (float_t)0.056,
    (float_t)0.055,
    (float_t)0.056,
    (float_t)0.059,
    (float_t)0.057,
    (float_t)0.055,
    (float_t)0.059,
    (float_t)0.059,
    (float_t)0.058,
    (float_t)0.059,
    (float_t)0.061,
    (float_t)0.061,
    (float_t)0.063,
    (float_t)0.063,
    (float_t)0.067,
    (float_t)0.068,
    (float_t)0.072,
    (float_t)0.08,
    (float_t)0.09,
    (float_t)0.099,
    (float_t)0.124,
    (float_t)0.154,
    (float_t)0.192,
    (float_t)0.255,
    (float_t)0.287,
    (float_t)0.349,
    (float_t)0.402,
    (float_t)0.443,
    (float_t)0.487,
    (float_t)0.513,
    (float_t)0.558,
    (float_t)0.584,
    (float_t)0.62,
    (float_t)0.606,
    (float_t)0.609,
    (float_t)0.651,
    (float_t)0.612,
    (float_t)0.61,
    (float_t)0.65,
    (float_t)0.638,
    (float_t)0.627,
    (float_t)0.62,
    (float_t)0.63,
    (float_t)0.628,
    (float_t)0.642,
    (float_t)0.639,
    (float_t)0.657,
    (float_t)0.639,
    (float_t)0.635,
    (float_t)0.642
};

const float_t green_wall_reflection_samples[76] = {
    (float_t)0.092,
    (float_t)0.096,
    (float_t)0.098,
    (float_t)0.097,
    (float_t)0.098,
    (float_t)0.095,
    (float_t)0.095,
    (float_t)0.097,
    (float_t)0.095,
    (float_t)0.094,
    (float_t)0.097,
    (float_t)0.098,
    (float_t)0.096,
    (float_t)0.101,
    (float_t)0.103,
    (float_t)0.104,
    (float_t)0.107,
    (float_t)0.109,
    (float_t)0.112,
    (float_t)0.115,
    (float_t)0.125,
    (float_t)0.14,
    (float_t)0.16,
    (float_t)0.187,
    (float_t)0.229,
    (float_t)0.285,
    (float_t)0.343,
    (float_t)0.39,
    (float_t)0.435,
    (float_t)0.464,
    (float_t)0.472,
    (float_t)0.476,
    (float_t)0.481,
    (float_t)0.462,
    (float_t)0.447,
    (float_t)0.441,
    (float_t)0.426,
    (float_t)0.406,
    (float_t)0.373,
    (float_t)0.347,
    (float_t)0.337,
    (float_t)0.314,
    (float_t)0.285,
    (float_t)0.277,
    (float_t)0.266,
    (float_t)0.25,
    (float_t)0.23,
    (float_t)0.207,
    (float_t)0.186,
    (float_t)0.171,
    (float_t)0.16,
    (float_t)0.148,
    (float_t)0.141,
    (float_t)0.136,
    (float_t)0.13,
    (float_t)0.126,
    (float_t)0.123,
    (float_t)0.121,
    (float_t)0.122,
    (float_t)0.119,
    (float_t)0.114,
    (float_t)0.115,
    (float_t)0.117,
    (float_t)0.117,
    (float_t)0.118,
    (float_t)0.12,
    (float_t)0.122,
    (float_t)0.128,
    (float_t)0.132,
    (float_t)0.139,
    (float_t)0.144,
    (float_t)0.146,
    (float_t)0.15,
    (float_t)0.152,
    (float_t)0.157,
    (float_t)0.159
};

const float_t white_wall_reflection_samples[76] = {
    (float_t)0.343,
    (float_t)0.445,
    (float_t)0.551,
    (float_t)0.624,
    (float_t)0.665,
    (float_t)0.687,
    (float_t)0.708,
    (float_t)0.723,
    (float_t)0.715,
    (float_t)0.71,
    (float_t)0.745,
    (float_t)0.758,
    (float_t)0.739,
    (float_t)0.767,
    (float_t)0.777,
    (float_t)0.765,
    (float_t)0.751,
    (float_t)0.745,
    (float_t)0.748,
    (float_t)0.729,
    (float_t)0.745,
    (float_t)0.757,
    (float_t)0.753,
    (float_t)0.75,
    (float_t)0.746,
    (float_t)0.747,
    (float_t)0.735,
    (float_t)0.732,
    (float_t)0.739,
    (float_t)0.734,
    (float_t)0.725,
    (float_t)0.721,
    (float_t)0.733,
    (float_t)0.725,
    (float_t)0.732,
    (float_t)0.743,
    (float_t)0.744,
    (float_t)0.748,
    (float_t)0.728,
    (float_t)0.716,
    (float_t)0.733,
    (float_t)0.726,
    (float_t)0.713,
    (float_t)0.74,
    (float_t)0.754,
    (float_t)0.764,
    (float_t)0.752,
    (float_t)0.736,
    (float_t)0.734,
    (float_t)0.741,
    (float_t)0.74,
    (float_t)0.732,
    (float_t)0.745,
    (float_t)0.755,
    (float_t)0.751,
    (float_t)0.744,
    (float_t)0.731,
    (float_t)0.733,
    (float_t)0.744,
    (float_t)0.731,
    (float_t)0.712,
    (float_t)0.708,
    (float_t)0.729,
    (float_t)0.73,
    (float_t)0.727,
    (float_t)0.707,
    (float_t)0.703,
    (float_t)0.729,
    (float_t)0.75,
    (float_t)0.76,
    (float_t)0.751,
    (float_t)0.739,
    (float_t)0.724,
    (float_t)0.73,
    (float_t)0.74,
    (float_t)0.737
};