/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "CppUnitTest.h"
#include "SpatialTree.h"
#include "Shape.h"
#include "Rect.h"
#include "Ray.h"
#include "Matrix3x3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sb;

namespace sb {
	TEST_CLASS(SpatialTreeTests) {
		TEST_METHOD(testStaticTree) {
			Shape* a = Shape::fromRect(Rect(Vec2(1.5f, 1.5f), Vec2::one));
			Shape* b = Shape::fromRect(Rect(Vec2(-1.5f, 1.5f), Vec2::one));
			Shape* c = Shape::fromRect(Rect(Vec2(1.5f, -1.5f), Vec2::one));
			Shape* d = Shape::fromRect(Rect(Vec2(-1.5f, -1.5f), Vec2::one));

			SpatialTree* tree = SpatialTree::create();
			tree->addStaticNode(a);
			tree->addStaticNode(b);
			tree->addStaticNode(c);
			tree->addStaticNode(d);
			auto r = tree->rayCast(Ray(Vec2::zero, Vec2::one));
			Assert::IsTrue(!r.empty);
			Assert::IsTrue(r.intersected == a);
			r = tree->rayCast(Ray(Vec2::zero, -Vec2::one));
			Assert::IsTrue(!r.empty);
			Assert::IsTrue(r.intersected == d);
			r = tree->rayCast(Ray(Vec2::zero, Vec2::up));
			Assert::IsTrue(r.empty);

			RangeQueryResult q;
			tree->rangeQuery(&q, Rect(1, 1, 2, 6));
			Assert::IsTrue(q.count == 2);
			Assert::IsTrue(q.shapes[0] != b && q.shapes[0] != d);
			Assert::IsTrue(q.shapes[1] != b && q.shapes[1] != d);
			q.clear();

			tree->pickQuery(&q, Vec2(-1.6f, 1.6f));
			Assert::IsTrue(q.count == 1);
			Assert::IsTrue(q.shapes[0] == b);

			delete tree;
			delete a;
			delete b;
			delete c;
			delete d;
		}

		TEST_METHOD(testDynamicTree) {
			Shape* a = Shape::fromRect(Rect(Vec2(1.5f, 1.5f), Vec2::one));
			Shape* b = Shape::fromRect(Rect(Vec2(-1.5f, 1.5f), Vec2::one));
			Shape* c = Shape::fromRect(Rect(Vec2(1.5f, -1.5f), Vec2::one));
			Shape* d = Shape::fromRect(Rect(Vec2(-1.5f, -1.5f), Vec2::one));

			SpatialTree* tree = SpatialTree::create();
			tree->addDynamicNode(a);
			tree->addDynamicNode(b);
			tree->addDynamicNode(c);
			tree->addDynamicNode(d);
			auto r = tree->rayCast(Ray(Vec2::zero, Vec2::one));
			Assert::IsTrue(!r.empty);
			Assert::IsTrue(r.intersected == a);
			r = tree->rayCast(Ray(Vec2::zero, -Vec2::one));
			Assert::IsTrue(!r.empty);
			Assert::IsTrue(r.intersected == d);
			r = tree->rayCast(Ray(Vec2::zero, Vec2::up));
			Assert::IsTrue(r.empty);

			RangeQueryResult q;
			tree->rangeQuery(&q, Rect(1, 1, 2, 6));
			Assert::IsTrue(q.count == 2);
			Assert::IsTrue(q.shapes[0] != b && q.shapes[0] != d);
			Assert::IsTrue(q.shapes[1] != b && q.shapes[1] != d);
			q.clear();

			tree->pickQuery(&q, Vec2(-1.6f, 1.6f));
			Assert::IsTrue(q.count == 1);
			Assert::IsTrue(q.shapes[0] == b);

			//Apply transform
			auto m = Matrix3x3::fromTranslation(6, 6);
			tree->transform(a, m);
			r = tree->rayCast(Ray(Vec2::zero, Vec2::one));
			Assert::IsTrue(!r.empty);
			Assert::IsTrue(r.intersected == a);
			m = Matrix3x3::fromTranslation(6, 0);
			tree->transform(a, m);
			r = tree->rayCast(Ray(Vec2::zero, Vec2::one));
			Assert::IsTrue(r.empty);
			m = Matrix3x3::fromTranslation(0, 3);
			tree->transform(c, m);
			r = tree->rayCast(Ray(Vec2::zero, Vec2::one));
			Assert::IsTrue(!r.empty);
			Assert::IsTrue(r.intersected == c);
			//Test range
			q.clear();
			tree->rangeQuery(&q, Rect(1, 1, 2, 6));
			Assert::IsTrue(q.count == 1);
			Assert::IsTrue(q.shapes[0] == c);

			delete tree;
			delete a;
			delete b;
			delete c;
			delete d;
		}

	};
}
