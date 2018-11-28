cmake_minimum_required(VERSION 3.11)
project(Pothole)
set(CMAKE_CXX_STANDARD 14)
# The version number.
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (STATIC 1)
set(xsens_gps_location C:/Program\ Files/Xsens/MT\ Software\ Suite\ 4.8/MT\ SDK)
set(tis_location C:/Users/saurav/Documents/IC\ Imaging\ Control\ 3.4)
set (XMP_ROOT "C:/dev/XMP-Toolkit-SDK-CC201607")
set (XMP_BUILDMODE_DIR $(Configuration))

# Platform specific config
if(UNIX)
  if(APPLE)
	# OSX -------------------------------------------
	if(STATIC)
		set(BUILD_FOLDER "xcode")
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".a")	
	else(STATIC)
		set(BUILD_FOLDER "xcode")
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".framework")	
	endif(STATIC)
	add_definitions(-DMAC_ENV=1)	
	if(CMAKE_CL_64)
		set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for OSX" FORCE)
		set(PLATFORM_FOLDER "macintosh/intel_64")
		add_definitions(-DXMP_64=1)
	else(CMAKE_CL_64)
		set(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
		set(PLATFORM_FOLDER "macintosh/intel")
	endif(CMAKE_CL_64)
	# set specific SDK and compiler version
	if (NOT DEFINED OSX_DEVELOPER_ROOT)
		set(OSX_DEVELOPER_ROOT "/Applications/Xcode.app/Contents/Developer")
	endif()
	#check is the path to OSX_DEVELOPER_ROOT exists
	if(IS_DIRECTORY ${OSX_DEVELOPER_ROOT})
		set(CMAKE_OSX_SYSROOT "${OSX_DEVELOPER_ROOT}" )
	else()
		message ( FATAL_ERROR "ERROR: The sdk dir set is ${OSX_DEVELOPER_ROOT}, but it does not exist. See readme.txt for resolution")
	endif()	
	message ( STATUS "OSX SDK is set to ${CMAKE_OSX_SYSROOT}")
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")
	set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
	
    # set Warning flags
    set(CMAKE_CXX_FLAGS "-Wno-trigraphs -Wnon-virtual-dtor -Woverloaded-virtual -Wmissing-braces -Wparentheses -Wswitch -Wunused-value -Wunknown-pragmas -Wsign-compare -Wreturn-type")
	# set Compiler build flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fshort-enums -funsigned-char  -mone-byte-bool -fno-common -ffast-math")
	# set config specific flagss
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG=1 -D_DEBUG=1")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG=1")

	set(CMAKE_C_FLAGS "")
  else(APPLE)
	# Linux -------------------------------------------
	if(STATIC)
		set(BUILD_FOLDER "gcc4")	
		set(XMPCORE_LIB "staticXMPCore")
		set(XMPFILES_LIB "staticXMPFiles")
		set(LIB_EXT ".ar")	
	else(STATIC)
		set(BUILD_FOLDER "gcc4")	
		set(XMPCORE_LIB "libXMPCore")
		set(XMPFILES_LIB "libXMPFiles")
		set(LIB_EXT ".so")	
	endif(STATIC)
	add_definitions(-DUNIX_ENV=1 -D__LITTLE_ENDIAN__=1)

	if(CMAKE_CL_64)
		add_definitions(-DXMP_64=1)
		set(PLATFORM_FOLDER "i80386linux_x64")
	else()	
		set(PLATFORM_FOLDER "i80386linux")	
	endif(CMAKE_CL_64)
    
    set(CMAKE_CXX_FLAGS "${CXX_FLAGS} -Wno-ctor-dtor-privacy -fPIC -funsigned-char -fexceptions -Wno-multichar -Wno-implicit")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG=1 -D_DEBUG=1")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG=1")
  endif(APPLE)
else(UNIX)
  if(WIN32)
  
	# Windows -------------------------------------------
	if(STATIC)
		set(BUILD_FOLDER "vc14")	
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".lib")
	else(STATIC)
		set(BUILD_FOLDER "vc14")	
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".lib")
	endif(STATIC)
	
	if(CMAKE_CL_64)
		add_definitions(-DXMP_64=1)
	    set(PLATFORM_FOLDER "windows_x64")
	else(CMAKE_CL_64)
	    set(PLATFORM_FOLDER "windows")
	endif(CMAKE_CL_64)
    
	# config independend preprocessor defines
	add_definitions(-DWIN_ENV=1 -DWIN32=1 -D_CONSOLE -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS=1 -D_SCL_SECURE_NO_WARNINGS=1 )

	if(STATIC)
	add_definitions(-DXMP_StaticBuild=1)
	else(STATIC)
	add_definitions(-DXMP_StaticBuild=0)
	endif(STATIC)
	
  else(WIN32)
	# unknown platform
	MESSAGE(ERROR ": Unknown Platform")
  endif(WIN32)
