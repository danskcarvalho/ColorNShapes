/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "Packer.h"
#include "PackerDelegate.h"
#include "Guillotine.h"
#include "Skyline.h"
#include "MaxRects.h"

using namespace sb;

enum class sizeConstraint {
	fixed,
	minimum,
	powerOf2
};

enum class packingAlgorithm {
	MaxRects,
	Skyline,
	Guillotine
};

enum class rectangleOrdering {
	largestFirst,
	smallestFirst
};

struct packerOptions {
public:
	packingAlgorithm packingAlgorithm;
	Guillotine::FitPolicy guillotineFitPolicy;
	Guillotine::SplitPolicy guillotineSplitPolicy;
	MaxRects::FitPolicy maxRectsFitPolicy;
	rectangleOrdering rectangleOrdering;
	bool wasteImprovement;
	Vec2 maxSize;
	sizeConstraint sizeConstraint;
	size_t border;
	size_t extrusion;
    std::string outputName;
	//Constructor
	packerOptions() {
		packingAlgorithm = packingAlgorithm::MaxRects;
		guillotineFitPolicy = Guillotine::FitPolicy::BSSF;
		guillotineSplitPolicy = Guillotine::SplitPolicy::MINAS;
		maxRectsFitPolicy = MaxRects::FitPolicy::BSSF;
		rectangleOrdering = rectangleOrdering::largestFirst;
		wasteImprovement = true;
		maxSize = Vec2::one * 4 * 1024;
		sizeConstraint = sizeConstraint::minimum;
		border = 1;
		extrusion = 2;
        outputName = "texture";
	}
};

struct packedTexture {
	size_t index;
	std::unordered_map<Rect, PackableImage> packedImages;
};

