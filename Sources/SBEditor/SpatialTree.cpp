/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "SpatialTree.h"
#include "SpatialTree.h" 
#include "Shape.h" 
#include "Rect.h" 
#include "Ray.h" 
#include "Intersection.h" 
#include "Matrix3x3.h" 

using namespace sb;

Rect computeBounds(const std::vector<Shape*>& shapes) {
	assert(shapes.size() != 0);
	Option<Rect> bounds = nullptr;
	for (auto& s : shapes) {
		if (!bounds)
			bounds = s->bounds();
		else
			bounds = getUnion(bounds.value(), s->bounds());
	}
	assert(bounds);
	return bounds.value();
}

class SpatialTreeNode {
public:
	//Constructors
	static SpatialTreeNode* nonTerminal(SpatialTreeNode* side0, SpatialTreeNode* side1) {
		auto node = new SpatialTreeNode();
		node->m_side0 = side0;
		node->m_side1 = side1;
		node->m_bounds = getUnion(side0->m_bounds, side1->m_bounds);
		return node;
	}
	static SpatialTreeNode* terminal(const std::vector<Shape*>& shapes) {
		auto node = new SpatialTreeNode();
		node->m_bounds = computeBounds(shapes);
		node->m_shapes = shapes;
		node->m_side0 = nullptr;
		node->m_side1 = nullptr;
		return node;
	}
	//Accessors
	const std::vector<Shape*>& shapes() const {
		return m_shapes;
	}
	const SpatialTreeNode* side0() const {
		return m_side0;
	}
	const SpatialTreeNode* side1() const {
		return m_side1;
	}
	const Rect& bounds() const {
		return m_bounds;
	}
	bool isTerminal() const {
		return m_shapes.size() != 0;
	}
	//Destructors
	~SpatialTreeNode() {
		if (m_side0)
			delete m_side0;
		if (m_side1)
			delete m_side1;
	}
private:
	SpatialTreeNode() {
	}
	SpatialTreeNode* m_side0;
	SpatialTreeNode* m_side1;
	std::vector<Shape*> m_shapes;
	Rect m_bounds;
};

enum class RayCastStaticTest {
	none,
	onlyA,
	onlyB,
	both,
	aAfterB,
	bAfterA
};

