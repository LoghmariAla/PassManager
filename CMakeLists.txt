cmake_minimum_required(VERSION 3.28)
project(GTKTutorial)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set the paths to the GTK 3 include and library directories
set(GTK3_INCLUDE_DIRS "C:/msys64/mingw64/include/")
set(GTK3_LIBRARY_DIRS "C:/msys64/mingw64/lib")

# Set the PKG_CONFIG_PATH environment variable to point to the GTK 3 pkgconfig files
set(ENV{PKG_CONFIG_PATH} "C:/msys64/mingw64/lib/pkgconfig")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)  # Use gtk+-3.0 for GTK 3

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(TutorialApp main.c)

# Set target properties if you want to create a Windows GUI application
# set_target_properties(TutorialApp PROPERTIES WIN32_EXECUTABLE true)

# Link with GTK 3 libraries
target_link_libraries(TutorialApp ${GTK3_LIBRARIES})  # Use GTK3_LIBRARIES for GTK 3
