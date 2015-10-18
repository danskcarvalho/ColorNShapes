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
			if (other.hasVB())
				copyVertexes(VBCount(), other.VB(), other.VBCount());
			if (other.hasIB())
				copyIndexes(IBCount(), other.IB(), other.IBCount());
		}
		//Description
		virtual const VertexItemDescription* description() const override {
			return Vertex::description();
		}
	};
}

