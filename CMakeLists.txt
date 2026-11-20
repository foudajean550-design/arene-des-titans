cmake_minimum_required(VERSION 3.10)
project(ArenedesTitans VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(ARENE_TESTS "Compiler les tests unitaires" ON)

if(MSVC)
  add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra)
endif()

# --- Logique pure (sans OpenGL) : testable en isolation ---
add_library(arene_logique STATIC
  src/noyau/Entrees.cpp
  src/jeu/Fiches.cpp
  src/jeu/Combattant.cpp
  src/jeu/Combat.cpp
  src/jeu/IA.cpp)
target_include_directories(arene_logique PUBLIC src)

# --- Rendu et personnages (OpenGL / GLUT) ---
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_library(arene_rendu STATIC
  src/rendu/Primitives.cpp
  src/rendu/Textures.cpp
  src/rendu/Lumieres.cpp
  src/rendu/Camera.cpp
  src/rendu/Hud.cpp
  src/rendu/Particules.cpp
  src/rendu/Scene.cpp
  src/rendu/Ecrans.cpp
  src/rendu/Capture.cpp
  src/personnages/Registre.cpp
  src/personnages/Dinosaure.cpp
  src/personnages/Robot.cpp
  src/personnages/Araignee.cpp
  src/personnages/Dragon.cpp
  src/personnages/Golem.cpp)
target_include_directories(arene_rendu PUBLIC src)
target_link_libraries(arene_rendu PUBLIC arene_logique OpenGL::GL OpenGL::GLU GLUT::GLUT)
target_compile_definitions(arene_rendu PRIVATE
  DOSSIER_ASSETS="${CMAKE_SOURCE_DIR}/assets")
if(APPLE)
  target_compile_definitions(arene_rendu PUBLIC GL_SILENCE_DEPRECATION)
endif()

add_executable(arene-des-titans src/main.cpp src/app/Partie.cpp)
target_link_libraries(arene-des-titans PRIVATE arene_rendu)

if(ARENE_TESTS)
  enable_testing()
  add_executable(test_combat tests/test_combat.cpp)
  target_link_libraries(test_combat PRIVATE arene_logique)
  add_test(NAME combat COMMAND test_combat)
endif()
