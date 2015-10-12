/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Rect.h"
#include "WinPackerDelegate.h"

#include <Windows.h>
#include <Shlwapi.h>
#include <Gdiplus.h>
#include <Shlobj.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Gdiplus.lib")

#undef min
#undef max

using namespace sb;

class WinPackerDelegate_implementation : public PackerDelegate {
public:
	//Constructor
	WinPackerDelegate_implementation() {
		m_bitmap = nullptr;
		m_graphics = nullptr;

		Gdiplus::GdiplusStartupInput	gdiplusStartupInput;
		// Initialize GDI+.
		Gdiplus::GdiplusStartup(&m_gdiplusToken,
								&gdiplusStartupInput,
								NULL);
	}
	//Destructor
	virtual ~WinPackerDelegate_implementation() override {
		if (m_graphics)
			delete m_graphics;
		if (m_bitmap)
			delete m_bitmap;

		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}
	//Inherited from PackerDelegate
	virtual std::vector<PackableImage> allPackableImages() const override {
		std::vector<PackableImage> pis;
		auto fs = getAllImageFilenames();
		size_t i = 0;
		for (auto& f : fs) {
			PackableImage p;
			p.path = f;
			p.index = i;
			p.name = getTextureName(f);
			auto s = getImageSize(f);
			if (!s) {
				std::cout << "ERROR: could not determine image size: " << f << ": skipping!" << std::endl;
				continue;
			}
			p.size = s.value();
			setTextureOptions(f, p);
			pis.push_back(p);
			i++;
		}
		return pis;
	}
	virtual void beginImage(const std::string & name, const Vec2 & size) override {
		assert(!m_bitmap);
		assert(!m_graphics);

		std::string filename = name + ".png";
		auto pathname = std::unique_ptr<char>(new char[MAX_PATH]);
		PathCombineA(pathname.get(), m_output.c_str(), filename.c_str());
		m_imagePath = pathname.get();

		auto result = SHCreateDirectoryExA(NULL, m_output.c_str(), NULL);
		if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS) {
			std::cout << "Error creating directory " << m_output.c_str() << std::endl;
			throw "error...";
		}

		Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap((INT)size.x, (INT)size.y, PixelFormat32bppARGB);
		Gdiplus::Graphics *graphics = Gdiplus::Graphics::FromImage(bitmap);

		Gdiplus::SolidBrush solidBrush(Gdiplus::Color(0, 0, 0, 0));
		graphics->FillRectangle(&solidBrush, 0, 0, (INT)size.x, (INT)size.y);

