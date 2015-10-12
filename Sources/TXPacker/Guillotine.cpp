/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "Guillotine.h"
#include "Vec2.h"

using namespace sb;

enum class split {
	horizontal,
	vertical
};

//Some Internal Methods
bool isBestFit(Guillotine::FitPolicy FitPolicy, const Rect& freeRectangle, const Rect& previousFreeRectangle, const Vec2& rectangle) {
	float a1 = 0, a2 = 0;
	switch (FitPolicy) {
		case Guillotine::FitPolicy::BAF:
			a1 = rectangle.area() / freeRectangle.area();
			a2 = rectangle.area() / previousFreeRectangle.area();
			return a1 > a2;
		case Guillotine::FitPolicy::BSSF:
			a1 = std::min(freeRectangle.width() - rectangle.width(), freeRectangle.height() - rectangle.height());
			a2 = std::min(previousFreeRectangle.width() - rectangle.width(), previousFreeRectangle.height() - rectangle.height());
			return a1 < a2;
		case Guillotine::FitPolicy::BLSF:
			a1 = std::max(freeRectangle.width() - rectangle.width(), freeRectangle.height() - rectangle.height());
			a2 = std::max(previousFreeRectangle.width() - rectangle.width(), previousFreeRectangle.height() - rectangle.height());
			return a1 < a2;
		default:
			throw "should never get here";
	}
}

split bestSplit(Guillotine::SplitPolicy SplitPolicy, const Rect& f, const Vec2& r) {
	float h1 = 0, v1 = 0, h2 = 0, v2 = 0;
	switch (SplitPolicy) {
		case Guillotine::SplitPolicy::SAS:
			return f.width() <= f.height() ? split::horizontal : split::vertical;
		case Guillotine::SplitPolicy::LAS:
			return f.width() >= f.height() ? split::horizontal : split::vertical;
		case Guillotine::SplitPolicy::SLAS:
			return (f.width() - r.width()) <= (f.height() - r.height()) ? split::horizontal : split::vertical;
		case Guillotine::SplitPolicy::LLAS:
			return (f.width() - r.width()) >= (f.height() - r.height()) ? split::horizontal : split::vertical;
		case Guillotine::SplitPolicy::MAXAS:
			h1 = (float)((f.width() - r.width()) * r.height()) / (float)(f.width() * (f.height() - r.height()));
			if (h1 > 1.0f)
				h1 = 1.0f / h1;
			v1 = (float)(r.width() * (f.height() - r.height())) / (float)((f.width() - r.width()) * f.height());
			if (v1 > 1.0f)
				v1 = 1.0f / v1;
			h1 = fabsf(h1 - 1.0f);
			v1 = fabsf(v1 - 1.0f);
			return h1 <= v1 ? split::horizontal : split::vertical;
		case Guillotine::SplitPolicy::MINAS:
			h2 = (float)((f.width() - r.width()) * r.height()) / (float)(f.width() * (f.height() - r.height()));
			if (h2 < 1.0f)
				h2 = 1.0f / h2;
			v2 = (float)(r.width() * (f.height() - r.height())) / (float)((f.width() - r.width()) * f.height());
			if (v2 < 1.0f)
				v2 = 1.0f / v2;
			return h2 >= v2 ? split::horizontal : split::vertical;
		default:
			throw "should never get here";
	}
}

static Rect newRect(const Vec2& bottomLeft, const Vec2& size) {
	return Rect(bottomLeft + size / 2.0f, size);
}

void performSplit(const Rect& freeRectangle, const Vec2& rectangle, split split, Rect& r1, Rect& r2) {
	if (split == split::horizontal) {
		r1 = newRect(Vec2(freeRectangle.left() + rectangle.width(), freeRectangle.bottom()),
						   Vec2(freeRectangle.width() - rectangle.width(), rectangle.height()));
		r2 = newRect(Vec2(freeRectangle.left(), freeRectangle.bottom() + rectangle.height()),
						   Vec2(freeRectangle.width(), freeRectangle.height() - rectangle.height()));
	}
	else {
		r1 = newRect(Vec2(freeRectangle.left(), freeRectangle.bottom() + rectangle.height()),
						   Vec2(rectangle.width(), freeRectangle.height() - rectangle.height()));
		r2 = newRect(Vec2(freeRectangle.left() + rectangle.width(), freeRectangle.bottom()),
						   Vec2(freeRectangle.width() - rectangle.width(), freeRectangle.height()));
	}
}

void sb::Guillotine::initializeContext(sb::Guillotine::Context* ctx, const Vec2& freeRectangle, FitPolicy FitPolicy, SplitPolicy SplitPolicy) {
	assert(ctx);
	ctx->totalArea = freeRectangle.area();
	ctx->packedArea = 0;
	ctx->FitPolicy = FitPolicy;
	ctx->SplitPolicy = SplitPolicy;
	ctx->freeRectangles.clear();
	ctx->freeRectangles.insert(newRect(Vec2::zero, freeRectangle));
	ctx->packedRectangles.clear();
}

std::vector<Rect>& sb::Guillotine::getPackedRectangles(sb::Guillotine::Context& ctx) {
	return ctx.packedRectangles;
}

Option<Rect> sb::Guillotine::tryPack(const Vec2& r, sb::Guillotine::Context& c) {
	if ((c.packedArea + r.area()) > c.totalArea)
		return nullptr;

	Option<Rect> previousF = nullptr;

	for(auto& freeF : c.freeRectangles) {
		//Fits?
		if (freeF.containsRect(newRect(freeF.bottomLeft(), r))) {
			if (previousF && isBestFit(c.FitPolicy, freeF, previousF.value(), r))
				previousF = freeF;
			else if (!previousF)
				previousF = freeF;
		}
	}

	if (previousF == nullptr) //rectangle can't be packed
		return nullptr;
	else {
		c.packedRectangles.push_back(newRect(previousF.value().bottomLeft(), r));
		c.freeRectangles.erase(previousF.value());
		Rect r1, r2;
		performSplit(previousF.value(), r, bestSplit(c.SplitPolicy, previousF.value(), r), r1, r2);
		if (r1.area() != 0.0)
			c.freeRectangles.insert(r1);
		if (r2.area() != 0.0)
			c.freeRectangles.insert(r2);
		c.packedArea += r.area();
		return newRect(previousF.value().bottomLeft(), r);
	}
}
