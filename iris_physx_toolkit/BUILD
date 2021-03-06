load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "aggregate_environmental_light",
    srcs = ["aggregate_environmental_light.c"],
    hdrs = ["aggregate_environmental_light.h"],
    deps = [
        "//iris_physx",
    ],
)

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
    name = "bump_map",
    srcs = ["bump_map.c"],
    hdrs = ["bump_map.h"],
    deps = [
        ":float_texture",
        ":uv_texture_coordinate",
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
    name = "color_extrapolator_vtable",
    hdrs = ["color_extrapolator_vtable.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "color_extrapolator",
    srcs = ["color_extrapolator.c"],
    hdrs = ["color_extrapolator.h"],
    deps = [
        ":color_extrapolator_vtable",
        "//common:alloc",
        "//common:safe_math",
        "//iris_physx",
        "//third_party/smhasher:murmur2",
        "//third_party/smhasher:murmur3",
    ],
)

cc_library(
    name = "color_spectra",
    srcs = ["color_spectra.c"],
    hdrs = ["color_spectra.h"],
    deps = [
        ":color_extrapolator",
        "//iris_physx",
    ],
)

cc_library(
    name = "constant_texture",
    srcs = ["constant_texture.c"],
    hdrs = ["constant_texture.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
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
    name = "float_texture_vtable",
    hdrs = ["float_texture_vtable.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "float_texture",
    srcs = ["float_texture.c"],
    hdrs = ["float_texture.h"],
    deps = [
        ":float_texture_vtable",
    ],
)

cc_library(
    name = "image_texture",
    srcs = ["image_texture.c"],
    hdrs = ["image_texture.h"],
    deps = [
        ":float_texture",
        ":mipmap",
        ":reflector_texture",
        ":uv_texture_coordinate",
    ],
)

cc_library(
    name = "infinite_environmental_light",
    srcs = ["infinite_environmental_light.c"],
    hdrs = ["infinite_environmental_light.h"],
    deps = [
        ":mipmap",
        "//common:safe_math",
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
    name = "mipmap",
    srcs = ["mipmap.c"],
    hdrs = ["mipmap.h"],
    deps = [
        ":color_extrapolator",
        "//iris_advanced_toolkit:color_io",
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
    name = "perlin_textures",
    srcs = ["perlin_textures.c"],
    hdrs = ["perlin_textures.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
        "//iris_physx",
    ],
)

cc_library(
    name = "png_mipmap",
    srcs = ["png_mipmap.c"],
    hdrs = ["png_mipmap.h"],
    deps = [
        ":mipmap",
        "//iris_advanced_toolkit:color_io",
        "//iris_advanced_toolkit:lanczos_upscale",
        "@stb//:stb_image",
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
    name = "product_texture",
    srcs = ["product_texture.c"],
    hdrs = ["product_texture.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
    ],
)

cc_library(
    name = "reflective_color_integrator",
    srcs = ["reflective_color_integrator.c"],
    hdrs = ["reflective_color_integrator.h"],
    deps = [
        ":interpolated_spectrum",
        "//iris_physx",
    ],
)

cc_library(
    name = "reflector_texture_vtable",
    hdrs = ["reflector_texture_vtable.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "reflector_texture",
    srcs = ["reflector_texture.c"],
    hdrs = ["reflector_texture.h"],
    deps = [
        ":reflector_texture_vtable",
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
    name = "smits_color_extrapolator",
    srcs = ["smits_color_extrapolator.c"],
    hdrs = ["smits_color_extrapolator.h"],
    deps = [
        ":color_extrapolator",
        ":interpolated_spectrum",
        "//iris_physx",
    ],
)

cc_library(
    name = "triangle_mesh_normal_map",
    srcs = ["triangle_mesh_normal_map.c"],
    hdrs = ["triangle_mesh_normal_map.h"],
    deps = [
        "//iris_physx",
        "//iris_physx_toolkit/shapes:triangle_mesh",
    ],
)

cc_library(
    name = "triangle_mesh_texture_coordinate_map",
    srcs = ["triangle_mesh_texture_coordinate_map.c"],
    hdrs = ["triangle_mesh_texture_coordinate_map.h"],
    deps = [
        ":uv_texture_coordinate",
        "//iris_physx",
        "//iris_physx_toolkit/shapes:triangle_mesh",
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

cc_library(
    name = "uv_texture_coordinate",
    hdrs = ["uv_texture_coordinate.h"],
    deps = [
        "//iris_physx",
    ],
)
