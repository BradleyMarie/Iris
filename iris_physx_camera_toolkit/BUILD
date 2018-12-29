package(default_visibility = ["//visibility:public"])

cc_library(
    name = "integrator_generator",
    hdrs = ["integrator_generator.h"],
    srcs = ["integrator_generator.c"],
    deps = [
        ":integrator_generator_internal",
        ":integrator_generator_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "integrator_generator_internal",
    hdrs = ["integrator_generator_internal.h"],
    deps = [
        ":integrator_generator_vtable"
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "integrator_generator_vtable",
    hdrs = ["integrator_generator_vtable.h"],
    deps = [
        "//iris_physx:iris_physx"
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