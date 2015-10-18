/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace sb {
	class color {
	public:
		color();
		//builder methods
		static color fromRGB(const float r, const float g, const float b);
		static color fromRGBA(const float r, const float g, const float b, const float a);
		static color fromPackedRGBA(uint32_t rgba);
		static color fromHSL(const float h, const float s, const float l);
		static color fromHSLA(const float h, const float s, const float l, const float a);
		static color fromHSV(const float h, const float s, const float v);
		static color fromHSVA(const float h, const float s, const float v, const float a);

		//inline properties
		inline color rbg() const {
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
		color clamped() const;
		static color lerp(const float f, const color& c1, const color& c2);
		color preMultiplied() const;
		void preMultiply();
		color nonPreMultiplied() const;
		void revertPreMultiply();

		//to_string
		std::string toString() const;

		//operators
		color& operator +=(const color&);
		color& operator -=(const color&);
		color& operator *=(const color&);
		color& operator /=(const color&);
		color& operator *=(const float);
		color& operator /=(const float);

		friend bool aeq(const color&, const color&);
		friend bool operator ==(const color&, const color&);
		friend bool operator !=(const color&, const color&);
		friend bool operator <(const color&, const color&);
		friend bool operator >(const color&, const color&);
		friend bool operator <=(const color&, const color&);
		friend bool operator >=(const color&, const color&);
		friend bool exact_match(const color&, const color&);
		friend color operator +(const color&, const color&);
		friend color operator -(const color&, const color&);
		friend color operator *(const color&, const color&);
		friend color operator /(const color&, const color&);
		friend color operator *(const color&, const float);
		friend color operator *(const float, const color&);
		friend color operator /(const color&, const float);
		friend color operator -(const color&);
	private:
		float m_r, m_g, m_b, m_a;
		bool m_premultiplied;

		void _fromHSL(const float h, const float s, const float l);
		void _fromHSV(const float h, const float s, const float v);
	};

	bool aeq(const color&, const color&);
	bool operator ==(const color&, const color&);
	bool operator !=(const color&, const color&);
	bool operator <(const color&, const color&);
	bool operator >(const color&, const color&);
	bool operator <=(const color&, const color&);
	bool operator >=(const color&, const color&);
	color operator +(const color&, const color&);
	color operator -(const color&, const color&);
	color operator *(const color&, const color&);
	color operator /(const color&, const color&);
	color operator *(const color&, const float);
	color operator *(const float, const color&);
	color operator /(const color&, const float);
	color operator -(const color&);
}

