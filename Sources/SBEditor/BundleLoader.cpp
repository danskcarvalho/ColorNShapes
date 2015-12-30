/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include <ppltasks.h>
#include "BundleLoader.h"
#include "DXContext.h"
#include "TextureLoader.h"
#include "TextureAtlasLoader.h"
#include "ShaderLoader.h"

using namespace sb;


BundleLoader::BundleLoader(const DXContext* context) : m_context(context), m_loaded(false), m_isLoading(false) {
	assert(context);
	m_isDescribing = false;
}

BundleLoader::~BundleLoader() {
	unload();
}

ResourceLoaderTask sb::BundleLoader::load() {
	if (isLoading())
		return Concurrency::task_from_result();
	if (hasLoaded())
		return Concurrency::task_from_result();

	if (m_loaders.size() == 0) {
		m_isDescribing = true;
		loadBundle();
		m_isDescribing = false;
	}

	if (m_loaders.size() == 0)
		return Concurrency::task_from_result();

	m_isLoading.store(true, std::memory_order_seq_cst);
	auto tasks = Concurrency::task_from_result();
	for (auto& kvp : m_loaders)
		tasks = tasks && kvp.second->load();

	std::atomic<bool>* loaded = &this->m_loaded;
	std::atomic<bool>* isLoading = &this->m_isLoading;

	return tasks.then([=]() {
		loaded->store(true, std::memory_order_seq_cst);
		isLoading->store(false, std::memory_order_seq_cst);
	});
}

void sb::BundleLoader::unload() {
	while (m_isLoading.load(std::memory_order_seq_cst));
	if (!hasLoaded())
		return;

	for (auto& kvp : m_loaders)
		delete kvp.second;

	m_loaders.clear();

	m_loaded.store(false, std::memory_order_seq_cst);
}

bool sb::BundleLoader::hasLoaded() const {
	return m_loaded.load(std::memory_order_seq_cst);
}

bool sb::BundleLoader::isLoading() const {
	return m_isLoading.load(std::memory_order_seq_cst);
}

ID3D11Resource * sb::BundleLoader::texture(size_t id) const {
	assert(m_loaders.count(id));
	return dynamic_cast<TextureLoader*>(m_loaders.at(id))->texture();
}

ID3D11ShaderResourceView * sb::BundleLoader::textureView(size_t id) const {
	assert(m_loaders.count(id));
	return dynamic_cast<TextureLoader*>(m_loaders.at(id))->textureView();
}

const TextureAtlas * sb::BundleLoader::textureAtlas(size_t id) const {
	assert(m_loaders.count(id));
	return dynamic_cast<TextureAtlasLoader*>(m_loaders.at(id))->textureAtlas();
}

ID3D11VertexShader * sb::BundleLoader::vertexShader(size_t id) const {
	assert(m_loaders.count(id));
	return dynamic_cast<ShaderLoader*>(m_loaders.at(id))->vertexShader();
}

ID3D11PixelShader * sb::BundleLoader::pixelShader(size_t id) const {
	assert(m_loaders.count(id));
	return dynamic_cast<ShaderLoader*>(m_loaders.at(id))->pixelShader();
}

const std::vector<byte>& sb::BundleLoader::vertexShaderBytes(size_t id) const {
	assert(m_loaders.count(id));
	return dynamic_cast<ShaderLoader*>(m_loaders.at(id))->vertexShaderBytes();
}

void sb::BundleLoader::loadTexture(size_t id, const std::string & name) {
	assert(m_isDescribing);
	assert(!m_loaders.count(id));
	m_loaders.insert({ id, new TextureLoader(m_context, name) });
}

void sb::BundleLoader::loadShader(size_t id, const std::string & vertexShader, const std::string & pixelShader) {
	assert(m_isDescribing);
	assert(!m_loaders.count(id));
	m_loaders.insert({ id, new ShaderLoader(m_context, vertexShader, pixelShader) });
}

void sb::BundleLoader::loadTextureAtlas(size_t id, const std::string & name) {
	assert(m_isDescribing);
	assert(!m_loaders.count(id));
	m_loaders.insert({ id, new TextureAtlasLoader(name) });
}
