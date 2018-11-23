package(
    default_visibility = [
        "//iris:__pkg__",
        "//iris_advanced:__pkg__",
        "//iris_camera:__pkg__",
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