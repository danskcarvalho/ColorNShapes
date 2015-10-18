/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "BaseMesh.h"

using namespace sb;

BaseMesh::BaseMesh(size_t vertexByteStride) : m_vertexByteStride(vertexByteStride) {
	m_indexBuffer = nullptr;
	m_vertexBuffer = nullptr;
	m_vertexBufferCapacity = m_indexBufferCapacity = m_vertexBufferCount = m_indexBufferCount = 0;
}

void sb::BaseMesh::BaseMesh::clone(const BaseMesh & other) {
	m_vertexBufferCapacity = other.m_vertexBufferCapacity;
	m_vertexBufferCount = other.m_vertexBufferCount;
	m_indexBufferCapacity = other.m_indexBufferCapacity;
	m_indexBufferCount = other.m_indexBufferCount;

	m_indexBuffer = nullptr;
	m_vertexBuffer = nullptr;

	if (m_vertexBufferCapacity > 0 && m_vertexByteStride > 0) {
		m_vertexBuffer = new char[m_vertexByteStride * m_vertexBufferCapacity];
		assert(m_vertexBuffer);
		if (other.m_vertexBuffer)
			memcpy(m_vertexBuffer, other.m_vertexBuffer, m_vertexByteStride * m_vertexBufferCapacity);
	}
	if (m_indexBufferCapacity > 0) {
		m_indexBuffer = new uint32_t[m_indexBufferCapacity];
		assert(m_indexBuffer);
		if (other.m_indexBuffer)
			memcpy(m_indexBuffer, other.m_indexBuffer, sizeof(uint32_t) * m_indexBufferCapacity);
	}
}

void sb::BaseMesh::BaseMesh::move(BaseMesh && other) {
	m_vertexBufferCapacity = other.m_vertexBufferCapacity;
	m_vertexBufferCount = other.m_vertexBufferCount;
	m_indexBufferCapacity = other.m_indexBufferCapacity;
	m_indexBufferCount = other.m_indexBufferCount;
	m_indexBuffer = other.m_indexBuffer;
	m_vertexBuffer = other.m_vertexBuffer;

	other.m_indexBuffer = nullptr;
	other.m_vertexBuffer = nullptr;
	other.m_vertexBufferCapacity = other.m_indexBufferCapacity = other.m_vertexBufferCount = other.m_indexBufferCount = 0;
}

sb::BaseMesh::~BaseMesh() {
	if (m_indexBuffer)
		delete[] m_indexBuffer;
	if (m_vertexBuffer)
		delete[] m_vertexBuffer;

	m_indexBuffer = nullptr;
	m_vertexBuffer = nullptr;
	m_vertexBufferCapacity = m_indexBufferCapacity = m_vertexBufferCount = m_indexBufferCount = 0;
}

void sb::BaseMesh::setVBCapacity(size_t value) {
	if (m_vertexBufferCapacity == value)
		return;

	if (value <= m_vertexBufferCount)
		m_vertexBufferCount = value;

	auto tempCapacity = m_vertexBufferCapacity;
	m_vertexBufferCapacity = value;

	auto tempBuffer = m_vertexBuffer;
	m_vertexBuffer = nullptr;

	if (m_vertexBufferCapacity != 0) {
		m_vertexBuffer = new char[m_vertexByteStride * m_vertexBufferCapacity];
		assert(m_vertexBuffer);
		if (tempBuffer) {
			memcpy(m_vertexBuffer, tempBuffer, std::min(m_vertexBufferCapacity, tempCapacity) * m_vertexByteStride);
			delete[] tempBuffer;
		}
	}
	else {
		if (tempBuffer)
			delete[] tempBuffer;
	}
}

void sb::BaseMesh::setIBCapacity(size_t value) {
	if (m_indexBufferCapacity == value)
		return;

	if (value <= m_indexBufferCount)
		m_indexBufferCount = value;

	auto tempCapacity = m_indexBufferCapacity;
	m_indexBufferCapacity = value;

	auto tempBuffer = m_indexBuffer;
	m_indexBuffer = nullptr;

	if (m_indexBufferCapacity != 0) {
		m_indexBuffer = new uint32_t[m_indexBufferCapacity];
		assert(m_indexBuffer);
		if (tempBuffer) {
			memcpy(m_indexBuffer, tempBuffer, std::min(m_indexBufferCapacity, tempCapacity) * sizeof(uint32_t));
			delete[] tempBuffer;
		}
	}
	else {
		if (tempBuffer)
			delete[] tempBuffer;
	}
}

void sb::BaseMesh::setVBCount(size_t value) {
	if (value > m_vertexBufferCapacity)
		ensureVBCapacity(value);
	m_vertexBufferCount = value;
}

void sb::BaseMesh::setIBCount(size_t value) {
	if (value > m_indexBufferCapacity)
		ensureIBCapacity(value);
	m_indexBufferCount = value;
}

void sb::BaseMesh::compressVB() {
	setVBCapacity(m_vertexBufferCount);
}

void sb::BaseMesh::compressIB() {
	setIBCapacity(m_indexBufferCount);
}

void sb::BaseMesh::compressBoth() {
	compressVB();
	compressIB();
}

void sb::BaseMesh::softCompressVB() {
	auto newCount = m_vertexBufferCount * 1.25;
	if (newCount <= 1)
		newCount = 4;
	setVBCapacity((size_t)newCount);
}

