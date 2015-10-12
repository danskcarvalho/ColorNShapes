/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "CppUnitTest.h"
#include "Matrix3x3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sb;

namespace SBTest
{
	TEST_CLASS(Matrix3x3Tests) {

		TEST_METHOD(testTransformations) {
			auto t = Matrix3x3::fromTranslation(1, 2);
			Assert::IsTrue(t.transformedVector(Vec2::zero) == Vec2::zero, L"Translation Failed.");
			Assert::IsTrue(t.transformedPoint(Vec2::zero) == Vec2(1, 2), L"Translation Failed.");
			auto s = Matrix3x3::fromScale(2, 3);
			Assert::IsTrue(s.transformedVector(Vec2(7, 5)) == Vec2(14, 15), L"Scale Failed");
			auto r = Matrix3x3::fromRotation(SbToRadians(90));
			Assert::IsTrue(aeq(r.transformedVector(Vec2::right), Vec2::up), L"Rotation Failed");
		}

		TEST_METHOD(testInverse) {
			auto r = Matrix3x3::fromRotation(SbToRadians(45));
			auto i = r.inverse();
			auto m = r * i;
			Assert::IsTrue(m.isIdentity(), L"m is not identity.");
		}

		TEST_METHOD(testAccessors) {
			auto m = Matrix3x3::identity;
			m.setTranslation(1, 2);
			m.setScale(2, 2);
			m.setRotation(SbToRadians(90));
			auto pt = m.transformedPoint(Vec2(1, 0));
			auto expectedPt = Vec2(1, 4);
			Assert::IsTrue(aeq(pt, expectedPt), L"Accessors failed.");
		}

		TEST_METHOD(testTranspose) {
			auto m = Matrix3x3::identity;
			Assert::IsTrue(m.transposed() == Matrix3x3::identity, L"Transpose failed.");
			m = Matrix3x3::fromRotation(SbToRadians(45));
			auto r = m * m.transposed();
			Assert::IsTrue(aeq(r, Matrix3x3::identity), L"Transpose failed.");
		}

	};
}