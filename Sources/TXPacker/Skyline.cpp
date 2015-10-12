/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "Skyline.h"
#include "Matrix3x3.h"

using namespace sb;

static Rect newRect(const Vec2& bottomLeft, const Vec2& size) {
	return Rect(bottomLeft + size / 2.0f, size);
}

std::list<Vec2> createSkyline(const Vec2& freeRectangle) {
	std::list<Vec2> Skyline;
	Skyline.push_back(Vec2(freeRectangle.width(), 0));
	return Skyline;
}

void findPlacesToPack(std::list<Vec2>& Skyline, float maxHeight, const Vec2& r, std::vector<std::list<Vec2>::iterator>& places) {
	places.clear();
	auto i = Skyline.begin();
	auto iend = Skyline.end();
	while (i != iend) {
		if (i->height() + r.height() > maxHeight) {
			i++;
			continue;
		}
		float width = 0;
		bool dontUpdateI = false;
		for (auto j = i; j != iend; j++) {
			if (j->height() > i->height()) {
				i = j;
				dontUpdateI = true; //to account for i = i.Next later...
				break;
			}
			width += j->width();
			if (width >= r.width())
				break;
		}
		if (width >= r.width())
			places.push_back(i);
		if (!dontUpdateI)
			i++;
	}
}

void computeLostAreas(std::list<Vec2>& Skyline, const Vec2& r, std::list<Vec2>::iterator place, std::vector<Rect>& lostAreas) {
	auto baseHeight = place->height();
	auto width = r.width();
	float x = 0;
	lostAreas.clear();

	//Initialize x
	auto first = Skyline.begin();
	auto last = Skyline.end();
	while (first != last && first != place) {
		x += first->width();
		first++;
	}

	while (place != last) {
		if (place->height() < baseHeight) {
			auto lostHeight = baseHeight - place->height();
			auto lostWidth = width >= place->width() ? place->width() : width;
			Rect lostRect = newRect(Vec2(x, place->height()), Vec2(lostWidth, lostHeight));
			if (lostRect.width() != 0 && lostRect.height() != 0)
				lostAreas.push_back(lostRect);
		}
		width -= place->width();
		x += place->width();
		place++;
		if (width <= 0)
			break;
	}
}

Rect packIntoPlace(std::list<Vec2>& Skyline, const Vec2& r, std::list<Vec2>::iterator place) {
	float x = 0;
	auto width = r.width();
	//Initialize x
	auto first = Skyline.begin();
	auto last = Skyline.end();
	while (first != last && first != place) {
		x += first->width();
		first++;
	}

	auto newHeight = place->height() + r.height();
	auto rectangle = newRect(Vec2(x, place->height()), r);

	while (place != last && width > 0) {
		if (width >= place->width()) {
			auto newPlace = Skyline.insert(std::next(place), Vec2(place->width(), newHeight));
			Skyline.erase(place);
			place = newPlace;
			width -= place->width();
		}
		else { //need to split skyline
			Skyline.insert(place, Vec2(width, newHeight));
			auto newPlace1 = Skyline.insert(std::next(place), Vec2(place->width() - width, place->height()));
			Skyline.erase(place);
			place = newPlace1;
			width = 0;
		}
		place++;
	}

	return rectangle;
}

Option<std::list<Vec2>::iterator> bestPlaceToPack(std::list<Vec2>& Skyline, float maxHeight, const Vec2& r, std::vector<Rect>& lostAreas) {
	lostAreas.clear();
	std::vector<std::list<Vec2>::iterator> places;
	findPlacesToPack(Skyline, maxHeight, r, places);
	Option<std::list<Vec2>::iterator> bestPlace = nullptr;
	std::vector<Rect> lostAreasRects;
	float lostAreaValue = std::numeric_limits<float>::max();
	for(auto& p : places) {
		computeLostAreas(Skyline, r, p, lostAreasRects);
		auto area = std::accumulate(lostAreasRects.begin(), lostAreasRects.end(), 0.0f, [](float a, const Rect& b) { return a + b.area(); });
		if (area < lostAreaValue) {
			area = lostAreaValue;
			bestPlace = p;
			lostAreas = lostAreasRects;
		}
		else if (area == lostAreaValue && p->height() < bestPlace.value()->height()) {
			bestPlace = p;
			lostAreas = lostAreasRects;
		}
	}
	return bestPlace;
}

void sb::Skyline::initializeContext(Context* c, const Vec2& freeRectangle, bool wasteImprovement) {
	c->totalArea = freeRectangle.area();
	c->packedArea = 0.0;

	c->guillotineContexts.clear();
	c->Skyline = createSkyline(freeRectangle);

	c->packedRectangles.clear();
	c->freeRectangle = freeRectangle;
	c->wasteImprovement = wasteImprovement;
}

std::vector<Rect>& sb::Skyline::getPackedRectangles(Context& ctx) {
	return ctx.packedRectangles;
}

Option<Rect> sb::Skyline::tryPack(const Vec2& r, Context& c) {
	Option<Rect> packedInWastedArea = nullptr;
	for(auto& gc : c.guillotineContexts) {
		auto gpacked = Guillotine::tryPack(r, gc.second);
		if (gpacked) {
			auto gpacked2 = gpacked.value();
			Matrix3x3::fromTranslation(gc.first.bottomLeft()).transformRect(&gpacked2);
			packedInWastedArea = gpacked2;
			c.packedRectangles.push_back(gpacked2);
		}
	}
	if (packedInWastedArea) {
		return packedInWastedArea;
	}

	if ((c.packedArea + r.area()) > c.totalArea)
		return nullptr;

	std::vector<Rect> lostAreas;
	auto bestPlace = bestPlaceToPack(c.Skyline, c.freeRectangle.height(), r, lostAreas);

	if (!bestPlace) //rectangle can't be packed
		return nullptr;
	else {
		auto packed = packIntoPlace(c.Skyline, r, bestPlace.value());
		c.packedRectangles.push_back(packed);
		if (lostAreas.size() != 0 && c.wasteImprovement) {
			for(auto& lostArea : lostAreas) {
				Guillotine::Context guiCtx;
				Guillotine::initializeContext(&guiCtx, lostArea.size(), Guillotine::FitPolicy::BSSF, Guillotine::SplitPolicy::MINAS);
				c.guillotineContexts.insert({ lostArea, guiCtx });
				c.packedArea += lostArea.area();
			}
		}
		c.packedArea += r.area();
		return packed;
	}
}
