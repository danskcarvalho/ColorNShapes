/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

#include "Option.h"
#include "Vec2.h"
#include "LineSegment.h"

namespace sb {
	struct Polygon;
	struct Rect {
	public:
		//Static Members
		static Rect unit;
		//Constructors
		inline Rect() {
			m_center = Vec2::zero;
			m_size = Vec2::zero;
		}
		inline Rect(const Vec2& center, const Vec2& size) {
			assert(size.x >= 0 && size.y >= 0);
			m_center = center;
			m_size = size;
		}
		inline Rect(float centerX, float centerY, float width, float height) {
			assert(width >= 0 && height >= 0);
			m_center = Vec2(centerX, centerY);
			m_size = Vec2(width, height);
		}
		inline Rect(const std::vector<Vec2>& points) {
			assert(points.size() != 0);
			float minx = std::numeric_limits<float>::max();
			float miny = std::numeric_limits<float>::max();
			float maxx = std::numeric_limits<float>::min();
			float maxy = std::numeric_limits<float>::min();
			for (const auto& v : points) {
				if (v.x < minx)
					minx = v.x;
				if (v.y < miny)
					miny = v.y;
				if (v.x > maxx)
					maxx = v.x;
				if (v.y > maxy)
					maxy = v.y;
			}
			m_center = Vec2((minx + maxx) / 2.0f, (miny + maxy) / 2.0f);
			m_size = Vec2(maxx - minx, maxy - miny);
		}
		inline Rect(const Vec2* points, size_t count) {
			assert(points);
			assert(count > 0);
			float minx = std::numeric_limits<float>::max();
			float miny = std::numeric_limits<float>::max();
			float maxx = std::numeric_limits<float>::min();
			float maxy = std::numeric_limits<float>::min();
			for (size_t i = 0; i < count; i++) {
				const auto& v = points[i];
				if (v.x < minx)
					minx = v.x;
				if (v.y < miny)
					miny = v.y;
				if (v.x > maxx)
					maxx = v.x;
				if (v.y > maxy)
					maxy = v.y;
			}
			m_center = Vec2((minx + maxx) / 2.0f, (miny + maxy) / 2.0f);
			m_size = Vec2(maxx - minx, maxy - miny);
		}
		//Accessors
		inline float area() const {
			return m_size.x * m_size.y;
		}
		inline float perimeter() const {
			return 2 * m_size.x + 2 * m_size.y;
		}
		inline const Vec2& center() const {
			return m_center;
		}
		inline float x() const {
			return m_center.x;
		}
		inline float y() const {
			return m_center.y;
		}
		inline void setCenter(const Vec2& value) {
			m_center = value;
		}
		inline void setX(float value) {
			m_center.x = value;
		}
		inline void setY(float value) {
			m_center.y = value;
		}
		inline const Vec2& size() const {
			return m_size;
		}
		inline float width() const {
			return m_size.x;
		}
		inline float height() const {
			return m_size.y;
		}
		inline void setWidth(float value) {
			assert(value >= 0);
			m_size.x = value;
		}
		inline void setHeight(float value) {
			assert(value >= 0);
			m_size.y = value;
		}
		inline void setSize(const Vec2& value) {
			assert(value.x >= 0 && value.y >= 0);
			m_size = value;
		}
		inline float left() const {
			return m_center.x - m_size.x / 2;
		}
		inline float right() const {
			return m_center.x + m_size.x / 2;
		}
		inline float top() const {
			return m_center.y + m_size.y / 2;
		}
		inline float bottom() const {
			return m_center.y - m_size.y / 2;
		}
		inline void setBounds(float left, float right, float bottom, float top) {
			if (left > right) {
				const auto temp = left;
				left = right;
				right = temp;
			}
			if (bottom > top) {
				const auto temp = top;
				top = bottom;
				bottom = temp;
			}
			m_size.x = right - left;
			m_size.y = top - bottom;
			m_center.x = left + m_size.x / 2;
			m_center.y = bottom + m_size.y / 2;
		}
		inline Vec2 topLeft() const {
			return Vec2(left(), top());
		}
		inline Vec2 topRight() const {
			return Vec2(right(), top());
		}
		inline Vec2 bottomLeft() const {
			return Vec2(left(), bottom());
		}
		inline Vec2 bottomRight() const {
			return Vec2(right(), bottom());
		}
		inline Vec2 point(ptrdiff_t index, bool wrap = false) const {
			if (!wrap)
				assert(index >= 0 && index < 4);
			else {
				if (index < 0) {
					index *= -1;
					index = index - (index / 4) * 4;
					index = 4 - index;
				}
				else {
					index = index - (index / 4) * 4;
				}
			}
			if (index == 0)
				return bottomRight();
			else if (index == 1)
				return topRight();
			else if (index == 2)
				return topLeft();
			else
				return bottomLeft();
		}
		inline LineSegment rightEdge() const {
			return LineSegment(bottomRight(), topRight());
		}
		inline LineSegment topEdge() const {
			return LineSegment(topRight(), topLeft());
		}
		inline LineSegment leftEdge() const {
			return LineSegment(topLeft(), bottomLeft());
		}
		inline LineSegment bottomEdge() const {
			return LineSegment(bottomLeft(), bottomRight());
		}
		inline LineSegment edge(ptrdiff_t index, bool wrap = false) const {
			if (!wrap)
				assert(index >= 0 && index < 4);
			else {
				if (index < 0) {
					index *= -1;
					index = index - (index / 4) * 4;
					index = 4 - index;
				}
				else {
					index = index - (index / 4) * 4;
				}
			}
			if (index == 0)
				return rightEdge();
			else if (index == 1)
				return topEdge();
			else if (index == 2)
				return leftEdge();
			else
				return bottomEdge();
		}
		inline Vec2 topNormal() const {
			return Vec2::up;
		}
		inline Vec2 leftNormal() const {
			return Vec2::left;
		}
		inline Vec2 rightNormal() const {
			return Vec2::right;
		}
		inline Vec2 bottomNormal() const {
			return Vec2::down;
		}
		inline Vec2 normal(ptrdiff_t index, bool wrap = false) const {
			if (!wrap)
				assert(index >= 0 && index < 4);
			else {
				if (index < 0) {
					index *= -1;
					index = index - (index / 4) * 4;
					index = 4 - index;
				}
				else {
					index = index - (index / 4) * 4;
				}
			}
			if (index == 0)
				return rightNormal();
			else if (index == 1)
				return topNormal();
			else if (index == 2)
				return leftNormal();
			else
				return bottomNormal();
		}
		//Tests
		inline bool isDegenerated(bool almost = false) const {
			return almost ? aeq(m_size.x, 0) || aeq(m_size.y, 0) :
				m_size.x == 0 || m_size.y == 0;
		}
		inline bool isLineSegment(bool almost = false) const {
			return almost ? aeq(m_size.x, 0) != aeq(m_size.y, 0) :
				(m_size.x == 0) != (m_size.y == 0);
		}
		inline bool isPoint(bool almost = false) const {
			return almost ? aeq(m_size.x, 0) && aeq(m_size.y, 0) :
				(m_size.x == 0) && (m_size.y == 0);
		}
		inline bool containsPoint(const Vec2& pt) const {
			const auto w2 = m_size.x / 2;
			const auto h2 = m_size.y / 2;
			return pt.x >= (m_center.x - w2) && pt.x <= (m_center.x + w2) && pt.y >= (m_center.y - h2) &&
				pt.y <= (m_center.y + h2);
		}
		inline bool containsRect(const Rect& another) const {
			return another.left() >= left() && another.right() <= right() &&
				another.bottom() >= bottom() && another.top() <= top();
		}
		//Description
		inline std::string toString() const {
			return "{" + m_center.toString() + ", " + m_size.toString() + "}";
		}
		//Conversions
		Polygon toPolygon() const;
	private:
		Vec2 m_center;
		Vec2 m_size;
	};

