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
    name = "color",
    hdrs = ["color.h"],
    deps = [
        "//iris:iris",
    ],
)

cc_test(
    name = "color_test",
    srcs = ["color_test.cc"],
    deps = [
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "framebuffer_internal",
    hdrs = ["framebuffer_internal.h"],
    deps = [
        ":color",
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
        ":color",
        ":framebuffer",
        ":pixel_sampler",
        ":random_generator",
        ":sample_tracer",
        ":sample_tracer_generator",
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
    name = "pixel_sampler_generator",
    hdrs = ["pixel_sampler_generator.h"],
    srcs = ["pixel_sampler_generator.c"],
    deps = [
        ":pixel_sampler_generator_internal",
        ":pixel_sampler_generator_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "pixel_sampler_generator_internal",
    hdrs = ["pixel_sampler_generator_internal.h"],
    deps = [
        ":pixel_sampler_generator_vtable",
    ],
)

cc_library(
    name = "pixel_sampler_generator_vtable",
    hdrs = ["pixel_sampler_generator_vtable.h"],
    deps = [
        ":pixel_sampler",
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
    name = "random_generator",
    hdrs = ["random_generator.h"],
    srcs = ["random_generator.c"],
    deps = [
        ":random_generator_internal",
        ":random_generator_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "random_generator_internal",
    hdrs = ["random_generator_internal.h"],
    deps = [
        ":random_generator_vtable",
    ],
)

cc_library(
    name = "random_generator_vtable",
    hdrs = ["random_generator_vtable.h"],
    deps = [
        "//iris_advanced:iris_advanced",
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
    name = "sample_tracer_generator",
    hdrs = ["sample_tracer_generator.h"],
    srcs = ["sample_tracer_generator.c"],
    deps = [
        ":sample_tracer_generator_internal",
        ":sample_tracer_generator_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "sample_tracer_generator_internal",
    hdrs = ["sample_tracer_generator_internal.h"],
    deps = [
        ":sample_tracer_generator_vtable",
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
    name = "sample_tracer_generator_vtable",
    hdrs = ["sample_tracer_generator_vtable.h"],
    deps = [
        ":sample_tracer",
    ],
)

cc_library(
    name = "sample_tracer_vtable",
    hdrs = ["sample_tracer_vtable.h"],
    deps = [
        ":color",
        "//common:free_routine",
    ],
)