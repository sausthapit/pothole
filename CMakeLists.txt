cmake_minimum_required(VERSION 3.11)
project(Pothole)
set(CMAKE_CXX_STANDARD 14)
# The version number.
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

set(xsens_gps_location C:/Program\ Files/Xsens/MT\ Software\ Suite\ 4.8/MT\ SDK)
set(tis_location C:/Users/saurav/Documents/IC\ Imaging\ Control\ 3.4)
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
        "${PROJECT_SOURCE_DIR}/Config.h.in"
        "${PROJECT_BINARY_DIR}/Config.h"
)

find_package(Threads REQUIRED)
list(APPEND srcFiles
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
        "${PROJECT_SOURCE_DIR}/src/GPS/DeviceContainer.cpp"
        "${PROJECT_SOURCE_DIR}/src/Vehicle.cpp"
        "${PROJECT_SOURCE_DIR}/src/Camera/Listener.cpp")

add_executable(Pothole ${srcFiles} src/Vehicle.cpp src/Vehicle.h)

include_directories(
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/GPS
        ${PROJECT_SOURCE_DIR}/src/Camera
        ${tis_location}/classlib/include
        ${tis_location}/samples/vc10/Common/)

if(${WIN32})
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        include_directories("C:/Program Files/Xsens/MT Software Suite 4.8/MT SDK/x64/include")
        link_directories("C:/Program Files/Xsens/MT Software Suite 4.8/MT SDK/x64/lib")
        target_link_libraries(Pothole xstypes64
                xsensdeviceapi64
                )
    else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        include_directories("C:/Program Files/Xsens/MT Software Suite 4.8/MT SDK/Win32/include")
        find_library(xstypes32_LIB xstypes32 HINTS ${xsens_gps_location}/Win32/lib)
        find_library(xsensdeviceapi32_LIB xsensdeviceapi32 HINTS "C:/Program Files/Xsens/MT Software Suite 4.8/MT SDK/Win32/lib")
        #link_directories("C:\\Program Files\\Xsens\\MT Software Suite 4.8\\MT SDK\\Win32\\lib\\")
        link_directories(${tis_location}/classlib/win32/debug)
            find_library(tis_udshl11d_LIB TIS_UDSHL11d HINTS "${tis_location}/classlib/win32/debug")
        target_link_libraries(Pothole
                ${xstypes32_LIB}
                ${xsensdeviceapi32_LIB}
                ${tis_udshl11d_LIB}
                )

    endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

else(${WIN32})
    ##    not windows

endif(${WIN32})

#copy dll to the execution directory post build

add_custom_command(TARGET Pothole POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        ${xsens_gps_location}/Win32/lib/xstypes32.dll      # <--this is in-file
        $<TARGET_FILE_DIR:Pothole>)                 # <--this is out-file path
add_custom_command(TARGET Pothole POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        ${xsens_gps_location}/Win32/lib/xsensdeviceapi32.dll       # <--this is in-file
        $<TARGET_FILE_DIR:Pothole>)                 # <--this is out-file path
add_custom_command(TARGET Pothole POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        ${tis_location}/classlib/win32/debug/TIS_UDSHL11d.dll       # <--this is in-file
        $<TARGET_FILE_DIR:Pothole>)                 # <--this is out-file path