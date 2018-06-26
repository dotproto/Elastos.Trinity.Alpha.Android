
set(CAR_FLAGS -n -u)
set(LUBE_FLAGS -n -u)

include(CARBridge.js.test.cmake)
xdk_compile_car(CAR_GENERATED_SOURCES CARBridge.js.test.car)

set(SOURCES
    ${CAR_GENERATED_SOURCES}
    ${CURRENT_MIRROR_SOURCES}
    CTest.cpp
)

add_library(CARBridge.js.test SHARED ${SOURCES})

target_link_libraries(CARBridge.js.test
    Elastos.Runtime
    Elastos.Runtime_static
    Elastos.CoreLibrary
)

