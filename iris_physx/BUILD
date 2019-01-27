package(default_visibility = ["//visibility:private"])

cc_library(
    name = "area_light",
    hdrs = ["area_light.h"],
    srcs = ["area_light.c"],
    deps = [
        ":emissive_material_internal",
        ":light",
        ":shape",
        ":shape_internal",
        ":visibility_tester_internal",
        "//common:alloc",
    ],
)

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
        ":reflector_compositor",
        "//iris:iris",
        "//iris_advanced:iris_advanced",
    ],
)

cc_library(
    name = "color_integrator",
    hdrs = ["color_integrator.h"],
    srcs = ["color_integrator.c"],
    deps = [
        ":reflector",
        ":spectrum",
        "//common:safe_math",
        "//iris_advanced:iris_advanced",
    ],
)

cc_library(
    name = "color_matcher",
    hdrs = ["color_matcher.h"],
    srcs = ["color_matcher.c"],
    deps = [
        ":color_matcher_internal",
        ":color_matcher_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "color_matcher_internal",
    hdrs = ["color_matcher_internal.h"],
    deps = [
        ":color_matcher_vtable",
    ],
)

cc_library(
    name = "color_matcher_vtable",
    hdrs = ["color_matcher_vtable.h"],
    deps = [
        ":spectrum",
        "//iris_advanced:iris_advanced",
    ],
)

cc_library(
    name = "emissive_material",
    hdrs = ["emissive_material.h"],
    srcs = ["emissive_material.c"],
    deps = [
        ":emissive_material_internal",
        ":emissive_material_vtable",
        "//common:alloc",
    ],
)

cc_library(
    name = "emissive_material_internal",
    hdrs = ["emissive_material_internal.h"],
    deps = [
        ":emissive_material_vtable",
    ],
)

cc_library(
    name = "emissive_material_vtable",
    hdrs = ["emissive_material_vtable.h"],
    deps = [
        ":spectrum",
        "//iris:iris",
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
        ":color_matcher",
        ":color_matcher_internal",
        ":integrator_vtable",
        ":light_sampler_internal",
        ":ray_tracer_internal",
        ":spectrum_compositor_internal",
        ":visibility_tester_internal",
    ],
)

cc_library(
    name = "integrator_vtable",
    hdrs = ["integrator_vtable.h"],
    deps = [
        ":hit_tester",
        ":light_sampler",
        ":spectrum_compositor",
        ":ray_tracer",
        ":reflector_compositor",
        ":visibility_tester",
        "//iris_advanced:iris_advanced",
    ],
)

cc_library(
    name = "iris_physx",
    hdrs = ["iris_physx.h"],
    deps = [
        ":area_light",
        ":brdf",
        ":brdf_allocator",
        ":color_matcher",
        ":emissive_material",
        ":hit_allocator",
        ":hit_tester",
        ":integrator",
        ":light",
        ":light_sampler",
        ":material",
        ":ray_tracer",
        ":reflector",
        ":reflector_compositor",
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
    name = "light_sample_collector",
    hdrs = ["light_sample_collector.h"],
    srcs = ["light_sample_collector.c"],
    deps = [
        ":light",
        ":light_sample_collector_internal",
    ],
)

cc_library(
    name = "light_sample_collector_internal",
    hdrs = ["light_sample_collector_internal.h"],
    deps = [
        ":light",
    ],
)

cc_library(
    name = "light_sampler",
    hdrs = ["light_sampler.h"],
    srcs = ["light_sampler.c"],
    deps = [
        ":light",
        ":light_sampler_internal",
    ],
)

cc_library(
    name = "light_sampler_internal",
    hdrs = ["light_sampler_internal.h"],
    deps = [
        ":light",
        ":light_sample_collector",
        ":light_sample_collector_internal",
        ":light_sampler_vtable",
    ],
)

cc_library(
    name = "light_sampler_vtable",
    hdrs = ["light_sampler_vtable.h"],
    deps = [
        ":light",
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
        ":reflector_compositor",
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
        ":reflector_compositor_internal",
        ":reflector_compositor",
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
    name = "reflector_compositor",
    hdrs = ["reflector_compositor.h"],
    srcs = ["reflector_compositor.c"],
    deps = [
        ":reflector",
        ":reflector_compositor_internal",
        "//common:static_allocator",
    ],
)

cc_library(
    name = "reflector_compositor_internal",
    hdrs = ["reflector_compositor_internal.h"],
    deps = [
        ":reflector_internal",
        "//common:static_allocator",
    ],
)

cc_test(
    name = "reflector_compositor_test",
    srcs = ["reflector_compositor_test.cc"],
    deps = [
        ":reflector_compositor",
        ":reflector_compositor_test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "reflector_compositor_test_util",
    hdrs = ["reflector_compositor_test_util.h"],
    srcs = ["reflector_compositor_test_util.c"],
    testonly = 1,
    deps = [
        ":reflector_compositor",
        ":reflector_compositor_internal",
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
        ":emissive_material",
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
        ":spectrum_compositor_test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "spectrum_compositor_test_util",
    hdrs = ["spectrum_compositor_test_util.h"],
    srcs = ["spectrum_compositor_test_util.c"],
    testonly = 1,
    deps = [
        ":spectrum_compositor",
        ":spectrum_compositor_internal",
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