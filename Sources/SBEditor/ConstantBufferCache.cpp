/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#define  NO_EXTERN_TEMPLATES
#include "ConstantBufferCache.h"
#include "DXContext.h"
#include "..\Common\DirectXHelper.h"

using namespace sb;

template class sb::ConstantBufferCache<20>;
template class sb::LinearCache<20, sb::ConstantBufferSize, ID3D11Buffer*>;

ID3D11Buffer * sb::ConstantBufferSize::build(const DXContext * ctx) const {
	ID3D11Buffer* _buffer;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = (UINT)size;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	DX::ThrowIfFailed(
		ctx->device()->CreateBuffer(
			&cbDesc,
			nullptr,
			&_buffer
			)
		);

	return _buffer;
}
