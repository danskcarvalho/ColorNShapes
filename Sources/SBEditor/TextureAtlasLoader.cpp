/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include <ppltasks.h>
#include "TextureAtlasLoader.h"
#include "TextureAtlas.h"
#include "Utils.h"
#include "..\Common\DirectXHelper.h"

using namespace sb;

TextureAtlasLoader::TextureAtlasLoader(const std::string& atlasName) 
	: m_atlasName(atlasName), m_loaded(false), m_isLoading(false) {
	m_textureAtlas = nullptr;
}

TextureAtlasLoader::~TextureAtlasLoader() {
	unload();
}

ResourceLoaderTask sb::TextureAtlasLoader::load() {
	if (isLoading())
		return Concurrency::task_from_result();
	if (hasLoaded())
		return Concurrency::task_from_result();

	m_isLoading.store(true, std::memory_order_seq_cst);
	auto atlasF = DX::ReadDataAsync(sb::fromString(m_atlasName));
	const TextureAtlas** ptrAtlas = &this->m_textureAtlas;
	std::atomic<bool>* loaded = &this->m_loaded;
	std::atomic<bool>* isLoading = &this->m_isLoading;

	return atlasF.then([=](const std::vector<byte>& fileData) {
		auto strAtlas = std::string(reinterpret_cast<const char*>(fileData.data()), fileData.size());
		*ptrAtlas = new TextureAtlas(strAtlas);

		loaded->store(true, std::memory_order_seq_cst);
		isLoading->store(false, std::memory_order_seq_cst);
	});
}

void sb::TextureAtlasLoader::unload() {
	while (m_isLoading.load(std::memory_order_seq_cst));
	if (!hasLoaded())
		return;

	delete m_textureAtlas;
	m_textureAtlas = nullptr;

	m_loaded.store(false, std::memory_order_seq_cst);
}

bool sb::TextureAtlasLoader::hasLoaded() const {
	return m_loaded.load(std::memory_order_seq_cst);
}

bool sb::TextureAtlasLoader::isLoading() const {
	return m_isLoading.load(std::memory_order_seq_cst);
}

const TextureAtlas * sb::TextureAtlasLoader::textureAtlas() const {
	return m_textureAtlas;
}
