package(default_visibility = ["//visibility:private"])

cc_library(
    name = "hit_allocator",
    hdrs = ["hit_allocator.h"],
    deps = [
        "//iris:hit_allocator",
    ],
)

cc_library(
    name = "iris_physx",
    hdrs = ["iris_physx.h"],
    deps = [
        ":hit_allocator",
        "//iris_advanced:iris_advanced",
        "//iris_spectrum:iris_spectrum",
    ],
    visibility = ["//visibility:public"],
)