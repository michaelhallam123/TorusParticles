#include "loadPreset.hpp"

#include "json/value.h"
#include "json/json.h"

#include <fstream>
#include <vector>
#include <array>
#include <iostream>

std::vector<BallType> loadPreset(const std::string& filepath)
/**
 * Load BallType data from "filepath" (a .json file) and 
 * output a vector of the data.
 */
{
	std::cout << "Calling loadPreset\n";
	std::vector<BallType> res;

	std::ifstream file(filepath);
	std::cout << "Found filepath\n";
	Json::Value   jsonTotal;
	Json::Reader  reader;

	reader.parse(file, jsonTotal);

	std::cout << "Parsed file\n";

	for (auto& json : jsonTotal)
	{
		std::cout << "Entered loop\n";
		BallType bt;

		bt.mass   = json["mass"].asFloat();
		bt.radius = json["radius"].asFloat();
		bt.count  = json["count"].asUInt();

		std::cout << "First json calls" << std::endl;

		std::cout << "Mass: " << bt.mass << std::endl;

		std::vector<float> rgba;
		for (auto& val : json["rgba"])
		{
			std::cout << val << std::endl;
			rgba.push_back(val.asFloat());
			std::cout << "pushed" << std::endl;
		}
		std::cout << "exited first for loop" << std::endl;
		for (int i = 0; i < 4; i++)
		{
			std::cout << "c";
			bt.rgba[i] = rgba[i];
			std::cout << "d";
		}

		std::cout << "Got rgba" << std::endl;

		std::vector<float> totalMomentum;
		for (auto& val : json["totalMomentum"])
		{
			totalMomentum.push_back(val.asFloat());
		}
		bt.totalMomentum.x = totalMomentum[0];
		bt.totalMomentum.y = totalMomentum[1];

		std::cout << "Got momentum" << std::endl;

		bt.wrapTexture = json["wrapTexture"].asBool();
		bt.render      = json["render"].asBool();

		res.push_back(bt);

		std::cout << "End of loop" << std::endl;
	}

	std::cout << "Exited loadPreset\n";

	return res;
}