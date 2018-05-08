licenses(["notice"])  # Apache 2

android_local_test(
    name = "StreamActionApiImplTest",
    size = "small",
    timeout = "moderate",
    srcs = ["StreamActionApiImplTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "@com_google_protobuf_javalite//:protobuf_java_lite",
        "//src/main/java/com/google/android/libraries/feed/api/actionparser",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal",
        "//src/main/java/com/google/android/libraries/feed/host/action",
        "//src/main/proto/search/now/proto/ui/action:feed_action_java_proto_lite",
        "//src/main/proto/search/now/proto/ui/action:feed_action_payload_java_proto_lite",
        "//src/main/proto/search/now/proto/wire/feed:feed_java_proto_lite",
        "@bazel_android//:robolectric",
        "@com_google_truth_truth//jar",
        "@org_mockito_mockito_all//jar",
    ],
)

android_local_test(
    name = "StreamRecyclerViewAdapterTest",
    size = "small",
    timeout = "moderate",
    srcs = ["StreamRecyclerViewAdapterTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "@com_google_protobuf_javalite//:protobuf_java_lite",
        "//src/main/java/com/google/android/libraries/feed/api/modelprovider",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal/contentmodels",
        "//src/main/java/com/google/android/libraries/feed/basicstream/internal/viewholders",
        "//src/main/java/com/google/android/libraries/feed/common",
        "//src/main/java/com/google/android/libraries/feed/common/functional",
        "//src/main/java/com/google/android/libraries/feed/host/stream",
        "//src/main/java/com/google/android/libraries/feed/piet",
        "//src/main/proto/com/google/android/libraries/feed/api/proto:client_feed_java_proto_lite",
        "//src/main/proto/search/now/proto/ui/piet:piet_java_proto_lite",
        "//src/main/proto/search/now/proto/ui/stream:stream_java_proto_lite",
        "@androidsdk//com.android.support:recyclerview-v7-25.3.1",
        "@bazel_android//:robolectric",
        "@com_google_guava_guava//jar",
        "@com_google_truth_truth//jar",
        "@org_mockito_mockito_all//jar",
    ],
)
