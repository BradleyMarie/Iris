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