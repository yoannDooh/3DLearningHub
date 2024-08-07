#pragma once

#include "..\header\mesh.h"
#include "..\header\motion.h"




std::string getDirename(std::string& path); //return directory of the file 
std::string getFileName(std::string& path);// return name of the file without its extension 

void writeModelSaveFile(Model& model);
void writeObjectSaveFile(Object& obj);

Object* readObjectSaveFile(std::string& path);