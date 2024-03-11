#include "../header/tuiWindow.h"
#include <iostream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, separator, operator|, vbox, border



namespace File
{
	std::ofstream debugInfo{};
}

//https://arthursonzogni.github.io/FTXUI/examples_2component_2gallery_8cpp-example.html

using namespace ftxui;
 
void caca()
{
	std::cout << "caca" ; 
}

// Display a component nicely with a title on the left.
Component frameLayout(std::string name, Component component,int caracNb) // xShrink=true 
{
	return Renderer(component, [name, component, caracNb,]
	{
    return hbox
	( {
	
        text(name) | size(WIDTH, EQUAL, caracNb),
        separator(),
        component->Render() | xflex,
	} ) 
		   | xflex | border ;

	}
	);
}


Component toggleLayout(std::vector<std::string>names, std::vector<int>selectors) // xShrink=true 
{

	/*
	return Renderer(component, [name, component, caracNb]
		{
			return vbox
			({

				text(name) | size(WIDTH, EQUAL, caracNb),
				separator(),
				component->Render() | xflex,
				})
				| xflex;
		}
	);
	*/
}


int stringMaxLength(std::vector<std::string>strings)
{
	int max{};
	int currentLength{};

	for (const auto& str : strings)
	{
		currentLength = str.length();

		if (currentLength > max)
			max = currentLength;
	}

	return max;
}

void displayTuiWindow()
{
	auto screen = ftxui::ScreenInteractive::TerminalOutput();

	//tabs names and entries 
	std::vector<std::string> tab_values{
	  "Presentation",
	  "Menu",
	  "tab_3",    
	};

	std::vector<std::string> tab_1_entries{
	"Forest",
	"Water",
	"I don't know",
	};

	std::vector<std::string> tab_2_Toggle_option
	{
		"on",
		"off"
	};

	std::vector<std::string> tab2_Toggle1_Name{
		"inverse",
		"greyscale",
		"blur",
		"edgeDetection",
		"wireFrameMode",
		"Rien changer",
	};

	std::vector<std::string> tab2_Toggle2_Name{
		"postion sur la map",
		"Direction du regard",
		"Heure + moment journee",
		"fps",
		"Rien changer",
	};

	std::vector<std::string> tab_3_entries{
	"Table",
	"Nothing",
	"Is",
	"Empty",
	};

	//selectors
	int selectedTab = 0;
	int tab_1_selected = 0;
	int tab2_Toogle1_selected = 0;
	int tab2_Toogle2_selected = 0;
	int tab_3_selected = 0;
	
	//button and tab toogle
	Component button = Button({
  .label = "Click to quit",
  .on_click = screen.ExitLoopClosure(), });

	auto tab_toggle = ftxui::Toggle(&tab_values, &selectedTab);

	//auto test = toggleLayout("r", ftxui::Toggle(&tab2_Toggle1_Name,&tab2_Toogle1_selected), 20);
	//TAB 2 LAYOUT
	int tab_2_Max_Carac{ stringMaxLength({"EFFETS RENDU","Print information tab3"})};

	auto layoutTab2 = Container::Vertical(
		{
		frameLayout("EFFETS RENDU3",ftxui::Radiobox(&tab_2_Toggle_option, &tab2_Toogle2_selected),tab_2_Max_Carac),
		frameLayout("Print information tab3",ftxui::Radiobox(&tab_2_Toggle_option, &tab2_Toogle2_selected),tab_2_Max_Carac),
		button | center,
		} );

	//TABS CONTAINER
	auto tab_container = ftxui::Container::Tab(
		{
			ftxui::Radiobox(&tab_1_entries, &tab_1_selected),
			layoutTab2,
			ftxui::Radiobox(&tab_3_entries, &tab_3_selected),
		},
		&selectedTab) ;

	//WINDOW CONTAINER
	auto container = ftxui::Container::Vertical({
		tab_toggle,
		tab_container,
		});

	//RENDER COMPONENTS
	auto renderer = ftxui::Renderer(container, [&] {
		return  ftxui::vbox({
					tab_toggle->Render(),
					ftxui::separator(),
				   tab_container->Render(),
			}) |
			ftxui::border;
		});

	screen.Loop(renderer);

//	std::cout << tab_2_1_selected;
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
