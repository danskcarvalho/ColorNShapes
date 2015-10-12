/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Polygon.h"
#include "Line.h"

using namespace sb;

Polygon Polygon::optimize() const {
	std::vector<Vec2> newPoints;
	size_t index = 0;
	for (auto& p : m_points) {
		if (newPoints.size() == 0) {
			newPoints.push_back(p);
			index++;
			continue;
		}
		if (aeq(newPoints.back(), p))
			continue;
		if (newPoints.size() >= 2) {
			auto ln = Line(newPoints[newPoints.size() - 1], newPoints[newPoints.size() - 2]);
			if (ln.containsPoint(p, true))
				newPoints.pop_back();
		}
		if (index == (m_points.size() - 1) && m_closed && newPoints.size() >= 2) {
			auto ln = Line(newPoints[0], newPoints[newPoints.size() - 1]);
			if (ln.containsPoint(p, true)) { //Don't include that point
				index++;
				continue;
			}
		}
		newPoints.push_back(p);
		index++;
	}
	return Polygon(newPoints, m_closed);
}

float Polygon::signedArea() const {
	if (m_signedArea.hasValue())
		return m_signedArea.value();

	if (m_points.size() <= 2) {
		m_signedArea = 0.0;
		return 0;
	}

	float a = 0;
	for (size_t i = 0; i < m_points.size(); i++) {
		const auto& pi = m_points[i];
		const auto& pi1 = i == (m_points.size() - 1) ? m_points[0] : m_points[i + 1];
		a += pi.x * pi1.y - pi1.x * pi.y;
	}
	a *= 0.5f;
	m_signedArea = a;
	return a;
}

float sb::Polygon::area() const {
	return fabsf(signedArea());
}

sb::Vec2 sb::Polygon::centroid() const {
	if (m_centroid.hasValue())
		return m_centroid.value();

	const auto sa = signedArea();
	if (m_points.size() == 0) {
		m_centroid = Vec2::zero;
		return m_centroid.value();
	}
	else if (m_points.size() <= 2 || aeq(sa, 0)) {
		Vec2 pt = Vec2::zero;
		for (size_t i = 0; i < m_points.size(); i++)
			pt += m_points[i];
		pt /= (float)m_points.size();
		m_centroid = pt;
		return m_centroid.value();
	}

	float cx = 0, cy = 0;
	float invA = 1.0f / (6 * sa);

	for (size_t i = 0; i < m_points.size(); i++) {
		const auto& pi = m_points[i];
		const auto& pi1 = i == (m_points.size() - 1) ? m_points[0] : m_points[i + 1];
		const auto a = pi.x * pi1.y - pi1.x * pi.y;
		cx += (pi.x + pi1.x) * a;
		cy += (pi.y + pi1.y) * a;
	}
	cx /= invA;
	cy /= invA;
	m_centroid = Vec2(cx, cy);
	return m_centroid.value();
}

float sb::Polygon::perimeter() const {
	if (m_perimeter.hasValue())
		return m_perimeter.value();

	const auto ec = edgeCount();
	float p = 0;
	for (ptrdiff_t i = 0; i < ec; i++)
		p += edge(i).length();
	m_perimeter = p;
	return p;
}

ptrdiff_t sb::Polygon::edgeCount() const {
	if (m_points.size() <= 1)
		return 0;
	if (m_closed)
		return m_points.size();
	else
		return m_points.size() - 1;
}

sb::LineSegment sb::Polygon::edge(ptrdiff_t index, bool wrap) const {
	const auto ec = edgeCount();
	if (!wrap)
		assert(index >= 0 && index < ec);
	else {
		assert(ec != 0);
		if (index < 0) {
			index *= -1;
			index = index - (index / ec) * ec;
			index = ec - index;
		}
		else {
			index = index - (index / ec) * ec;
		}
	}
	if (index == (ec - 1) && m_closed)
		return LineSegment(m_points[index], m_points[0]);
	else
		return LineSegment(m_points[index], m_points[index + 1]);
}

sb::Vec2 sb::Polygon::normal(ptrdiff_t index, bool wrap) const {
	const auto ec = edgeCount();
	if (!wrap)
		assert(index >= 0 && index < ec);
	else {
		assert(ec != 0);
		if (index < 0) {
			index *= -1;
			index = index - (index / ec) * ec;
			index = ec - index;
		}
		else {
			index = index - (index / ec) * ec;
		}
	}

	if (m_normals.size() == ec)
		return m_normals[index];

	const auto o = ordering();
	assert(o != PointOrdering::unknown);

	for (ptrdiff_t i = 0; i < ec; i++) {
		Vec2 v;
		if (i == (ec - 1) && m_closed)
			v = m_points[0] - m_points[i];
		else
			v = m_points[i + 1] - m_points[i];

		v.normalize();
		v.rotate90();
		if (o == PointOrdering::ccw)
			v = -v;
		m_normals.push_back(v);
	}
	return m_normals[index];
}

