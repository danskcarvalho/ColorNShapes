/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "MaxRects.h"

using namespace sb;

static Rect newRect(const Vec2& bottomLeft, const Vec2& size) {
	return Rect(bottomLeft + size / 2.0f, size);
}

bool isBestFit(MaxRects::FitPolicy FitPolicy, const Rect& freeRectangle, const Rect& previousFreeRectangle, const Vec2& rectangle) {
	float a1, a2, b1, b2, c1, c2;
	switch (FitPolicy) {
		case MaxRects::FitPolicy::BL:
			if (freeRectangle.bottom() == previousFreeRectangle.bottom())
				return freeRectangle.left() <= previousFreeRectangle.left();
			else
				return freeRectangle.bottom() <= previousFreeRectangle.bottom();
		case MaxRects::FitPolicy::BAF:
			a1 = rectangle.area() / freeRectangle.area();
			a2 = rectangle.area() / previousFreeRectangle.area();
			return a1 > a2;
		case MaxRects::FitPolicy::BSSF:
			b1 = std::min(freeRectangle.width() - rectangle.width(), freeRectangle.height() - rectangle.height());
			b2 = std::min(previousFreeRectangle.width() - rectangle.width(), previousFreeRectangle.height() - rectangle.height());
			return b1 < b2;
		case MaxRects::FitPolicy::BLSF:
			c1 = std::max(freeRectangle.width() - rectangle.width(), freeRectangle.height() - rectangle.height());
			c2 = std::max(previousFreeRectangle.width() - rectangle.width(), previousFreeRectangle.height() - rectangle.height());
			return c1 < c2;
		default:
			throw "should never get here.";
	}
}

void split(const Rect& free, const Rect& r, std::vector<Rect>& results) {
	results.clear();
	Option<Rect> ro = getIntersection(free, r);
	if (!ro || ro->width() == 0 || ro->height() == 0)
		return;
	auto ur = ro.value();
	auto left = newRect(Vec2(free.left(), free.bottom()), Vec2(ur.left() - free.left(), free.height()));
	auto right = newRect(Vec2(ur.right(), free.bottom()), Vec2(free.right() - ur.right(), free.height()));
	auto top = newRect(Vec2(free.left(), ur.top()), Vec2(free.width(), free.top() - ur.top()));
	auto bottom = newRect(Vec2(free.left(), free.bottom()), Vec2(free.width(), ur.bottom() - free.bottom()));
	if (left.width() != 0 && left.height() != 0)
		results.push_back(left);
	if (right.width() != 0 && right.height() != 0)
		results.push_back(right);
	if (top.width() != 0 && top.height() != 0)
		results.push_back(top);
	if (bottom.width() != 0 && bottom.height() != 0)
		results.push_back(bottom);
}

template<class T>
std::vector<std::pair<T, T>> allPairs(const std::vector<T>& v) {
	std::vector<std::pair<T, T>> result;
	for (size_t i = 0; i < v.size(); i++) {
		for (size_t j = i + 1; j < v.size(); j++) {
			result.push_back({ v[i], v[j] });
		}
	}
	return result;
}

void sb::MaxRects::initializeContext(Context* c, const Vec2& freeRectangle, FitPolicy FitPolicy) {
	c->totalArea = freeRectangle.area();
	c->packedArea = 0.0;
	c->fp = FitPolicy;

	c->freeRectangles.clear();
	c->freeRectangles.insert(newRect(Vec2::zero, freeRectangle));

	c->packedRectangles.clear();
}

std::vector<Rect>& sb::MaxRects::getPackedRectangles(Context& ctx) {
	return ctx.packedRectangles;
}

Option<Rect> sb::MaxRects::tryPack(const Vec2& r, Context& c) {
	std::vector<Rect> splitted;
	if ((c.packedArea + r.area()) > c.totalArea)
		return nullptr;

	Option<Rect> previousF = nullptr;

	for(auto& freeF : c.freeRectangles) {
		//Fits?
		if (freeF.containsRect(newRect(freeF.bottomLeft(), r))) {
			if (previousF && isBestFit(c.fp, freeF, previousF.value(), r))
				previousF = freeF;
			else if (!previousF)
				previousF = freeF;
		}
	}

	if (!previousF) //rectangle can't be packed
		return nullptr;
	else {
		auto packed = newRect(previousF->bottomLeft(), r);
		c.packedRectangles.push_back(packed);
		c.freeRectangles.erase(previousF.value());
		split(previousF.value(), packed, splitted);
		for(auto& sr : splitted)
			c.freeRectangles.insert(sr);
		std::vector<Rect> frCopy(c.freeRectangles.begin(), c.freeRectangles.end());
		for(auto& fr : frCopy) {
			split(fr, packed, splitted);
			if (splitted.size() != 0) {
				c.freeRectangles.erase(fr);
				for(auto& sr : splitted)
					c.freeRectangles.insert(sr);
			}
		}
		frCopy = std::vector<Rect>(c.freeRectangles.begin(), c.freeRectangles.end());
		auto pairs = allPairs(frCopy);
		for (auto& p : pairs) {
			if (p.first.containsRect(p.second)) {
				c.freeRectangles.erase(p.second);
			}
			else if (p.second.containsRect(p.first)) {
				c.freeRectangles.erase(p.first);
			}
		}
	}
	return newRect(previousF->bottomLeft(), r);
}
