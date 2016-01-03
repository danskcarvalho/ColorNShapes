/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#define	 NO_EXTERN_TEMPLATES
#include "RenderTargetCache.h"
#include "DXContext.h"
#include "..\Common\DirectXHelper.h"

using namespace sb;

template class sb::RenderTargetCache<6>;
template class sb::LinearCache<6, sb::RenderTargetConfiguration, sb::RenderTargetInterfaces, sb::RenderTargetMapper>;

RenderTargetInterfaces sb::RenderTargetMapper::operator()(const RenderTargetConfiguration & input, 
														  const DXContext * ctx) const {
	return input.build(ctx);
}

void sb::RenderTargetMapper::destroy(const RenderTargetInterfaces & output) const {
	if (output.renderTargetTexture)
		output.renderTargetTexture->Release();

	if (output.renderTargetView)
		output.renderTargetView->Release();

	if (output.shaderResourceView)
		output.shaderResourceView->Release();
}

RenderTargetInterfaces sb::RenderTargetConfiguration::build(const DXContext * ctx) const {
	RenderTargetInterfaces rti;

	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = (UINT)this->width;
	textureDesc.Height = (UINT)this->height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	DX::ThrowIfFailed(
		ctx->device()->CreateTexture2D(
			&textureDesc, 
			NULL, 
			&rti.renderTargetTexture));

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	DX::ThrowIfFailed(
		ctx->device()->CreateRenderTargetView(
			rti.renderTargetTexture, 
			&renderTargetViewDesc, 
			&rti.renderTargetView));

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	DX::ThrowIfFailed(
		ctx->device()->CreateShaderResourceView(
			rti.renderTargetTexture, 
			&shaderResourceViewDesc, 
			&rti.shaderResourceView));

	return rti;
}
