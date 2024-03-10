#include "../header/debug.h"
#include <iostream>


namespace File
{
	std::ofstream debugInfo{};
}

void openDebugFile()
{
	/*
	File::debugInfo.open("test.txt", std::ofstream::out | std::ofstream::app);
	

	system("echo merde > test.txt; notepad.exe test.txt");
	*/

	std::string ntm{};
	std::cin >> ntm;

	std::cout << ntm;
}

void printVec3(std::string_view objectName, glm::vec3 object)
{
	std::cout << objectName << ":" << '\n';

	std::cout << '\t' << "x VALUE : " << object.x << "\t(in world space unit : " << object.x / 10.0f << ')' << '\n';
	std::cout << '\t' << "y VALUE : " << object.y << "\t(in world space unit : " << object.y / 10.0f << ')' << '\n';
	std::cout << '\t' << "z VALUE : " << object.z << "\t(in world space unit : " << object.z / 10.0f << ')' << '\n';
	std::cout << "\n\n";
}

void printColor(glm::vec3 color)
{
	std::cout << '\t' << "RED  : " << color.x << '\n';
	std::cout << '\t' << "BLUE : " << color.y << '\n';
	std::cout << '\t' << "GREEN : " << color.z << '\n';
	std::cout << "\n\n";
}

void printTimeInGame()
{
	std::cout << "TIME IN GAME " << ":" << '\n';

	std::cout << '\t' << "Day : " << Time::timeInGame.day << '\n';
	std::cout << '\t' << "Hour : " << Time::timeInGame.hour << '\n';
	std::cout << '\t' << "Min : " << Time::timeInGame.min << '\n';
	std::cout << '\t' << "Sec : " << Time::timeInGame.sec << '\n';
	std::cout << '\t' << "SunAzimuth : " << World::sunAzimuth << '\n';
	std::cout << '\t' << "SunAltitude : " << World::sunAltitude << '\n';
	printCurrentDayPhase();
	std::cout << "\n\n";

	std::cout << "\n" << "couleur sunlight" << '\n';
	printColor(World::directLights[0].color);
	std::cout << "\n\n";
}

void printCurrentDayPhase()
{
	std::cout << '\t' << "Phase of the Day : ";
	switch (Time::dayPhase)
	{
	case dawn:
		std::cout << "Dawn";
		break;

	case daytime:
		std::cout << "daytime";
		break;

	case sunset:
		std::cout << "sunset";
		break;

	case night:
		std::cout << "night";
		break;

	}
	std::cout << '\n';
}

void printLine(int dashNb)
{
	for (int count{}; count <= dashNb; ++count)
	{
		std::cout << '-';
	}
}