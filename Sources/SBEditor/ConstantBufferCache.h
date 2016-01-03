/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "LinearCache.h"
#pragma once

namespace sb {
	struct ConstantBufferSize {
	public:
		SbDirectXMapperIsAFriend();

		ConstantBufferSize(size_t sz) {
			size = sz;
		}
		size_t size;
	private:
		ID3D11Buffer* build(const DXContext* ctx) const;
	};

	inline bool operator==(const ConstantBufferSize& cbs1, const ConstantBufferSize& cbs2) {
		return cbs1.size == cbs2.size;
	}
	inline bool operator!=(const ConstantBufferSize& cbs1, const ConstantBufferSize& cbs2) {
		return cbs1.size != cbs2.size;
	}

	class DXContext;
	template<size_t _CacheSize>
	class ConstantBufferCache : public LinearCache<_CacheSize, ConstantBufferSize, ID3D11Buffer*> {
	public:
		SbGenerateLinearCacheCtor(ConstantBufferCache, ConstantBufferSize, ID3D11Buffer*);
	};

	typedef ConstantBufferCache<20> DefaultConstantBufferCache;
}

#if !defined(NO_EXTERN_TEMPLATES)
extern template class sb::ConstantBufferCache<20>;
extern template class sb::LinearCache<20, sb::ConstantBufferSize, ID3D11Buffer*>;
#endif

