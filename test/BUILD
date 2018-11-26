package(
    default_visibility = [
        "//iris:__pkg__",
        "//iris_advanced:__pkg__",
        "//iris_camera:__pkg__",
    ],
)

cc_test(
    name = "empty_physx",
    srcs = ["empty_physx.cc"],
    deps = [
        ":spectra",
        "//iris_advanced_toolkit:pcg_random",
        "//iris_camera_toolkit:grid_pixel_sampler",
        "//iris_camera_toolkit:pinhole_camera",
        "//iris_physx_camera_toolkit:physx_sample_tracer",
        "//iris_physx_toolkit:all_light_sampler",
        "//iris_physx_toolkit:list_scene",
        "//iris_physx_toolkit:path_tracer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "spectra",
    hdrs = ["spectra.h"],
    srcs = ["spectra.c"],
    testonly = 1,
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "test_util",
    hdrs = ["test_util.h"],
    testonly = 1,
    deps = [
        "//iris:iris",
        "//iris_camera:iris_camera",
        "@com_google_googletest//:gtest_main",
    ],
)