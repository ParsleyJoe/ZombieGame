cmake_minimum_required(VERSION 4.0)

project(ZombieGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(thirdparty/raylib)

add_executable(ZombieGame)

target_sources(ZombieGame
	PRIVATE
		src/main.cpp
		src/game.cpp
		src/player.cpp
		src/enemy.cpp
		src/particlesystem.cpp

		include/game.hpp
		include/player.hpp
		include/enemy.hpp
		include/particlesystem.hpp
)

target_include_directories(ZombieGame
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(ZombieGame PRIVATE /W4)
else()
    target_compile_options(ZombieGame PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(ZombieGame PRIVATE raylib)
