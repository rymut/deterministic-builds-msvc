cmake_minimum_required(VERSION 3.15)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CONAN_HOST_PROFILE "auto-cmake")
set(CONAN_SUBFOLDER "conan")

# download conan provider
set(CONAN_PROVIDER "${CMAKE_BINARY_DIR}/conan_provider.cmake")
set(CONAN_PROVIDER_GITHUB "rymut/cmake-conan")
if(NOT EXISTS "${CONAN_PROVIDER}")
  message(STATUS "Downloading conan_provider.cmake from https://github.com/${CONAN_PROVIDER_GITHUB}")
  file(DOWNLOAD "https://raw.githubusercontent.com/${CONAN_PROVIDER_GITHUB}/develop2/conan_provider.cmake"
                "${CONAN_PROVIDER}"
                TLS_VERIFY ON)
endif()


project(deterministic_builds_msvc LANGUAGES CXX)
set(CONAN_ARGS "")
include("${CONAN_PROVIDER}")

find_package(detours REQUIRED)

add_subdirectory(tests)