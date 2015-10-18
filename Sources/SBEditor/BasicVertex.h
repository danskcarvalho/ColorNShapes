/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "Vec2.h"
#include "Mesh.h"

#pragma once

namespace sb {
	class VertexItemDescription;

	class BasicVertex {
	public:
		float x, y;
		float u, v;
		
		inline Vec2 position() const {
			return Vec2(x, y);
		}
		inline void setPosition(float x, float y) {
			this->x = x;
			this->y = y;
		}
		inline void setPosition(const Vec2& xy) {
			this->x = xy.x;
			this->y = xy.y;
		}
		inline Vec2 texture() const {
			return Vec2(u, v);
		}
		inline void setTexture(float u, float v) {
			this->u = u;
			this->v = v;
		}
		inline void setTexture(const Vec2& uv) {
			this->u = uv.x;
			this->v = uv.y;
		}

		static const VertexItemDescription* description();
	};

	typedef Mesh<BasicVertex> BasicMesh;
}

