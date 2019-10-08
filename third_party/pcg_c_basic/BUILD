load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

licenses(["notice"])

cc_library(
    name = "pcg_basic",
    srcs = ["pcg_basic.c"],
    hdrs = ["pcg_basic.h"],
    visibility = [
        "//iris_advanced_toolkit:__pkg__",
        "//iris_camera_toolkit:__pkg__",
    ],
)
