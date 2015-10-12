/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "Rect.h"
#pragma once

namespace sb {
	struct Vec2;
	struct Rect;
	template<class T>
	struct Option;

	class Guillotine {
	public:
		//Types
		enum class FitPolicy {
			BAF,
			BSSF,
			BLSF
		};
		enum class SplitPolicy {
			SAS,
			LAS,
			SLAS,
			LLAS,
			MAXAS,
			MINAS
		};
		class Context {
		public:
			friend class Guillotine;
		private:
			double totalArea;
			double packedArea;
			FitPolicy FitPolicy;
			SplitPolicy SplitPolicy;
			std::unordered_set<Rect> freeRectangles;
			std::vector<Rect> packedRectangles;
		};
		//Methods
		static void initializeContext(Context* ctx, const Vec2& freeRectangle, FitPolicy FitPolicy, SplitPolicy SplitPolicy);
		static Option<Rect> tryPack(const Vec2& r, Context& ctx);
		static std::vector<Rect>& getPackedRectangles(Context& ctx);
	};
}