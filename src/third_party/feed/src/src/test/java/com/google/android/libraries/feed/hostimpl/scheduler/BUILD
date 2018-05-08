licenses(["notice"])  # Apache 2

android_local_test(
    name = "SchedulerApiImplTest",
    size = "small",
    timeout = "moderate",
    srcs = ["SchedulerApiImplTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "@com_google_protobuf_javalite//:protobuf_java_lite",
        "//src/main/java/com/google/android/libraries/feed/common",
        "//src/main/java/com/google/android/libraries/feed/host/proto",
        "//src/main/java/com/google/android/libraries/feed/host/scheduler",
        "//src/main/java/com/google/android/libraries/feed/hostimpl/scheduler",
        "//src/main/java/com/google/android/libraries/feed/testing/conformance/scheduler",
        "@bazel_android//:robolectric",
        "@com_google_code_findbugs_jsr305//jar",
        "@com_google_truth_truth//jar",
        "@org_mockito_mockito_all//jar",
    ],
)
