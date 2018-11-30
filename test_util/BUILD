package(
    default_visibility = [
        "//iris:__pkg__",
        "//iris_advanced:__pkg__",
        "//iris_camera:__pkg__",
        "//test_cases:__pkg__",
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
    name = "spectra",
    hdrs = ["spectra.h"],
    srcs = ["spectra.c"],
    testonly = 1,
    deps = [
        "//iris_physx:iris_physx",
    ],
)