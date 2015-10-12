/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "CppUnitTest.h"
#include "Vec2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sb;

namespace SBTest
{
	TEST_CLASS(Vec2Tests) {
	public:
		TEST_METHOD(testLength) {
			Vec2 v = Vec2::right;
			auto lenght = v.length();
			Assert::IsTrue(lenght == 1, L"Length is not 1.");
			v *= 2;
			lenght = v.length();
			Assert::IsTrue(lenght == 2, L"Length is not 2.");
		}

		TEST_METHOD(testCross) {
			Vec2 v1 = Vec2::right;
			Vec2 v2 = Vec2::up;
			Assert::IsTrue(cross(v1, v2) > 0, L"Cross is not greater than 0.");
			v1 = Vec2::right;
			v2 = Vec2::left;
			Assert::IsTrue(cross(v1, v2) == 0, L"Cross is not equal to 0.");
			v1 = Vec2::right;
			v2 = Vec2::down;
			Assert::IsTrue(cross(v1, v2) < 0, L"Cross is not equal to 0.");
		}

		TEST_METHOD(testMethods) {
			Vec2 v1 = Vec2::right;
			Vec2 v2 = Vec2::up;
			Assert::IsTrue(v1.isPerpendicularTo(v2), L"v1 is not perpendicular to v2.");
			v1 = Vec2::right;
			v2 = Vec2::right * 3;
			Assert::IsTrue(v1.isParallelTo(v2), L"v1 is not parallel to v2.");
			v1 = Vec2::right;
			v2 = Vec2::up;
			auto v3 = (v1 + v2) / 2;
			Assert::IsTrue(v3.isBetween(v1, v2), L"v3 is not between v1 and v2.");
			Assert::IsTrue(v3.isBetween(v2, v1), L"v3 is not between v2 and v1.");
			Assert::IsFalse((-v3).isBetween(v2, v1), L"v3 is between v1 and v2.");
			Assert::IsTrue(v1.angleBetween(v2, RotationDirection::ccw) == SbToRadians(90), L"Angle between v1 and v2 != 90");
			Assert::IsTrue(v1.angleBetween(v2, RotationDirection::cw) == SbToRadians(360 - 90), L"Angle between v1 and v2 != 270");
		}

		TEST_METHOD(testRotations) {
			Vec2 v1 = Vec2::right;
			Assert::IsTrue(v1.rotated90() == Vec2::up, L"Rotation by 90 failed.");
			auto v2 = v1.rotatedBy(SbPI);
			Assert::IsTrue(aeq(v2, Vec2::left), L"Rotation by 180 failed.");
		}
	};
}
