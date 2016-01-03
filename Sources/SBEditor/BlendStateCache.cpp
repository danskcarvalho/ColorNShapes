/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include	"pch.h"
#define		NO_EXTERN_TEMPLATES
#include	"BlendStateCache.h"
#include	"DXContext.h"
#include	"..\Common\DirectXHelper.h"

using namespace sb;

template class BlendStateCache<20>;
template class LinearCache<20, Blending, ID3D11BlendState*, BlendStateMapper>;

namespace {
	HRESULT createBlendState(const DXContext* context, D3D11_BLEND srcBlend, D3D11_BLEND destBlend, ID3D11BlendState** pResult) {
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.RenderTarget[0].BlendEnable = (srcBlend != D3D11_BLEND_ONE) ||
			(destBlend != D3D11_BLEND_ZERO);

		desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = srcBlend;
		desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = destBlend;
		desc.RenderTarget[0].BlendOp = desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		if (desc.RenderTarget[0].SrcBlendAlpha == D3D11_BLEND_DEST_COLOR)
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		if (desc.RenderTarget[0].DestBlendAlpha == D3D11_BLEND_DEST_COLOR)
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		if (desc.RenderTarget[0].SrcBlendAlpha == D3D11_BLEND_SRC_COLOR)
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		if (desc.RenderTarget[0].DestBlendAlpha == D3D11_BLEND_SRC_COLOR)
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;

		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = context->device()->CreateBlendState(&desc, pResult);

		return hr;
	}
}

ID3D11BlendState * sb::BlendStateMapper::operator()(Blending input, const DXContext * ctx) const {
	ID3D11BlendState * r = nullptr;
	switch (input) {
	case sb::Blending::Opaque:
		createBlendState(ctx, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, &r);
		break;
	case sb::Blending::Normal:
		createBlendState(ctx, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, &r);
		break;
	case sb::Blending::NpNormal:
		createBlendState(ctx, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, &r);
		break;
	case sb::Blending::Add:
		createBlendState(ctx, D3D11_BLEND_ONE, D3D11_BLEND_ONE, &r);
		break;
	case sb::Blending::NpAdd:
		createBlendState(ctx, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, &r);
		break;
	case sb::Blending::Multiply:
		createBlendState(ctx, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_INV_SRC_ALPHA, &r);
		break;
	case sb::Blending::NpMultiply:
		createBlendState(ctx, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_ZERO, &r);
		break;
	default:
		break;
	}
	return r;
}

void sb::BlendStateMapper::destroy(ID3D11BlendState * output) const {
	output->Release();
}
