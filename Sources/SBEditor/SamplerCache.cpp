/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include	"pch.h"
#define		NO_EXTERN_TEMPLATES
#include	"SamplerCache.h"
#include	"DXContext.h"
#include	"..\Common\DirectXHelper.h"

using namespace sb;

template class SamplerCache<20>;
template class LinearCache<20, Sampler, ID3D11SamplerState*>;

ID3D11SamplerState * sb::Sampler::build(const DXContext * ctx) const {
	D3D11_SAMPLER_DESC samplerDesc;
	memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.AddressU       = (D3D11_TEXTURE_ADDRESS_MODE)this->addressModeU;
	samplerDesc.AddressV       = (D3D11_TEXTURE_ADDRESS_MODE)this->addressModeV;
	samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.BorderColor[0] = this->borderColor.r();
	samplerDesc.BorderColor[1] = this->borderColor.g();
	samplerDesc.BorderColor[2] = this->borderColor.b();
	samplerDesc.BorderColor[3] = this->borderColor.a();
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.Filter         = this->filter == SamplerFilter::Linear ?
		D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.MinLOD         = 0;
	samplerDesc.MaxLOD         = 0;
	samplerDesc.MipLODBias     = 0;
	samplerDesc.MaxAnisotropy  = 1;

	ID3D11SamplerState* samplerState;
	DX::ThrowIfFailed(
		ctx->device()->CreateSamplerState(&samplerDesc, &samplerState)
		);
	return samplerState;
}