		m_bitmap = bitmap;
		m_graphics = graphics;
	}
	virtual void packImage(const PackableImage & img, const Rect & frame, size_t extrusion) override {
		if (!m_graphics)
			return;

		wchar_t wpath[MAX_PATH];
		mbstowcs_s(NULL, wpath, img.path.c_str(), MAX_PATH - 1);
		auto imgRef = std::unique_ptr<Gdiplus::Image>(Gdiplus::Image::FromFile(wpath));
		auto textureBrush = std::unique_ptr<Gdiplus::TextureBrush>(new Gdiplus::TextureBrush(imgRef.get(), Gdiplus::WrapModeTile));

		textureBrush->TranslateTransform(frame.left(), frame.bottom());

		Rect newFrame = frame;
		newFrame.setSize(newFrame.size() + Vec2::one * 2 * (float)extrusion);

		m_graphics->FillRectangle(textureBrush.get(), newFrame.left(), newFrame.bottom(), newFrame.width(), newFrame.height());
		m_graphics->Flush();
	}
	virtual void endImage() override {
		if (!m_graphics)
			return;

		m_graphics->Flush();

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);

		wchar_t wpath[MAX_PATH];
		mbstowcs_s(NULL, wpath, m_imagePath.c_str(), MAX_PATH - 1);
		m_bitmap->Save(wpath, &pngClsid);

		delete m_graphics; m_graphics = nullptr;
		delete m_bitmap; m_bitmap = nullptr;
	}
	virtual void beginInputTextFile(const std::string & name) override {
		m_fileContents = "";
		std::unique_ptr<char> wkDir = std::unique_ptr<char>(new char[MAX_PATH]);
		GetCurrentDirectoryA(MAX_PATH, wkDir.get());

		auto pathname = std::unique_ptr<char>(new char[MAX_PATH]);
		PathCombineA(pathname.get(), wkDir.get(), name.c_str());
		
		m_filePath = pathname.get();
	}
	virtual void beginOutputTextFile(const std::string & name) override {
		m_fileContents = "";

		auto pathname = std::unique_ptr<char>(new char[MAX_PATH]);
		PathCombineA(pathname.get(), m_output.c_str(), name.c_str());
		
		m_filePath = pathname.get();
	}
	virtual void writeLine(const std::string & line) override {
		m_fileContents += line;
		m_fileContents += "\n";
	}
	virtual void endTextFile() override {
		if (m_filePath.size() == 0)
			return;

		std::ofstream out(m_filePath, std::ios_base::out | std::ios_base::trunc);
		if (out.bad() || out.fail()) {
			std::cout << "ERROR: error writing to " << m_filePath << "!" << std::endl;
			return;
		}
		out << m_fileContents;
		out.close();
	}
	virtual Option<std::string> optionsFile() const override {
		std::unique_ptr<char> wkDir = std::unique_ptr<char>(new char[MAX_PATH]);
		GetCurrentDirectoryA(MAX_PATH, wkDir.get());

		auto pathname = std::unique_ptr<char>(new char[MAX_PATH]);
		PathCombineA(pathname.get(), wkDir.get(), "config.txt");
		
		std::fstream file(pathname.get(), std::ios_base::in);

		if (file.bad())
			return nullptr;
		if (file.fail())
			return nullptr;

		std::string result;
		std::string str;
		while (std::getline(file, str)) {
			result += str;
			result += "\n";
		}
		return result;
	}
	virtual void log(const std::string & msg) const override {
		std::cout << msg << std::endl;
	}
	virtual void setOutputDir(const std::string & dir) override {
		std::unique_ptr<char> wkDir = std::unique_ptr<char>(new char[MAX_PATH]);
		GetCurrentDirectoryA(MAX_PATH, wkDir.get());

		auto pathname = std::unique_ptr<char>(new char[MAX_PATH]);
		PathCombineA(pathname.get(), wkDir.get(), dir.c_str());
		m_output = pathname.get();
	}
