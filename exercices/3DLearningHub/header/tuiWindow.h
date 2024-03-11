#pragma once
#include <fstream>
#include "../header/window.h"
#include "../header/shader.h"
#include "../header/motion.h"
#include "../header/mesh.h"

namespace File
{
	extern std::ofstream debugInfo;
}

//debugInfoFIle function
void displayTuiWindow();

//print functions
void printVec3(std::string_view objectName, glm::vec3 object);
void printTimeInGame();
void printLine(int dashNb);
void printCurrentDayPhase();

