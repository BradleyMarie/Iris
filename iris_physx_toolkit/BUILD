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
    name = "attenuated_reflector",
    hdrs = ["attenuated_reflector.h"],
    srcs = ["attenuated_reflector.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "constant_emissive_material",
    hdrs = ["constant_emissive_material.h"],
    srcs = ["constant_emissive_material.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "cie_color_integrator",
    hdrs = ["cie_color_integrator.h"],
    srcs = ["cie_color_integrator.c"],
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
    name = "constructive_solid_geometry",
    hdrs = ["constructive_solid_geometry.h"],
    srcs = ["constructive_solid_geometry.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "directional_light",
    hdrs = ["directional_light.h"],
    srcs = ["directional_light.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "fresnel_bsdf",
    hdrs = ["fresnel_bsdf.h"],
    srcs = ["fresnel_bsdf.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "interpolated_spectrum",
    hdrs = ["interpolated_spectrum.h"],
    srcs = ["interpolated_spectrum.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "lambertian_bsdf",
    hdrs = ["lambertian_bsdf.h"],
    srcs = ["lambertian_bsdf.c"],
    deps = [
        ":attenuated_reflector",
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
    name = "kd_tree_scene",
    hdrs = ["kd_tree_scene.h"],
    srcs = ["kd_tree_scene.c"],
    deps = [
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
    name = "mirror_bsdf",
    hdrs = ["mirror_bsdf.h"],
    srcs = ["mirror_bsdf.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "one_light_sampler",
    hdrs = ["one_light_sampler.h"],
    srcs = ["one_light_sampler.c"],
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
        "//common:safe_math",
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "ply_reader",
    hdrs = ["ply_reader.h"],
    srcs = ["ply_reader.c"],
    deps = [
        ":triangle",
        "//iris_physx:iris_physx",
        "//third_party/rply:rply",
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
    name = "triangle",
    hdrs = ["triangle.h"],
    srcs = ["triangle.c"],
    deps = [
        "//iris_physx:iris_physx",
    ],
)

cc_library(
    name = "triangle_mesh",
    hdrs = ["triangle_mesh.h"],
    srcs = ["triangle_mesh.c"],
    deps = [
        ":triangle",
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