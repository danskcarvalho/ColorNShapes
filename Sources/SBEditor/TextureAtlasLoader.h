/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "ResourceLoader.h"

#pragma once

namespace sb {
	class DXContext;
	class TextureAtlas;

	class TextureAtlasLoader : public ResourceLoader {
	public:
		TextureAtlasLoader(const std::string& atlasName);
		~TextureAtlasLoader();

		virtual ResourceLoaderTask load() override;
		virtual void unload() override;
		virtual bool hasLoaded() const override;
		virtual bool isLoading() const override;

		const TextureAtlas* textureAtlas() const;
	private:
		const std::string m_atlasName;
		std::atomic<bool> m_loaded;
		std::atomic<bool> m_isLoading;

		const TextureAtlas* m_textureAtlas;
	};
}