class SpatialTree_implementation {
public:
	//Constructor
	SpatialTree_implementation(SpatialTree* parent) {
		m_staticTree = nullptr;
		m_staticDirty = true;
		m_parent = parent;
	}
	//Destructor
	~SpatialTree_implementation() {
		if (m_staticTree)
			delete m_staticTree;
	}
	//Static Tree
	void addStaticNode(Shape* s) {
		assert(s);
		assert(!s->parent());
		m_staticShapes.insert(s);
		s->setParent(m_parent);
		s->setDynamic(false);
		m_staticDirty = true;
	}
	void removeStaticNode(Shape* s) {
		assert(s);
		assert(s->parent() == m_parent);
		assert(!s->isDynamic());
		m_staticShapes.erase(s);
		s->setParent(nullptr);
		s->setDynamic(false);
		m_staticDirty = true;
	}
	void rebuildStaticTree() const {
		if (!m_staticDirty)
			return;
		std::vector<Shape*> shapes;
		for (auto& s : m_staticShapes)
			shapes.push_back(s);

		if (m_staticTree)
			delete m_staticTree;
		if (shapes.size() == 0) {
			m_staticTree = nullptr;
			m_staticDirty = false;
			return;
		}
		m_staticTree = partition(shapes);
		m_staticDirty = false;
	}
	RayCastResult staticRayCast(const Ray& r, size_t mask) const {
		if (aeq(r.direction(), Vec2::zero))
			return RayCastResult(); //no Intersection

		rebuildStaticTree();
		if (!m_staticTree)
			return RayCastResult();
		return staticRayCast(r, mask, m_staticTree);
	}
	void staticRangeQuery(RangeQueryResult* result, const Rect& r, size_t mask) const {
		return staticRangeQuery(result, r, mask, nullptr);
	}
	void staticRangeQuery(RangeQueryResult* result, const Rect& r, size_t mask, Shape* exclude) const {
		rebuildStaticTree();
		if (!m_staticTree)
			return;
		staticRangeQuery(r, mask, exclude, m_staticTree, *result);
	}
	void staticPickQuery(RangeQueryResult* result, const Vec2& pt, size_t mask) const {
		rebuildStaticTree();
		if (!m_staticTree)
			return;
		staticPickQuery(pt, mask, m_staticTree, *result);
	}
	//Dynamic Tree
	void addDynamicNode(Shape* s) {
		assert(s);
		assert(!s->parent());
		addToDynamicTable(s);
		s->setParent(m_parent);
		s->setDynamic(true);
	}
	void removeDynamicNode(Shape* s) {
		assert(s);
		assert(s->parent() == m_parent);
		assert(s->isDynamic());
		removeFromDynamicTable(s);
		s->setParent(nullptr);
		s->setDynamic(false);
	}
	void transformDynamicNode(Shape* s, const Matrix3x3& m) {
		assert(s);
		assert(s->parent() == m_parent);
		assert(s->isDynamic());
		removeFromDynamicTable(s);
		s->applyTransform(m);
		addToDynamicTable(s);
	}
	RayCastResult dynamicRayCast(const Ray& r, size_t mask, const Option<float>& maxSqrdLen) const {
		if (aeq(r.direction(), Vec2::zero))
			return RayCastResult(); //no Intersection

		return internalDynamicRayCast(r, mask, maxSqrdLen);
	}
	void dynamicRangeQuery(RangeQueryResult* result, const Rect& r, size_t mask, Shape* exclude) const {
		dynamicRangeQuery(r, mask, exclude, *result);
	}
	void dynamicRangeQuery(RangeQueryResult* result, const Rect& r, size_t mask) const {
		dynamicRangeQuery(result, r, mask, nullptr);
	}
	void dynamicPickQuery(RangeQueryResult* result, const Vec2& pt, size_t mask) const {
		dynamicPickQuery(pt, mask, *result);
	}
	//utility
	void testIntersection(ExIntersectionInfo* ei, Shape* bodyA, Shape* bodyB) {
		IntersectionInfo ii;
		float invert = 1;

		switch (bodyA->type()) {
		case ShapeType::circle:
			switch (bodyB->type()) {
			case ShapeType::circle:
				ii = Intersection::get(bodyA->circle(), bodyB->circle());
				break;
			case ShapeType::Polygon:
				ii = Intersection::get(bodyB->Polygon(), bodyA->circle());
				invert = -1;
				break;
			case ShapeType::Rect:
				ii = Intersection::get(bodyB->Rect(), bodyA->circle());
				invert = -1;
				break;
			default:
				throw "invalid point";
			}
			break;
		case ShapeType::Polygon:
			switch (bodyB->type()) {
			case ShapeType::circle:
				ii = Intersection::get(bodyA->Polygon(), bodyB->circle());
				break;
			case ShapeType::Polygon:
				ii = Intersection::get(bodyA->Polygon(), bodyB->Polygon());
				break;
			case ShapeType::Rect:
				ii = Intersection::get(bodyA->Polygon(), bodyB->Rect());
				break;
			default:
				throw "invalid point";
			}
			break;
		case ShapeType::Rect:
			switch (bodyB->type()) {
			case ShapeType::circle:
				ii = Intersection::get(bodyA->Rect(), bodyB->circle());
				break;
			case ShapeType::Polygon:
				ii = Intersection::get(bodyB->Polygon(), bodyA->Rect());
				invert = -1;
				break;
			case ShapeType::Rect:
				ii = Intersection::get(bodyA->Rect(), bodyB->Rect());
				break;
			default:
				throw "invalid point";
			}
			break;
		default:
			throw "invalid point";
		}
		ei->empty = ii.empty;
		ei->bodyA = bodyA;
		ei->bodyB = bodyB;
		ei->normal = ii.normal * invert;
		ei->penetration = ii.penetration;
	}
private:
	//Fields
	SpatialTree* m_parent;
	//Static
	mutable SpatialTreeNode* m_staticTree;
	std::unordered_set<Shape*> m_staticShapes;
	mutable bool m_staticDirty;
	//Dynamic
	std::unordered_multimap<Vec2, Shape*, std::hash<Vec2>,
		std::equal_to<Vec2>, boost::pool_allocator<std::pair<const Vec2, Shape*>>> m_dynamicTable;
	std::map<float, size_t, std::less<float>, boost::fast_pool_allocator<std::pair<const float, size_t>>> m_xrange;
	std::map<float, size_t, std::less<float>, boost::fast_pool_allocator<std::pair<const float, size_t>>> m_yrange;
	//Methods
	//Static
	static bool staticHorizontalPartition(const std::vector<Shape*>& shapes, std::vector<Shape*>& side0, std::vector<Shape*>& side1) {
		assert(shapes.size() != 0);
		side0.clear();
		side1.clear();

		if (shapes.size() == 1)
			return false;

		auto bounds = computeBounds(shapes);

		auto sortedShapes = shapes;
		std::sort(sortedShapes.begin(), sortedShapes.end(), [](Shape* s1, Shape* s2) {
			return s1->bounds().right() < s2->bounds().right();
		});
		auto middle = sortedShapes.size() % 2 == 0 ? (sortedShapes.size() / 2) - 1 : sortedShapes.size() / 2;
		auto rSide0 = Rect();
		rSide0.setBounds(bounds.left(), sortedShapes[middle]->bounds().right(), bounds.bottom(), bounds.top());

		for (auto& s : sortedShapes) {
			if (strictlyIntersects(s->bounds(), rSide0))
				side0.push_back(s);
			else
				side1.push_back(s);
		}
		if (side0.size() == 0 || side1.size() == 0) {
			side0.clear();
			side1.clear();
			return false;
		}
		return true;
	}
	static bool staticVerticalPartition(const std::vector<Shape*>& shapes, std::vector<Shape*>& side0, std::vector<Shape*>& side1) {
		assert(shapes.size() != 0);
		side0.clear();
		side1.clear();

		if (shapes.size() == 1)
			return false;

		auto bounds = computeBounds(shapes);

		auto sortedShapes = shapes;
		std::sort(sortedShapes.begin(), sortedShapes.end(), [](Shape* s1, Shape* s2) {
			return s1->bounds().bottom() < s2->bounds().bottom();
		});
		auto middle = sortedShapes.size() % 2 == 0 ? (sortedShapes.size() / 2) - 1 : sortedShapes.size() / 2;
		auto rSide0 = Rect();
		rSide0.setBounds(bounds.left(), bounds.right(), sortedShapes[middle]->bounds().bottom(), bounds.top());

		for (auto& s : sortedShapes) {
			if (strictlyIntersects(s->bounds(), rSide0))
				side0.push_back(s);
			else
				side1.push_back(s);
		}
		if (side0.size() == 0 || side1.size() == 0) {
			side0.clear();
			side1.clear();
			return false;
		}
		return true;
	}
	static bool staticBestPartition(const std::vector<Shape*>& shapes, std::vector<Shape*>& side0, std::vector<Shape*>& side1) {
		std::vector<Shape*> hs0, hs1, vs0, vs1;
		auto hr = staticHorizontalPartition(shapes, hs0, hs1);
		auto vr = staticHorizontalPartition(shapes, vs0, vs1);

		if (!hr) {
			side0 = vs0;
			side1 = vs1;
			return vr;
		}
		if (!vr) {
			side0 = hs0;
			side1 = hs1;
			return hr;
		}

		auto middle = (float)shapes.size() / 2.0f;
		auto hd = fabsf(middle - (float)hs0.size());
		auto vd = fabsf(middle - (float)vs0.size());
		if (hd < vd) {
			side0 = hs0;
			side1 = hs1;
			return true;
		}
		else {
			side0 = vs0;
			side1 = vs1;
			return true;
		}
	}
	static SpatialTreeNode* partition(const std::vector<Shape*>& shapes) {
		std::vector<Shape*> s0, s1;
		auto r = staticBestPartition(shapes, s0, s1);
		if (!r) {
			return SpatialTreeNode::terminal(shapes);
		}
		else {
			return SpatialTreeNode::nonTerminal(
				partition(s0),
				partition(s1)
				);
		}
	}
	//Ray Cast
	static RayCastStaticTest computeRayCastStaticTest(const Ray& r, const SpatialTreeNode* node) {
		assert(!node->isTerminal());
		Option<Vec2> i0 = Intersection::get(r, node->side0()->bounds());
		Option<Vec2> i1 = Intersection::get(r, node->side1()->bounds());

		if (i0.hasValue()) {
			if (i1.hasValue()) {
				auto containsA = node->side0()->bounds().containsPoint(r.point());
				auto containsB = node->side1()->bounds().containsPoint(r.point());

				if (containsA && containsB)
					return RayCastStaticTest::both;
				else {
					if (containsA)
						return RayCastStaticTest::aAfterB;
					else if (containsB)
						return RayCastStaticTest::bAfterA;
					else {
						auto p1 = r.computeT(i0.value());
						auto p2 = r.computeT(i1.value());
						return p1 < p2 ? RayCastStaticTest::aAfterB : RayCastStaticTest::bAfterA;
					}
				}
			}
			else
				return RayCastStaticTest::onlyA;
		}
		else {
			if (i1.hasValue()) {
				return RayCastStaticTest::onlyB;
			}
			else
				return RayCastStaticTest::none;
		}
	}
	static RayCastResult staticRayCast(const Ray& r, size_t mask, const SpatialTreeNode* node) {
		if (node->isTerminal()) {
			RayCastResult rr;
			auto cp = std::numeric_limits<float>::max();
			for (auto& s : node->shapes()) {
				if (!(s->typeMask() & mask))
					continue;
				auto tr = s->type() == ShapeType::circle ? Intersection::get(r, s->circle()) : (
					s->type() == ShapeType::Rect ? Intersection::get(r, s->Rect()) : Intersection::get(r, s->Polygon())
					);
				if (!tr.hasValue())
					continue;
				auto p = r.computeT(tr.value());
				if (p < cp) {
					cp = p;
					rr.parameter = p;
					rr.empty = false;
					rr.intersected = s;
					rr.point = tr.value();
				}
			}
			return rr;
		}
		auto rt = computeRayCastStaticTest(r, node);
		RayCastResult r0, r1;
		switch (rt) {
		case RayCastStaticTest::none:
			return RayCastResult();
		case RayCastStaticTest::onlyA:
			return staticRayCast(r, mask, node->side0());
		case RayCastStaticTest::onlyB:
			return staticRayCast(r, mask, node->side1());;
		case RayCastStaticTest::both:
			r0 = staticRayCast(r, mask, node->side0());
			r1 = staticRayCast(r, mask, node->side1());
			if (r0.empty)
				return r1;
			if (r1.empty)
				return r0;
			return r0.parameter < r1.parameter ? r0 : r1;
		case RayCastStaticTest::aAfterB:
			r0 = staticRayCast(r, mask, node->side0());
			if (!r0.empty && !node->side1()->bounds().containsPoint(r0.point))
				return r0;
			r1 = staticRayCast(r, mask, node->side1());
			if (r0.empty)
				return r1;
			if (r1.empty)
				return r0;
			return r0.parameter < r1.parameter ? r0 : r1;
		case RayCastStaticTest::bAfterA:
			r1 = staticRayCast(r, mask, node->side1());
			if (!r1.empty && !node->side0()->bounds().containsPoint(r1.point))
				return r1;
			r0 = staticRayCast(r, mask, node->side0());
			if (r0.empty)
				return r1;
			if (r1.empty)
				return r0;
			return r0.parameter < r1.parameter ? r0 : r1;
		default:
			throw "should never get here!";
		}
	}
	//Range Query
	static void staticRangeQuery(const Rect& r, size_t mask, Shape* exclude, const SpatialTreeNode* node, RangeQueryResult& rq) {
		if (rq.isFull())
			return;

		if (node->isTerminal()) {
			for (auto& s : node->shapes()) {
				if (rq.isFull())
					break;
				if (exclude && s == exclude)
					continue;
				if (!(s->typeMask() & mask))
					continue;
				if (Intersection::test(r, s->bounds()))
					rq.pushShape(s);
			}
		}
		else {
			if (Intersection::test(r, node->side0()->bounds()))
				staticRangeQuery(r, mask, exclude, node->side0(), rq);
			if (Intersection::test(r, node->side1()->bounds()))
				staticRangeQuery(r, mask, exclude, node->side1(), rq);
		}
	}
	static void staticPickQuery(const Vec2& pt, size_t mask, const SpatialTreeNode* node, RangeQueryResult& rq) {
		if (rq.isFull())
			return;

		if (node->isTerminal()) {
			for (auto& s : node->shapes()) {
				if (rq.isFull())
					break;
				if (!(s->typeMask() & mask))
					continue;
				if (Intersection::test(s->bounds(), pt)) {
					if (s->type() == ShapeType::circle && Intersection::test(s->circle(), pt))
						rq.pushShape(s);
					else if (s->type() == ShapeType::Rect)
						rq.pushShape(s);
					else if (s->type() == ShapeType::Polygon && Intersection::test(s->Polygon(), pt))
						rq.pushShape(s);
				}
			}
		}
		else {
			if (Intersection::test(node->side0()->bounds(), pt))
				staticPickQuery(pt, mask, node->side0(), rq);
			if (Intersection::test(node->side1()->bounds(), pt))
				staticPickQuery(pt, mask, node->side1(), rq);
		}
	}
	//Dynamic
	static Vec2 cellAddress(const Vec2& v) {
		float vx = v.x / SbCellSize;
		float vy = v.y / SbCellSize;
		return Vec2(floor(vx), floor(vy));
	}
	static Rect cellBounds(const Vec2& v) {
		Rect r;
		r.setBounds(v.x * SbCellSize, (v.x + 1) * SbCellSize, v.y * SbCellSize, (v.y + 1) * SbCellSize);
		return r;
	}
	static void cellAddressRange(const Rect& r, Vec2& aMin, Vec2& aMax) {
		aMin = cellAddress(r.bottomLeft());
		aMax = cellAddress(r.topRight());
	}
	void addToDynamicTable(Shape* s) {
		auto b = s->bounds();
		Vec2 aMin, aMax;
		cellAddressRange(b, aMin, aMax);

		for (float i = aMin.x; i <= aMax.x; i += 1.0f) {
			for (float j = aMin.y; j <= aMax.y; j += 1.0f)
				m_dynamicTable.insert({ Vec2(i, j), s });

			if (m_xrange.count(i))
				m_xrange[i]++;
			else
				m_xrange[i] = 1;
		}

		for (float j = aMin.y; j <= aMax.y; j += 1.0f) {
			if (m_yrange.count(j))
				m_yrange[j]++;
			else
				m_yrange[j] = 1;
		}
	}
	void removeFromDynamicTable(Shape* s) {
		auto b = s->bounds();
		Vec2 aMin, aMax;
		cellAddressRange(b, aMin, aMax);

		for (float i = aMin.x; i <= aMax.x; i += 1.0f) {
			for (float j = aMin.y; j <= aMax.y; j += 1.0f) {
				auto shapes = m_dynamicTable.equal_range(Vec2(i, j));
				auto it = shapes.first;
				while (it != shapes.second) {
					if (it->second == s) {
						it = m_dynamicTable.erase(it);
						break;
					}
					else
						it++;
				}
			}

			m_xrange[i]--;
			if (m_xrange[i] == 0)
				m_xrange.erase(i);
		}

		for (float j = aMin.y; j <= aMax.y; j += 1.0f) {
			m_yrange[j]--;
			if (m_yrange[j] == 0)
				m_yrange.erase(j);
		}
	}
	Rect dynamicBounds() const {
		if (m_xrange.size() == 0 || m_yrange.size() == 0)
			return Rect();
		float xmin = m_xrange.begin()->first;
		float xmax = m_xrange.rbegin()->first;
		float ymin = m_yrange.begin()->first;
		float ymax = m_yrange.rbegin()->first;
		Rect r1 = cellBounds(Vec2(xmin, ymin));
		Rect r2 = cellBounds(Vec2(xmax, ymax));
		return getUnion(r1, r2);
	}
	//Ray Cast
	RayCastResult internalDynamicRayCast(const Ray& r, size_t mask, const Option<float>& maxSqrdLen) const {
		if (m_dynamicTable.size() == 0)
			return RayCastResult();
		if (aeq(r.direction(), Vec2::zero))
			return RayCastResult(); //no Intersection
		auto bd = dynamicBounds();
		Vec2 startAddr;
		if (!bd.containsPoint(r.point())) {
			auto ri = Intersection::get(r, bd);
			if (!ri)
				return RayCastResult(); //doesn't intersect anyone...
			else
				startAddr = cellAddress(ri.value());
		}
		else
			startAddr = cellAddress(r.point());
		RayCastResult res;
		bool inside = false;
		std::unordered_set<Shape*, std::hash<Shape*>, std::equal_to<Shape*>, boost::pool_allocator<Shape*>> checkedShapes;

		while (true) {
			auto cbd = cellBounds(startAddr);
			if (inside) {
				if (!bd.containsPoint(cbd.center()))
					break;
			}
			if (bd.containsPoint(cbd.center()))
				inside = true;
			auto shapes = m_dynamicTable.equal_range(startAddr);
			for (auto it = shapes.first; it != shapes.second; it++) {
				if (!(it->second->typeMask() & mask))
					continue;
				if (checkedShapes.count(it->second))
					continue;
				Option<Vec2> pt = it->second->type() == ShapeType::circle ? Intersection::get(r, it->second->circle()) :
					(it->second->type() == ShapeType::Rect ? Intersection::get(r, it->second->Rect()) :
					 Intersection::get(r, it->second->Polygon()));
				checkedShapes.insert(it->second);
				if (pt.hasValue()) {
					auto p = r.computeT(pt.value());
					if (res.empty || p < res.parameter) {
						res.empty = false;
						res.intersected = it->second;
						res.parameter = p;
						res.point = pt.value();
					}
				}
			}
			if (!res.empty && cbd.containsPoint(res.point))
				break;
			//to the next address
			auto previousAddr = startAddr;
			//horizontally
			if (r.direction().x > 0 && Intersection::get(r, cbd.rightEdge()).hasValue())
				startAddr.x += 1;
			else if (r.direction().x < 0 && Intersection::get(r, cbd.leftEdge()).hasValue())
				startAddr.x -= 1;
			//vertically
			if (r.direction().y > 0 && Intersection::get(r, cbd.topEdge()).hasValue())
				startAddr.y += 1;
			else if (r.direction().y < 0 && Intersection::get(r, cbd.bottomEdge()).hasValue())
				startAddr.y -= 1;
			if (previousAddr == startAddr) //this shouldn't happen
				return RayCastResult();

			if (maxSqrdLen.hasValue()) {
				auto itt = Intersection::get(r, cbd);
				if (itt.hasValue()) {
					auto vv = itt.value() - r.point();
					auto sqrdLen = vv.squaredLength();
					if (sqrdLen > maxSqrdLen.value())
						break;
				}
			}
		}

		return res;
	}
	//Range Query
	void dynamicRangeQuery(const Rect& r, size_t mask, Shape* exclude, RangeQueryResult& rd) const {
		if (m_dynamicTable.size() == 0)
			return;
		auto bd = dynamicBounds();
		if (!Intersection::test(r, bd))
			return;
		Vec2 aMin, aMax;
		cellAddressRange(r, aMin, aMax);
		std::unordered_set<Shape*, std::hash<Shape*>, std::equal_to<Shape*>, boost::pool_allocator<Shape*>> checkedShapes;

		for (float i = aMin.x; i <= aMax.x; i += 1.0f) {
			for (float j = aMin.y; j <= aMax.y; j += 1.0f) {
				if (rd.isFull())
					return;

				auto shapes = m_dynamicTable.equal_range(Vec2(i, j));
				for (auto it = shapes.first; it != shapes.second; it++) {
					if (rd.isFull())
						return;
					if (!(it->second->typeMask() & mask))
						continue;
					if (exclude && it->second == exclude)
						continue;
					if (checkedShapes.count(it->second))
						continue;
					if (Intersection::test(it->second->bounds(), r))
						rd.pushShape(it->second);
					checkedShapes.insert(it->second);
				}
			}
		}
	}
	void dynamicPickQuery(const Vec2& pt, size_t mask, RangeQueryResult& rd) const {
		if (m_dynamicTable.size() == 0)
			return;
		auto bd = dynamicBounds();
		if (!Intersection::test(bd, pt))
			return;
		auto addr = cellAddress(pt);

		auto shapes = m_dynamicTable.equal_range(addr);
		for (auto it = shapes.first; it != shapes.second; it++) {
			if (rd.isFull())
				return;
			if (!(it->second->typeMask() & mask))
				continue;
			if (Intersection::test(it->second->bounds(), pt)) {
				if (it->second->type() == ShapeType::circle && Intersection::test(it->second->circle(), pt))
					rd.pushShape(it->second);
				else if (it->second->type() == ShapeType::Rect)
					rd.pushShape(it->second);
				else if (it->second->type() == ShapeType::Polygon && Intersection::test(it->second->Polygon(), pt))
					rd.pushShape(it->second);
			}
		}
	}
};

