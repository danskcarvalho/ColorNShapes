/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "DynamicBatcher.h"
#include "DXContext.h"
#include "LayoutBuilder.h"
#include "StaticBatch.h"

using namespace sb;

sb::DynamicBatcher::DynamicBatcher(const DXContext * ctx, const LayoutBuilder * layoutBuilder, DrawFrequency frequency) : m_ctx(ctx), m_layoutBuilder(layoutBuilder), m_frequency(frequency) {
	assert(m_ctx);
	assert(m_layoutBuilder);

	m_modelDescription = nullptr;
	m_instanceDescription = nullptr;
	m_modelVertexByteStride = 0;
	m_instanceVertexByteStride = 0;

	m_modelBuffer = nullptr;
	m_modelBufferSize = 0;
	m_modelBufferOffset = 0;
	m_instanceBuffer = nullptr;
	m_instanceBufferSize = 0;
	m_instanceBufferOffset = 0;
	m_indexBuffer = nullptr;
	m_indexBufferSize = 0;
	m_indexBufferOffset = 0;

	m_d3dModel = nullptr;
	m_d3dModelSize = 0;
	m_d3dInst = nullptr;
	m_d3dInstSize = 0;
	m_d3dIdx = nullptr;
	m_d3dIdxSize = 0;

	m_dirty = true;
	m_started = false;
}

sb::DynamicBatcher::~DynamicBatcher() {
	if (m_modelBuffer)
		free(m_modelBuffer);
	if (m_instanceBuffer)
		free(m_instanceBuffer);
	if (m_indexBuffer)
		free(m_indexBuffer);
}

void sb::DynamicBatcher::_begin(const VertexItemDescription * model, 
								const VertexItemDescription * instance, 
								size_t modelVertexByteStride, 
								size_t instanceVertexByteStride) {
	assert(!m_started);
	assert(model);
	assert(modelVertexByteStride > 0);
	m_modelDescription = model;
	m_instanceDescription = instance;
	m_modelVertexByteStride = modelVertexByteStride;
	m_instanceVertexByteStride = instanceVertexByteStride;

	//Clean-up
	m_modelBufferOffset = 0;
	m_instanceBufferOffset = 0;
	m_indexBufferOffset = 0;
	m_drawCalls.clear();
	m_dirty = true;
	m_started = true;
}

void sb::DynamicBatcher::_end() {
	assert(m_started);
	assert(m_drawCalls.size() == 0 || m_drawCalls.back().ended);

	m_started = false;
}

