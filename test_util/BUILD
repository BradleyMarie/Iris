load("@rules_cc//cc:defs.bzl", "cc_library")

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
    testonly = 1,
    srcs = ["cornell_box.c"],
    hdrs = ["cornell_box.h"],
    deps = [
        "//iris",
    ],
)

cc_library(
    name = "equality",
    testonly = 1,
    hdrs = ["equality.h"],
    deps = [
        "//iris",
        "//iris_advanced",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "pfm",
    testonly = 1,
    srcs = ["pfm.c"],
    hdrs = ["pfm.h"],
    deps = [
        "//iris_camera",
    ],
)

cc_library(
    name = "quad",
    testonly = 1,
    srcs = ["quad.c"],
    hdrs = ["quad.h"],
    deps = [
        "//iris_physx",
        "//iris_physx_toolkit:triangle",
    ],
)

cc_library(
    name = "spectra",
    testonly = 1,
    srcs = ["spectra.c"],
    hdrs = ["spectra.h"],
    deps = [
        "//iris_physx",
        "//iris_physx_toolkit:color_extrapolator",
    ],
)

cc_library(
    name = "teapot",
    testonly = 1,
    srcs = ["teapot.c"],
    hdrs = ["teapot.h"],
    deps = [
        "//iris",
    ],
)