sb::SpatialTree::SpatialTree() {
	m_impl = new SpatialTree_implementation(this);
}

sb::SpatialTree::~SpatialTree() {
	delete m_impl;
}

sb::SpatialTree* sb::SpatialTree::create() {
	return new SpatialTree();
}

void sb::SpatialTree::addStaticNode(Shape* s) const {
	m_impl->addStaticNode(s);
}

void sb::SpatialTree::addDynamicNode(Shape* s) const {
	m_impl->addDynamicNode(s);
}

void sb::SpatialTree::removeNode(Shape* s) const {
	if (s->isDynamic())
		m_impl->removeDynamicNode(s);
	else
		m_impl->removeStaticNode(s);
}

void sb::SpatialTree::transform(Shape* s, const Matrix3x3& m) {
	m_impl->transformDynamicNode(s, m);
}

sb::RayCastResult sb::SpatialTree::rayCast(const Ray& r,
										   size_t mask /*= std::numeric_limits<size_t>::max()*/,
										   StaticDynamicMask sdFilter /*= sdmAll*/) const {
	RayCastResult rc;
	if (sdFilter & sdmStatic) {
		rc = m_impl->staticRayCast(r, mask);
	}
	if (sdFilter & sdmDynamic) {
		Option<float> sqrdLen = nullptr;
		if (!rc.empty)
			sqrdLen = (rc.point - r.point()).squaredLength();

		RayCastResult rc2;
		rc2 = m_impl->dynamicRayCast(r, mask, sqrdLen);
		if (rc.empty || (!rc2.empty && rc2.parameter < rc.parameter))
			rc = rc2;
	}
	return rc;
}

