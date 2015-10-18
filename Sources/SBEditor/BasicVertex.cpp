/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "BasicVertex.h"
#include "VertexItemDescription.h"

using namespace sb;

namespace {
	static VertexItemDescription descriptions[] = {
		VertexItemDescription("POSITION", 0, VertexItemFormat::R32G32_FLOAT),
		VertexItemDescription("TEXTURE", 0, VertexItemFormat::R32G32_FLOAT),
		VertexItemDescription::endMarker
	};
}

const VertexItemDescription * sb::BasicVertex::description() {
	return descriptions;
}
