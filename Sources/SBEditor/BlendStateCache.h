/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "LinearCache.h"

#pragma once

namespace sb {
	enum class Blending {
		Opaque,
		Normal,
		NpNormal,
		Add,
		NpAdd,
		Multiply,
		NpMultiply
	};

	class DXContext;
	class BlendStateMapper {
	public:
		ID3D11BlendState* operator()(Blending input, const DXContext* ctx) const;
		void destroy(ID3D11BlendState* output) const;
	};

	template<size_t _CacheSize>
	class BlendStateCache : public LinearCache<_CacheSize, Blending, ID3D11BlendState*, BlendStateMapper> {
	public:
		SbGenerateLinearCacheCtorWithMapper(BlendStateCache, Blending, ID3D11BlendState*, BlendStateMapper);
	};

	typedef BlendStateCache<20> DefaultBlendStateCache;
}

#if !defined(NO_EXTERN_TEMPLATES)
extern template class sb::BlendStateCache<20>;
extern template class sb::LinearCache<20, sb::Blending, ID3D11BlendState*, sb::BlendStateMapper>;
#endif

