cc_library(
    name = "mnemosyne",
    srcs = [
        "mnemosyne.cc",
        "mnemosyne.h"
    ],
    deps = [
        "//third_party/detours"
    ],
    linkopts = [
        "Advapi32.lib",
        "User32.lib",
    ],
    copts = ["/Wall", "/O2", "/std:c++17"],
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "mnemosyne_test",
    size = "small",
    srcs = [
        "tests/address_test.cc",
        "tests/memory_edit_test.cc",
        "tests/pattern_match_test.cc",
        "tests/util_test.cc",
    ],
    deps = [
        ":mnemosyne",
        "//third_party/detours",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    args = ["test_output=errors"],
    copts = ["/W0", "/Od", "/std:c++17"],
)