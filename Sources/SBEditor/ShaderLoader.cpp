/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include <ppltasks.h>
#include "ShaderLoader.h"
#include "Utils.h"
#include "DXContext.h"
#include "..\Common\DirectXHelper.h"

using namespace sb;

ShaderLoader::ShaderLoader(const DXContext* ctx, const std::string& vertexShader, const std::string& pixelShader) 
	: m_context(ctx), m_vertexShaderName(vertexShader), m_pixelShaderName(pixelShader), m_loaded(false), m_isLoading(false) {
	assert(ctx);
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
}

ShaderLoader::~ShaderLoader() {
	unload();
}

ResourceLoaderTask sb::ShaderLoader::load() {
	if (isLoading())
		return Concurrency::task_from_result();
	if (hasLoaded())
		return Concurrency::task_from_result();

	m_isLoading.store(true, std::memory_order_seq_cst);
	auto pixelShaderF = DX::ReadDataAsync(sb::fromString(m_pixelShaderName));
	auto vertexShaderF = DX::ReadDataAsync(sb::fromString(m_vertexShaderName));
	auto ctx = this->m_context;
	std::vector<byte>* vShaderBytes = &this->m_vertexShaderBytes;
	ID3D11VertexShader** vShader = &this->m_vertexShader;
	ID3D11PixelShader** pShader = &this->m_pixelShader;
	std::atomic<bool>* loaded = &this->m_loaded;
	std::atomic<bool>* isLoading = &this->m_isLoading;

	auto loadPixelShader = pixelShaderF.then([=](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			ctx->device()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				pShader
				)
			);
	});

	auto loadVertexShader = vertexShaderF.then([=](const std::vector<byte>& fileData) {
		*vShaderBytes = fileData;

		DX::ThrowIfFailed(
			ctx->device()->CreateVertexShader(
				vShaderBytes->data(),
				vShaderBytes->size(),
				nullptr,
				vShader
				)
			);
	});

	return (loadPixelShader && loadVertexShader).then([=]() {
		loaded->store(true, std::memory_order_seq_cst);
		isLoading->store(false, std::memory_order_seq_cst);
	});
}

void sb::ShaderLoader::unload() {
	while (m_isLoading.load(std::memory_order_seq_cst));
	if (!hasLoaded())
		return;

	m_vertexShader->Release();
	m_pixelShader->Release();
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;

	m_loaded.store(false, std::memory_order_seq_cst);
}

bool sb::ShaderLoader::hasLoaded() const {
	return m_loaded.load(std::memory_order_seq_cst);
}

bool sb::ShaderLoader::isLoading() const {
	return m_isLoading.load(std::memory_order_seq_cst);
}

ID3D11VertexShader * sb::ShaderLoader::vertexShader() const {
	return m_vertexShader;
}

ID3D11PixelShader * sb::ShaderLoader::pixelShader() const {
	return m_pixelShader;
}

const std::vector<byte>& sb::ShaderLoader::vertexShaderBytes() const {
	return m_vertexShaderBytes;
}
