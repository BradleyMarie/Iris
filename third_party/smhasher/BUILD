load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "murmur2",
    srcs = ["MurmurHash2.c"],
    hdrs = ["MurmurHash2.h"],
    visibility = [
        "//iris_physx:__pkg__",
        "//iris_physx_toolkit:__pkg__",
    ],
)

cc_library(
    name = "murmur3",
    srcs = ["MurmurHash3.c"],
    hdrs = ["MurmurHash3.h"],
    visibility = [
        "//iris_physx:__pkg__",
        "//iris_physx_toolkit:__pkg__",
    ],
)
