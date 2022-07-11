# Basic parameters
cmake_minimum_required(VERSION 3.9)

project(NinjaThief_32blit)

# Add your sources here (adding headers is optional, but helps some CMake generators)
set(PROJECT_SOURCES
	"ninja_thief.cpp"
	"level.cpp"
)

# ... and any other files you want in the release here
set(PROJECT_DISTRIBS LICENSE README.md)

# All our .cpp files are in the src directory, so we need to add "src/" to the start of each filename
list(TRANSFORM PROJECT_SOURCES PREPEND src/)

# Build configuration; approach this with caution!
if(MSVC)
  add_compile_options("/W4" "/wd4244" "/wd4324" "/wd4458" "/wd4100")
else()
  add_compile_options("-Wall" "-Wextra" "-Wdouble-promotion" "-Wno-unused-parameter")
endif()

find_package(32BLIT CONFIG REQUIRED PATHS ../32blit-sdk)

include_directories(${PROJECT_SOURCE_DIR}/include)

blit_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
blit_assets_yaml(${PROJECT_NAME} assets.yml)
blit_metadata(${PROJECT_NAME} metadata.yml)
add_custom_target(flash DEPENDS ${PROJECT_NAME}.flash)

# Setup release packages
install(FILES ${PROJECT_DISTRIBS} DESTINATION .)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)