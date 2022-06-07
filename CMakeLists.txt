cmake_minimum_required(VERSION 3.0)

# Get the library name.
file(STRINGS "src/lib/name.txt" LIB_NAME)
# Get the binary name.
file(STRINGS "src/app/name.txt" BIN_NAME)
# Set the project name.
project(${LIB_NAME})

# Get the sources for the underlying library from `src/lib/`.
file(GLOB_RECURSE library "src/lib/*.cpp")
# Get the sources for the executable from `src/bin/`.
file(GLOB_RECURSE binary "src/app/*.cpp")
set(headers "include")

# The project will be backed by this library.
add_library(${LIB_NAME} ${library})
# Actual executable.
add_executable(${BIN_NAME} ${binary})

# Link the backing library to the executable.
target_link_libraries(${BIN_NAME} ${LIB_NAME})
# Expose headers to the library.
target_include_directories(${LIB_NAME} PUBLIC ${headers})

# Add allolib as a subdirectory.
add_subdirectory(allolib)

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/al_ext)
    message("Buiding extensions in al_ext")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/al_ext)
    get_target_property(AL_EXT_LIBRARIES al_ext AL_EXT_LIBRARIES)
    target_link_libraries(${LIB_NAME} PRIVATE ${AL_EXT_LIBRARIES})
endif()

# Link allolib to project.
target_link_libraries(${LIB_NAME} PUBLIC al)

# example line for find_package usage
# find_package(Qt5Core REQUIRED CONFIG PATHS "C:/Qt/5.12.0/msvc2017_64/lib" NO_DEFAULT_PATH)

# replace ${PATH_TO_INCLUDE_DIR} before including other libraries
# target_include_directories(${APP_NAME} PRIVATE ${PATH_TO_INCLUDE_DIR})

# replace ${PATH_TO_LIB_FILE} before linking other libraries
# target_link_libraries(${APP_NAME} PRIVATE ${PATH_TO_LIB_FILE})

set_target_properties(${LIB_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
)

# Binaries are put into the `./bin` directory by default.
set_target_properties(${BIN_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
)
