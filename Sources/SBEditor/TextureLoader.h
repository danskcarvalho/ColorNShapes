/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "ResourceLoader.h"

#pragma once

namespace sb {
	class DXContext;

	class TextureLoader : public ResourceLoader {
	public:
		TextureLoader(const DXContext* ctx, const std::string& textureName);
		virtual ~TextureLoader();

		virtual ResourceLoaderTask load() override;
		virtual void unload() override;
		virtual bool hasLoaded() const override;
		virtual bool isLoading() const override;

		ID3D11Resource* texture() const;
		ID3D11ShaderResourceView* textureView() const;
	private:
		const DXContext* m_context;
		const std::string m_textureName;
		std::atomic<bool> m_loaded;
		std::atomic<bool> m_isLoading;

		ID3D11Resource* m_texture;
		ID3D11ShaderResourceView* m_textureView;
	};
}

