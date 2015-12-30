/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Rect.h"
#include "TextureAtlas.h"

using namespace sb;

TextureAtlas::TextureAtlas(const std::string& atlas) : m_textures(createFromString(atlas)) {
}

TextureAtlas::~TextureAtlas() {
}

const sb::Rect & sb::TextureAtlas::operator[](const std::string & txName) const {
	return m_textures.at(txName);
}

size_t sb::TextureAtlas::textureCount() const {
	return m_textures.size();
}

std::vector<std::string> sb::TextureAtlas::allTextures() const {
	std::vector<std::string> texNames;
	for (auto& kvp : m_textures)
		texNames.push_back(kvp.first);
	return texNames;
}

std::unordered_map<std::string, sb::Rect> sb::TextureAtlas::createFromString(const std::string & atlas) {
	std::unordered_map<std::string, sb::Rect> map;
	std::vector<std::string> lines;
	float w = 0, h = 0;
	boost::split(lines, atlas, boost::is_any_of("\n"), boost::token_compress_on);
	for (auto& l : lines) {
		std::vector<std::string> values;
		boost::split(values, l, boost::is_any_of(":"), boost::token_compress_on);
		if (values.size() != 5)
			continue;
		for (size_t i = 0; i < values.size(); i++)
			boost::trim(values[i]);
		auto width = std::stof(values[3]);
		auto height = std::stof(values[4]);

		if (values[0] == "@@TX_SELF") {
			w = width;
			h = height;
			continue;
		}

		auto centerX = std::stof(values[1]) + width / 2.0f;
		auto centerY = std::stof(values[2]) + height / 2.0f;
		//Normalized
		auto r = sb::Rect(centerX / w, centerY / h, width / w, height / h);
		map.insert({ values[0], r });
	}
	return map;
}
