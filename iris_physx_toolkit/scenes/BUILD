load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bvh",
    srcs = ["bvh.c"],
    hdrs = ["bvh.h"],
    deps = [
        "//common:safe_math",
        "//iris_physx",
    ],
)

cc_library(
    name = "list",
    srcs = ["list.c"],
    hdrs = ["list.h"],
    deps = [
        "//common:pointer_list",
        "//iris_physx",
    ],
)

cc_library(
    name = "kd_tree",
    srcs = ["kd_tree.c"],
    hdrs = ["kd_tree.h"],
    deps = [
        "//common:pointer_list",
        "//common:safe_math",
        "//iris_physx",
    ],
)