void sb::SpatialTree::rangeQuery(RangeQueryResult* result,
								 const Rect& r,
								 size_t mask /*= std::numeric_limits<size_t>::max()*/,
								 StaticDynamicMask sdFilter /*= sdmAll*/) const {
	assert(result);
	if (result->isFull())
		return;
	if (sdFilter & sdmDynamic) {
		m_impl->dynamicRangeQuery(result, r, mask);
	}

	if ((sdFilter & sdmStatic) && !result->isFull()) {
		m_impl->staticRangeQuery(result, r, mask);
	}
}

void sb::SpatialTree::pickQuery(RangeQueryResult* result,
								const Vec2& pt,
								size_t mask /*= std::numeric_limits<size_t>::max()*/,
								StaticDynamicMask sdFilter /*= sdmAll*/) const {
	assert(result);
	if (result->isFull())
		return;
	if (sdFilter & sdmDynamic) {
		m_impl->dynamicPickQuery(result, pt, mask);
	}

	if ((sdFilter & sdmStatic) && !result->isFull()) {
		m_impl->staticPickQuery(result, pt, mask);
	}
}

void sb::SpatialTree::intersectionQuery(IntersectionQueryResult* result, Shape* bodyA, StaticDynamicMask sdFilter) const {
	assert(result);
	assert(bodyA);
	assert(bodyA->parent() == this);
	if (result->isFull())
		return;
	if (sdFilter & sdmDynamic) {
		RangeQueryResult r;
		m_impl->dynamicRangeQuery(&r, bodyA->bounds(), bodyA->collisionMask(), bodyA);
		for (size_t i = 0; i < r.count; i++) {
			if (result->isFull())
				break;
			ExIntersectionInfo ei;
			m_impl->testIntersection(&ei, bodyA, r.shapes[i]);
			if (!ei.empty)
				result->pushInfo(ei);
		}
	}
	if (result->isFull())
		return;
	if (sdFilter & sdmStatic) {
		RangeQueryResult r;
		m_impl->staticRangeQuery(&r, bodyA->bounds(), bodyA->collisionMask(), bodyA);
		for (size_t i = 0; i < r.count; i++) {
			if (result->isFull())
				break;
			ExIntersectionInfo ei;
			m_impl->testIntersection(&ei, bodyA, r.shapes[i]);
			if (!ei.empty)
				result->pushInfo(ei);
		}
	}
}