sb::Rect sb::Polygon::bounds() const {
	if (m_bounds.hasValue())
		return m_bounds.value();

	float minx, miny;
	float maxx, maxy;
	minx = miny = std::numeric_limits<float>::max();
	maxx = maxy = std::numeric_limits<float>::min();
	for (size_t i = 0; i < m_points.size(); i++) {
		const auto& p = m_points[i];
		if (p.x < minx)
			minx = p.x;
		if (p.y < miny)
			miny = p.y;
		if (p.x > maxx)
			maxx = p.x;
		if (p.y > maxy)
			maxy = p.y;
	}
	m_bounds = Rect((maxx + minx) / 2.0f, (maxy + miny) / 2.0f, maxx - minx, maxy - miny);
	return m_bounds.value();
}

sb::LineSegment closedPolygonEdge(const std::vector<Vec2>& points, size_t index) {
	if (index == (points.size() - 1))
		return LineSegment(points[index], points[0]);
	else
		return LineSegment(points[index], points[index + 1]);
}

bool sb::Polygon::isConvex() const {
	if (m_isConvex.hasValue())
		return m_isConvex.value();

	if (m_points.size() <= 3) {
		m_isConvex = true;
		return true;
	}

	const auto ec = m_points.size();

	size_t p = 0, n = 0;
	for (size_t i = 0; i < ec; i++) {
		auto e1 = closedPolygonEdge(m_points, i);
		auto e2 = i == 0 ? closedPolygonEdge(m_points, ec - 1) : closedPolygonEdge(m_points, i - 1);

		auto v1 = e1.pointB() - e1.pointA();
		auto v2 = e2.pointB() - e2.pointA();

		auto sign = cross(v1, v2);

		if (sign > 0)
			p++;
		else if (sign < 0)
			n++;
	}

	if (p * n == 0) //all positive or all negative?
		m_isConvex = true;
	else
		m_isConvex = false;
	return m_isConvex.value();
}

bool sb::Polygon::isSimple() const {
	if (m_isSimple.hasValue())
		return m_isSimple.value();

	if (m_points.size() <= 1) {
		m_isSimple = true;
		return true;
	}

	if (m_points.size() == 2) {
		m_isSimple = !aeq(m_points[0], m_points[1]);
		return m_isSimple.value();
	}

	if (m_closed) {
		typedef boost::geometry::model::d2::point_xy<float> b_point;
		typedef boost::geometry::model::polygon<b_point, false> b_polygon;
		b_polygon bPolygon;
		std::vector<b_point> bPoints;
		std::vector<Vec2> myPoints = m_points;
		//we add the points
		if (ordering() == PointOrdering::cw)
			std::reverse(myPoints.begin(), myPoints.end());
		for (auto& p : myPoints)
			bPoints.push_back(b_point(p.x, p.y));

		//we initialize the polygon
		boost::geometry::append(bPolygon, bPoints);
		boost::geometry::correct(bPolygon);

		//we then compute the convex hull
		m_isSimple = !boost::geometry::intersects(bPolygon);
	}
	else {
		typedef boost::geometry::model::d2::point_xy<float> b_point;
		typedef boost::geometry::model::polygon<b_point, false, false> b_polygon;
		b_polygon bPolygon;
		std::vector<b_point> bPoints;
		std::vector<Vec2> myPoints = m_points;
		//we add the points
		if (ordering() == PointOrdering::cw)
			std::reverse(myPoints.begin(), myPoints.end());
		for (auto& p : myPoints)
			bPoints.push_back(b_point(p.x, p.y));

		//we initialize the polygon
		boost::geometry::append(bPolygon, bPoints);
		boost::geometry::correct(bPolygon);

		//we then compute the convex hull
		m_isSimple = !boost::geometry::intersects(bPolygon);
	}
	return m_isSimple.value();
}

LineSegment sb::Polygon::closestEdgeFrom(const Vec2& pt, size_t* index) const {
	float d = std::numeric_limits<float>::max();
	auto ec = edgeCount();
	LineSegment ls;
	if (index)
		*index = -1;
	for (ptrdiff_t i = 0; i < ec; i++) {
		auto tls = edge(i);
		float td = tls.distanceFrom(pt);
		if (td < d) {
			d = td;
			ls = tls;
			if (index)
				*index = i;
		}
	}
	return ls;
}

sb::Vec2 sb::Polygon::closestPointFrom(const Vec2& pt) const {
	return closestEdgeFrom(pt).closestPointFrom(pt);
}