	inline Option<Rect> getIntersection(const Rect& r1, const Rect& r2) {
		const auto nleft = std::max(r1.left(), r2.left());
		const auto nbottom = std::max(r1.bottom(), r2.bottom());
		const auto xoverlap = std::min(r1.right(), r2.right()) - nleft;
		if (xoverlap < 0)
			return nullptr;
		const auto yoverlap = std::min(r1.top(), r2.top()) - nbottom;
		if (yoverlap < 0)
			return nullptr;
		return Rect(nleft + xoverlap / 2.0f, nbottom + yoverlap / 2.0f, xoverlap, yoverlap);
	}
	inline Rect getUnion(const Rect& r1, const Rect& r2) {
		const auto nleft = std::min(r1.left(), r2.left());
		const auto nbottom = std::min(r1.bottom(), r2.bottom());
		const auto xrange = std::max(r1.right(), r2.right()) - nleft;
		const auto yrange = std::max(r1.top(), r2.top()) - nbottom;
		return Rect(nleft + xrange / 2.0f, nbottom + yrange / 2.0f, xrange, yrange);
	}
	inline bool strictlyIntersects(const Rect& r1, const Rect& r2) {
		const auto nleft = std::max(r1.left(), r2.left());
		const auto nbottom = std::max(r1.bottom(), r2.bottom());
		const auto xoverlap = std::min(r1.right(), r2.right()) - nleft;
		if (xoverlap <= 0)
			return false;
		const auto yoverlap = std::min(r1.top(), r2.top()) - nbottom;
		if (yoverlap <= 0)
			return false;
		return true;
	}

	inline bool operator ==(const Rect& r1, const Rect& r2) {
		return r1.center() == r2.center() && r1.size() == r2.size();
	}
	inline bool operator !=(const Rect& r1, const Rect& r2) {
		return r1.center() != r2.center() || r1.size() != r2.size();
	}
	inline bool operator >(const Rect& r1, const Rect& r2) {
		if (r1.center() == r2.center())
			return r1.size() > r2.size();
		else
			return r1.center() > r2.center();
	}
	inline bool operator <(const Rect& r1, const Rect& r2) {
		if (r1.center() == r2.center())
			return r1.size() < r2.size();
		else
			return r1.center() < r2.center();
	}
	inline bool operator >=(const Rect& r1, const Rect& r2) {
		if (r1 == r2)
			return true;

		if (r1.center() == r2.center())
			return r1.size() > r2.size();
		else
			return r1.center() > r2.center();
	}
	inline bool operator <=(const Rect& r1, const Rect& r2) {
		if (r1 == r2)
			return true;

		if (r1.center() == r2.center())
			return r1.size() < r2.size();
		else
			return r1.center() < r2.center();
	}
	inline bool aeq(const Rect& r1, const Rect& r2) {
		return aeq(r1.center(), r2.center()) && aeq(r1.size(), r2.size());
	}
}

namespace std {
	template<>
	class hash < sb::Rect > {
	public:
		size_t operator()(const sb::Rect& r) const {
			return sb::bitwiseHash((const unsigned char*)&r, sizeof(sb::Rect));
		}
	};
}