void sb::BaseMesh::softCompressIB() {
	auto newCount = m_indexBufferCount * 1.25;
	if (newCount <= 1)
		newCount = 4;
	setIBCapacity((size_t)newCount);
}

void sb::BaseMesh::softCompressBoth() {
	softCompressIB();
	softCompressVB();
}

void sb::BaseMesh::ensureVBCapacity(size_t capacity) {
	if (capacity <= m_vertexBufferCapacity)
		return;
	auto newCapacity = (double)m_vertexBufferCapacity;
	if (newCapacity < 1)
		newCapacity = 1;
	while (newCapacity < capacity)
		newCapacity *= 1.25;
	setVBCapacity((size_t)newCapacity);
}

void sb::BaseMesh::ensureIBCapacity(size_t capacity) {
	if (capacity <= m_indexBufferCapacity)
		return;
	auto newCapacity = (double)m_indexBufferCapacity;
	if (newCapacity < 1)
		newCapacity = 1;
	while (newCapacity < capacity)
		newCapacity *= 1.25;
	setIBCapacity((size_t)newCapacity);
}

void sb::BaseMesh::appendIndex(uint32_t index) {
	ensureIBCapacity(m_indexBufferCount + 1);
	m_indexBuffer[m_indexBufferCount] = index;
	m_indexBufferCount++;
}

void sb::BaseMesh::copyIndexes(size_t start, const uint32_t * indexes, size_t count) {
	if (count == 0)
		return;
	ensureIBCapacity(start + count);
	memcpy(m_indexBuffer + start, indexes, sizeof(uint32_t) * count);
	m_indexBufferCount = std::max(m_indexBufferCount, start + count);
}

BaseMesh& BaseMesh::operator=(const BaseMesh& other) {
	assert(m_vertexByteStride == other.m_vertexByteStride);

	if (other.m_vertexBufferCapacity == 0) {
		if (m_vertexBuffer)
			delete[] m_vertexBuffer;

		m_vertexBuffer = nullptr;
		m_vertexBufferCapacity = 0;
		m_vertexBufferCount = 0;
	}
	else if (m_vertexBufferCapacity >= other.m_vertexBufferCapacity) {
		memcpy(m_vertexBuffer, other.m_vertexBuffer, m_vertexByteStride * other.m_vertexBufferCapacity);
		m_vertexBufferCount = other.m_vertexBufferCount;
	}
	else {
		if (m_vertexBuffer)
			delete[] m_vertexBuffer;

		m_vertexBuffer = new char[m_vertexByteStride * other.m_vertexBufferCapacity];
		memcpy(m_vertexBuffer, other.m_vertexBuffer, m_vertexByteStride * other.m_vertexBufferCapacity);
		m_vertexBufferCapacity = other.m_vertexBufferCapacity;
		m_vertexBufferCount = other.m_vertexBufferCount;
	}

	if (other.m_indexBufferCapacity == 0) {
		if (m_indexBuffer)
			delete[] m_indexBuffer;

		m_indexBuffer = nullptr;
		m_indexBufferCapacity = 0;
		m_indexBufferCount = 0;
	}
	else if (m_indexBufferCapacity >= other.m_indexBufferCapacity) {
		memcpy(m_indexBuffer, other.m_indexBuffer, sizeof(uint32) * other.m_indexBufferCapacity);
		m_indexBufferCount = other.m_indexBufferCount;
	}
	else {
		if (m_indexBuffer)
			delete[] m_indexBuffer;

		m_indexBuffer = new uint32_t[other.m_indexBufferCapacity];
		memcpy(m_indexBuffer, other.m_indexBuffer, sizeof(uint32) * other.m_indexBufferCapacity);
		m_indexBufferCapacity = other.m_indexBufferCapacity;
		m_indexBufferCount = other.m_indexBufferCount;
	}

	return *this;
}

BaseMesh& BaseMesh::operator=(BaseMesh&& other) {
	assert(m_vertexByteStride == other.m_vertexByteStride);

	if (m_indexBuffer)
		delete[] m_indexBuffer;
	if (m_vertexBuffer)
		delete[] m_vertexBuffer;

	m_vertexBufferCapacity = other.m_vertexBufferCapacity;
	m_vertexBufferCount = other.m_vertexBufferCount;
	m_indexBufferCapacity = other.m_indexBufferCapacity;
	m_indexBufferCount = other.m_indexBufferCount;
	m_indexBuffer = other.m_indexBuffer;
	m_vertexBuffer = other.m_vertexBuffer;

	other.m_indexBuffer = nullptr;
	other.m_vertexBuffer = nullptr;
	other.m_vertexBufferCapacity = other.m_indexBufferCapacity = other.m_vertexBufferCount = other.m_indexBufferCount = 0;

	return *this;
}

void sb::BaseMesh::appendVertex(void * vertex) {
	ensureVBCapacity(m_vertexBufferCount + 1);
	memcpy((char*)m_vertexBuffer + m_vertexByteStride * m_vertexBufferCount, vertex, m_vertexByteStride);
	m_vertexBufferCount++;
}

void sb::BaseMesh::copyVertexes(size_t start, const void * vertexes, size_t count) {
	if (count == 0)
		return;
	ensureVBCapacity(start + count);
	memcpy((char*)m_vertexBuffer + start * m_vertexByteStride, vertexes, m_vertexByteStride * count);
	m_vertexBufferCount = std::max(m_vertexBufferCount, start + count);
}
