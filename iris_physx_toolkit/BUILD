load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "all_light_sampler",
    srcs = ["all_light_sampler.c"],
    hdrs = ["all_light_sampler.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "attenuated_reflector",
    srcs = ["attenuated_reflector.c"],
    hdrs = ["attenuated_reflector.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "constant_emissive_material",
    srcs = ["constant_emissive_material.c"],
    hdrs = ["constant_emissive_material.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "cie_color_integrator",
    srcs = ["cie_color_integrator.c"],
    hdrs = ["cie_color_integrator.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "constant_material",
    srcs = ["constant_material.c"],
    hdrs = ["constant_material.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "constructive_solid_geometry",
    srcs = ["constructive_solid_geometry.c"],
    hdrs = ["constructive_solid_geometry.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "directional_light",
    srcs = ["directional_light.c"],
    hdrs = ["directional_light.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "fresnel_bsdf",
    srcs = ["fresnel_bsdf.c"],
    hdrs = ["fresnel_bsdf.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "interpolated_spectrum",
    srcs = ["interpolated_spectrum.c"],
    hdrs = ["interpolated_spectrum.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "lambertian_bsdf",
    srcs = ["lambertian_bsdf.c"],
    hdrs = ["lambertian_bsdf.h"],
    deps = [
        ":attenuated_reflector",
        "//iris_physx",
    ],
)

cc_library(
    name = "list_scene",
    srcs = ["list_scene.c"],
    hdrs = ["list_scene.h"],
    deps = [
        "//common:pointer_list",
        "//iris_physx",
    ],
)

cc_library(
    name = "kd_tree_scene",
    srcs = ["kd_tree_scene.c"],
    hdrs = ["kd_tree_scene.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "metric_black_body",
    srcs = ["metric_black_body.c"],
    hdrs = ["metric_black_body.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "mirror_bsdf",
    srcs = ["mirror_bsdf.c"],
    hdrs = ["mirror_bsdf.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "one_light_sampler",
    srcs = ["one_light_sampler.c"],
    hdrs = ["one_light_sampler.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "path_tracer",
    srcs = ["path_tracer.c"],
    hdrs = ["path_tracer.h"],
    deps = [
        ":sample_direct_lighting",
        "//common:safe_math",
        "//iris_physx",
    ],
)

cc_library(
    name = "point_light",
    srcs = ["point_light.c"],
    hdrs = ["point_light.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "rgb_interpolator",
    srcs = ["rgb_interpolator.c"],
    hdrs = ["rgb_interpolator.h"],
    deps = [
        ":interpolated_spectrum",
        "//iris_physx",
    ],
)

cc_library(
    name = "sample_direct_lighting",
    srcs = ["sample_direct_lighting.c"],
    hdrs = ["sample_direct_lighting.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "sample_tracer",
    srcs = ["sample_tracer.c"],
    hdrs = ["sample_tracer.h"],
    deps = [
        "//iris_camera",
        "//iris_physx",
    ],
)

cc_library(
    name = "sphere",
    srcs = ["sphere.c"],
    hdrs = ["sphere.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "triangle",
    srcs = ["triangle.c"],
    hdrs = ["triangle.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "triangle_mesh",
    srcs = ["triangle_mesh.c"],
    hdrs = ["triangle_mesh.h"],
    deps = [
        ":triangle",
        "//iris_physx",
    ],
)

cc_library(
    name = "uniform_reflector",
    srcs = ["uniform_reflector.c"],
    hdrs = ["uniform_reflector.h"],
    deps = [
        "//iris_physx",
    ],
)
