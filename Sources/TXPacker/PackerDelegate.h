/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "Vec2.h"
#include "Option.h"
#pragma once

namespace sb {
	struct Rect;
	struct PackableImage {
	public:
		size_t index;
		std::string name;
        std::string path;
		Vec2 size;
		Option<size_t> border;
		Option<size_t> extrusion;
	};
	class PackerDelegate {
	public:
		virtual std::vector<PackableImage> allPackableImages() const = 0;
		virtual void beginImage(const std::string& name, const Vec2& size) = 0;
		virtual void packImage(const PackableImage& img, const Rect& frame, size_t extrusion) = 0;
		virtual void endImage() = 0;
		virtual void beginInputTextFile(const std::string& name) = 0;
		virtual void beginOutputTextFile(const std::string& name) = 0;
		virtual void writeLine(const std::string& line) = 0;
		virtual void endTextFile() = 0;
		virtual Option<std::string> optionsFile() const = 0;
		virtual void log(const std::string& msg) const = 0;
		virtual void setOutputDir(const std::string& dir) = 0;
		virtual ~PackerDelegate() { }
	};
}
