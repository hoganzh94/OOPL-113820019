cmake_minimum_required(VERSION 3.16)
project(PvZ_113820019)

set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    ptsd

    URL         https://github.com/ntut-open-source-club/practical-tools-for-simple-design/archive/refs/tags/v0.2.zip

    # Or alternatively if you want to use a specific git commit instead of a release:
    # GIT_REPOSITORY  https://github.com/ntut-open-source-club/practical-tools-for-simple-design.git
    # GIT_TAG         <git_tag>

    SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/PTSD
)

FetchContent_MakeAvailable(ptsd)

include(files.cmake)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
list(TRANSFORM SRC_FILES PREPEND ${SRC_DIR}/)
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR}/)

# file(GLOB_RECURSE SRC_FILES src/*)
# file(GLOB_RECURSE HEADER_FILES include/*)

add_executable(${PROJECT_NAME}
    ${SRC_DIR}/main.cpp
    ${SRC_FILES}
    ${HEADER_FILES}
        include/GameObject.hpp
        include/Plants.hpp
        include/SceneManager.hpp
        src/SceneManager.cpp
        include/Grid.hpp
        include/SeedPacket.hpp
        include/SeedPacketManager.hpp
        include/Sun.hpp
        include/SunManager.hpp
        include/PotatoMine.hpp
        include/CherryBomb.hpp
        include/Peashooter.hpp
        include/Sunflower.hpp
        include/Wallnut.hpp
        include/Zombies.hpp
        include/NormalZombie.hpp
        include/Pea.hpp
        include/Config.hpp
        include/AllPlants.hpp
        include/AllZombies.hpp
        include/LevelLoader.hpp
        include/PlantFactory.hpp
        include/LawnMower.hpp
        include/FlagZombie.hpp
        include/ConeHeadZombie.hpp
        include/PlantType.hpp
        include/AbilityInterfaces.hpp
        include/GameWorld.hpp
        include/CombatSystem.hpp
        include/LevelController.hpp
        src/LevelLoader.cpp
        src/PlantFactory.cpp
        include/RenderSystem.hpp
        include/UIController.hpp
        include/SnowPea.hpp
        include/SnowPeaBullet.hpp
        include/Chomper.hpp
        include/BucketHeadZombie.hpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Resources")
else()
    message(FATAL_ERROR "relative RESOURCE_DIR is WIP, Please use `-DCMAKE_BUILD_TYPE=Debug` build for now.")
    # target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/Resources")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PTSD/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2main
    PTSD
)
