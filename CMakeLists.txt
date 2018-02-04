cmake_minimum_required (VERSION 3.0)

project (noextrun)

foreach (SUFFIX ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER "${SUFFIX}" SUFFIX)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${SUFFIX} ${CMAKE_BINARY_DIR})
endforeach ()

add_executable (noextrun NoExtRun.cpp)
target_link_libraries (noextrun "shlwapi.lib")
set_target_properties (noextrun PROPERTIES LINK_FLAGS "-static -static-libstdc++ -static-libgcc")