sb::Polygon sb::Polygon::toConvex() const {
	assert(!aeq(area(), 0));
	assert(isSimple());

	typedef boost::geometry::model::d2::point_xy<float> b_point;
	typedef boost::geometry::model::polygon<b_point, false> b_polygon;
	b_polygon inputPolygon;
	b_polygon convexHull;
	std::vector<b_point> inputPoints;
	std::vector<Vec2> outputPoints = m_points;
	//we add the points
	if (ordering() == PointOrdering::cw)
		std::reverse(outputPoints.begin(), outputPoints.end());
	for (auto& p : outputPoints)
		inputPoints.push_back(b_point(p.x, p.y));

	//we initialize the polygon
	boost::geometry::append(inputPolygon, inputPoints);
	boost::geometry::correct(inputPolygon);

	//we then compute the convex hull
	boost::geometry::convex_hull(inputPolygon, convexHull);

	//we then add the points
	outputPoints.clear();
	inputPoints = convexHull.outer();
	for (auto& p : inputPoints)
		outputPoints.push_back(Vec2(p.x(), p.y()));

	return Polygon(std::move(outputPoints));
}

bool sb::Polygon::containsPoint(const Vec2& pt) const {
	assert(isSimple());
	if (isConvex()) {
		const auto ec = edgeCount();
		for (ptrdiff_t i = 0; i < ec; i++) {
			const auto n = normal(i);
			const auto ep = edge(i).pointA();
			const auto v = pt - ep;
			const auto dt = dot(v, n);
			if (dt > 0) //found separating axis
				return false;
		}
		return true;
	}
	else {
		typedef boost::geometry::model::d2::point_xy<float> b_point;
		typedef boost::geometry::model::polygon<b_point, false> b_polygon;
		b_polygon bPolygon;
		std::vector<b_point> bPoints;
		std::vector<Vec2> inputPoints = m_points;
		//we add the points
		if (ordering() == PointOrdering::cw)
			std::reverse(inputPoints.begin(), inputPoints.end());
		for (auto& p : inputPoints)
			bPoints.push_back(b_point(p.x, p.y));

		//we initialize the polygon
		boost::geometry::append(bPolygon, bPoints);
		boost::geometry::correct(bPolygon);

		return boost::geometry::within(b_point(pt.x, pt.y), bPolygon);
	}
}

PointOrdering Polygon::ordering() const {
	const auto sa = signedArea();
	if (aeq(sa, 0))
		return PointOrdering::unknown;
	else
		return sa < 0 ? PointOrdering::cw : PointOrdering::ccw;
}

sb::polygonPath::polygonPath() {
	m_totalLength = 0;
}

sb::polygonPath::polygonPath(const struct Polygon& p) {
	assert(p.pointCount() >= 2);
	m_polygon = p;
	m_totalLength = 0;
	for (ptrdiff_t i = 0; i < m_polygon.edgeCount(); i++) {
		if (i == 0)
			m_length.push_back(m_polygon.edge(i).length());
		else
			m_length.push_back(m_polygon.edge(i).length() + m_length[i - 1]);
		m_totalLength += m_polygon.edge(i).length();
	}
}

sb::Vec2 sb::polygonPath::pointAt(float l, size_t low_index, size_t hi_index) const {
	if (low_index == hi_index) {
		auto lEdge = low_index != 0 ? l - m_length[low_index - 1] : l;
		auto t = lEdge / m_polygon.edge((uint32_t)low_index).length();
		return lerp(t, m_polygon.edge((uint32_t)low_index).pointA(), m_polygon.edge((uint32_t)low_index).pointB());
	}
	else {
		auto mid_index = (hi_index + low_index) / 2;
		auto _mid_index_1 = mid_index + 1;
		if (l <= m_length[mid_index])
			return pointAt(l, low_index, mid_index);
		else
			return pointAt(l, _mid_index_1, hi_index);
	}
}

sb::Vec2 sb::polygonPath::pointAt(float l, bool clamp /*= false*/) const {
	if (l == m_totalLength)
		return m_polygon.edge(m_polygon.edgeCount() - 1).pointB();
	if (l == 0)
		return m_polygon.edge(0).pointA();
	if (l > m_totalLength) {
		if (clamp)
			return m_polygon.edge(m_polygon.edgeCount() - 1).pointB();
		auto intPart = (int)(l / m_totalLength);
		l -= intPart * m_totalLength;
	}
	if (l < 0) {
		if (clamp)
			return m_polygon.edge(0).pointA();
		//first we remove unnecessary walks throughout the polygon...
		auto intPart = (int)(l / m_totalLength);
		l = l - intPart * m_totalLength;
		//we then make l positive
		l = m_totalLength + l;
	}
	return pointAt(l, 0, m_length.size() - 1);
}

float sb::polygonPath::lengthAt(const Vec2& pt) const {
	size_t edgeIndex = 0;
	auto ce = m_polygon.closestEdgeFrom(pt, &edgeIndex); //closest edge
	float f = 0;
	for (uint32_t i = 0; i < edgeIndex; i++)
		f += m_polygon.edge(i).length();
	auto v = pt - ce.pointA();
	auto vdir = ce.pointB() - ce.pointA();
	v.projectOver(vdir);
	f += v.length();
	return f;
}

const struct Polygon& sb::polygonPath::Polygon() const {
	return m_polygon;
}

float sb::polygonPath::length() const {
	return m_totalLength;
}
