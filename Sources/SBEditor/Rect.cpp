/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Rect.h"
#include "Polygon.h"

using namespace sb;
Rect Rect::unit = Rect(0, 0, 1, 1);

sb::Polygon sb::Rect::toPolygon() const {
	return Polygon({ bottomRight(), topRight(), topLeft(), bottomLeft() });
}