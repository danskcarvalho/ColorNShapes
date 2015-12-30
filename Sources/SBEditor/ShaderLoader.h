/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "ResourceLoader.h"

#pragma once

namespace sb {
	class DXContext;

	class ShaderLoader : public ResourceLoader {
	public:
		ShaderLoader(const DXContext* ctx, const std::string& vertexShader, const std::string& pixelShader);
		virtual ~ShaderLoader();

		virtual ResourceLoaderTask load() override;
		virtual void unload() override;
		virtual bool hasLoaded() const override;
		virtual bool isLoading() const override;

		ID3D11VertexShader* vertexShader() const;
		ID3D11PixelShader* pixelShader() const;
		const std::vector<byte>& vertexShaderBytes() const;
	private:
		const DXContext* m_context;
		const std::string m_vertexShaderName;
		const std::string m_pixelShaderName;
		std::atomic<bool> m_loaded;
		std::atomic<bool> m_isLoading;

		std::vector<byte> m_vertexShaderBytes;
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
	};
}

