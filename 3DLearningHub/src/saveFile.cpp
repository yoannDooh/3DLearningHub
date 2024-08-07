#pragma warning(disable:4996)//to use fopen 

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <string.h>


#include "../header/saveFile.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h" 

std::string getDirename(std::string& path)
{
	std::string directory;
	int direNameEndIndex{ static_cast<int>(path.length()) - 1 };

	while (path[direNameEndIndex] != '\\') //faudrait check si l'os c'est pas autre chose que windows
	{
		--direNameEndIndex;
	}

	directory.reserve(direNameEndIndex + 1);

	for (int caracIndex{}; caracIndex < direNameEndIndex; ++caracIndex)
	{
		directory.push_back(path[caracIndex]);
	}

	return directory;
}

std::string getFileName(std::string& path)
{
	std::string fileName;
	int fileNameEndIndex{ static_cast<int>(path.length()) - 1 };

	//move the index after the extension
	while (path[fileNameEndIndex] != '.')
	{
		--fileNameEndIndex;
	}
	--fileNameEndIndex;

	while (path[fileNameEndIndex] != '\\') //faudrait check si l'os c'est pas autre chose que windows
	{
		fileName = path[fileNameEndIndex] + fileName;
		--fileNameEndIndex;
	}

	return fileName;
}

void writeModelSaveFile(Model& model)
{
	std::string path{ ".\\saveFiles\\modelFiles\\" + model.fileName + "_modelSave.json" };
	std::ofstream file(path); //TODO: adapt relative path according to the os 

	if (!file.is_open())
	{
		std::cerr << "failed to open model save file";
		return;
	}


	//meshesNr attribute
	file << "{ \"meshesNr\" : " << model.meshes.size() << ',\n';

	//path attribute
	file << "{ \"path\" : " << model.path << ',\n';


	//directory attribute
	file << "{ \"path\" : " << model.directoryName << ',\n';

	//meshes array 
	file << "{ \"meshes\" :\n[\n";

	for (int meshIndex{}; meshIndex < model.meshes.size(); ++meshIndex)
	{
		//vertices property

		file << "{\n \"vertices\" : [\n";
		for (int vertexIndex{}; vertexIndex < model.meshes[meshIndex].vertices.size(); ++vertexIndex)
		{
			//coords property
			file << "{ \n \"coord\": {\n";

			file << " \"x\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].coord[0] << ",\n";
			file << " \"y\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].coord[1] << ",\n";
			file << " \"z\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].coord[2] << "\n},";

			//normal property
			file << "{ \n \"normal\": {\n";

			file << " \"x\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].normal[0] << ",\n";
			file << " \"y\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].normal[1] << ",\n";
			file << " \"z\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].normal[2] << "\n},";

			/*
			//tangent property
			file << "{ \n \"tangent\": {\n";

			file << " \"x\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].tangent[0] << ",\n";
			file << " \"y\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].tangent[1] << ",\n";
			file << " \"z\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].tangent[2] << "\n},";

			//bitangent property
			file << "{ \n \"bitangent\": {\n";

			file << " \"x\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].bitangent[0] << ",\n";
			file << " \"y\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].bitangent[1] << ",\n";
			file << " \"z\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].bitangent[2] << "\n},";

			*/

			//textCoord property
			file << "{ \n \"textCoord\": {\n";

			file << " \"u\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].textCoord[0] << ",\n";
			file << " \"v\": {\n \"x\": " << model.meshes[meshIndex].vertices[vertexIndex].textCoord[1] << "\n},";

			//color property
			file << "{ \n \"color\": {\n";

			file << " \"u\": {\n \"r\": " << model.meshes[meshIndex].vertices[vertexIndex].coolors[0] << ",\n";
			file << " \"v\": {\n \"g\": " << model.meshes[meshIndex].vertices[vertexIndex].coolors[1] << ",\n";
			file << " \"v\": {\n \"b\": " << model.meshes[meshIndex].vertices[vertexIndex].coolors[2] << "\n},";

			if (vertexIndex == model.meshes[meshIndex].vertices.size() - 1) // there should not be comma after last element of list in json
				//vertexNr property
				file << " \"vertexNr\": " << model.meshes[meshIndex].vertices[vertexIndex].vertexNb << "\n } ],";

			else
				//vertexNr property
				file << " \"vertexNr\": " << model.meshes[meshIndex].vertices[vertexIndex].vertexNb << "\n },";

		}

		//indices property
		file << "{\n \"indices\" : [\n";
		for (int indiceIndex{}; indiceIndex < model.meshes[meshIndex].indices.size() - 1; ++indiceIndex)
		{
			file << indiceIndex << ',';
		}
		file << model.meshes[meshIndex].indices[model.meshes[meshIndex].indices.size() - 1] << "\n],"; // there should not be comma after last element of list in json

		//textures property
		file << "{\n \"textures\" : [\n";
		for (int textureIndex{}; textureIndex < model.meshes[meshIndex].textures.size(); ++textureIndex)
		{
			//path property
			file << "{ \n \"path\": " << model.meshes[meshIndex].textures[textureIndex].path;

			//type property
			file << "{ \n \"type\": " << textureMapToStr(model.meshes[meshIndex].textures[textureIndex].type);

			if (textureIndex == model.meshes[meshIndex].textures.size() - 1) // there should not be comma after last element of list in json
				file << "{ \n \"ID\": " << model.meshes[meshIndex].textures[textureIndex].ID << "\n} \n],";

			else
				//id property
				file << "{ \n \"ID\": " << model.meshes[meshIndex].textures[textureIndex].ID << "\n},";
		}

		//activateCubeMap property
		if (model.meshes[meshIndex].activateCubeMap)
			file << "{\n \"activateCubeMap\" : true,";
		else
			file << "{\n \"activateCubeMap\" : false,";

		if (model.meshes[meshIndex].activateShadow)
			file << "{\n \"activateShadow\" : true";
		else
			file << "{\n \"activateShadow\" : false";

		//end of current mesh object 
		if (meshIndex == model.meshes.size() - 1) // there should not be comma after last element of list in json
			file << "} \n ] \n }"; //end of the file 

		else
			file << "},";

		file.flush();
	}

}

