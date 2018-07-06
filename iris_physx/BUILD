package(default_visibility = ["//visibility:private"])

cc_library(
    name = "brdf",
    hdrs = ["brdf.h"],
    srcs = ["brdf.c"],
    deps = [
        ":brdf_internal",
        "//common:alloc",
    ],
)

cc_library(
    name = "brdf_allocator",
    hdrs = ["brdf_allocator.h"],
    srcs = ["brdf_allocator.c"],
    deps = [
        ":brdf",
        ":brdf_allocator_internal",
    ],
)

cc_library(
    name = "brdf_allocator_internal",
    hdrs = ["brdf_allocator_internal.h"],
    deps = [
        ":brdf_internal",
        "//common:dynamic_allocator",
    ],
)

cc_library(
    name = "brdf_allocator_context",
    hdrs = ["brdf_allocator_context.h"],
    srcs = ["brdf_allocator_context.c"],
    deps = [
        ":brdf_allocator",
        ":brdf_allocator_internal",
    ],
)

cc_library(
    name = "brdf_internal",
    hdrs = ["brdf_internal.h"],
    deps = [
        ":brdf_vtable",
    ],
)

cc_library(
    name = "brdf_vtable",
    hdrs = ["brdf_vtable.h"],
    deps = [
        "//iris:iris",
        "//iris_advanced:iris_advanced",
        "//iris_spectrum:iris_spectrum",
    ],
)

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
        ":brdf",
        ":brdf_allocator",
        ":brdf_allocator_context",
        ":hit_allocator",
        ":material",
        ":shape",
        "//iris_advanced:iris_advanced",
        "//iris_spectrum:iris_spectrum",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "material",
    hdrs = ["material.h"],
    srcs = ["material.c"],
    deps = [
        ":material_internal",
        ":material_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "material_internal",
    hdrs = ["material_internal.h"],
    deps = [
        ":material_vtable",
    ],
)

cc_library(
    name = "material_vtable",
    hdrs = ["material_vtable.h"],
    deps = [
        ":brdf_allocator",
    ],
)

cc_library(
    name = "shape",
    hdrs = ["shape.h"],
    srcs = ["shape.c"],
    deps = [
        ":shape_internal",
        ":shape_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "shape_internal",
    hdrs = ["shape_internal.h"],
    deps = [
        ":shape_vtable",
    ],
)

cc_library(
    name = "shape_vtable",
    hdrs = ["shape_vtable.h"],
    deps = [
        ":hit_allocator",
        ":material",
    ],
)