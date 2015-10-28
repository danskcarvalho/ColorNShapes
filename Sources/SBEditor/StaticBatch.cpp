/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "StaticBatch.h"
#include "DXContext.h"
#include "LayoutBuilder.h"

sb::StaticBatch::~StaticBatch() {
	if (m_modelBuffer)
		free(m_modelBuffer);
	if (m_instanceBuffer)
		free(m_instanceBuffer);
	if (m_indexBuffer)
		free(m_indexBuffer);
}

void sb::StaticBatch::draw() {
	allocDXBuffers();

	if (!m_d3dModel || !m_d3dIdx)
		return;


	auto ctx = m_ctx->deviceContext();
	bool lastInstanced = m_drawCalls.front().isInstanced;
	auto lastTopology = m_drawCalls.front().topology;
	setVertexBuffers(ctx, lastInstanced);
	ctx->IASetIndexBuffer(m_d3dIdx.Get(), DXGI_FORMAT_R32_UINT, 0);
	ctx->IASetInputLayout(m_layoutBuilder->get(m_modelDescription, m_instanceDescription));
	ctx->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)lastTopology);

	for (size_t i = 0; i < m_drawCalls.size(); i++) {
		auto& d = m_drawCalls[i];

		if (d.modelIBCount == 0)
			continue; //nothing to draw
		if (d.isInstanced && d.instanceCount == 0)
			continue; //nothing to draw

		if (lastInstanced != d.isInstanced) {
			lastInstanced = m_drawCalls.front().isInstanced;
			setVertexBuffers(ctx, lastInstanced);
		}
		if (lastTopology != d.topology) {
			lastTopology = m_drawCalls.front().topology;
			ctx->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)lastTopology);
		}

		if (d.isInstanced)
			ctx->DrawIndexedInstanced((UINT)d.modelIBCount,
									  (UINT)d.instanceCount,
									  (UINT)d.modelIBOffset,
									  (INT)d.modelVBOffset,
									  (UINT)d.instanceOffset);
		else
			ctx->DrawIndexed((UINT)d.modelIBCount,
							 (UINT)d.modelIBOffset,
							 (INT)d.modelVBOffset);
	}
}

void sb::StaticBatch::allocDXBuffers() {
	if (m_d3dModel)
		return;

	auto device = m_ctx->device();

	if (m_modelBuffer) {
		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = (UINT)m_modelBufferSize;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA resData;
		memset(&resData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		resData.pSysMem = m_modelBuffer;

		device->CreateBuffer(&bufferDesc, &resData, &m_d3dModel);
	}

	if (m_indexBuffer) {
		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = (UINT)m_indexBufferSize;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA resData;
		memset(&resData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		resData.pSysMem = m_indexBuffer;

		device->CreateBuffer(&bufferDesc, &resData, &m_d3dIdx);
	}

	if (m_instanceBuffer) {
		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = (UINT)m_instanceBufferSize;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA resData;
		memset(&resData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		resData.pSysMem = m_instanceBuffer;

		device->CreateBuffer(&bufferDesc, &resData, &m_d3dInst);
	}
}

void sb::StaticBatch::setVertexBuffers(ID3D11DeviceContext2 * ctx, bool instanced) {
	if (instanced) {
		ID3D11Buffer* buffers[] = { m_d3dModel.Get(), m_d3dInst.Get() };
		UINT strides[] = { (UINT)m_modelVertexByteStride, (UINT)m_instanceVertexByteStride };
		UINT offsets[] = { 0, 0 };
		ctx->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	}
	else {
		UINT stride = (UINT)m_modelVertexByteStride;
		UINT offset = 0;
		ctx->IASetVertexBuffers(0, 1, m_d3dModel.GetAddressOf(),
								&stride, &offset);
	}
}

sb::StaticBatch::StaticBatch(const DXContext * ctx, const LayoutBuilder * layoutBuilder, const DynamicDrawContext & drawCtx): m_ctx(ctx), m_layoutBuilder(layoutBuilder) {
	m_modelDescription = drawCtx.modelDescription;
	m_instanceDescription = drawCtx.instanceDescription;
	m_modelVertexByteStride = drawCtx.modelVertexByteStride;
	m_instanceVertexByteStride = drawCtx.instanceVertexByteStride;

	if (drawCtx.modelBuffer) {
		m_modelBuffer = malloc(drawCtx.modelBufferSize);
		memcpy(m_modelBuffer, drawCtx.modelBuffer, drawCtx.modelBufferSize);
	}
	if (drawCtx.indexBuffer) {
		m_indexBuffer = (uint32_t*)malloc(drawCtx.indexBufferSize);
		memcpy(m_indexBuffer, drawCtx.indexBuffer, drawCtx.indexBufferSize);
	}
	if (drawCtx.instanceBuffer) {
		m_instanceBuffer = malloc(drawCtx.instanceBufferSize);
		memcpy(m_instanceBuffer, drawCtx.instanceBuffer, drawCtx.instanceBufferSize);
	}

	for (size_t i = 0; i < drawCtx.drawCalls.size(); i++) {
		auto& s = drawCtx.drawCalls[i];
		DrawCall d;
		d.instanceCount = s.instanceCount;
		d.instanceOffset = s.instanceOffset;
		d.isInstanced = s.isInstanced;
		d.modelIBCount = s.modelIBCount;
		d.modelIBOffset = s.modelIBOffset;
		d.modelVBOffset = s.modelVBOffset;
		d.topology = s.topology;

		m_drawCalls.push_back(d);
	}
}
