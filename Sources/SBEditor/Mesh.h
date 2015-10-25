/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "BaseMesh.h"

#pragma once
namespace sb {
	template<class _Vertex>
	class Mesh : public BaseMesh {
	public:
		//Constexpr
		static constexpr size_t VertexByteStride = sizeof(_Vertex);
		//Typedefs
		typedef _Vertex Vertex;

		//Constructors
		inline Mesh() : BaseMesh(sizeof(Vertex)) {
		}
		inline Mesh(const Mesh& other) : BaseMesh(sizeof(Vertex)) {
			this->clone(other);
		}
		inline Mesh(Mesh&& other) : BaseMesh(sizeof(Vertex)) {
			this->move(other);
		}
		//Destructor
		virtual ~Mesh(){ }
		//Assignment
		inline Mesh& operator=(const Mesh& other) {
			BaseMesh::operator=(other);
			return *this;
		}
		inline Mesh& operator=(Mesh&& other) {
			BaseMesh::operator=(other);
			return *this;
		}
		//Access
		inline const Vertex* VB() const { return reinterpret_cast<const Vertex*>(rawVB()); }
		inline Vertex* VB() { return reinterpret_cast<Vertex*>(rawVB()); }
		//Append
		inline void appendVertex(Vertex* vertex) {
			BaseMesh::appendVertex(vertex);
		}
		//Copy
		void copyVertexes(size_t start, const Vertex* vertexes, size_t count) {
			BaseMesh::copyVertexes(start, vertexes, count);
		}
		//Merge
		void merge(const Mesh& other) {
			auto vb_start = VBCount();
			if (other.hasVB())
				copyVertexes(VBCount(), other.VB(), other.VBCount());
			if (hasIB() || other.hasIB()) {
				if (!hasIB()) {
					setIBCount(vb_start);
					for (size_t i = 0; i < IBCount(); i++) {
						IB()[i] = i;
					}
				}
				if (other.hasIB()) {
					auto ib_start = IBCount();
					copyIndexes(IBCount(), other.IB(), other.IBCount());
					for (size_t i = ib_start; i < IBCount(); i++) {
						IB()[i] += vb_start;
					}
				}
				else {
					auto ib_start = IBCount();
					setIBCount(IBCount() + other.VBCount());
					for (size_t i = ib_start; i < IBCount(); i++) {
						IB()[i] = i + vb_start;
					}
				}
			}
		}
		//Description
		virtual const VertexItemDescription* description() const override {
			return Vertex::description();
		}
		static const VertexItemDescription* staticDescription() {
			return Vertex::description();
		}
	};
}

