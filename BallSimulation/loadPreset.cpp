#include "loadPreset.hpp"

#include "json/value.h"
#include "json/json.h"
#include "balltype.hpp"
#include <fstream>
#include <vector>
#include <array>
#include "vec2.hpp"

std::vector<balltype> loadPreset(const std::string& filepath)
/*
 * Uses jsoncpp (https://github.com/open-source-parsers/jsoncpp) to
 * load preset settings into a vector of balltypes, to be passed
 * into the Solver constructor.
 * 
 * Examples of preset files are located in the project resources
 * folder.
 */
{
	std::vector<balltype> res;

	std::ifstream file(filepath);
	Json::Value jsonTotal;
	Json::Reader reader;

	reader.parse(file, jsonTotal);

	for (auto& json : jsonTotal)
	{
		balltype bt;

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

		res.push_back(bt);
	}


	return res;
}