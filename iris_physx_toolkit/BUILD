package(default_visibility = ["//visibility:public"])

cc_library(
    name = "all_light_sampler",
    hdrs = ["all_light_sampler.h"],
    srcs = ["all_light_sampler.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "constant_material",
    hdrs = ["constant_material.h"],
    srcs = ["constant_material.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "lambertian_brdf",
    hdrs = ["lambertian_brdf.h"],
    srcs = ["lambertian_brdf.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "list_scene",
    hdrs = ["list_scene.h"],
    srcs = ["list_scene.c"],
    deps = [
        "//common:pointer_list",
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "metric_black_body",
    hdrs = ["metric_black_body.h"],
    srcs = ["metric_black_body.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "path_tracer",
    hdrs = ["path_tracer.h"],
    srcs = ["path_tracer.c"],
    deps = [
        ":sample_direct_lighting",
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "point_light",
    hdrs = ["point_light.h"],
    srcs = ["point_light.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "sample_direct_lighting",
    hdrs = ["sample_direct_lighting.h"],
    srcs = ["sample_direct_lighting.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "sample_tracer",
    hdrs = ["sample_tracer.h"],
    srcs = ["sample_tracer.c"],
    deps = [
        "//iris_camera:iris_camera",
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "sphere",
    hdrs = ["sphere.h"],
    srcs = ["sphere.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "uniform_reflector",
    hdrs = ["uniform_reflector.h"],
    srcs = ["uniform_reflector.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)