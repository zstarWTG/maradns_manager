cmake_minimum_required(VERSION 3.30)
project(dns_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "E:/Qt/5.15.2/mingw81_64")
set(TS_FILES
        ${CMAKE_SOURCE_DIR}/lang/zh_CN.ts)

find_package(Qt5 COMPONENTS
  Core
  Gui
  Widgets
    LinguistTools
    REQUIRED)

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

add_executable(dns_manager WIN32 main.cpp
        mainwindow.cpp
        mainwindow.h
        config.h
        TxtHandler.cpp
        TxtHandler.h
        res.qrc
        ${TS_FILES})
target_link_libraries(dns_manager
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
)

# 设置图标文件路径
set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/appicon.ico")
if(WIN32)
    # 创建 RC 文件内容
    set(RC_CONTENT "IDI_ICON1 ICON \"${APP_ICON}\"\n")

    # 生成 RC 文件
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/appicon.rc "${RC_CONTENT}")

    # 将 RC 文件添加到可执行目标
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/appicon.rc)
endif()


if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach(QT_LIB)
endif()
