load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest",
    tag = "release-1.8.1",
)

new_git_repository(
    name = "pcg_c_basic",
    build_file = "pcg_c_basic.BUILD",
    remote = "https://github.com/imneme/pcg-c-basic.git",
    commit = "bc39cd76ac3d541e618606bcc6e1e5ba5e5e6aa3",
)

new_git_repository(
    name = "stb",
    build_file = "stb.BUILD",
    remote = "https://github.com/nothings/stb.git",
    commit = "f54acd4e13430c5122cab4ca657705c84aa61b08",
)