endif(UNIX)
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
        "${PROJECT_SOURCE_DIR}/Config.h.in"
        "${PROJECT_BINARY_DIR}/Config.h"
)

find_package(Threads REQUIRED)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

list(APPEND srcFiles       
        "${PROJECT_SOURCE_DIR}/src/GPS/DeviceContainer.cpp"
        "${PROJECT_SOURCE_DIR}/src/Vehicle.cpp"
		"${PROJECT_SOURCE_DIR}/src/Camera/Listener.cpp"
		"${PROJECT_SOURCE_DIR}/src/Camera/TriggerCam.cpp"
		"${PROJECT_SOURCE_DIR}/src/XMP/AddMetadata.cpp"
		"${PROJECT_SOURCE_DIR}/src/HeavyWorker.cpp"
		"${PROJECT_SOURCE_DIR}/src/main.cpp")


 macro(GroupSources curdir)

    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
 ${PROJECT_SOURCE_DIR}/${curdir}/*)

    foreach(child ${children})

           if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})

                   GroupSources(${curdir}/${child})

           else()

                   string(REPLACE "/" "\\" groupname ${curdir})

           # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)

#                    string(REPLACE "src" "Common" groupname ${groupname})

             source_group(${groupname} FILES
 ${PROJECT_SOURCE_DIR}/${curdir}/${child})

           endif()

    endforeach()

 endmacro()



 # Execute the macro

 GroupSources(src)
add_executable(Pothole ${srcFiles} src/Vehicle.cpp src/Vehicle.h)
target_link_libraries( Pothole ${OpenCV_LIBS} )
include_directories(
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/GPS
        ${PROJECT_SOURCE_DIR}/src/Camera
		${PROJECT_SOURCE_DIR}/src/XMP
        ${tis_location}/classlib/include
        ${tis_location}/samples/vc10/Common/
		"C:/dev/XMP-Toolkit-SDK-CC201607/public/include"
		"C:/opencv32/build/install/include"
		"C:/eigen-eigen-5a0156e40feb"
		"C:/dev/curl-7.62.0-win64-mingw/include"
#		"C:/dev/curl-7.62.0/include"
)

if(${WIN32})
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        include_directories("C:/Program Files/Xsens/MT Software Suite 4.8/MT SDK/x64/include")
        link_directories("C:/Program Files/Xsens/MT Software Suite 4.8/MT SDK/x64/lib")
		find_library(xstypes64_LIB xstypes64 HINTS ${xsens_gps_location}/x64/lib)
        find_library(xsensdeviceapi64_LIB xsensdeviceapi64 HINTS ${xsens_gps_location}/x64/lib)
		link_directories(${tis_location}/classlib/win32/debug)
        find_library(tis_udshl11d_LIB TIS_UDSHL11d_x64 HINTS "${tis_location}/classlib/x64/debug")
		find_library(tis_udshl11_LIB TIS_UDSHL11_x64 HINTS "${tis_location}/classlib/x64/release")
		find_library(exif2_LIB exiv2 HINTS "C:/dev/PotholeProject/exiv2/2015/x64/dll/Release/lib")
        target_link_libraries(Pothole ${xstypes64_LIB}
                ${xsensdeviceapi64_LIB}
				${exif2_LIB}
#				"C:/dev/curl-7.62.0-win64-mingw/lib/libcurl.lib"
#				"C:/dev/curl-7.62.0-win64-mingw/bin/libcurl-x64"
				"C:/dev/curl-7.62.0/build/lib/Debug/libcurl-d_imp.lib"
                )
		target_link_libraries(Pothole debug ${tis_udshl11d_LIB} optimized ${tis_udshl11_LIB})
		set(xstypes_file ${xsens_gps_location}/x64/lib/xstypes64.dll)
		set(xsensdeviceapi_file ${xsens_gps_location}/x64/lib/xsensdeviceapi64.dll)
		set(exiv2_file "C:/dev/PotholeProject/exiv2/2015/x64/dll/Release/bin/*.dll")
		file(GLOB exif_files "C:/dev/PotholeProject/exiv2/2015/x64/dll/Release/bin/*.dll")
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
		set(xstypes_file ${xsens_gps_location}/Win32/lib/xstypes32.dll)
		set(xsensdeviceapi_file ${xsens_gps_location}/Win32/lib/xsensdeviceapi32.dll)
		
    endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

else(${WIN32})
    ##    not windows

endif(${WIN32})

if(STATIC)
add_definitions(-DENABLE_CPP_DOM_MODEL=1)
else(STATIC)
add_definitions(-DENABLE_CPP_DOM_MODEL=0)
endif(STATIC)
#addding XMP libs and setting output path
if(STATIC)
	if(UNIX)
		if(APPLE) #For Mac
			target_link_libraries(${PROJECT_NAME}  ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/lib${XMPCORE_LIB}Static${LIB_EXT} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/lib${XMPFILES_LIB}Static${LIB_EXT} )
			set(OUTPUT_DIR ${SAMPLE_SOURCE_ROOT}/../target/${PLATFORM_FOLDER}/ ) 
			set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})       
		else(APPLE) #For Linux
			SetPlatformLinkFlags(${PROJECT_NAME} "" "")
			target_link_libraries(${PROJECT_NAME} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPCORE_LIB}${LIB_EXT} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPFILES_LIB}${LIB_EXT} )
			set(OUTPUT_DIR ${SAMPLE_SOURCE_ROOT}/../target/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR} ) 
			set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})		
		endif(APPLE)	
	else(UNIX) #For Windows
		target_link_libraries(${PROJECT_NAME} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPCORE_LIB}Static${LIB_EXT} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPFILES_LIB}Static${LIB_EXT} Rpcrt4.lib)	
		set(OUTPUT_DIR ${SAMPLE_SOURCE_ROOT}/../target/${PLATFORM_FOLDER}/ ) 
		set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})
	endif(UNIX)
else(STATIC)
	if(UNIX)
		if(APPLE) #For Mac
			target_link_libraries(${PROJECT_NAME}  ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPCORE_LIB}${LIB_EXT}/Versions/A/${XMPCORE_LIB} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPFILES_LIB}${LIB_EXT}/Versions/A/${XMPFILES_LIB} )
			set(OUTPUT_DIR ${SAMPLE_SOURCE_ROOT}/../target/${PLATFORM_FOLDER}/ ) 
			set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})  
			add_custom_command (TARGET ${PROJECT_NAME} COMMAND ${CMAKE_COMMAND} -E copy_directory ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR} ${OUTPUT_DIR}/${XMP_BUILDMODE_DIR} )     
		else(APPLE) #For Linux
			SetPlatformLinkFlags(${PROJECT_NAME} "" "")
			target_link_libraries(${PROJECT_NAME} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPCORE_LIB}${LIB_EXT} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPFILES_LIB}${LIB_EXT} )
			set(OUTPUT_DIR ${SAMPLE_SOURCE_ROOT}/../target/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR} ) 
			set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})		
			add_custom_command (TARGET ${PROJECT_NAME} COMMAND ${CMAKE_COMMAND} -E copy_directory ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR} ${OUTPUT_DIR} )
		endif(APPLE)	
	else(UNIX) #For Windows
		target_link_libraries(${PROJECT_NAME} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPCORE_LIB}${LIB_EXT} ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR}/${XMPFILES_LIB}${LIB_EXT} Rpcrt4.lib)	
		set(OUTPUT_DIR ${SAMPLE_SOURCE_ROOT}/../target/${PLATFORM_FOLDER}/ ) 
		set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})
		add_custom_command (TARGET ${PROJECT_NAME} COMMAND ${CMAKE_COMMAND} -E copy_directory ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER}/${XMP_BUILDMODE_DIR} ${OUTPUT_DIR}/${XMP_BUILDMODE_DIR} )
	endif(UNIX)
endif(STATIC)
#copy dll to the execution directory post build

add_custom_command(TARGET Pothole POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        ${xstypes_file}      # <--this is in-file
        $<TARGET_FILE_DIR:Pothole>)                 # <--this is out-file path
add_custom_command(TARGET Pothole POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        ${xsensdeviceapi_file}       # <--this is in-file
        $<TARGET_FILE_DIR:Pothole>)                 # <--this is out-file path
add_custom_command(TARGET Pothole POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        ${tis_location}/classlib/win32/debug/TIS_UDSHL11d.dll       # <--this is in-file
        $<TARGET_FILE_DIR:Pothole>)                 # <--this is out-file path
add_custom_command(TARGET Pothole POST_BUILD 	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${exif_files} $<TARGET_FILE_DIR:Pothole>)
#add_custom_command(TARGET Pothole POST_BUILD    COMMAND ${CMAKE_COMMAND} -E copy_if_different C:/dev/curl-7.62.0/build/lib/Debug/libcurl-d.dll)