/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "PrimitiveTopology.h"

#pragma once

namespace sb {
	class DXContext;
	class LayoutBuilder;
	class VertexItemDescription;
	class DynamicBatcher;

	class DynamicDrawContext {
	public:
		//Classes
		struct DrawCall {
			PrimitiveTopology topology;
			ptrdiff_t modelVBOffset; //in elements
			ptrdiff_t modelIBOffset; //in elements
			ptrdiff_t instanceOffset; //in elements
			size_t modelIBCount; //in elements
			size_t instanceCount; //in elements
			bool isInstanced;
		};
	public:
		//Fields
		const VertexItemDescription* modelDescription;
		const VertexItemDescription* instanceDescription;
		size_t modelVertexByteStride;
		size_t instanceVertexByteStride;

		void* modelBuffer;
		size_t modelBufferSize; //in bytes
		void* instanceBuffer;
		size_t instanceBufferSize; //in bytes
		uint32_t* indexBuffer;
		size_t indexBufferSize; //in bytes

		std::vector<DrawCall> drawCalls;
	};

	class StaticBatch {
	public:
		friend class DynamicBatcher;
		//Destrutor
		~StaticBatch();
		//Methods
		void draw();
	private:
		void allocDXBuffers();
		void setVertexBuffers(ID3D11DeviceContext2* ctx, bool instanced);
	private:
		//Classes
		struct DrawCall {
			PrimitiveTopology topology;
			ptrdiff_t modelVBOffset; //in elements
			ptrdiff_t modelIBOffset; //in elements
			ptrdiff_t instanceOffset; //in elements
			size_t modelIBCount; //in elements
			size_t instanceCount; //in elements
			bool isInstanced;
		};
		//Construtor
		StaticBatch(const DXContext* ctx, const LayoutBuilder* layoutBuilder, const DynamicDrawContext& drawCtx);
		//Fields
		const DXContext* m_ctx;
		const LayoutBuilder* m_layoutBuilder;

		const VertexItemDescription* m_modelDescription;
		const VertexItemDescription* m_instanceDescription;
		size_t m_modelVertexByteStride;
		size_t m_instanceVertexByteStride;

		void* m_modelBuffer;
		size_t m_modelBufferSize; //in bytes
		void* m_instanceBuffer;
		size_t m_instanceBufferSize; //in bytes
		uint32_t* m_indexBuffer;
		size_t m_indexBufferSize; //in bytes

		std::vector<DrawCall> m_drawCalls;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dModel;
		size_t m_d3dModelSize; //in bytes
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dInst;
		size_t m_d3dInstSize; //in bytes
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dIdx;
		size_t m_d3dIdxSize; //in bytes
	};
}

