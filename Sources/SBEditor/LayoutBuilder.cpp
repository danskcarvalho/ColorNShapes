/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "LayoutBuilder.h"
#include "DXContext.h"
#include "VertexItemDescription.h"
#include "..\Common\DirectXHelper.h"

using namespace sb;

sb::LayoutBuilder::LayoutBuilder(const DXContext * ctx) : m_ctx(ctx) {
	assert(m_ctx);
}

ID3D11InputLayout * sb::LayoutBuilder::get(const VertexItemDescription* model, const VertexItemDescription* instance) const {
	if (!m_layouts.count({ model, instance}))
		createLayout(model, instance);

	return m_layouts.at({ model, instance }).Get();
}

void sb::LayoutBuilder::releaseAll() {
	m_layouts.clear();
}

void sb::LayoutBuilder::createLayout(const VertexItemDescription* model, const VertexItemDescription* instance) const {
	auto omodel = model;
	auto oinstance = instance;
	
	assert(model);
	assert(!model->isEndMarker());

	std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_descriptions;
	while (!model->isEndMarker()) {
		D3D11_INPUT_ELEMENT_DESC element;
		memset(&element, 0, sizeof(D3D11_INPUT_ELEMENT_DESC));
		element.SemanticName = model->semanticName;
		element.SemanticIndex = (UINT)model->semanticIndex;
		element.Format = (DXGI_FORMAT)model->format;
		element.InputSlot = 0;
		element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = 0;
		d3d_descriptions.push_back(element);
		model++;
	}

	if (instance) {
		assert(!instance->isEndMarker());
		while (!instance->isEndMarker()) {
			D3D11_INPUT_ELEMENT_DESC element;
			memset(&element, 0, sizeof(D3D11_INPUT_ELEMENT_DESC));
			element.SemanticName = instance->semanticName;
			element.SemanticIndex = (UINT)instance->semanticIndex;
			element.Format = (DXGI_FORMAT)instance->format;
			element.InputSlot = 1;
			element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			element.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			element.InstanceDataStepRate = 1;
			d3d_descriptions.push_back(element);
			instance++;
		}
	}

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	DX::ThrowIfFailed(
		m_ctx->device()->CreateInputLayout(
			d3d_descriptions.data(),
			(UINT)d3d_descriptions.size(),
			m_ctx->currentShaderBytecode().data(),
			m_ctx->currentShaderBytecode().size(),
			&inputLayout
			)
		);

	m_layouts[std::make_pair(omodel, oinstance)] = inputLayout;
}
