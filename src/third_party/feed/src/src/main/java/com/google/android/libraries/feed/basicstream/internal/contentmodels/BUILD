package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2

android_library(
    name = "contentmodels",
    srcs = glob(["*.java"]),
    deps = [
        "//src/main/java/com/google/android/libraries/feed/api/modelprovider",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal/util",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal/viewholders",
        "//src/main/proto/search/now/proto/ui/piet:piet_java_proto_lite",
        "//src/main/proto/search/now/proto/ui/stream:stream_java_proto_lite",
        "@androidsdk//com.android.support:recyclerview-v7-25.3.1",
        "@androidsdk//com.android.support:support-annotations-25.3.1",
    ],
)
