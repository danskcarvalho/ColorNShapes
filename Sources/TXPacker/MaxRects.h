/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "Rect.h"
#pragma once

namespace sb {
	class MaxRects {
	public:
		enum class FitPolicy {
			BL,
			BAF,
			BSSF,
			BLSF
		};
		class Context {
		public:
			friend class MaxRects;
		private:
			float totalArea;
			float packedArea;
			FitPolicy fp;
			std::unordered_set<Rect> freeRectangles;
			std::vector<Rect> packedRectangles;
		};
		//Methods
		static void initializeContext(Context* ctx, const Vec2& freeRectangle, FitPolicy FitPolicy);
		static Option<Rect> tryPack(const Vec2& r, Context& ctx);
		static std::vector<Rect>& getPackedRectangles(Context& ctx);
	};
}