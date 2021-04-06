
cmake_minimum_required(VERSION 3.1.0)

project(unetbootin)

if(CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "CMAKE_BUILD_TYPE not defined, 'Release' will be used")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
endif()

# Add Qt5 dependency
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS LinguistTools REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
   ${SOURCES}
   ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/unetbootin.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/distrolst.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/distrovercust.cpp
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/unetbootin.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/distrover.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/customdistrolst.cpp
)

set(TS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(TS_FILES
    ${TS_FILES}
    ${TS_DIR}/unetbootin_am.ts
    ${TS_DIR}/unetbootin_ar.ts
    ${TS_DIR}/unetbootin_ast.ts
    ${TS_DIR}/unetbootin_be.ts
    ${TS_DIR}/unetbootin_bg.ts
    ${TS_DIR}/unetbootin_bn.ts
    ${TS_DIR}/unetbootin_ca.ts
    ${TS_DIR}/unetbootin_cs.ts
    ${TS_DIR}/unetbootin_custom.ts
    ${TS_DIR}/unetbootin_da.ts
    ${TS_DIR}/unetbootin_de.ts
    ${TS_DIR}/unetbootin_el.ts
    ${TS_DIR}/unetbootin_eo.ts
    ${TS_DIR}/unetbootin_es.ts
    ${TS_DIR}/unetbootin_et.ts
    ${TS_DIR}/unetbootin_eu.ts
    ${TS_DIR}/unetbootin_fa.ts
    ${TS_DIR}/unetbootin_fi.ts
    ${TS_DIR}/unetbootin_fo.ts
    ${TS_DIR}/unetbootin_fr.ts
    ${TS_DIR}/unetbootin_gl.ts
    ${TS_DIR}/unetbootin_he.ts
    ${TS_DIR}/unetbootin_hr.ts
    ${TS_DIR}/unetbootin_hu.ts
    ${TS_DIR}/unetbootin_id.ts
    ${TS_DIR}/unetbootin_it.ts
    ${TS_DIR}/unetbootin_ja.ts
    ${TS_DIR}/unetbootin_lt.ts
    ${TS_DIR}/unetbootin_lv.ts
    ${TS_DIR}/unetbootin_ml.ts
    ${TS_DIR}/unetbootin_ms.ts
    ${TS_DIR}/unetbootin_nan.ts
    ${TS_DIR}/unetbootin_nb.ts
    ${TS_DIR}/unetbootin_nl.ts
    ${TS_DIR}/unetbootin_nn.ts
    ${TS_DIR}/unetbootin_pl.ts
    ${TS_DIR}/unetbootin_pt_BR.ts
    ${TS_DIR}/unetbootin_pt.ts
    ${TS_DIR}/unetbootin_ro.ts
    ${TS_DIR}/unetbootin_ru.ts
    ${TS_DIR}/unetbootin_si.ts
    ${TS_DIR}/unetbootin_sk.ts
    ${TS_DIR}/unetbootin_sl.ts
    ${TS_DIR}/unetbootin_sr.ts
    ${TS_DIR}/unetbootin_sv.ts
    ${TS_DIR}/unetbootin_sw.ts
    ${TS_DIR}/unetbootin_tr.ts
    ${TS_DIR}/unetbootin.ts
    ${TS_DIR}/unetbootin_uk.ts
    ${TS_DIR}/unetbootin_ur.ts
    ${TS_DIR}/unetbootin_vi.ts
    ${TS_DIR}/unetbootin_zh_CN.ts
    ${TS_DIR}/unetbootin_zh_TW.ts
)

qt5_add_translation(QM_FILES ${TS_FILES})

# qt5_wrap_ui(UIS_HDRS
#     ${CMAKE_CURRENT_SOURCE_DIR}/unetbootin.ui
# )

# qt5_use_modules(${CMAKE_PROJECT_NAME} Core Gui Widgets Network) # This macro depends from Qt version

# Create executable file from sources
add_executable(${CMAKE_PROJECT_NAME}
               ${SOURCES}
               ${HEADERS}
               ${QM_FILES}
            #    ${UIS_HDRS}
               ${CMAKE_CURRENT_SOURCE_DIR}/unetbootin.ui
            #    unetbootin.qrc
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
                        Qt5::Core
                        Qt5::Gui
                        Qt5::Widgets
                        Qt5::Network
)