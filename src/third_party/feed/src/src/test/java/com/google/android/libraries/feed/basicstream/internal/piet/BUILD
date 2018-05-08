licenses(["notice"])  # Apache 2

android_local_test(
    name = "PietAssetProviderTest",
    size = "small",
    timeout = "moderate",
    srcs = ["PietAssetProviderTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "@com_google_protobuf_javalite//:protobuf_java_lite",
        "//src/main/java/com/google/android/libraries/feed/api/common",
        "//src/main/java/com/google/android/libraries/feed/api/stream",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal/piet",
        "//src/main/java/com/google/android/libraries/feed/common",
        "//src/main/java/com/google/android/libraries/feed/common/functional",
        "//src/main/java/com/google/android/libraries/feed/common/testing",
        "//src/main/java/com/google/android/libraries/feed/host/imageloader",
        "//src/main/java/com/google/android/libraries/feed/host/stream",
        "//src/main/proto/search/now/proto/ui/piet:piet_java_proto_lite",
        "@bazel_android//:robolectric",
        "@com_google_code_findbugs_jsr305//jar",
        "@com_google_truth_truth//jar",
        "@org_mockito_mockito_all//jar",
    ],
)

android_local_test(
    name = "PietHostBindingProviderTest",
    size = "small",
    timeout = "moderate",
    srcs = ["PietHostBindingProviderTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "@com_google_protobuf_javalite//:protobuf_java_lite",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal/piet",
        "//src/main/java/com/google/android/libraries/feed/piet/host",
        "//src/main/proto/search/now/proto/ui/piet:piet_java_proto_lite",
        "@bazel_android//:robolectric",
        "@com_google_truth_truth//jar",
        "@org_mockito_mockito_all//jar",
    ],
)

android_local_test(
    name = "PietCustomElementProviderTest",
    size = "small",
    timeout = "moderate",
    srcs = ["PietCustomElementProviderTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "@com_google_protobuf_javalite//:protobuf_java_lite",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal/piet",
        "//src/main/java/com/google/android/libraries/feed/piet/host",
        "//src/main/proto/search/now/proto/ui/piet:piet_java_proto_lite",
        "@bazel_android//:robolectric",
        "@com_google_code_findbugs_jsr305//jar",
        "@com_google_truth_truth//jar",
        "@org_mockito_mockito_all//jar",
    ],
)