void writeObjectSaveFile(Object& obj)
{
	std::ofstream file(".\\saveFiles\\objectFiles\\ID-" + std::to_string(obj.id) + "_objectSave.json"); //TODO: adapt relative path according to the os

	if (!file.is_open())
	{
		std::cerr << "failed to open object save file";
		return;
	}

	//start of the file and model path property
	if (obj.model3d != nullptr)
		file << "{\n\"modelPath\" :  \" " << obj.model3d->path << " \",\n";
	else
		file << "{\n\"modelPath\" :  \"\",\n";


	//id property
	file << "\"id\" : " << obj.id << ",\n";

	//worldObjIndex property
	file << "\"worldObjIndex\" : " << obj.worldObjIndex << ",\n";

	//worldLighPointIndex property
	file << "\"worldLighPointIndex\" : " << obj.worldLighPointIndex << ",\n";

	//worldSpotLightIndex property
	file << "\"worldSpotLightIndex\" : " << obj.worldSpotLightIndex << ",\n";

	/*
	//shaderOutline property
	file << "\"shaderOutline\" :  \" " << obj.shaderOutline << " \",\n";
	*/

	//modelMat property, it an array of 16 floats, it goes in colomuns in descding order to represent the 4x4 mat model
	file << "\"modelMat\" :  [";

	for (int matIndex{}; matIndex < 4; ++matIndex)
	{
		file << "\"" << std::to_string(obj.matModel[matIndex].x) << "\",";
		file << "\"" << std::to_string(obj.matModel[matIndex].y) << "\",";
		file << "\"" << std::to_string(obj.matModel[matIndex].z) << "\",";

		if (matIndex != 3)
			file << "\"" << std::to_string(obj.matModel[matIndex].w) << "\",";
		else
			file << "\"" << std::to_string(obj.matModel[matIndex].w) << "\"],\n";

	}

	//localOrigin property
	file << "\"localOrigin\" :  [";
	for (int matIndex{}; matIndex < 4; ++matIndex)
	{
		file << "\"" << std::to_string(obj.localOrigin[matIndex].x) << "\",";
		file << "\"" << std::to_string(obj.localOrigin[matIndex].y) << "\",";
		file << "\"" << std::to_string(obj.localOrigin[matIndex].z) << "\",";

		if (matIndex != 3)
			file << "\"" << std::to_string(obj.localOrigin[matIndex].w) << "\",";
		else
			file << "\"" << std::to_string(obj.localOrigin[matIndex].w) << "\"],\n";
	}

	//pos property
	file << "\"pos\" :  {\n";
	file << " \"x\" : " << "\"" << std::to_string(obj.pos.x) << "\",\n";
	file << " \"y\" : " << "\"" << std::to_string(obj.pos.y) << "\",\n";
	file << " \"z\" : " << "\"" << std::to_string(obj.pos.z) << "\"\n},\n";

	//basePos property
	file << "\"basePos\" :  {\n";
	file << " \"x\" : " << "\"" << std::to_string(obj.basePos.x) << "\",\n";
	file << " \"y\" : " << "\"" << std::to_string(obj.basePos.y) << "\",\n";
	file << " \"z\" : " << "\"" << std::to_string(obj.basePos.z) << "\"\n},\n";

	//orientation property
	file << "\"orientation\" :  {\n";
	file << " \"x\" : " << "\"" << std::to_string(obj.orientation.x) << "\",\n";
	file << " \"y\" : " << "\"" << std::to_string(obj.orientation.y) << "\",\n";
	file << " \"z\" : " << "\"" << std::to_string(obj.orientation.z) << "\"\n},\n";


	//materialShininess property
	file << "\"materialShininess\" : " << "\"" << std::to_string(obj.materialShininess) << "\",\n";

	//enableTranslation property
	if (obj.enableTranslation)
		file << "\"enableTranslation\" : true,\n";
	else
		file << "\"enableTranslation\" : false,\n";

	//enableRotation property
	if (obj.enableRotation)
		file << "\"enableRotation\" : true,\n";
	else
		file << "\"enableRotation\" : false,\n";


	//enableScale property
	if (obj.enableScale)
		file << "\"enableScale\" : true,\n";
	else
		file << "\"enableScale\" : false,\n";

	//enableCollision property
	if (obj.enableCollision)
		file << "\"enableCollision\" : true,\n";
	else
		file << "\"enableCollision\" : false,\n";

	//enableOutLine property
	if (obj.enableOutLine)
		file << "\"enableOutLine\" : true,\n";
	else
		file << "\"enableOutLine\" : false,\n";

	//isGlowing property
	if (obj.isGlowing)
		file << "\"isGlowing\" : true,\n";
	else
		file << "\"isGlowing\" : false,\n";

	//isOrbiting property and end of file
	if (obj.isOrbiting)
		file << "\"isOrbiting\" : true\n}";
	else
		file << "\"isOrbiting\" : false\n}";
}

