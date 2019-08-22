package(
    default_visibility = [
        "//iris:__pkg__",
        "//iris_advanced:__pkg__",
        "//iris_advanced_toolkit:__pkg__",
        "//iris_camera:__pkg__",
        "//test_cases:__pkg__",
    ],
)

cc_library(
    name = "cornell_box",
    hdrs = ["cornell_box.h"],
    srcs = ["cornell_box.c"],
    testonly = 1,
    deps = [
        "//iris:iris",
    ],
)

cc_library(
    name = "equality",
    hdrs = ["equality.h"],
    testonly = 1,
    deps = [
        "//iris:iris",
        "//iris_advanced:iris_advanced",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "pfm",
    hdrs = ["pfm.h"],
    srcs = ["pfm.c"],
    testonly = 1,
    deps = [
        "//iris_camera:iris_camera",
        "//iris_camera_toolkit:pfm_writer",
    ],
)

cc_library(
    name = "quad",
    hdrs = ["quad.h"],
    srcs = ["quad.c"],
    testonly = 1,
    deps = [
        "//iris_physx:iris_physx",
        "//iris_physx_toolkit:triangle",
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
    name = "teapot",
    hdrs = ["teapot.h"],
    srcs = ["teapot.c"],
    testonly = 1,
    deps = [
        "//iris:iris",
    ],
)