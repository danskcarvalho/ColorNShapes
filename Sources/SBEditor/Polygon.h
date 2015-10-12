/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

#include "Vec2.h"
#include "Option.h"
#include "Rect.h"

namespace sb {
	enum class PointOrdering {
		ccw,
		cw,
		unknown
	};
	struct Polygon {
	public:
		//Constructors
		Polygon() {
			m_closed = false;
		}
		Polygon(const Polygon& other)
			: m_points(other.m_points), m_normals(other.m_normals) {
			m_closed = other.m_closed;
			m_bounds = other.m_bounds;
			m_signedArea = other.m_signedArea;
			m_perimeter = other.m_perimeter;
			m_isConvex = other.m_isConvex;
			m_isSimple = other.m_isSimple;
			m_centroid = other.m_centroid;
		}
		Polygon(Polygon&& other)
			: m_points(std::move(other.m_points)), m_normals(std::move(other.m_normals)) {
			m_closed = other.m_closed;
			m_bounds = other.m_bounds;
			m_signedArea = other.m_signedArea;
			m_perimeter = other.m_perimeter;
			m_isConvex = other.m_isConvex;
			m_isSimple = other.m_isSimple;
			m_centroid = other.m_centroid;
		}
		Polygon(const std::vector<Vec2>& points, bool closed = true) {
			m_points = points;
			m_closed = closed;
		}
		Polygon(std::vector<Vec2>&& points, bool closed = true)
			: m_points(points) {
			m_closed = closed;
		}
		Polygon(const std::initializer_list<Vec2>& points, bool closed = true)
			: m_points(points) {
			m_closed = closed;
		}
		Polygon(const Vec2* points, size_t count, bool closed = true) {
			assert(count >= 0);
			if (count != 0)
				m_points.reserve(count);
			for (size_t i = 0; i < count; i++)
				m_points.push_back(points[i]);
			m_closed = closed;
		}
		//Assignment operator
		Polygon& operator=(const Polygon& other) {
			m_points = other.m_points;
			m_normals = other.m_normals;
			m_closed = other.m_closed;
			m_bounds = other.m_bounds;
			m_signedArea = other.m_signedArea;
			m_perimeter = other.m_perimeter;
			m_isConvex = other.m_isConvex;
			m_isSimple = other.m_isSimple;
			m_centroid = other.m_centroid;
			return *this;
		}
		Polygon& operator=(Polygon&& other) {
			m_points = std::move(other.m_points);
			m_normals = std::move(other.m_normals);
			m_closed = other.m_closed;
			m_bounds = other.m_bounds;
			m_signedArea = other.m_signedArea;
			m_perimeter = other.m_perimeter;
			m_isConvex = other.m_isConvex;
			m_isSimple = other.m_isSimple;
			m_centroid = other.m_centroid;
			return *this;
		}
		//Accessors
		inline bool isClosed() const {
			return m_closed;
		}
		inline ptrdiff_t pointCount() const {
			return m_points.size();
		}
		inline const Vec2& point(ptrdiff_t index, bool wrap = false) const {
			if (!wrap)
				assert(index >= 0 && index < (ptrdiff_t)m_points.size());
			else {
				const auto cc = pointCount();
				assert(cc != 0);
				if (index < 0) {
					index *= -1;
					index = index - (index / cc) * cc;
					index = cc - index;
				}
				else {
					index = index - (index / cc) * cc;
				}
			}

			return m_points[index];
		}
		inline const std::vector<Vec2>& allPoints() const {
			return m_points;
		}
		//Edges
		ptrdiff_t edgeCount() const;
		LineSegment edge(ptrdiff_t index, bool wrap = false) const;
		Vec2 normal(ptrdiff_t index, bool wrap = false) const;
		//Info
		float signedArea() const;
		float area() const;
		Vec2 centroid() const;
		PointOrdering ordering() const;
		float perimeter() const;
		Rect bounds() const;
		bool isConvex() const;
		bool isSimple() const;
		LineSegment closestEdgeFrom(const Vec2& pt, size_t* index = nullptr) const;
		Vec2 closestPointFrom(const Vec2& pt) const;
		//Tests
		bool containsPoint(const Vec2& pt) const;
		//Operations
		template<typename F>
		Polygon map(const F& mapFunction, bool linearTransformation = false) const {
			Polygon p = *this;
			p.m_bounds = nullptr;
			p.m_centroid = nullptr;
			p.m_normals.clear();
			p.m_perimeter = nullptr;
			p.m_signedArea = nullptr;
			for (ptrdiff_t i = 0; i < (ptrdiff_t)p.pointCount(); i++)
				p.m_points[i] = mapFunction(p.m_points[i]);
			if (!linearTransformation) {
				p.m_isConvex = nullptr;
				p.m_isSimple = nullptr;
			}
			return p;
		}
		Polygon optimize() const;
		Polygon asClosed() const {
			if (m_closed)
				return *this;
			else
				return Polygon(m_points, true);
		}
		Polygon asOpened() const {
			if (!m_closed)
				return *this;
			else
				return Polygon(m_points, false);
		}
		Polygon toConvex() const;
		//Description
		inline std::string toString() const {
			std::string rs = "{";
			bool first = true;
			for (const auto& pt : m_points) {
				if (!first)
					rs += ", ";
				rs += pt.toString();
				first = false;
			}
			rs += "}";
			return rs;
		}
	private:
		//Data
		mutable Option<Rect> m_bounds;
		mutable Option<float> m_signedArea;
		mutable Option<float> m_perimeter;
		mutable Option<Vec2> m_centroid;
		mutable Option<bool> m_isConvex;
		mutable Option<bool> m_isSimple;
		std::vector<Vec2> m_points;
		mutable std::vector<Vec2> m_normals;
		bool m_closed;
	};