class Packer_implementation {
public:
	Packer_implementation(PackerDelegate* delegate) {
		m_delegate = delegate;
	}
	void generateOptionsFile() const {
		m_delegate->log("generating config.txt");
		m_delegate->beginInputTextFile("config.txt");
		m_delegate->writeLine("packing-algorithm: max-rects|guillotine|skyline");
		m_delegate->writeLine("guillotine-fit: baf|bssf|blsf");
		m_delegate->writeLine("guillotine-split: sas|las|slas|llas|maxas|minas");
		m_delegate->writeLine("maxrects-fit: bl|baf|bssf|blsf");
		m_delegate->writeLine("rect-ordering: largest-first|smallest-first");
		m_delegate->writeLine("waste-improvement: true");
		m_delegate->writeLine("max-size: 4096");
		m_delegate->writeLine("size-constraint: fixed|minimum|powerof2");
		m_delegate->writeLine("border: 1");
		m_delegate->writeLine("extrusion: 2");
		m_delegate->writeLine("output-dir: output");
        m_delegate->writeLine("output-name: texture");
		m_delegate->endTextFile();
		m_delegate->log("generated config.txt successfully");
	}
	void pack() const {
		try {
			//Set Options
			m_delegate->log("reading options...");
            packerOptions options;
			m_delegate->setOutputDir("output");
			setOptions(options);

			//Packing Images
			m_delegate->log("packing images...");
			auto packed = internalPack(options);
			m_delegate->log("images packed");
			for (auto& t : packed) {
				m_delegate->log("creating " + options.outputName + std::to_string(t.index) + "...");
				m_delegate->beginImage(options.outputName + std::to_string(t.index), computeImageSize(options, t));
				for (auto& i : t.packedImages) {
					m_delegate->log("copying " + i.second.name + "...");
                    m_delegate->packImage(i.second, i.first, i.second.extrusion ? i.second.extrusion.value() : options.extrusion);
				}
				m_delegate->endImage();
				m_delegate->log("created " + options.outputName + std::to_string(t.index));

				//Generating Data
				m_delegate->log("creating " + options.outputName + std::to_string(t.index) + ".txt...");
				m_delegate->beginOutputTextFile(options.outputName + std::to_string(t.index) + ".txt");
				for (auto& i : t.packedImages) {
					m_delegate->writeLine(i.second.name + ":" + 
										  std::to_string((int)i.first.left()) + ":" + 
										  std::to_string((int)i.first.bottom()) + ":" +
										  std::to_string((int)i.first.width()) + ":" +
										  std::to_string((int)i.first.height()));
				}
				m_delegate->endTextFile();
				m_delegate->log("created " + options.outputName + std::to_string(t.index) + ".txt");
			}
			m_delegate->log("finished!");
		}
		catch (...) {
			m_delegate->log("error encountered: must stop!");
		}
	}
	//Destructor
	~Packer_implementation() {
		delete m_delegate;
	}
private:
	//Fields
	PackerDelegate* m_delegate;
	//Methods
	//Options
	void warn(const std::string& warning) const {
		m_delegate->log("WARNING: " + warning);
	}
	void setOption(packerOptions& options, const std::string& optionName, const std::string& optionValue) const {
		auto newOptionName = boost::to_lower_copy(optionName);
		boost::trim(newOptionName);
		auto newOptionValue = boost::to_lower_copy(optionValue);
		boost::trim(newOptionValue);
		if (newOptionName.size() == 0) {
			warn("empty option");
			return;
		}
		if (newOptionValue.size() == 0) {
			warn("empty value");
			return;
		}
		if (newOptionName == "packing-algorithm") {
			if (newOptionValue == "max-rects")
				options.packingAlgorithm = packingAlgorithm::MaxRects;
			else if (newOptionValue == "skyline")
				options.packingAlgorithm = packingAlgorithm::Skyline;
			else if (newOptionValue == "guillotine")
				options.packingAlgorithm = packingAlgorithm::Guillotine;
			else
				warn("unknown packing-algorithm: " + newOptionValue);
            m_delegate->log(newOptionName + ": " + newOptionValue);
		}
		else if (newOptionName == "guillotine-fit") {
			if (newOptionValue == "baf")
				options.guillotineFitPolicy = Guillotine::FitPolicy::BAF;
			else if (newOptionValue == "bssf")
				options.guillotineFitPolicy = Guillotine::FitPolicy::BSSF;
			else if (newOptionValue == "blsf")
				options.guillotineFitPolicy = Guillotine::FitPolicy::BLSF;
			else
				warn("unknown guillotine-fit: " + newOptionValue);
            m_delegate->log(newOptionName + ": " + newOptionValue);
		}
		else if (newOptionName == "guillotine-split") {
			if (newOptionValue == "sas")
				options.guillotineSplitPolicy = Guillotine::SplitPolicy::SAS;
			else if (newOptionValue == "las")
				options.guillotineSplitPolicy = Guillotine::SplitPolicy::LAS;
			else if (newOptionValue == "slas")
				options.guillotineSplitPolicy = Guillotine::SplitPolicy::SLAS;
			else if (newOptionValue == "llas")
				options.guillotineSplitPolicy = Guillotine::SplitPolicy::LLAS;
			else if (newOptionValue == "maxas")
				options.guillotineSplitPolicy = Guillotine::SplitPolicy::MAXAS;
			else if (newOptionValue == "minas")
				options.guillotineSplitPolicy = Guillotine::SplitPolicy::MINAS;
			else
				warn("unknown guillotine-split: " + newOptionValue);
            m_delegate->log(newOptionName + ": " + newOptionValue);
		}
		else if (newOptionName == "maxrects-fit") {
			if (newOptionValue == "bl")
				options.maxRectsFitPolicy = MaxRects::FitPolicy::BL;
			else if (newOptionValue == "baf")
				options.maxRectsFitPolicy = MaxRects::FitPolicy::BAF;
			else if (newOptionValue == "bssf")
				options.maxRectsFitPolicy = MaxRects::FitPolicy::BSSF;
			else if (newOptionValue == "blsf")
				options.maxRectsFitPolicy = MaxRects::FitPolicy::BLSF;
			else
				warn("unknown maxrecrs-fit: " + newOptionValue);
            m_delegate->log(newOptionName + ": " + newOptionValue);
		}
		else if (newOptionName == "rect-ordering") {
			if (newOptionValue == "largest-first")
				options.rectangleOrdering = rectangleOrdering::largestFirst;
			else if (newOptionValue == "smallest-first")
				options.rectangleOrdering = rectangleOrdering::smallestFirst;
			else
				warn("unknown rect-ordering: " + newOptionValue);
            m_delegate->log(newOptionName + ": " + newOptionValue);
		}
		else if (newOptionName == "waste-improvement") {
			if (newOptionValue == "true" || newOptionValue == "1")
				options.wasteImprovement = true;
			else if (newOptionValue == "false" || newOptionValue == "0")
				options.wasteImprovement = false;
			else
				warn("unknown boolean value: " + newOptionValue);
            m_delegate->log(newOptionName + ": " + (options.wasteImprovement ? "true" : "false"));
		}
		else if (newOptionName == "max-size") {
			try {
				auto wh = std::stoi(newOptionValue);
				if (wh < 32 || wh > 8192) {
					warn("invalid size: " + newOptionValue);
					return;
				}
				options.maxSize = Vec2((float)wh, (float)wh);
                m_delegate->log(newOptionName + ": " + std::to_string((int)options.maxSize.x));
			}
			catch (...) {
				warn("invalid integer: " + newOptionValue);
			}
		}
		else if (newOptionName == "size-constraint") {
			if (newOptionValue == "fixed")
				options.sizeConstraint = sizeConstraint::fixed;
			else if (newOptionValue == "minimum")
				options.sizeConstraint = sizeConstraint::minimum;
			else if (newOptionValue == "powerof2")
				options.sizeConstraint = sizeConstraint::powerOf2;
			else
				warn("unknown size-constraint: " + newOptionValue);
            m_delegate->log(newOptionName + ": " + newOptionValue);
		}
		else if (newOptionName == "border") {
			try {
				auto v = std::stoi(newOptionValue);
				if (v < 0 || v > 32) {
					warn("invalid border: " + newOptionValue);
					return;
				}
				options.border = v;
                m_delegate->log(newOptionName + ": " + std::to_string((int)options.border));
			}
			catch (...) {
				warn("invalid integer: " + newOptionValue);
			}
		}
		else if (newOptionName == "extrusion") {
			try {
				auto v = std::stoi(newOptionValue);
				if (v < 0 || v > 32) {
					warn("invalid extrusion: " + newOptionValue);
					return;
				}
				options.extrusion = v;
                m_delegate->log(newOptionName + ": " + std::to_string((int)options.extrusion));
			}
			catch (...) {
				warn("invalid integer: " + newOptionValue);
			}
		}
		else if (newOptionName == "output-dir") {
			m_delegate->setOutputDir(newOptionValue);
            m_delegate->log(newOptionName + ": " + newOptionValue);
		}
        else if (newOptionName == "output-name") {
            if (newOptionValue == ""){
                warn("empty output-name");
                return;
            }
            options.outputName = newOptionValue;
            m_delegate->log(newOptionName + ": " + newOptionValue);
        }
		else
			warn("unknown option " + newOptionName);
	}
	void setOption(packerOptions& options, const std::string& optionPair) const {
		std::vector<std::string> pair;
        std::string trimmed = boost::trim_copy(optionPair);
        if (trimmed.size() == 0)
            return;
		boost::split(pair, optionPair, boost::is_any_of(":"));
		if (pair.size() != 2) {
			warn("invalid option line: " + optionPair);
			return;
		}
		setOption(options, pair[0], pair[1]);
	}
	void setOptions(packerOptions& options) const {
		auto optionFile = m_delegate->optionsFile();
		if (!optionFile) {
			m_delegate->log("no options file: using default options");
			return;
		}
		std::vector<std::string> lines;
		boost::split(lines, optionFile.value(), boost::is_any_of("\n"));
		for (auto& l : lines)
			setOption(options, l);
	}
	//Packing
    static void addBorderExtrusion(const packerOptions& options, Vec2& imgSize, const PackableImage& pi){
        auto border = options.border;
        auto extrusion = options.extrusion;
        if (pi.border)
            border = pi.border.value();
        if (pi.extrusion)
            extrusion = pi.extrusion.value();
        imgSize += Vec2::one * 2 * (float)(border + extrusion);

    }
    static void addBorderExtrusion(const packerOptions& options, Rect& r, const PackableImage& pi){
        auto border = options.border;
        auto extrusion = options.extrusion;
        if (pi.border)
            border = pi.border.value();
        if (pi.extrusion)
            extrusion = pi.extrusion.value();
        r.setSize(r.size() + Vec2::one * 2 * (float)(border + extrusion));

    }
    static void removeBorderExtrusion(const packerOptions& options, Rect& r, const PackableImage& pi){
        auto border = options.border;
        auto extrusion = options.extrusion;
        if (pi.border)
            border = pi.border.value();
        if (pi.extrusion)
            extrusion = pi.extrusion.value();
        r.setSize(r.size() - Vec2::one * 2 * (float)(border + extrusion));
    }
	std::vector<packedTexture> internalPack(const packerOptions& options) const {
		auto up = m_delegate->allPackableImages();
		if (up.size() == 0) {
			m_delegate->log("no images to pack...");
			return std::vector<packedTexture>();
		}
		std::set<std::string> names;
		for (auto& p : up) {
			if (names.count(p.name)) {
				m_delegate->log("ERROR: non unique texture name: " + p.name);
				throw "error";
			}
			names.insert(p.name);
		}
		std::sort(up.begin(), up.end(), [&options](const PackableImage& a, const PackableImage& b) {
			return options.rectangleOrdering == rectangleOrdering::largestFirst ? a.size.area() > b.size.area() : a.size.area() < b.size.area();
		});
		std::vector<packedTexture> packed;
		std::list<PackableImage> unpacked(up.begin(), up.end());
		size_t texIndex = 0;
		while (unpacked.size() != 0) {
			auto it = unpacked.begin();
			auto end = unpacked.end();
			Guillotine::Context gc;
			MaxRects::Context mc;
			Skyline::Context sc;
			Guillotine::initializeContext(&gc, options.maxSize, options.guillotineFitPolicy, options.guillotineSplitPolicy);
			MaxRects::initializeContext(&mc, options.maxSize, options.maxRectsFitPolicy);
			Skyline::initializeContext(&sc, options.maxSize, options.wasteImprovement);
			packedTexture texture;
			texture.index = texIndex;
			while (it != end) {
				if (it->size.x > options.maxSize.x || it->size.y > options.maxSize.y) {
					warn("texture " + it->name + " cannot be packed due to its size: it will be ignored");
					it = unpacked.erase(it);
					continue;
				}
				m_delegate->log("packing " + it->name + "...");
				Option<Rect> packedRect;
				auto imgSize = it->size;
                addBorderExtrusion(options, imgSize, *it); //add border and extrusion
				if (options.packingAlgorithm == packingAlgorithm::Guillotine)
					packedRect = Guillotine::tryPack(imgSize, gc);
				else if (options.packingAlgorithm == packingAlgorithm::MaxRects)
					packedRect = MaxRects::tryPack(imgSize, mc);
				else
					packedRect = Skyline::tryPack(imgSize, sc);

				if (packedRect) {
					//remove border and extrusion
					auto newRect = packedRect.value();
					removeBorderExtrusion(options, newRect, *it); //remove border and extrusion
					texture.packedImages[newRect] = *it;
					it = unpacked.erase(it);
				}
				else {
					m_delegate->log("could not pack " + it->name + ": trying again later...");
					it++;
				}
			}
			packed.push_back(texture);
			texIndex++;
		}
		return packed;
	}
	//Image Size
	Vec2 computeImageSize(const packerOptions& options, const packedTexture& tx) const {
		if (options.sizeConstraint == sizeConstraint::fixed)
			return options.maxSize;
		else {
			Option<Rect> ur;
			for (auto & i : tx.packedImages) {
				auto r = i.first;
                addBorderExtrusion(options, r, i.second);
				if (ur)
					ur = getUnion(ur.value(), r);
				else
					ur = r;
			}
			if (options.sizeConstraint == sizeConstraint::minimum)
				return ur->size();
			else {
				Vec2 s = Vec2::one;
				while (s.x < ur->width() || s.y < ur->height())
					s *= 2;
				return s;
			}
		}
	}
};

sb::Packer::Packer(PackerDelegate* delegate) {
	m_impl = new Packer_implementation(delegate);
}

sb::Packer::~Packer() {
	delete m_impl;
}

void sb::Packer::generateOptionsFile() const {
	m_impl->generateOptionsFile();
}

void sb::Packer::pack() const {
	m_impl->pack();
}

sb::Packer* sb::Packer::create(PackerDelegate* delegate) {
	return new Packer(delegate);
}
