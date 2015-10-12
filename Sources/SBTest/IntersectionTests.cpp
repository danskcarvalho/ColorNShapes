/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "CppUnitTest.h"
#include "Ray.h"
#include "Line.h"
#include "LineSegment.h"
#include "Rect.h"
#include "Circle.h"
#include "Polygon.h"
#include "Intersection.h"
#include "Option.h"
#include "Matrix3x3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sb;

namespace sb {
	TEST_CLASS(IntersectionTests) {

		TEST_METHOD(testRayIntersection) {
			Ray a = Ray(Vec2::zero, Vec2::right);
			Ray b = Ray(Vec2(1, 1), Vec2::down);
			auto result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(result == Vec2(1, 0));
			b = Ray(Vec2(1, 1), Vec2::up);
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.hasValue());
			b = Ray(Vec2(0, 1), Vec2::down);
			result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(result == Vec2::zero);
		}

		TEST_METHOD(testLineIntersection) {
			Line a = Line(Vec2::zero, Vec2::one);
			Line b = Line(Vec2(0, 1), Vec2(1, 0));
			auto result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(result == Vec2(0.5f, 0.5f));
			b = Line(Vec2(0, 2), Vec2(2, 2));
			result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(aeq(result.value(), Vec2(2, 2)));
			a = Line(Vec2::zero, Vec2(1, 0));
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.hasValue());
		}

		TEST_METHOD(testRayLineIntersection) {
			Ray a = Ray(Vec2::zero, Vec2::one);
			Line b = Line(Vec2(2, 0), Vec2(2, 2));
			auto result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(aeq(result.value(), Vec2(2, 2)));
			a.setDirection(-a.direction());
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.hasValue());
		}

		TEST_METHOD(testRayLineSegmentIntersection) {
			Ray a = Ray(Vec2::zero, Vec2::one);
			LineSegment b = LineSegment(Vec2(2, 0), Vec2(2, 4));
			auto result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(aeq(result.value(), Vec2(2, 2)));
			b = LineSegment(Vec2(2, 0), Vec2(2, 1));
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.hasValue());
			b = LineSegment(Vec2(2, 0), Vec2(2, 2));
			result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(aeq(result.value(), Vec2(2, 2)));
		}

		TEST_METHOD(testLineSegmentIntersection) {
			LineSegment a = LineSegment(Vec2::zero, Vec2(0, 4));
			LineSegment b = LineSegment(Vec2(-2, 2), Vec2(2, 2));
			auto result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(result == Vec2(0, 2));
			b = LineSegment(Vec2(-2, 4), Vec2(2, 4));
			result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(result == Vec2(0, 4));
			b = LineSegment(Vec2(-2, 4.5f), Vec2(2, 4.5f));
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.hasValue());
		}

		TEST_METHOD(testRayRectIntersection) {
			Ray a = Ray(Vec2(0, 0.5f), Vec2::right);
			Rect b = Rect(Vec2(1.5f, 0.5f), Vec2::one);
			auto result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(aeq(result.value(), Vec2(1.0f, 0.5f)));
			a.setDirection(Vec2(1, 1) - a.point());
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.hasValue());
			a = Ray(Vec2(0, 1.5f), Vec2(1, 1) - Vec2(0, 1.5f));
			result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(aeq(result.value(), Vec2::one));
		}

		TEST_METHOD(testRectIntersection) {
			Rect a = Rect(Vec2(0.5f, 0.5f), Vec2::one);
			Rect b = Rect(Vec2(1.5f, 0.5f), Vec2::one);
			auto result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(result.penetration == 0);
			Assert::IsTrue(result.normal == Vec2::left);
			auto testResult = Intersection::test(a, b);
			Assert::IsTrue(testResult);
			result = Intersection::get(b, a);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(result.penetration == 0);
			Assert::IsTrue(result.normal == Vec2::right);
			testResult = Intersection::test(b, a);
			Assert::IsTrue(testResult);
			b = Rect(Vec2(1.6f, 0.5f), Vec2::one);
			result = Intersection::get(a, b);
			Assert::IsTrue(result.empty);
			testResult = Intersection::test(a, b);
			Assert::IsTrue(!testResult);
			b = Rect(Vec2(1.4f, 0.3f), Vec2::one);
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(aeq(result.penetration, 0.1f));
			Assert::IsTrue(result.normal == Vec2::left);
			b = Rect(Vec2(0.5f, 0.5f), Vec2::one / 2.0f);
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
		}

		TEST_METHOD(testRayCircleIntersection) {
			Ray a = Ray(Vec2(0, 0.5f), Vec2::right);
			Circle b = Circle(Vec2(1.5f, 0.5f), 0.5f);
			auto result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
			Assert::IsTrue(aeq(result.value(), Vec2(1, 0.5f)));
			b.setCenter(b.center() + Vec2::up * 0.5f);
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.hasValue());
			b.setCenter(Vec2(0, 0.5f));
			b.setCenter(b.center() + Vec2::up * 0.4f);
			result = Intersection::get(a, b);
			Assert::IsTrue(result.hasValue());
		}

		TEST_METHOD(testRectCircleIntersection) {
			Rect a = Rect(Vec2(0.5f, 0.5f), Vec2::one);
			Circle b = Circle(Vec2(1.5f, 0.5f), 0.5f);
			auto result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(aeq(result.penetration, 0));
			b.setCenter(b.center() - 0.1f * Vec2::right);
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(aeq(result.penetration, 0.1f));
			b.setCenter(b.center() + 0.2f * Vec2::right);
			result = Intersection::get(a, b);
			Assert::IsTrue(result.empty);
			b.setCenter(Vec2(1.5f, 1.5f));
			b.setRadius(1);
			result = Intersection::get(a, b);
			auto penetration = b.radius() - distance(b.center(), a.topRight());
			auto normal = a.topRight() - b.center();
			normal.normalize();
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(aeq(penetration, result.penetration));
			Assert::IsTrue(aeq(normal, result.normal));
		}

		TEST_METHOD(testPolygonIntersection) {
			Polygon a = Rect(Vec2(0.5f, 0.5f), Vec2::one).toPolygon();
			Polygon b = Rect(Vec2(1.5f, 0.5f), Vec2::one).toPolygon();
			auto result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(result.penetration == 0);
			Assert::IsTrue(result.normal == Vec2::left);
			result = Intersection::get(b, a);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(result.penetration == 0);
			Assert::IsTrue(result.normal == Vec2::right);
			Polygon c = Matrix3x3::fromRotation(SbToRadians(45)).transformedPolygon(a);
			result = Intersection::get(c, b);
			Assert::IsTrue(result.empty);
			a = Matrix3x3::fromRotation(SbToRadians(-45)).transformedPolygon(a);
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
			a = Matrix3x3::fromTranslation(result.penetration * result.normal * 0.9999f).transformedPolygon(a);
			result = Intersection::get(a, b);
			Assert::IsTrue(!result.empty);
			Assert::IsTrue(aeq(result.penetration, 0));
		}

	};
}