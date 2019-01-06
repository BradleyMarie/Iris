package(default_visibility = ["//visibility:private"])

cc_library(
    name = "camera",
    hdrs = ["camera.h"],
    srcs = ["camera.c"],
    deps = [
        ":camera_internal",
        ":camera_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "camera_internal",
    hdrs = ["camera_internal.h"],
    deps = [
        ":camera_vtable",
    ],
)

cc_library(
    name = "camera_vtable",
    hdrs = ["camera_vtable.h"],
    deps = [
        "//common:free_routine",
        "//iris:iris",
    ],
)

cc_library(
    name = "framebuffer_internal",
    hdrs = ["framebuffer_internal.h"],
    deps = [
        "//iris_advanced:iris_advanced",
    ],
)

cc_library(
    name = "framebuffer",
    hdrs = ["framebuffer.h"],
    srcs = ["framebuffer.c"],
    deps = [
        ":framebuffer_internal",
        "//common:safe_math",
        "//iris:iris",
    ],
)

cc_library(
    name = "iris_camera",
    hdrs = ["iris_camera.h"],
    deps = [
        ":camera",
        ":framebuffer",
        ":pixel_sampler",
        ":render",
        ":sample_tracer",
        "//iris_advanced:iris_advanced",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "pixel_sampler",
    hdrs = ["pixel_sampler.h"],
    srcs = ["pixel_sampler.c"],
    deps = [
        ":pixel_sampler_internal",
        ":pixel_sampler_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "pixel_sampler_internal",
    hdrs = ["pixel_sampler_internal.h"],
    deps = [
        ":pixel_sampler_vtable",
    ],
)

cc_library(
    name = "pixel_sampler_vtable",
    hdrs = ["pixel_sampler_vtable.h"],
    deps = [
        "//common:free_routine",
        "//iris_advanced:iris_advanced",
    ],
)

cc_library(
    name = "render",
    hdrs = ["render.h"],
    srcs = ["render.c"],
    copts = ["-fopenmp"],
    linkopts = ["-lgomp"],
    deps = [
        ":camera",
        ":camera_internal",
        ":framebuffer",
        ":framebuffer_internal",
        ":pixel_sampler",
        ":pixel_sampler_internal",
        ":sample_tracer",
        ":sample_tracer_internal",
    ],
)

cc_library(
    name = "sample_tracer",
    hdrs = ["sample_tracer.h"],
    srcs = ["sample_tracer.c"],
    deps = [
        ":sample_tracer_internal",
        ":sample_tracer_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "sample_tracer_internal",
    hdrs = ["sample_tracer_internal.h"],
    deps = [
        ":sample_tracer_vtable",
    ],
)

cc_library(
    name = "sample_tracer_vtable",
    hdrs = ["sample_tracer_vtable.h"],
    deps = [
        "//iris_advanced:iris_advanced",
        "//common:free_routine",
    ],
)