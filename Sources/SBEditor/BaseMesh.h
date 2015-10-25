/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace sb {
	class VertexItemDescription;

	class BaseMesh {
	public:
		//Destructors
		virtual ~BaseMesh();
		//Access
		inline const uint32_t* IB() const { return m_indexBuffer; }
		inline uint32_t* IB() { return m_indexBuffer; }
		inline const void* rawVB() const { return m_vertexBuffer; }
		inline void* rawVB() { return m_vertexBuffer; }
		//Tests
		inline bool hasIB() const { return m_indexBuffer != nullptr && m_indexBufferCount > 0;  }
		inline bool hasVB() const { return m_vertexBuffer != nullptr && m_vertexBufferCount > 0; }
		//Stride
		inline size_t vertexByteStride() const { return m_vertexByteStride; }
		//Size + Capacity
		inline size_t VBCapacity() const { return m_vertexBufferCapacity; }
		inline size_t IBCapacity() const { return m_indexBufferCapacity;  }
		void setVBCapacity(size_t value);
		void setIBCapacity(size_t value);
		inline size_t VBCount() const { return m_vertexBufferCount; }
		inline size_t IBCount() const { return m_indexBufferCount; }
		void setVBCount(size_t value);
		void setIBCount(size_t value);
		//Other operations
		void compressVB();
		void compressIB();
		void compressBoth();
		void softCompressVB();
		void softCompressIB();
		void softCompressBoth();
		void ensureVBCapacity(size_t capacity);
		void ensureIBCapacity(size_t capacity);
		//Append
		void appendIndex(uint32_t index);
		//Copy
		void copyIndexes(size_t start, const uint32_t* indexes, size_t count);
		//Description
		virtual const VertexItemDescription* description() const = 0;
	protected:
		//Constructors
		BaseMesh(size_t vertexByteStride);
		//Constructors
		void clone(const BaseMesh& other);
		void move(BaseMesh&& other);
		//Assignment
		BaseMesh& operator=(const BaseMesh& other);
		BaseMesh& operator=(BaseMesh&& other);
		//Fields
		const size_t m_vertexByteStride;
		//Append
		void appendVertex(void* vertex);
		//Copy
		void copyVertexes(size_t start, const void* vertexes, size_t count);
	private:
		void* m_vertexBuffer;
		uint32_t* m_indexBuffer;
		size_t m_vertexBufferCapacity;
		size_t m_indexBufferCapacity;
		size_t m_vertexBufferCount;
		size_t m_indexBufferCount;
	};
}