private:
	//Fields
	std::string m_output;
	std::string m_filePath;
	std::string m_imagePath;
	std::string m_fileContents;
	Gdiplus::Bitmap* m_bitmap;
	Gdiplus::Graphics* m_graphics;
	ULONG_PTR m_gdiplusToken;
	//Functions
	static std::vector<std::string> getAllImageFilenames() {
		std::unique_ptr<char> wkDir = std::unique_ptr<char>(new char[MAX_PATH]);
		GetCurrentDirectoryA(MAX_PATH, wkDir.get());

		auto allFilesDir = std::unique_ptr<char>(new char[MAX_PATH]);
		strcpy_s(allFilesDir.get(), MAX_PATH, wkDir.get());
		strcat_s(allFilesDir.get(), MAX_PATH, "\\*");

		WIN32_FIND_DATAA data;
		auto handle = FindFirstFileA(allFilesDir.get(), &data);
		if (handle == INVALID_HANDLE_VALUE)
			return std::vector<std::string>();

		std::vector<std::string> files;

		do {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			else {
				auto extension = PathFindExtensionA(data.cFileName);
				if (strcmp(extension, ".png") == 0) {
					auto pathname = std::unique_ptr<char>(new char[MAX_PATH]);
					PathCombineA(pathname.get(), wkDir.get(), data.cFileName);
					files.push_back(pathname.get());
				}
			}
		} while (FindNextFileA(handle, &data) != 0);

		FindClose(handle);
		return files;
	}
	static std::string getTextureName(const std::string& path) {
		std::vector<std::string> components;
		boost::split(components, path, boost::is_any_of("\\"));
		auto& filename = components.back();
		std::vector<std::string> parts;
		boost::split(parts, filename, boost::is_any_of("."), boost::token_compress_on);
		std::string result = "";
		for (size_t i = 0; i < parts.size(); i++) {
			if (i == (parts.size() - 1))
				continue;
			if (!boost::starts_with(parts[i], "-e") && !boost::starts_with(parts[i], "-b")) {
				if (result != "")
					result += ".";
				result += parts[i];
			}
		}
		return result;
	}
	static void setTextureOptions(const std::string& path, PackableImage& img) {
		std::vector<std::string> components;
		boost::split(components, path, boost::is_any_of("\\"));
		auto& filename = components.back();
		std::vector<std::string> parts;
		boost::split(parts, filename, boost::is_any_of("."), boost::token_compress_on);
		std::string result = "";
		for (size_t i = 0; i < parts.size(); i++) {
			if (boost::starts_with(parts[i], "-e")) {
				try {
					auto strnum = parts[i].substr(2);
					auto num = std::stoi(strnum);
					if (num >= 0)
						img.extrusion = num;
				}
				catch (...) {

				}
			}
			else if (boost::starts_with(parts[i], "-b")) {
				try {
					auto strnum = parts[i].substr(2);
					auto num = std::stoi(strnum);
					if (num >= 0)
						img.border = num;
				}
				catch (...) {

				}
			}
		}
	}
	static Option<Vec2> getImageSize(const std::string& path) {
		if (path.size() == 0)
			return nullptr;

		wchar_t wpath[MAX_PATH];
		mbstowcs_s(NULL, wpath, path.c_str(), MAX_PATH - 1);
		auto img = std::unique_ptr<Gdiplus::Image>(Gdiplus::Image::FromFile(wpath));

		if (!img)
			return nullptr;

		if (img->GetLastStatus() != Gdiplus::Ok)
			return nullptr;

		return Vec2((float)img->GetWidth(), (float)img->GetHeight());
	}
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j) {
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}
};

WinPackerDelegate * sb::WinPackerDelegate::create() {
	return new WinPackerDelegate();
}

sb::WinPackerDelegate::~WinPackerDelegate() {
	delete m_impl;
}

sb::WinPackerDelegate::WinPackerDelegate() {
	m_impl = new WinPackerDelegate_implementation();
}

std::vector<PackableImage> sb::WinPackerDelegate::allPackableImages() const {
	return m_impl->allPackableImages();
}

void sb::WinPackerDelegate::beginImage(const std::string & name, const Vec2 & size) {
	m_impl->beginImage(name, size);
}

void sb::WinPackerDelegate::packImage(const PackableImage & img, const Rect & frame, size_t extrusion) {
	m_impl->packImage(img, frame, extrusion);
}

void sb::WinPackerDelegate::endImage() {
	m_impl->endImage();
}

void sb::WinPackerDelegate::beginInputTextFile(const std::string & name) {
	m_impl->beginInputTextFile(name);
}

void sb::WinPackerDelegate::beginOutputTextFile(const std::string & name) {
	m_impl->beginOutputTextFile(name);
}

void sb::WinPackerDelegate::writeLine(const std::string & line) {
	m_impl->writeLine(line);
}

void sb::WinPackerDelegate::endTextFile() {
	m_impl->endTextFile();
}

Option<std::string> sb::WinPackerDelegate::optionsFile() const {
	return m_impl->optionsFile();
}

void sb::WinPackerDelegate::log(const std::string & msg) const {
	m_impl->log(msg);
}

void sb::WinPackerDelegate::setOutputDir(const std::string & dir) {
	m_impl->setOutputDir(dir);
}
