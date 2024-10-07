#include "loadPreset.hpp"

#include "json/value.h"
#include "json/json.h"

#include <fstream>
#include <vector>
#include <array>
#include <iostream>

Preset loadPreset(const std::string& filepath)
/**
 * Load BallType data from "filepath" (a .json file) and 
 * output a vector of the data.
 */
{
	Preset preset;
	std::vector<BallType>& ballTypes = preset.ballTypes;

	std::ifstream file(filepath);
	Json::Value   jsonTotal;
	Json::Reader  reader;

	reader.parse(file, jsonTotal);

	preset.dt = jsonTotal["dt"].asFloat();
	preset.worldAspectRatio = jsonTotal["worldAspectRatio"].asFloat();
	preset.antialiasing = jsonTotal["antialiasing"].asBool();

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

	return preset;
}