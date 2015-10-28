/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "PrimitiveTopology.h"
#include "BaseMesh.h"

#pragma once

namespace sb {
	class DXContext;
	class LayoutBuilder;
	class VertexItemDescription;
	class StaticBatch;

	enum class DrawFrequency {
		Dynamic,
		Default
	};
	class NullMesh : public BaseMesh {
	public:
		//Constexpr
		static constexpr size_t VertexByteStride = 0;
		//Description
		virtual const VertexItemDescription* description() const override {
			return nullptr;
		}
		static const VertexItemDescription* staticDescription() {
			return nullptr;
		}
	};
	class DynamicBatcher {
	public:
		//Constructor
		DynamicBatcher(const DXContext* ctx, const LayoutBuilder* layoutBuilder, DrawFrequency frequency = DrawFrequency::Default);
		~DynamicBatcher();
		//Drawing
		template<class Model, class Instance = NullMesh>
		inline void begin() {
			_begin(Model::staticDescription(), Instance::staticDescription(), Model::VertexByteStride, Instance::InstanceByteStride);
		}
		inline void end() {
			_end();
		}
		//Meshes
		inline void beginMeshes(PrimitiveTopology topology) {
			_beginMeshes(topology);
		}
		inline void batchMesh(const BaseMesh* mesh) {
			_batchMesh(mesh);
		}
		inline void batchMeshes(const BaseMesh* const* meshes, size_t count) {
			_batchMeshes(meshes, count);
		}
		template<class Model>
		inline void batchMeshes(const Model* const* meshes, size_t count) {
			for (size_t i = 0; i < count; i++) {
				_batchMesh(meshes[i]);
			}
		}
		inline void batchMeshes(const std::vector<const BaseMesh*>& meshes) {
			_batchMeshes(meshes);
		}
		template<class Model>
		inline void batchMeshes(const std::vector<const Model*>& meshes) {
			for (size_t i = 0; i < meshes.size(); i++) {
				_batchMesh(meshes[i]);
			}
		}
		inline void endMeshes() {
			_endMeshes();
		}
		//Instances
		inline void beginInstances(PrimitiveTopology topology, const BaseMesh* model) {
			_beginInstances(topology, model);
		}
		inline void batchInstance(const BaseMesh* instance) {
			_batchInstance(instance);
		}
		inline void batchInstances(const BaseMesh* const* instances, size_t count) {
			_batchInstances(instances, count);
		}
		template<class Model>
		inline void batchInstances(const Model* const* instances, size_t count) {
			for (size_t i = 0; i < count; i++) {
				_batchInstance(instances[i]);
			}
		}
		inline void batchInstances(const std::vector<const BaseMesh*>& instances) {
			_batchInstances(instances);
		}
		template<class Model>
		inline void batchInstances(const std::vector<const Model*>& instances) {
			for (size_t i = 0; i < instances.size(); i++) {
				_batchInstance(instances[i]);
			}
		}
		inline void endInstances() {
			_endInstances();
		}
		//Release All
		void releaseAll();
		//Drawing
		void draw();
		//Compile
		StaticBatch* compile() const;
	private:
		void _begin(const VertexItemDescription* model, 
					const VertexItemDescription* instance,
					size_t modelVertexByteStride,
					size_t instanceVertexByteStride);
		void _end();
		void _beginMeshes(PrimitiveTopology topology);
		void _batchMesh(const BaseMesh* mesh);
		void _batchMeshes(const BaseMesh* const* meshes, size_t count);
		void _batchMeshes(const std::vector<const BaseMesh*>& meshes);
		void _endMeshes();
		void _beginInstances(PrimitiveTopology topology, const BaseMesh* model);
		void _batchInstance(const BaseMesh* instance);
		void _batchInstances(const BaseMesh* const* instances, size_t count);
		void _batchInstances(const std::vector<const BaseMesh*>& instances);
		void _endInstances();
		void reallocDXBuffers(bool* modelBufferReallocated, bool* indexBufferReallocated, bool* instanceBufferReallocated);
		void updateDXBuffers();
		void setVertexBuffers(ID3D11DeviceContext2* ctx, bool instanced);
		void ensureBufferSize(void** buffer, size_t* sz, size_t min_sz, size_t alignment);
	private:
		const DXContext* m_ctx;
		const LayoutBuilder* m_layoutBuilder;
		const DrawFrequency m_frequency;
		//Draw calls
		struct DrawCall {
			PrimitiveTopology topology;
			ptrdiff_t modelVBOffset; //in elements
			ptrdiff_t modelIBOffset; //in elements
			ptrdiff_t instanceOffset; //in elements
			size_t modelIBCount; //in elements
			size_t instanceCount; //in elements
			bool isInstanced;
			bool ended;
		};
		std::vector<DrawCall> m_drawCalls;

		const VertexItemDescription* m_modelDescription;
		const VertexItemDescription* m_instanceDescription;
		size_t m_modelVertexByteStride;
		size_t m_instanceVertexByteStride;

		void* m_modelBuffer;
		size_t m_modelBufferSize; //in bytes
		size_t m_modelBufferOffset; //in bytes
		void* m_instanceBuffer;
		size_t m_instanceBufferSize; //in bytes
		size_t m_instanceBufferOffset; //in bytes
		uint32_t* m_indexBuffer;
		size_t m_indexBufferSize; //in bytes
		size_t m_indexBufferOffset; //in bytes

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dModel;
		size_t m_d3dModelSize; //in bytes
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dInst;
		size_t m_d3dInstSize; //in bytes
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dIdx;
		size_t m_d3dIdxSize; //in bytes

		bool m_dirty;
		bool m_started;
	};
}

