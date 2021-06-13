load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "constructive_solid_geometry",
    srcs = ["constructive_solid_geometry.c"],
    hdrs = ["constructive_solid_geometry.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "sphere",
    srcs = ["sphere.c"],
    hdrs = ["sphere.h"],
    deps = [
        "//iris_advanced_toolkit:sample_geometry",
        "//iris_physx",
    ],
)

cc_library(
    name = "triangle",
    srcs = ["triangle.c"],
    hdrs = ["triangle.h"],
    deps = [
        ":triangle_mesh",
        "//iris_physx",
    ],
)

cc_library(
    name = "triangle_mesh",
    srcs = ["triangle_mesh.c"],
    hdrs = ["triangle_mesh.h"],
    deps = [
        "//iris_physx",
    ],
)