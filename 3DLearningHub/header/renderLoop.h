#pragma once
#include "../header/window.h"
#include "../header/motion.h"
#include "../header/mesh.h"
#include "../header/shader.h"

void createAndSetLightCube(Shader& shader, std::array<Object, 2>& lightCubesObject);
void createAndSetWoodCube(Shader& shader, Shader& outlineShader, Object& woodCubeObj);
void debugInfo();
