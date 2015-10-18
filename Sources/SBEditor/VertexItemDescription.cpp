/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "VertexItemDescription.h"

using namespace sb;

const VertexItemDescription VertexItemDescription::endMarker = VertexItemDescription();

sb::VertexItemDescription::VertexItemDescription() {
	semanticName = nullptr;
	semanticIndex = 0;
	format = VertexItemFormat::UNKNOWN;
}

sb::VertexItemDescription::VertexItemDescription(const char * semanticName, size_t semanticIndex, VertexItemFormat format) {
	this->semanticName = semanticName;
	this->semanticIndex = semanticIndex;
	this->format = format;
}