Object* readObjectSaveFile(std::string& path) //read and allocate Object instance from saveFile 
{
	using namespace rapidjson;

	Object* obj = new Object();

	FILE* fp = fopen(path.c_str(), "r");
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document doc;

	doc.ParseStream(is);

	assert(doc.IsObject()); //document must be an object 


	//mesh Property
	//TODO: check if object is not only a mesh and if so construct mesh and not model property


	//modelPath Property
	//TODO: should construct the model after that by calling readAssimpSaveFile
	obj->model3d = new Model();
	assert(doc.HasMember("modelPath"));
	assert(doc["modelPath"].IsString());
	obj->model3d->path = doc["modelPath"].GetString();


	//id Property
	assert(doc.HasMember("id"));
	assert(doc["id"].IsInt());
	obj->id = doc["id"].GetInt();

	//worldObjIndex Property
	assert(doc.HasMember("worldObjIndex"));
	assert(doc["worldObjIndex"].IsInt());
	obj->worldObjIndex = doc["worldObjIndex"].GetInt();

	//worldLighPointIndex Property
	assert(doc.HasMember("worldLighPointIndex"));
	assert(doc["worldLighPointIndex"].IsInt());
	obj->worldLighPointIndex = doc["worldLighPointIndex"].GetInt();

	//worldSpotLightIndex Property
	assert(doc.HasMember("worldSpotLightIndex"));
	assert(doc["worldSpotLightIndex"].IsInt());
	obj->worldSpotLightIndex = doc["worldSpotLightIndex"].GetInt();


	//modelMat Property
	assert(doc.HasMember("modelMat"));
	assert(doc["modelMat"].IsArray());
	for (int matIndex{}; matIndex < doc["modelMat"].GetArray().Size() / 4; ++matIndex)
	{
		assert(doc["modelMat"].GetArray()[matIndex * 4].IsString());
		assert(doc["modelMat"].GetArray()[matIndex * 4 + 1].IsString());
		assert(doc["modelMat"].GetArray()[matIndex * 4 + 2].IsString());
		assert(doc["modelMat"].GetArray()[matIndex * 4 + 3].IsString());


		obj->matModel[matIndex].x = std::stof(doc["modelMat"].GetArray()[matIndex * 4].GetString());
		obj->matModel[matIndex].y = std::stof(doc["modelMat"].GetArray()[matIndex * 4 + 1].GetString());
		obj->matModel[matIndex].z = std::stof(doc["modelMat"].GetArray()[matIndex * 4 + 2].GetString());
		obj->matModel[matIndex].w = std::stof(doc["modelMat"].GetArray()[matIndex * 4 + 3].GetString());
	}

	//localOrigin Property
	assert(doc.HasMember("localOrigin"));
	assert(doc["localOrigin"].IsArray());
	for (int matIndex{}; matIndex < doc["localOrigin"].GetArray().Size() / 4; ++matIndex)
	{
		assert(doc["localOrigin"].GetArray()[matIndex * 4].IsString());
		assert(doc["localOrigin"].GetArray()[matIndex * 4 + 1].IsString());
		assert(doc["localOrigin"].GetArray()[matIndex * 4 + 2].IsString());
		assert(doc["localOrigin"].GetArray()[matIndex * 4 + 3].IsString());


		obj->localOrigin[matIndex].x = std::stof(doc["localOrigin"].GetArray()[matIndex * 4].GetString());
		obj->localOrigin[matIndex].y = std::stof(doc["localOrigin"].GetArray()[matIndex * 4 + 1].GetString());
		obj->localOrigin[matIndex].z = std::stof(doc["localOrigin"].GetArray()[matIndex * 4 + 2].GetString());
		obj->localOrigin[matIndex].w = std::stof(doc["localOrigin"].GetArray()[matIndex * 4 + 3].GetString());
	}

	//pos Property
	assert(doc.HasMember("pos"));
	assert(doc["pos"].IsObject());

	assert(doc["pos"].HasMember("x"));
	assert(doc["pos"].HasMember("y"));
	assert(doc["pos"].HasMember("z"));

	assert(doc["pos"]["x"].IsString());
	assert(doc["pos"]["y"].IsString());
	assert(doc["pos"]["z"].IsString());

	obj->pos.x = std::stof(doc["pos"]["x"].GetString());
	obj->pos.y = std::stof(doc["pos"]["y"].GetString());
	obj->pos.z = std::stof(doc["pos"]["z"].GetString());

	//basePos Property
	assert(doc.HasMember("basePos"));
	assert(doc["basePos"].IsObject());

	assert(doc["basePos"].HasMember("x"));
	assert(doc["basePos"].HasMember("y"));
	assert(doc["basePos"].HasMember("z"));

	assert(doc["basePos"]["x"].IsString());
	assert(doc["basePos"]["y"].IsString());
	assert(doc["basePos"]["z"].IsString());

	obj->basePos.x = std::stof(doc["basePos"]["x"].GetString());
	obj->basePos.y = std::stof(doc["basePos"]["y"].GetString());
	obj->basePos.z = std::stof(doc["basePos"]["z"].GetString());

	//orientation Property
	assert(doc.HasMember("orientation"));
	assert(doc["orientation"].IsObject());

	assert(doc["orientation"].HasMember("x"));
	assert(doc["orientation"].HasMember("y"));
	assert(doc["orientation"].HasMember("z"));

	assert(doc["orientation"]["x"].IsString());
	assert(doc["orientation"]["y"].IsString());
	assert(doc["orientation"]["z"].IsString());

	obj->orientation.x = std::stof(doc["orientation"]["x"].GetString());
	obj->orientation.y = std::stof(doc["orientation"]["y"].GetString());
	obj->orientation.z = std::stof(doc["orientation"]["z"].GetString());

	//materialShininess Property
	assert(doc.HasMember("materialShininess"));
	assert(doc["materialShininess"].IsString());
	obj->materialShininess = std::stof(doc["materialShininess"].GetString());


	//enableTranslation Property
	assert(doc.HasMember("enableTranslation"));
	assert(doc["enableTranslation"].IsBool());
	obj->enableTranslation = doc["enableTranslation"].GetBool();


	//enableRotation Property
	assert(doc.HasMember("enableRotation"));
	assert(doc["enableRotation"].IsBool());
	obj->enableRotation = doc["enableRotation"].GetBool();

	//enableScale Property
	assert(doc.HasMember("enableScale"));
	assert(doc["enableScale"].IsBool());
	obj->enableScale = doc["enableScale"].GetBool();

	//enableCollision Property
	assert(doc.HasMember("enableCollision"));
	assert(doc["enableCollision"].IsBool());
	obj->enableCollision = doc["enableCollision"].GetBool();

	//enableOutLine Property
	assert(doc.HasMember("enableOutLine"));
	assert(doc["enableOutLine"].IsBool());
	obj->enableOutLine = doc["enableOutLine"].GetBool();

	//isGlowing Property
	assert(doc.HasMember("isGlowing"));
	assert(doc["isGlowing"].IsBool());
	obj->isGlowing = doc["isGlowing"].GetBool();

	//isOrbiting Property
	assert(doc.HasMember("isOrbiting"));
	assert(doc["isOrbiting"].IsBool());
	obj->isOrbiting = doc["isOrbiting"].GetBool();


	return obj;
}