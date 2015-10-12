/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once
#include "PackerDelegate.h"

class WinPackerDelegate_implementation;

namespace sb {
	class WinPackerDelegate : public PackerDelegate {
	public:
		//Creation
		static WinPackerDelegate* create();
		//Destruction
		virtual ~WinPackerDelegate() override;
		// Inherited via PackerDelegate
		virtual std::vector<PackableImage> allPackableImages() const override;
		virtual void beginImage(const std::string & name, const Vec2 & size) override;
		virtual void packImage(const PackableImage & img, const Rect & frame, size_t extrusion) override;
		virtual void endImage() override;
		virtual void beginInputTextFile(const std::string & name) override;
		virtual void beginOutputTextFile(const std::string & name) override;
		virtual void writeLine(const std::string & line) override;
		virtual void endTextFile() override;
		virtual Option<std::string> optionsFile() const override;
		virtual void log(const std::string & msg) const override;
		virtual void setOutputDir(const std::string & dir) override;
	private:
		WinPackerDelegate();
		WinPackerDelegate_implementation* m_impl;
	};
}

