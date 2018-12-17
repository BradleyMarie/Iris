/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    cornell_box.h

Abstract:

    Coordinates of the Cornell box vertices.

--*/

#include "test_util/cornell_box.h"

//
// Camera Data
//

const POINT3 cornell_box_camera_location = {
    (float_t)278.0, (float_t)273.0, (float_t)-800.0
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
    {(float_t)552.8, (float_t)0.0, (float_t)0.0},
    {(float_t)549.6, (float_t)0.0, (float_t)559.2},
    {(float_t)556.0, (float_t)548.8, (float_t)559.2},
    {(float_t)556.0, (float_t)548.8, (float_t)0.0}
};

const POINT3 cornell_box_right_wall[4] = {
    {(float_t)0.0, (float_t)0.0, (float_t)559.2},
    {(float_t)0.0, (float_t)0.0, (float_t)0.0},
    {(float_t)0.0, (float_t)548.8, (float_t)0.0},
    {(float_t)0.0, (float_t)548.8, (float_t)559.2}
};

const POINT3 cornell_box_back_wall[4] = {
    {(float_t)549.6, (float_t)0.0, (float_t)559.2},
    {(float_t)0.0, (float_t)0.0, (float_t)559.2},
    {(float_t)0.0, (float_t)548.8, (float_t)559.2},
    {(float_t)556.0, (float_t)548.8, (float_t)559.2}
};

// Height lowered from 548.8
const POINT3 cornell_box_light[4] = {
    {(float_t)343.0, (float_t)548.75, (float_t)227.0},
    {(float_t)343.0, (float_t)548.75, (float_t)332.0},
    {(float_t)213.0, (float_t)548.75, (float_t)332.0},
    {(float_t)213.0, (float_t)548.75, (float_t)227.0}
};

const POINT3 cornell_box_ceiling[4] = {
    {(float_t)556.0, (float_t)548.8, (float_t)0.0},
    {(float_t)556.0, (float_t)548.8, (float_t)559.2},
    {(float_t)0.0, (float_t)548.8, (float_t)559.2},
    {(float_t)0.0, (float_t)548.8, (float_t)0.0}
};

const POINT3 cornell_box_floor[4] = {
    {(float_t)552.8, (float_t)0.0, (float_t)0.0},
    {(float_t)0.0, (float_t)0.0, (float_t)0.0},
    {(float_t)0.0, (float_t)0.0, (float_t)559.2},
    {(float_t)549.6, (float_t)0.0, (float_t)559.2}
};

const POINT3 cornell_box_short_box[5][4] = {
    {
        {(float_t)130.0, (float_t)165.0, (float_t)65.0},
        {(float_t)82.0, (float_t)165.0, (float_t)225.0},
        {(float_t)240.0, (float_t)165.0, (float_t)272.0},
        {(float_t)290.0, (float_t)165.0, (float_t)114.0}
    },
    {
        {(float_t)290.0, (float_t)0.0, (float_t)114.0},
        {(float_t)290.0, (float_t)165.0, (float_t)114.0},
        {(float_t)240.0, (float_t)165.0, (float_t)272.0},
        {(float_t)240.0, (float_t)0.0, (float_t)272.0}
    },
    {
        {(float_t)130.0, (float_t)0.0, (float_t)65.0},
        {(float_t)130.0, (float_t)165.0, (float_t)65.0},
        {(float_t)290.0, (float_t)165.0, (float_t)114.0},
        {(float_t)290.0, (float_t)0.0, (float_t)114.0}
    },
    {
        {(float_t)82.0, (float_t)0.0, (float_t)225.0},
        {(float_t)82.0, (float_t)165.0, (float_t)225.0},
        {(float_t)130.0, (float_t)165.0, (float_t)65.0},
        {(float_t)130.0, (float_t)0.0, (float_t)65.0}
    },
    {
        {(float_t)240.0, (float_t)0.0, (float_t)272.0},
        {(float_t)240.0, (float_t)165.0, (float_t)272.0},
        {(float_t)82.0, (float_t)165.0, (float_t)225.0},
        {(float_t)82.0, (float_t)0.0, (float_t)225.0}
    }
};

const POINT3 cornell_box_tall_box[5][4] = {
    {
        {(float_t)423.0, (float_t)330.0, (float_t)247.0},
        {(float_t)265.0, (float_t)330.0, (float_t)296.0},
        {(float_t)314.0, (float_t)330.0, (float_t)456.0},
        {(float_t)472.0, (float_t)330.0, (float_t)406.0}
    },
    {
        {(float_t)423.0, (float_t)0.0, (float_t)247.0},
        {(float_t)423.0, (float_t)330.0, (float_t)247.0},
        {(float_t)472.0, (float_t)330.0, (float_t)406.0},
        {(float_t)472.0, (float_t)0.0, (float_t)406.0}
    },
    {
        {(float_t)472.0, (float_t)0.0, (float_t)406.0},
        {(float_t)472.0, (float_t)330.0, (float_t)406.0},
        {(float_t)314.0, (float_t)330.0, (float_t)456.0},
        {(float_t)314.0, (float_t)0.0, (float_t)456.0}
    },
    {
        {(float_t)314.0, (float_t)0.0, (float_t)456.0},
        {(float_t)314.0, (float_t)330.0, (float_t)456.0},
        {(float_t)265.0, (float_t)330.0, (float_t)296.0},
        {(float_t)265.0, (float_t)0.0, (float_t)296.0}
    },
    {
        {(float_t)265.0, (float_t)0.0, (float_t)296.0},
        {(float_t)265.0, (float_t)330.0, (float_t)296.0},
        {(float_t)423.0, (float_t)330.0, (float_t)247.0},
        {(float_t)423.0, (float_t)0.0, (float_t)247.0}
    }
};