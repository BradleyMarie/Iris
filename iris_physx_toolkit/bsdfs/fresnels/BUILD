load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//iris_physx_toolkit:__subpackages__"])

cc_library(
    name = "dielectric",
    srcs = ["dielectric.c"],
    hdrs = ["dielectric.h"],
    deps = [
        ":fresnel"
    ],
)

cc_library(
    name = "fresnel",
    hdrs = ["fresnel.h"],
    deps = [
        "//iris_physx",
    ],
)