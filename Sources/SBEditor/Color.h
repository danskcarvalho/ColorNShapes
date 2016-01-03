/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace sb {
	class Color {
	public:
		Color();
		//builder methods
		static Color fromRGB(const float r, const float g, const float b);
		static Color fromRGBA(const float r, const float g, const float b, const float a);
		static Color fromPackedRGBA(uint32_t rgba);
		static Color fromHSL(const float h, const float s, const float l);
		static Color fromHSLA(const float h, const float s, const float l, const float a);
		static Color fromHSV(const float h, const float s, const float v);
		static Color fromHSVA(const float h, const float s, const float v, const float a);

		//inline properties
		inline Color rbg() const {
			return fromRGB(r(), g(), b());
		}
		inline float r() const {
			return m_r;
		}
		inline void setR(const float value) {
			m_r = value;
		}
		inline float g() const {
			return m_g;
		}
		inline void setG(const float value) {
			m_g = value;
		}
		inline float b() const {
			return m_b;
		}
		inline void setB(const float value) {
			m_b = value;
		}
		inline float a() const {
			return m_a;
		}
		inline void setA(const float value) {
			m_a = value;
		}
		inline bool isPremultiplied() const {
			return m_premultiplied;
		}
		//other properties
		float h() const;
		float sl() const;
		float sv() const;
		float l() const;
		float v() const;
		uint32_t packedRGBA() const;

		//operations
		void snap();
		void clamp();
		Color clamped() const;
		static Color lerp(const float f, const Color& c1, const Color& c2);
		Color preMultiplied() const;
		void preMultiply();
		Color nonPreMultiplied() const;
		void revertPreMultiply();

		//to_string
		std::string toString() const;

		//operators
		Color& operator +=(const Color&);
		Color& operator -=(const Color&);
		Color& operator *=(const Color&);
		Color& operator /=(const Color&);
		Color& operator *=(const float);
		Color& operator /=(const float);

		friend bool aeq(const Color&, const Color&);
		friend bool operator ==(const Color&, const Color&);
		friend bool operator !=(const Color&, const Color&);
		friend bool operator <(const Color&, const Color&);
		friend bool operator >(const Color&, const Color&);
		friend bool operator <=(const Color&, const Color&);
		friend bool operator >=(const Color&, const Color&);
		friend bool exact_match(const Color&, const Color&);
		friend Color operator +(const Color&, const Color&);
		friend Color operator -(const Color&, const Color&);
		friend Color operator *(const Color&, const Color&);
		friend Color operator /(const Color&, const Color&);
		friend Color operator *(const Color&, const float);
		friend Color operator *(const float, const Color&);
		friend Color operator /(const Color&, const float);
		friend Color operator -(const Color&);
	private:
		float m_r, m_g, m_b, m_a;
		bool m_premultiplied;

		void _fromHSL(const float h, const float s, const float l);
		void _fromHSV(const float h, const float s, const float v);
	};

	bool aeq(const Color&, const Color&);
	bool operator ==(const Color&, const Color&);
	bool operator !=(const Color&, const Color&);
	bool operator <(const Color&, const Color&);
	bool operator >(const Color&, const Color&);
	bool operator <=(const Color&, const Color&);
	bool operator >=(const Color&, const Color&);
	Color operator +(const Color&, const Color&);
	Color operator -(const Color&, const Color&);
	Color operator *(const Color&, const Color&);
	Color operator /(const Color&, const Color&);
	Color operator *(const Color&, const float);
	Color operator *(const float, const Color&);
	Color operator /(const Color&, const float);
	Color operator -(const Color&);
}

