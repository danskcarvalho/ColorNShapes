/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "Color.h"
#include "LinearCache.h"

#pragma once

namespace sb {
	enum class SamplerFilter {
		Point = 0,
		Linear = 1
	};
	enum class SamplerAddressMode {
		Wrap = 1,
		Mirror = 2,
		Clamp = 3,
		Border = 4,
		MirrorOnce = 5
	};

	struct Sampler {
	public:
		SbDirectXMapperIsAFriend();

		SamplerFilter filter;
		SamplerAddressMode addressModeU;
		SamplerAddressMode addressModeV;
		Color borderColor;

		Sampler(SamplerFilter filter, 
				SamplerAddressMode addressModeU, 
				SamplerAddressMode addressModeV, 
				Color borderColor) {
			this->filter       = filter;
			this->addressModeU = addressModeU;
			this->addressModeV = addressModeV;
			this->borderColor  = borderColor;
		}
		Sampler() {
			filter       = SamplerFilter::Linear;
			addressModeU = SamplerAddressMode::Clamp;
			addressModeV = SamplerAddressMode::Clamp;
			borderColor  = Color::fromRGBA(1, 1, 1, 1);
		}
	private:
		ID3D11SamplerState* build(const DXContext* ctx) const;
	};

	inline bool operator==(const Sampler& s1, const Sampler& s2) {
		if (s1.filter       != s2.filter)
			return false;
		if (s1.addressModeU != s2.addressModeU)
			return false;
		if (s1.addressModeV != s2.addressModeV)
			return false;
		if (s1.borderColor  != s2.borderColor)
			return false;
		return true;
	}
	inline bool operator!=(const Sampler& s1, const Sampler& s2) {
		if (s1.filter       != s2.filter)
			return true;
		if (s1.addressModeU != s2.addressModeU)
			return true;
		if (s1.addressModeV != s2.addressModeV)
			return true;
		if (s1.borderColor  != s2.borderColor)
			return true;
		return false;
	}

	class DXContext;
	template<size_t _CacheSize>
	class SamplerCache : public LinearCache<_CacheSize, Sampler, ID3D11SamplerState*> {
	public:
		SbGenerateLinearCacheCtor(SamplerCache, Sampler, ID3D11SamplerState*);
	};

	typedef SamplerCache<20> DefaultSamplerCache;
}

#if !defined(NO_EXTERN_TEMPLATES)
extern template class sb::SamplerCache<20>;
extern template class sb::LinearCache<20, sb::Sampler, ID3D11SamplerState*>;
#endif

