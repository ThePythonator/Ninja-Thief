# Basic parameters
cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME NinjaThief_PicoSystem)

# Set your project sources here
set(PROJECT_SOURCES
	"ninja_thief.cpp"
	"level.cpp"
	"ninja.cpp"
	"player_ninja.cpp"
	"enemy_ninja.cpp"
	"utilities.cpp"
)

# All our .cpp files are in the src directory, so we need to add "src/" to the start of each filename
list(TRANSFORM PROJECT_SOURCES PREPEND src/)

if(NOT PICO_SDK_PATH)
    message(WARNING "PICO_SDK_PATH not specified. Forcing fetch from git!")
    set(PICO_SDK_FETCH_FROM_GIT true)
endif()

# Pull in PICO SDK
include(pico_sdk_import.cmake)

# Set up proejct
project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Find the PicoSystem library
include(picosystem_sdk_import.cmake)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Create the output target
picosystem_executable(
  ${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

# Set your build options here
pixel_double(${PROJECT_NAME})          # 120x120 resolution game, pixel-doubled to 240x240
#disable_startup_logo(${PROJECT_NAME})  # Skip the PicoSystem splash
#no_font(${PROJECT_NAME})              # Omit the default font
#no_spritesheet(${PROJECT_NAME})       # Omit the default spritesheet
#no_overclock(${PROJECT_NAME})         # Don't overclock