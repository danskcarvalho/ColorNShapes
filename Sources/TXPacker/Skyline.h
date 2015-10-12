/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "Vec2.h"
#include "Rect.h"
#include "Guillotine.h"

#pragma once
namespace sb {
	class Skyline {
	public:
		//Types
		class Context {
		public:
			friend class Skyline;
		private:
			double totalArea;
			double packedArea;
			std::vector<Rect> packedRectangles;
			std::unordered_map<Rect, Guillotine::Context> guillotineContexts;
			std::list<Vec2> Skyline;
			Vec2 freeRectangle;
			bool wasteImprovement;
		};
		//Methods
		static void initializeContext(Context* ctx, const Vec2& freeRectangle, bool wasteImprovement);
		static Option<Rect> tryPack(const Vec2& r, Context& ctx);
		static std::vector<Rect>& getPackedRectangles(Context& ctx);
	};
}