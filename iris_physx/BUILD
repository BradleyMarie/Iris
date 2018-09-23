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
        ":reflector_allocator",
        "//iris:iris",
        "//iris_advanced:iris_advanced",
    ],
)

cc_library(
    name = "hit_allocator",
    hdrs = ["hit_allocator.h"],
    deps = [
        "//iris:iris",
    ],
)

cc_library(
    name = "hit_tester",
    hdrs = ["hit_tester.h"],
    deps = [
        ":hit_allocator",
        ":shape",
    ],
)

cc_library(
    name = "integrator",
    hdrs = ["integrator.h"],
    srcs = ["integrator.c"],
    deps = [
        ":brdf",
        ":brdf_internal",
        ":hit_tester",
        ":spectrum_compositor_internal",
        ":ray_tracer",
        ":ray_tracer_internal",
        ":reflector_allocator_internal",
        ":visibility_tester",
        ":visibility_tester_internal",
    ],
)

cc_library(
    name = "iris_physx",
    hdrs = ["iris_physx.h"],
    deps = [
        ":brdf",
        ":brdf_allocator",
        ":hit_allocator",
        ":hit_tester",
        ":integrator",
        ":light",
        ":material",
        ":ray_tracer",
        ":reflector",
        ":reflector_allocator",
        ":shape",
        ":spectrum",
        ":spectrum_compositor",
        ":visibility_tester",
        "//iris_advanced:iris_advanced",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "light",
    hdrs = ["light.h"],
    srcs = ["light.c"],
    deps = [
        ":light_internal",
        ":light_vtable",
    ],
)

cc_library(
    name = "light_internal",
    hdrs = ["light_internal.h"],
    deps = [
        ":light_vtable",
    ],
)

cc_library(
    name = "light_vtable",
    hdrs = ["light_vtable.h"],
    deps = [
        ":spectrum_compositor",
        ":visibility_tester",
        "//iris_advanced:iris_advanced",
    ],
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
    name = "ray_tracer",
    hdrs = ["ray_tracer.h"],
    srcs = ["ray_tracer.c"],
    deps = [
        ":brdf",
        ":light",
        ":material_internal",
        ":ray_tracer_internal",
        ":shape",
        ":shape_internal",
    ],
)

cc_library(
    name = "ray_tracer_internal",
    hdrs = ["ray_tracer_internal.h"],
    deps = [
        ":brdf_allocator",
        ":light",
    ],
)

cc_library(
    name = "reflector",
    hdrs = ["reflector.h"],
    srcs = ["reflector.c"],
    deps = [
        ":reflector_internal",
        "//common:alloc",
    ],
)

cc_library(
    name = "reflector_allocator",
    hdrs = ["reflector_allocator.h"],
    srcs = ["reflector_allocator.c"],
    deps = [
        ":reflector",
        ":reflector_allocator_internal",
        "//common:dynamic_allocator",
    ],
)

cc_library(
    name = "reflector_allocator_internal",
    hdrs = ["reflector_allocator_internal.h"],
    deps = [
        ":reflector_internal",
        "//common:dynamic_allocator",
    ],
)

cc_test(
    name = "reflector_allocator_test",
    srcs = ["reflector_allocator_test.cc"],
    deps = [
        ":reflector_allocator",
        ":test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "reflector_internal",
    hdrs = ["reflector_internal.h"],
    deps = [
        ":reflector_vtable",
    ],
)

cc_test(
    name = "reflector_test",
    srcs = ["reflector_test.cc"],
    deps = [
        ":reflector",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "reflector_vtable",
    hdrs = ["reflector_vtable.h"],
    deps = [
        "//common:free_routine",
        "//common:sal",
        "//common:status",
    ],
)

cc_library(
    name = "shape",
    hdrs = ["shape.h"],
    srcs = ["shape.c"],
    deps = [
        ":light",
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

cc_library(
    name = "spectrum",
    hdrs = ["spectrum.h"],
    srcs = ["spectrum.c"],
    deps = [
        ":spectrum_internal",
        "//common:alloc",
    ],
)

cc_library(
    name = "spectrum_compositor",
    hdrs = ["spectrum_compositor.h"],
    srcs = ["spectrum_compositor.c"],
    deps = [
        ":reflector",
        ":reflector_internal",
        ":spectrum",
        ":spectrum_compositor_internal",
        ":spectrum_internal",
    ],
)

cc_library(
    name = "spectrum_compositor_internal",
    hdrs = ["spectrum_compositor_internal.h"],
    deps = [
        ":reflector",
        ":spectrum_internal",
        "//common:static_allocator",
    ],
)

cc_test(
    name = "spectrum_compositor_test",
    srcs = ["spectrum_compositor_test.cc"],
    deps = [
        ":spectrum_compositor",
        ":test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "spectrum_internal",
    hdrs = ["spectrum_internal.h"],
    deps = [
        ":spectrum_vtable",
    ],
)

cc_test(
    name = "spectrum_test",
    srcs = ["spectrum_test.cc"],
    deps = [
        ":spectrum",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "spectrum_vtable",
    hdrs = ["spectrum_vtable.h"],
    deps = [
        "//common:free_routine",
        "//common:sal",
        "//common:status",
    ],
)

cc_library(
    name = "test_util",
    hdrs = ["test_util.h"],
    srcs = ["test_util.c"],
    testonly = 1,
    deps = [
        ":reflector_allocator",
        ":reflector_allocator_internal",
        ":spectrum_compositor",
        ":spectrum_compositor_internal",
    ],
)

cc_library(
    name = "visibility_tester_internal",
    hdrs = ["visibility_tester_internal.h"],
    deps = [
        "//iris:iris",
    ],
)

cc_library(
    name = "visibility_tester",
    hdrs = ["visibility_tester.h"],
    srcs = ["visibility_tester.c"],
    deps = [
        ":visibility_tester_internal",
    ],
)