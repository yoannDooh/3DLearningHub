#include "../header/tuiWindow.h"
#include <iostream>
#include <algorithm>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, separator, operator|, vbox, border

#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")



namespace File
{
	std::ofstream debugInfo{};
}

namespace TuiSelectors
{
	int effects_Selector{ 4 };
	int wireframe_Selector{ 1 };
	std::array<int, 7> infoToPrint_Selector{};
}


//https://arthursonzogni.github.io/FTXUI/examples_2component_2gallery_8cpp-example.html

using namespace ftxui;
 
// Display a component nicely with a title on the left.
Component frameLayout(std::string name, Component component,int caracNb) // xShrink=true 
{
	return Renderer(component, [name, component, caracNb]
	{
    return hbox
	( {
	
        text(name) | size(WIDTH, EQUAL, caracNb) | center  ,
        separator(),
        component->Render() | xflex,
	} ) 
		   | xflex | border ;

	}
	);
}

Component verticalFrameLayout(std::string name, Component component, int caracNb) // xShrink=true 
{
	return Renderer(component, [name, component, caracNb]
		{
			return vbox
			({

				text(name) | size(WIDTH, EQUAL, caracNb) | center,
				separator(),
				component->Render() | xflex | center,
				})
				| xflex | border;

		}
	);
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

template<int size>
int stringMaxLength(std::array<std::string,size> strings)
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
	  "Render Information",    
	};

	std::vector<std::string> on_off_toggleOption{
		"on",
		"off"
	};

	std::vector<std::string> tab2_Toggle1_Name{
		"inverse",
		"greyscale",
		"blur",
		"edgeDetection",
		"none"
	};

	std::vector<std::string> tab2_Toggle2_Name{
		"postion sur la map",
		"Direction du regard",
		"Heure + moment journee",
		"fps",
	};

	//selectors
	int selectedTab = 0;
	std::fill_n(TuiSelectors::infoToPrint_Selector.begin(), 7, 1);

	//tab togle
	auto tab_toggle = ftxui::Toggle(&tab_values, &selectedTab);

	
	//button
	std::function<void()> buttonPressed_Callback = &buttonPressed;
	Component button{ Button("Click to validate",buttonPressed_Callback) };


	//TAB 1 LAYOUT
	Component textRenderer = Renderer([] {
		return text("HOLA BONJOUR TEXTE DE PRESENTATION ON VA ALLER LE CHERCHER DANS UN AUTRE FICHIER APRES MAIS VOILA EN ATTENDANT J'AI RIEN A DIRE");
		});

	Component layoutTab1 = Container::Vertical(
		{
			textRenderer
		});

	//TAB 2 LAYOUT
	Components tab2_infoToPrint_Toggles;
	Components tab2_infoToPrint_Toggles_ToBeRender; //je suis vraiment fatigue pardon les noms de variables 

	for (int EffectsToggleIndex{}; EffectsToggleIndex < tab2_Toggle2_Name.size(); ++EffectsToggleIndex)
	{
		tab2_infoToPrint_Toggles.push_back(ftxui::Toggle(&on_off_toggleOption, &TuiSelectors::infoToPrint_Selector[EffectsToggleIndex]));
		tab2_infoToPrint_Toggles_ToBeRender.push_back(verticalFrameLayout(tab2_Toggle2_Name[EffectsToggleIndex], tab2_infoToPrint_Toggles[EffectsToggleIndex], tab2_Toggle2_Name[EffectsToggleIndex].length()));
	}
	Component informationToPrintToggle = Container::Horizontal(tab2_infoToPrint_Toggles_ToBeRender);
	informationToPrintToggle = frameLayout("Information to print", informationToPrintToggle, 21);



	Component effectToggle = frameLayout("Effects", Toggle(&tab2_Toggle1_Name, &TuiSelectors::effects_Selector), stringMaxLength(tab2_Toggle1_Name));
	Component wireFrameToggle = frameLayout("Wireframe", Toggle(&on_off_toggleOption, &TuiSelectors::wireframe_Selector),10);

	Component layoutTab2 = Container::Vertical(
		{
			effectToggle,
			wireFrameToggle,
			informationToPrintToggle,
			button | center,
		} );

	//TAB 3 LAYOUT
	Component textRendererTab3 = Renderer([] {
		return text("ici ca va print des infos a balles ");
		});

	Component layoutTab3 = Container::Vertical(
		{
			textRendererTab3
		});

	//TABS CONTAINER
	auto tab_container = ftxui::Container::Tab(
		{
			layoutTab1,			
			layoutTab2,
			layoutTab3,
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

}

void buttonPressed()
{
	//effect
	UsrParameters::currentEffect = static_cast<Effects>(TuiSelectors::effects_Selector);

	//wireframe
	UsrParameters::activateWireframe = !static_cast<bool>(TuiSelectors::wireframe_Selector);

	//info to print
	for (int count{}; count < USR_PARAMETERS_INFO_TO_PRINT_NB; ++count)
	{
		if (TuiSelectors::infoToPrint_Selector[count] == 0)
			UsrParameters::infoOptions[static_cast<UsrParameters::InfoOption>(count)] = false;
		else
			UsrParameters::infoOptions[static_cast<UsrParameters::InfoOption>(count)] = true;
	}

	PlaySound(TEXT("sound"), NULL, SND_FILENAME);
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
