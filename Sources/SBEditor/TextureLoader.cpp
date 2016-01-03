/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include <ppltasks.h>
#include "TextureLoader.h"
#include "WICTextureLoader.h"
#include "Utils.h"
#include "DXContext.h"
#include "..\Common\DirectXHelper.h"

using namespace sb;

TextureLoader::TextureLoader(const DXContext* ctx, const std::string& textureName) 
	: m_context(ctx), m_textureName(textureName), m_loaded(false), m_isLoading(false) {
	assert(ctx);
	m_texture = nullptr;
	m_textureView = nullptr;
}

bool TextureLoader::hasLoaded() const {
	return m_loaded.load(std::memory_order_seq_cst);
}

bool TextureLoader::isLoading() const {
	return m_isLoading.load(std::memory_order_seq_cst);
}

ID3D11Resource * sb::TextureLoader::texture() const {
	return m_texture;
}

ID3D11ShaderResourceView * sb::TextureLoader::textureView() const {
	return m_textureView;
}

ResourceLoaderTask TextureLoader::load() {
	if (isLoading())
		return Concurrency::task_from_result();
	if (hasLoaded())
		return Concurrency::task_from_result();

	m_isLoading.store(true, std::memory_order_seq_cst);
	auto txBytes = DX::ReadDataAsync(sb::fromString(m_textureName));
	auto ctx = this->m_context;
	ID3D11Resource** tx = &this->m_texture;
	ID3D11ShaderResourceView** txView = &this->m_textureView;
	std::atomic<bool>* loaded = &this->m_loaded;
	std::atomic<bool>* isLoading = &this->m_isLoading;

	return txBytes.then([=](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromMemory(
				ctx->device(),
				ctx->deviceContext(),
				fileData.data(),
				fileData.size(),
				tx,
				txView
				)
			);

		loaded->store(true, std::memory_order_seq_cst);
		isLoading->store(false, std::memory_order_seq_cst);
	});
}

void sb::TextureLoader::unload() {
	while (m_isLoading.load(std::memory_order_seq_cst));
	if (!hasLoaded())
		return;

	m_textureView->Release();
	m_texture->Release();

	m_textureView = nullptr;
	m_texture = nullptr;

	m_loaded.store(false, std::memory_order_seq_cst);
}

TextureLoader::~TextureLoader() {
	unload();
}
