/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "LinearCache.h"

#pragma once

namespace sb {
	class DXContext;

	class RenderTargetInterfaces {
	public:
		ID3D11Texture2D* renderTargetTexture;
		ID3D11RenderTargetView* renderTargetView;
		ID3D11ShaderResourceView* shaderResourceView;

		inline RenderTargetInterfaces() {
			renderTargetTexture = nullptr;
			renderTargetView = nullptr;
			shaderResourceView = nullptr;
		}
	};

	class RenderTargetConfiguration {
	public:
		friend class RenderTargetMapper;
		RenderTargetConfiguration(size_t width, size_t height) {
			this->width = width;
			this->height = height;
		}

		size_t width;
		size_t height;
	private:
		RenderTargetInterfaces build(const DXContext* ctx) const;
	};

	inline bool operator==(const RenderTargetConfiguration& c1, const RenderTargetConfiguration& c2) {
		return c1.width == c2.width && c1.height == c2.height;
	}
	inline bool operator!=(const RenderTargetConfiguration& c1, const RenderTargetConfiguration& c2) {
		return c1.width != c2.width || c1.height != c2.height;
	}

	class RenderTargetMapper {
	public:
		RenderTargetInterfaces operator()(const RenderTargetConfiguration& input, const DXContext* ctx) const;
		void destroy(const RenderTargetInterfaces& output) const;
	};

	template<size_t _CacheSize>
	class RenderTargetCache : 
		public LinearCache<
		_CacheSize, 
		RenderTargetConfiguration, 
		RenderTargetInterfaces, 
		RenderTargetMapper> {
	public:
		SbGenerateLinearCacheCtorWithMapper(RenderTargetCache, RenderTargetConfiguration, RenderTargetInterfaces, RenderTargetMapper);
	};

	typedef RenderTargetCache<6> DefaultRenderTargetCache;
}

#if !defined(NO_EXTERN_TEMPLATES)
extern template class sb::RenderTargetCache<6>;
extern template class sb::LinearCache<6, sb::RenderTargetConfiguration, sb::RenderTargetInterfaces, sb::RenderTargetMapper>;
#endif

