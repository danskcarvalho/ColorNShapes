/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Shape.h"
#include "Circle.h"
#include "Rect.h"
#include "Polygon.h"
#include "Matrix3x3.h"
#include "SpatialTree.h"


using namespace sb;

class shape_implementation {
public:
	ShapeType m_type;
	size_t m_typeMask;
	size_t m_collisionMask;
	bool m_dynamic;
	SpatialTree* m_parent;
	Circle m_circle;
	Rect m_rect;
	Polygon m_polygon;
};


sb::Shape::Shape() {
	m_impl = new shape_implementation();
	m_impl->m_dynamic = false;
	m_impl->m_typeMask = std::numeric_limits<size_t>::max();
	m_impl->m_collisionMask = std::numeric_limits<size_t>::max();
	m_impl->m_parent = nullptr;
}

sb::Shape* sb::Shape::fromCircle(const sb::Circle& c) {
	sb::Shape* obj = new Shape();
	obj->m_impl->m_type = ShapeType::circle;
	obj->m_impl->m_circle = c;
	return obj;
}

sb::Shape::~Shape() {
	delete m_impl;
}

sb::Shape* sb::Shape::fromRect(const sb::Rect& r) {
	sb::Shape* obj = new Shape();
	obj->m_impl->m_type = ShapeType::Rect;
	obj->m_impl->m_rect = r;
	return obj;
}

sb::Shape* sb::Shape::fromPolygon(const sb::Polygon& p) {
	sb::Shape* obj = new Shape();
	obj->m_impl->m_type = ShapeType::Polygon;
	obj->m_impl->m_polygon = p.toConvex();
	return obj;
}

sb::ShapeType sb::Shape::type() const {
	return m_impl->m_type;
}

const Circle& sb::Shape::circle() const {
	assert(m_impl->m_type == ShapeType::circle);
	return m_impl->m_circle;
}

const Rect& sb::Shape::Rect() const {
	assert(m_impl->m_type == ShapeType::Rect);
	return m_impl->m_rect;
}

const Polygon& sb::Shape::Polygon() const {
	assert(m_impl->m_type == ShapeType::Polygon);
	return m_impl->m_polygon;
}

void sb::Shape::applyTransform(const Matrix3x3& m) {
	if (m_impl->m_type == ShapeType::circle)
		m.transformCircle(&m_impl->m_circle);
	else if (m_impl->m_type == ShapeType::Rect)
		m.transformRect(&m_impl->m_rect);
	else
		m_impl->m_polygon = m.transformedPolygon(m_impl->m_polygon);
}

const size_t sb::Shape::typeMask() const {
	return m_impl->m_typeMask;
}

void sb::Shape::setTypeMask(size_t value) {
	m_impl->m_typeMask = value;
}

const size_t sb::Shape::collisionMask() const {
	return m_impl->m_collisionMask;
}

void sb::Shape::setCollisionMask(size_t value) {
	m_impl->m_collisionMask = value;
}

bool sb::Shape::isDynamic() const {
	return m_impl->m_dynamic;
}

void sb::Shape::setDynamic(bool value) {
	m_impl->m_dynamic = value;
}

struct Rect sb::Shape::bounds() const {
	if (m_impl->m_type == ShapeType::circle)
		return m_impl->m_circle.bounds();
	else if (m_impl->m_type == ShapeType::Polygon)
		return m_impl->m_polygon.bounds();
	else
		return m_impl->m_rect;
}

sb::SpatialTree* sb::Shape::parent() const {
	return m_impl->m_parent;
}

void sb::Shape::setParent(SpatialTree* value) {
	m_impl->m_parent = value;
}

