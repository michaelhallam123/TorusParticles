#include "loadPreset.hpp"

#include "json/value.h"
#include "json/json.h"

#include <fstream>
#include <vector>
#include <array>
#include <iostream>

Preset loadPreset(const std::string& filepath)
/**
 * Load Preset data from "filepath" (a .json file).
 */
{
	Preset preset;

	std::ifstream file(filepath);

	if (!file.is_open())
	{
		std::cout << "Error: could not open file at location \"" << filepath << "\"" << std::endl;
		return preset;
	}

	Json::Value   jsonTotal;
	Json::Reader  reader;

	bool readSuccessful = reader.parse(file, jsonTotal);

	if (!readSuccessful)
	{
		std::cout << "Error: failed to parse file \"" << filepath << "\"" << std::endl;
		std::cout << "Please ensure file is a valid .json file" << std::endl;
		return preset;
	}

	preset.dt = jsonTotal["dt"].asFloat();
	preset.worldAspectRatio = jsonTotal["worldAspectRatio"].asFloat();
	preset.antialiasing = jsonTotal["antialiasing"].asBool();
	
	std::vector<BallType>& ballTypes = preset.ballTypes;

	Json::Value ballTypesJson = jsonTotal["ballTypes"];

	for (auto& json : ballTypesJson)
	{
		BallType bt;

		bt.mass   = json["mass"].asFloat();
		bt.radius = json["radius"].asFloat();
		bt.count  = json["count"].asUInt();

		std::vector<float> rgba;
		for (auto& val : json["rgba"])
		{
			rgba.push_back(val.asFloat());
		}
		for (int i = 0; i < 4; i++)
		{
			bt.rgba[i] = rgba[i];
		}

		std::vector<float> totalMomentum;
		for (auto& val : json["totalMomentum"])
		{
			totalMomentum.push_back(val.asFloat());
		}
		bt.totalMomentum.x = totalMomentum[0];
		bt.totalMomentum.y = totalMomentum[1];

		bt.wrapTexture = json["wrapTexture"].asBool();
		bt.render      = json["render"].asBool();

		ballTypes.push_back(bt);
	}

	preset.loadSuccessful = true;

	return preset;
}