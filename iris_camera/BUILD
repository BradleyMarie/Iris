load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "camera",
    srcs = ["camera.c"],
    hdrs = ["camera.h"],
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
        "//iris_advanced",
    ],
)

cc_library(
    name = "framebuffer_internal",
    hdrs = ["framebuffer_internal.h"],
    deps = [
        "//iris_advanced",
    ],
)

cc_library(
    name = "framebuffer",
    srcs = ["framebuffer.c"],
    hdrs = ["framebuffer.h"],
    deps = [
        ":framebuffer_internal",
        "//common:safe_math",
        "//iris",
    ],
)

cc_library(
    name = "image_sampler",
    srcs = ["image_sampler.c"],
    hdrs = ["image_sampler.h"],
    deps = [
        ":image_sampler_internal",
        ":image_sampler_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "image_sampler_internal",
    hdrs = ["image_sampler_internal.h"],
    deps = [
        ":image_sampler_vtable",
    ],
)

cc_library(
    name = "image_sampler_vtable",
    hdrs = ["image_sampler_vtable.h"],
    deps = [
        "//common:free_routine",
        "//iris_advanced",
    ],
)

cc_library(
    name = "iris_camera",
    hdrs = ["iris_camera.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":camera",
        ":framebuffer",
        ":image_sampler",
        ":render",
        ":sample_tracer",
        "//iris_advanced",
    ],
)

cc_library(
    name = "progress_reporter",
    srcs = ["progress_reporter.c"],
    hdrs = ["progress_reporter.h"],
    deps = [
        ":progress_reporter_internal",
        ":progress_reporter_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "progress_reporter_internal",
    hdrs = ["progress_reporter_internal.h"],
    deps = [
        ":progress_reporter_vtable",
    ],
)

cc_library(
    name = "progress_reporter_vtable",
    hdrs = ["progress_reporter_vtable.h"],
    deps = [
        "//common:free_routine",
        "//iris_advanced",
    ],
)

cc_library(
    name = "render",
    srcs = ["render.c"],
    hdrs = ["render.h"],
    deps = [
        ":camera",
        ":camera_internal",
        ":framebuffer",
        ":framebuffer_internal",
        ":image_sampler",
        ":image_sampler_internal",
        ":progress_reporter",
        ":progress_reporter_internal",
        ":sample_tracer",
        ":sample_tracer_internal",
    ],
)

cc_library(
    name = "sample_tracer",
    srcs = ["sample_tracer.c"],
    hdrs = ["sample_tracer.h"],
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
        "//common:free_routine",
        "//iris_advanced",
    ],
)