	inline bool aeq(const Polygon& p1, const Polygon& p2) {
		if (p1.pointCount() != p2.pointCount())
			return false;

		for (ptrdiff_t i = 0; i < (ptrdiff_t)p1.pointCount(); i++) {
			if (!aeq(p1.point(i), p2.point(i)))
				return false;
		}

		return p1.isClosed() == p2.isClosed();
	}
	inline bool operator ==(const Polygon& p1, const Polygon& p2) {
		return p1.allPoints() == p2.allPoints() && p1.isClosed() == p2.isClosed();
	}
	inline bool operator !=(const Polygon& p1, const Polygon& p2) {
		return p1.allPoints() != p2.allPoints() || p1.isClosed() != p2.isClosed();
	}
	inline bool operator >(const Polygon& p1, const Polygon& p2) {
		if (p1.isClosed() == p2.isClosed())
			return p1.allPoints() > p2.allPoints();
		else
			return p1.isClosed() > p2.isClosed();
	}
	inline bool operator <(const Polygon& p1, const Polygon& p2) {
		if (p1.isClosed() == p2.isClosed())
			return p1.allPoints() < p2.allPoints();
		else
			return p1.isClosed() < p2.isClosed();
	}
	inline bool operator >=(const Polygon& p1, const Polygon& p2) {
		if (p1 == p2)
			return true;

		if (p1.isClosed() == p2.isClosed())
			return p1.allPoints() > p2.allPoints();
		else
			return p1.isClosed() > p2.isClosed();
	}
	inline bool operator <=(const Polygon& p1, const Polygon& p2) {
		if (p1 == p2)
			return true;

		if (p1.isClosed() == p2.isClosed())
			return p1.allPoints() < p2.allPoints();
		else
			return p1.isClosed() < p2.isClosed();
	}

	struct polygonPath {
	public:
		polygonPath();
		polygonPath(const Polygon& p);
		const struct Polygon& Polygon() const;
		float length() const;
		Vec2 pointAt(float l, bool clamp = false) const;
		float lengthAt(const Vec2& pt) const;
	private:
		struct Polygon m_polygon;
		std::vector<float> m_length;
		float m_totalLength;
		Vec2 pointAt(float l, size_t low_index, size_t hi_index) const;
	};
}

namespace std {
	template<>
	class hash < sb::Polygon > {
	public:
		template<typename T>
		std::size_t make_hash(const T& v) const {
			return std::hash<T>()(v);
		}
		void hash_combine(std::size_t& h, const std::size_t& v) {
			h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
		}

		size_t operator()(const sb::Polygon& m) {
			size_t h = 0;
			for (const auto& e : m.allPoints()) {
				hash_combine(h, make_hash(e));
			}
			hash_combine(h, make_hash(m.isClosed()));
			return h;
		}
	};
}

