/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

namespace sb {
	struct Rect;
	class TextureAtlas {
	public:
		TextureAtlas(const std::string& atlas);
		~TextureAtlas();
		const sb::Rect& operator[](const std::string& txName) const;
		size_t textureCount() const;
		std::vector<std::string> allTextures() const;
	private:
		const std::unordered_map<std::string, sb::Rect> m_textures;
		static std::unordered_map<std::string, sb::Rect> createFromString(const std::string& atlas);
	};
}

