cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME prismatic-game)
set(PLAYDATE_GAME_DEVICE prismatic-game_DEVICE)
set(PROJECT_DIR "/home/dan/Projects/Playdate/prismatic-game/")

project(${PLAYDATE_GAME_NAME} C ASM)

# Set source files
set(SOURCE_FILES
    src/main.c
    src/core/game.c
    src/prismatic/prismatic.c
    src/prismatic/input/input.c
    src/prismatic/logger/logger.c
    src/prismatic/scene/scene.c
    src/prismatic/sprite/sprite.c
    src/prismatic/state/state.c
    src/prismatic/text/text.c
    src/prismatic/transition/transition.c
)

# Set header files
set(HEADER_FILES
    src/core/game.h
    src/prismatic/prismatic.h
    src/prismatic/input/input.h
    src/prismatic/logger/logger.h
    src/prismatic/scene/scene.h
    src/prismatic/sprite/sprite.h
    src/prismatic/state/state.h
    src/prismatic/text/text.h
    src/prismatic/transition/transition.h
)

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE} ${SOURCE_FILES} ${HEADER_FILES})
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES} )
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

