/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "ResourceLoader.h"

#pragma once

namespace sb {
	class TextureAtlas;
	class DXContext;
	class BundleManager;

	class BundleLoader : public ResourceLoader {
	public:
		friend class BundleManager;

		BundleLoader(const DXContext* context);
		virtual ~BundleLoader();

		virtual bool hasLoaded() const override;
		virtual bool isLoading() const override;

		ID3D11Resource* texture(size_t id) const;
		ID3D11ShaderResourceView* textureView(size_t id) const;
		const TextureAtlas* textureAtlas(size_t id) const;
		ID3D11VertexShader* vertexShader(size_t id) const;
		ID3D11PixelShader* pixelShader(size_t id) const;
		const std::vector<byte>& vertexShaderBytes(size_t id) const;
	protected:
		void loadTexture(size_t id, const std::string& name);
		void loadShader(size_t id, const std::string& vertexShader, const std::string& pixelShader);
		void loadTextureAtlas(size_t id, const std::string& name);
		virtual void loadBundle() = 0;
	private:
		virtual ResourceLoaderTask load() override;
		virtual void unload() override;
	private:
		const DXContext* m_context;
		std::atomic<bool> m_loaded;
		std::atomic<bool> m_isLoading;
		bool m_isDescribing;

		std::unordered_map<size_t, ResourceLoader*> m_loaders;
	};
}

