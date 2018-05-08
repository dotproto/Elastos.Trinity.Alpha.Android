package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2

proto_library(
    name = "stream_proto",
    srcs = ["stream_structure.proto"],
    deps = [
        "//src/main/proto/search/now/proto/ui/piet:piet_proto",
        "//src/main/proto/search/now/proto/wire/feed:feed_proto",
    ],
)

java_proto_library(
    name = "stream_java_proto",
    deps = [":stream_proto"],
)

java_lite_proto_library(
    name = "stream_java_proto_lite",
    deps = [":stream_proto"],
)