void sb::DynamicBatcher::draw() {
	assert(m_modelDescription);
	assert(m_modelVertexByteStride > 0);
	assert(!m_started);

	if (m_drawCalls.size() == 0 || m_modelBufferOffset == 0 || m_indexBufferOffset == 0)
		return; //no draws to make

	//update vertex, index and instance data
	if (m_dirty) {
		updateDXBuffers();
		m_dirty = false;
	}

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

StaticBatch * sb::DynamicBatcher::compile() const {
	assert(m_modelDescription);
	assert(m_modelVertexByteStride > 0);
	assert(!m_started);

	DynamicDrawContext ddc;
	ddc.indexBuffer = m_indexBuffer;
	ddc.indexBufferSize = m_indexBufferOffset;
	ddc.instanceBuffer = m_instanceBuffer;
	ddc.instanceBufferSize = m_instanceBufferOffset;
	ddc.modelBuffer = m_modelBuffer;
	ddc.modelBufferSize = m_modelBufferOffset;
	ddc.modelDescription = m_modelDescription;
	ddc.instanceDescription = m_instanceDescription;
	ddc.modelVertexByteStride = m_modelVertexByteStride;
	ddc.instanceVertexByteStride = m_instanceVertexByteStride;

	for (size_t i = 0; i < m_drawCalls.size(); i++) {
		auto& s = m_drawCalls[i];

		DynamicDrawContext::DrawCall d;
		d.instanceCount = s.instanceCount;
		d.instanceOffset = s.instanceOffset;
		d.isInstanced = s.isInstanced;
		d.modelIBCount = s.modelIBCount;
		d.modelIBOffset = s.modelIBOffset;
		d.modelVBOffset = s.modelVBOffset;
		d.topology = s.topology;

		ddc.drawCalls.push_back(d);
	}

	return new StaticBatch(m_ctx, m_layoutBuilder, ddc);
}

void sb::DynamicBatcher::_beginMeshes(PrimitiveTopology topology) {
	assert(m_started);
	assert(m_drawCalls.size() == 0 || m_drawCalls.back().ended);
	assert(m_modelDescription);
	assert(m_modelVertexByteStride > 0);

	DrawCall dc;
	dc.topology = topology;
	dc.modelVBOffset = m_modelBufferOffset / m_modelVertexByteStride;
	dc.modelIBOffset = m_indexBufferOffset / sizeof(uint32_t);
	dc.instanceOffset = 0;
	dc.modelIBCount = 0;
	dc.instanceCount = 0;
	dc.isInstanced = false;
	dc.ended = false;

	m_drawCalls.push_back(dc);
}

void sb::DynamicBatcher::_batchMesh(const BaseMesh * mesh) {
	assert(m_started);
	assert(m_drawCalls.size() != 0 && !m_drawCalls.back().ended && !m_drawCalls.back().isInstanced);
	assert(mesh);
	assert(mesh->hasVB());
	assert(mesh->vertexByteStride() == m_modelVertexByteStride);
	assert(mesh->description() == m_modelDescription);

	auto& dc = m_drawCalls.back();

	//Update IB
	if (mesh->hasIB()) {
		ensureBufferSize(reinterpret_cast<void**>(&m_indexBuffer), &m_indexBufferSize, m_indexBufferOffset + mesh->IBCount() * sizeof(uint32_t), sizeof(uint32_t));
		memcpy(reinterpret_cast<char*>(m_indexBuffer) + m_indexBufferOffset, mesh->IB(), mesh->IBCount() * sizeof(uint32_t));
		auto idxStart = m_indexBuffer + (m_indexBufferOffset / sizeof(uint32_t));
		auto meshCorrection = (m_modelBufferOffset / m_modelVertexByteStride) - dc.modelVBOffset;
		for (size_t i = 0; i < mesh->IBCount(); i++)
			idxStart[i] += (uint32_t)meshCorrection;

		m_indexBufferOffset += mesh->IBCount() * sizeof(uint32_t);
		dc.modelIBCount += mesh->IBCount();
	}
	else {
		ensureBufferSize(reinterpret_cast<void**>(&m_indexBuffer), &m_indexBufferSize, m_indexBufferOffset + mesh->VBCount() * sizeof(uint32_t), sizeof(uint32_t));
		auto idxStart = m_indexBuffer + (m_indexBufferOffset / sizeof(uint32_t));
		auto meshCorrection = (m_modelBufferOffset / m_modelVertexByteStride) - dc.modelVBOffset;
		for (size_t i = 0; i < mesh->VBCount(); i++)
			idxStart[i] = (uint32_t)(i + meshCorrection);

		m_indexBufferOffset += mesh->VBCount() * sizeof(uint32_t);
		dc.modelIBCount += mesh->VBCount();
	}

	//Update VB
	ensureBufferSize(&m_modelBuffer, &m_modelBufferSize, m_modelBufferOffset + mesh->VBCount() * m_modelVertexByteStride, m_modelVertexByteStride);
	memcpy(reinterpret_cast<char*>(m_modelBuffer) + m_modelBufferOffset, mesh->rawVB(), mesh->VBCount() * m_modelVertexByteStride);
	m_modelBufferOffset += mesh->VBCount() * m_modelVertexByteStride;
}

void sb::DynamicBatcher::_batchMeshes(const BaseMesh * const * meshes, size_t count) {
	assert(meshes);
	if (count == 0)
		return;
	for (size_t i = 0; i < count; i++)
		_batchMesh(meshes[i]);
}

void sb::DynamicBatcher::_batchMeshes(const std::vector<const BaseMesh*>& meshes) {
	for (size_t i = 0; i < meshes.size(); i++) {
		_batchMesh(meshes[i]);
	}
}

void sb::DynamicBatcher::_endMeshes() {
	assert(m_started);
	assert(m_drawCalls.size() != 0 && !m_drawCalls.back().ended && !m_drawCalls.back().isInstanced);
	m_drawCalls.back().ended = true;
}

void sb::DynamicBatcher::_beginInstances(PrimitiveTopology topology, const BaseMesh * model) {
	assert(m_started);
	assert(m_drawCalls.size() == 0 || m_drawCalls.back().ended);
	assert(model);
	assert(model->hasVB());
	assert(model->vertexByteStride() == m_modelVertexByteStride);
	assert(model->description() == m_modelDescription);
	assert(m_modelDescription);
	assert(m_instanceDescription);
	assert(m_modelVertexByteStride > 0);
	assert(m_instanceVertexByteStride > 0);

	DrawCall dc;
	dc.topology = topology;
	dc.modelVBOffset = m_modelBufferOffset / m_modelVertexByteStride;
	dc.modelIBOffset = m_indexBufferOffset / sizeof(uint32_t);
	dc.instanceOffset = m_instanceBufferOffset / m_instanceVertexByteStride;
	dc.modelIBCount = 0;
	dc.instanceCount = 0;
	dc.isInstanced = true;
	dc.ended = false;

	//Update IB
	if (model->hasIB()) {
		ensureBufferSize(reinterpret_cast<void**>(&m_indexBuffer), &m_indexBufferSize, m_indexBufferOffset + model->IBCount() * sizeof(uint32_t), sizeof(uint32_t));
		memcpy(reinterpret_cast<char*>(m_indexBuffer) + m_indexBufferOffset, model->IB(), model->IBCount() * sizeof(uint32_t));

		m_indexBufferOffset += model->IBCount() * sizeof(uint32_t);
		dc.modelIBCount += model->IBCount();
	}
	else {
		ensureBufferSize(reinterpret_cast<void**>(&m_indexBuffer), &m_indexBufferSize, m_indexBufferOffset + model->VBCount() * sizeof(uint32_t), sizeof(uint32_t));
		auto idxStart = m_indexBuffer + (m_indexBufferOffset / sizeof(uint32_t));
		for (size_t i = 0; i < model->VBCount(); i++)
			idxStart[i] = (uint32_t)i;

		m_indexBufferOffset += model->VBCount() * sizeof(uint32_t);
		dc.modelIBCount += model->VBCount();
	}

	//Update VB
	ensureBufferSize(&m_modelBuffer, &m_modelBufferSize, m_modelBufferOffset + model->VBCount() * m_modelVertexByteStride, m_modelVertexByteStride);
	memcpy(reinterpret_cast<char*>(m_modelBuffer) + m_modelBufferOffset, model->rawVB(), model->VBCount() * m_modelVertexByteStride);

	m_drawCalls.push_back(dc);
}

void sb::DynamicBatcher::_batchInstance(const BaseMesh * instance) {
	assert(m_started);
	assert(m_drawCalls.size() != 0 && !m_drawCalls.back().ended && m_drawCalls.back().isInstanced);
	assert(instance);
	assert(instance->hasVB());
	assert(instance->vertexByteStride() == m_instanceVertexByteStride);
	assert(instance->description() == m_instanceDescription);

	auto& dc = m_drawCalls.back();

	//Update instance buffer
	ensureBufferSize(&m_instanceBuffer, &m_instanceBufferSize, m_instanceBufferOffset + instance->VBCount() * m_instanceVertexByteStride, m_instanceVertexByteStride);
	memcpy(reinterpret_cast<char*>(m_instanceBuffer) + m_instanceBufferOffset, instance->rawVB(), instance->VBCount() * m_instanceVertexByteStride);
	dc.instanceCount++;
}

void sb::DynamicBatcher::_batchInstances(const BaseMesh * const * instances, size_t count) {
	assert(instances);
	for (size_t i = 0; i < count; i++) {
		_batchInstance(instances[i]);
	}
}

void sb::DynamicBatcher::_batchInstances(const std::vector<const BaseMesh*>& instances) {
	for (size_t i = 0; i < instances.size(); i++) {
		_batchInstance(instances[i]);
	}
}

void sb::DynamicBatcher::_endInstances() {
	assert(m_started);
	assert(m_drawCalls.size() != 0 && !m_drawCalls.back().ended && m_drawCalls.back().isInstanced);
	m_drawCalls.back().ended = true;
}

void sb::DynamicBatcher::reallocDXBuffers(bool* modelBufferReallocated, bool* indexBufferReallocated, bool* instanceBufferReallocated) {
	if (modelBufferReallocated)
		*modelBufferReallocated = false;
	if (indexBufferReallocated)
		*indexBufferReallocated = false;
	if (instanceBufferReallocated)
		*instanceBufferReallocated = false;
	
	auto device = m_ctx->device();
	if (m_d3dModelSize < m_modelBufferOffset) {
		m_d3dModel.Reset();
		
		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = (UINT)m_modelBufferSize;
		bufferDesc.Usage = m_frequency == DrawFrequency::Default ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = m_frequency == DrawFrequency::Default ? 0 : D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA resData;
		memset(&resData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		resData.pSysMem = m_modelBuffer;

		device->CreateBuffer(&bufferDesc, &resData, &m_d3dModel);
		if (modelBufferReallocated)
			*modelBufferReallocated = true;
	}

	if (m_d3dIdxSize < m_indexBufferOffset) {
		m_d3dIdx.Reset();

		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = (UINT)m_indexBufferSize;
		bufferDesc.Usage = m_frequency == DrawFrequency::Default ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = m_frequency == DrawFrequency::Default ? 0 : D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA resData;
		memset(&resData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		resData.pSysMem = m_indexBuffer;

		device->CreateBuffer(&bufferDesc, &resData, &m_d3dIdx);
		if (indexBufferReallocated)
			*indexBufferReallocated = true;
	}

	if (m_d3dInstSize < m_instanceBufferOffset) {
		m_d3dInst.Reset();

		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = (UINT)m_instanceBufferSize;
		bufferDesc.Usage = m_frequency == DrawFrequency::Default ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = m_frequency == DrawFrequency::Default ? 0 : D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA resData;
		memset(&resData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		resData.pSysMem = m_instanceBuffer;

		device->CreateBuffer(&bufferDesc, &resData, &m_d3dInst);
		if (instanceBufferReallocated)
			*instanceBufferReallocated = true;
	}
}

void sb::DynamicBatcher::updateDXBuffers() {
	bool modelBufferReallocated;
	bool indexBufferReallocated;
	bool instanceBufferReallocated;

	reallocDXBuffers(&modelBufferReallocated, &indexBufferReallocated, &instanceBufferReallocated);
	auto ctx = m_ctx->deviceContext();

	if (!modelBufferReallocated) {
		if (m_frequency == DrawFrequency::Default) {
			D3D11_BOX box;
			box.left = 0;
			box.right = (UINT)m_modelBufferOffset;
			box.top = 0;
			box.bottom = 1;
			box.front = 0;
			box.back = 1;

			ctx->UpdateSubresource(m_d3dModel.Get(),
								   0,
								   &box,
								   m_modelBuffer,
								   0,
								   0);
		}
		else {
			D3D11_MAPPED_SUBRESOURCE res;
			ctx->Map(m_d3dModel.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
			memcpy(res.pData, m_modelBuffer, m_modelBufferOffset);
			ctx->Unmap(m_d3dModel.Get(), 0);
		}
	}
	if (!indexBufferReallocated) {
		if (m_frequency == DrawFrequency::Default) {
			D3D11_BOX box;
			box.left = 0;
			box.right = (UINT)m_indexBufferOffset;
			box.top = 0;
			box.bottom = 1;
			box.front = 0;
			box.back = 1;

			ctx->UpdateSubresource(m_d3dIdx.Get(),
								   0,
								   &box,
								   m_indexBuffer,
								   0,
								   0);
		}
		else {
			D3D11_MAPPED_SUBRESOURCE res;
			ctx->Map(m_d3dIdx.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
			memcpy(res.pData, m_indexBuffer, m_indexBufferOffset);
			ctx->Unmap(m_d3dIdx.Get(), 0);
		}
	}
	if (!instanceBufferReallocated && m_instanceBufferOffset > 0) {
		if (m_frequency == DrawFrequency::Default) {
			D3D11_BOX box;
			box.left = 0;
			box.right = (UINT)m_instanceBufferOffset;
			box.top = 0;
			box.bottom = 1;
			box.front = 0;
			box.back = 1;

			ctx->UpdateSubresource(m_d3dInst.Get(),
								   0,
								   &box,
								   m_instanceBuffer,
								   0,
								   0);
		}
		else {
			D3D11_MAPPED_SUBRESOURCE res;
			ctx->Map(m_d3dInst.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
			memcpy(res.pData, m_instanceBuffer, m_instanceBufferOffset);
			ctx->Unmap(m_d3dInst.Get(), 0);
		}
	}
}

void sb::DynamicBatcher::setVertexBuffers(ID3D11DeviceContext2* ctx, bool instanced) {

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

void sb::DynamicBatcher::ensureBufferSize(void ** buffer, size_t * sz, size_t min_sz, size_t alignment) {
	if (*sz >= min_sz)
		return;

	double dsz = (double)*sz;
	if (dsz <= 0)
		dsz = 4;

	while (dsz < min_sz)
		dsz *= 1.25;

	auto fsz = (size_t)dsz;
	fsz /= alignment;
	fsz++;
	fsz *= alignment;

	if (*buffer)
		*buffer = malloc(fsz);
	else
		*buffer = realloc(*buffer, fsz);
}
