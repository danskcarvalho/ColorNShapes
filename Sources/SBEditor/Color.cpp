/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Utils.h"
#include "Color.h"

using namespace sb;

void color::_fromHSL(const float h, const float s, const float l) {
	float _h = h;
	float _s = s;
	float _l = l;

	float _h_s = _h >= 0 ? 1.0f : -1.0f;

	if (fabsf(_h) > (2 * M_PI))
		_h = _h_s * (fabsf(_h) - floorf(fabsf(_h) / (2 * (float)M_PI)) * (2 * (float)M_PI));

	if (_h < 0)
		_h = 2 * (float)M_PI + _h;

	if (_s >= 1)
		_s = 1;
	else if (_s <= 0)
		_s = 0;

	if (_l >= 1)
		_l = 1;
	else if (_l <= 0)
		_l = 0;

	_h = SbToDegrees(_h) / 60.0f;
	float c = (1 - fabsf(2 * _l - 1)) * _s;
	float x = c * (1 - fabsf(fmodf(_h, 2) - 1));
	float _r, _g, _b;

	if (_h < 1) {
		_r = c;
		_g = x;
		_b = 0;
	}
	else if (_h < 2) {
		_r = x;
		_g = c;
		_b = 0;
	}
	else if (_h < 3) {
		_r = 0;
		_g = c;
		_b = x;
	}
	else if (_h < 4) {
		_r = 0;
		_g = x;
		_b = c;
	}
	else if (_h < 5) {
		_r = x;
		_g = 0;
		_b = c;
	}
	else if (_h < 6) {
		_r = c;
		_g = 0;
		_b = x;
	}
	else {
		_r = _g = _b = 0;
	}

	float m = _l - 0.5f * c;
	this->m_r = _r + m;
	this->m_g = _g + m;
	this->m_b = _b + m;
}
void color::_fromHSV(const float h, const float s, const float v) {
	float _h = h;
	float _s = s;
	float _v
		= v;

	float _h_s = _h >= 0 ? 1.0f : -1.0f;

	if (fabsf(_h) > (2 * (float)M_PI))
		_h = _h_s * (fabsf(_h) - floorf(fabsf(_h) / (2 * (float)M_PI)) * (2 * (float)M_PI));

	if (_h < 0)
		_h = 2 * (float)M_PI + _h;

	if (_s >= 1)
		_s = 1;
	else if (_s <= 0)
		_s = 0;

	if (_v >= 1)
		_v = 1;
	else if (_v <= 0)
		_v = 0;

	_h = SbToDegrees(_h) / 60.0f;
	float c = _v * _s;
	float x = c * (1 - fabsf(fmodf(_h, 2) - 1));
	float _r, _g, _b;

	if (_h < 1) {
		_r = c;
		_g = x;
		_b = 0;
	}
	else if (_h < 2) {
		_r = x;
		_g = c;
		_b = 0;
	}
	else if (_h < 3) {
		_r = 0;
		_g = c;
		_b = x;
	}
	else if (_h < 4) {
		_r = 0;
		_g = x;
		_b = c;
	}
	else if (_h < 5) {
		_r = x;
		_g = 0;
		_b = c;
	}
	else if (_h < 6) {
		_r = c;
		_g = 0;
		_b = x;
	}
	else {
		_r = _g = _b = 0;
	}

	float m = _v - c;
	this->m_r = _r + m;
	this->m_g = _g + m;
	this->m_b = _b + m;
}

color::color() {
	m_r = m_g = m_b = m_a = 0.0f;
	m_premultiplied = false;
}
color color::fromRGB(const float r, const float g, const float b) {
	color _c;

	_c.m_r = r;
	_c.m_g = g;
	_c.m_b = b;
	_c.m_a = 1.0f;
	_c.m_premultiplied = false;

	return _c;
}
color color::fromRGBA(const float r, const float g, const float b, const float a) {
	color _c;

	_c.m_r = r;
	_c.m_g = g;
	_c.m_b = b;
	_c.m_a = a;
	_c.m_premultiplied = false;

	return _c;
}
color color::fromPackedRGBA(uint32_t rgba) {
	uint8_t r = (rgba & 0xFF000000) >> 24;
	uint8_t g = (rgba & 0x00FF0000) >> 16;
	uint8_t b = (rgba & 0x0000FF00) >> 8;
	uint8_t a = (rgba & 0x000000FF);

	color _c;
	_c.m_r = (float)r / 255.0f;
	_c.m_g = (float)g / 255.0f;
	_c.m_b = (float)b / 255.0f;
	_c.m_a = (float)a / 255.0f;
	_c.m_premultiplied = false;
	return _c;
}
color color::fromHSL(const float h, const float s, const float l) {
	color _c;
	_c._fromHSL(h, s, l);
	_c.m_premultiplied = false;
	return _c;
}
color color::fromHSLA(const float h, const float s, const float l, const float a) {
	color _c;
	_c._fromHSL(h, s, l);
	_c.m_premultiplied = false;
	_c.m_a = a;
	return _c;
}
color color::fromHSV(const float h, const float s, const float v) {
	color _c;
	_c._fromHSV(h, s, v);
	_c.m_premultiplied = false;
	return _c;
}
color color::fromHSVA(const float h, const float s, const float v, const float a) {
	color _c;
	_c._fromHSV(h, s, v);
	_c.m_premultiplied = false;
	_c.m_a = a;
	return _c;
}

float color::h() const {
	float as = 0.5f * (2 * m_r - m_g - m_b);
	float bs = 0.5f * (sqrtf(3) * (m_g - m_b));
	float h = atan2f(bs, as);

	if (h < 0)
		h = 2 * (float)M_PI + h;

	return h;
}
float color::sl() const {
	auto M = fmaxf(fmaxf(m_r, m_g), m_b);
	auto m = fminf(fminf(m_r, m_g), m_b);
	float c = M - m;
	if (c == 0)
		return 0;
	else {
		auto _l = l();
		return c / (1 - fabsf(2 * _l - 1));
	}
}
float color::sv() const {
	auto M = fmaxf(fmaxf(m_r, m_g), m_b);
	auto m = fminf(fminf(m_r, m_g), m_b);
	float c = M - m;
	if (c == 0)
		return 0;
	else {
		auto _v = v();
		return c / _v;
	}
}
float color::l() const {
	auto M = fmaxf(fmaxf(m_r, m_g), m_b);
	auto m = fminf(fminf(m_r, m_g), m_b);
	return 0.5f * (M + m);
}
float color::v() const {
	return fmaxf(fmaxf(m_r, m_g), m_b);
}
uint32_t color::packedRGBA() const {
	auto _pr = (int32_t)(m_r * 255.0f) & 0xFF;
	auto _pg = (int32_t)(m_g * 255.0f) & 0xFF;
	auto _pb = (int32_t)(m_b * 255.0f) & 0xFF;
	auto _pa = (int32_t)(m_a * 255.0f) & 0xFF;
	return (_pr << 0) | (_pg << 8) | (_pb << 16) | (_pa << 24);
}

void color::snap() {
	if (aeq(m_r, 0))
		m_r = 0;
	if (aeq(m_r, 1))
		m_r = 1;

	if (aeq(m_g, 0))
		m_g = 0;
	if (aeq(m_g, 1))
		m_g = 1;

	if (aeq(m_b, 0))
		m_b = 0;
	if (aeq(m_b, 1))
		m_b = 1;

	if (aeq(m_a, 0))
		m_a = 0;
	if (aeq(m_a, 1))
		m_a = 1;
}

void color::clamp() {
	if (m_r <= 0)
		m_r = 0;
	else if (m_r >= 1)
		m_r = 1;

	if (m_g <= 0)
		m_g = 0;
	else if (m_g >= 1)
		m_g = 1;

	if (m_b <= 0)
		m_b = 0;
	else if (m_b >= 1)
		m_b = 1;

	if (m_a <= 0)
		m_a = 0;
	else if (m_a >= 1)
		m_a = 1;
}

color color::clamped() const {
	color _c = *this;
	_c.clamp();
	return _c;
}

color color::lerp(const float f, const color& c1, const color& c2) {
	return (1 - f) * c1 + f * c2;
}

color color::preMultiplied() const {
	if (m_premultiplied)
		return *this;

	auto c = color::fromRGBA(m_r * m_a, m_g * m_a, m_b * m_a, m_a);
	c.m_premultiplied = true;
	return c;
}
void color::preMultiply() {
	if (m_premultiplied)
		return;

	m_r *= m_a;
	m_g *= m_a;
	m_b *= m_a;
	m_premultiplied = true;
}
color color::nonPreMultiplied() const {
	if (!m_premultiplied)
		return *this;

	if (aeq(m_a, 0.0f)) {
		auto _c = *this;
		_c.m_premultiplied = false;
		return _c;
	}
	else {
		auto _c = *this;
		_c.m_r /= _c.m_a;
		_c.m_g /= _c.m_a;
		_c.m_b /= _c.m_a;
		_c.m_premultiplied = false;
		return _c;
	}
}
void color::revertPreMultiply() {
	if (!m_premultiplied)
		return;

	if (aeq(m_a, 0.0f)) {
		m_premultiplied = false;
	}
	else {
		m_r /= m_a;
		m_g /= m_a;
		m_b /= m_a;
		m_premultiplied = false;
	}
}

std::string color::toString() const {
	return std::string("(") + std::to_string(m_r) + std::string(", ") + std::to_string(m_g) + std::string(", ") + std::to_string(m_b) + std::string(", ") + std::to_string(m_a) + std::string(")");
}

color& color::operator +=(const color& c2) {
	m_r += c2.m_r;
	m_g += c2.m_g;
	m_b += c2.m_b;
	m_a += c2.m_a;
	return *this;
}
color& color::operator -=(const color& c2) {
	m_r -= c2.m_r;
	m_g -= c2.m_g;
	m_b -= c2.m_b;
	m_a -= c2.m_a;
	return *this;
}
color& color::operator *=(const color& c2) {
	m_r *= c2.m_r;
	m_g *= c2.m_g;
	m_b *= c2.m_b;
	m_a *= c2.m_a;
	return *this;
}
color& color::operator /=(const color& c2) {
	m_r /= c2.m_r;
	m_g /= c2.m_g;
	m_b /= c2.m_b;
	m_a /= c2.m_a;
	return *this;
}
color& color::operator *=(const float t) {
	m_r *= t;
	m_g *= t;
	m_b *= t;
	m_a *= t;
	return *this;
}
color& color::operator /=(const float t) {
	m_r /= t;
	m_g /= t;
	m_b /= t;
	m_a /= t;
	return *this;
}
bool sb::operator ==(const color& c1, const color& c2) {
	return (c1.m_r == c2.m_r) && (c1.m_g == c2.m_g) && (c1.m_b == c2.m_b) && (c1.m_a == c2.m_a);
}

bool sb::operator <(const color& c1, const color& c2) {
	if (c1.m_r == c2.m_r) {
		if (c1.m_g == c2.m_g) {
			if (c1.m_b == c2.m_b) {
				return c1.m_a < c2.m_a;
			}
			else
				return c1.m_b < c2.m_b;
		}
		else
			return c1.m_g < c2.m_g;
	}
	else
		return c1.m_r < c2.m_r;
}

bool sb::operator >(const color& c1, const color& c2) {
	if (c1.m_r == c2.m_r) {
		if (c1.m_g == c2.m_g) {
			if (c1.m_b == c2.m_b) {
				return c1.m_a > c2.m_a;
			}
			else
				return c1.m_b > c2.m_b;
		}
		else
			return c1.m_g > c2.m_g;
	}
	else
		return c1.m_r > c2.m_r;
}

bool sb::operator <=(const color& c1, const color& c2) {
	if (c1 == c2)
		return true;

	if (c1.m_r == c2.m_r) {
		if (c1.m_g == c2.m_g) {
			if (c1.m_b == c2.m_b) {
				return c1.m_a < c2.m_a;
			}
			else
				return c1.m_b < c2.m_b;
		}
		else
			return c1.m_g < c2.m_g;
	}
	else
		return c1.m_r < c2.m_r;
}

bool sb::operator >=(const color& c1, const color& c2) {
	if (c1 == c2)
		return true;

	if (c1.m_r == c2.m_r) {
		if (c1.m_g == c2.m_g) {
			if (c1.m_b == c2.m_b) {
				return c1.m_a > c2.m_a;
			}
			else
				return c1.m_b > c2.m_b;
		}
		else
			return c1.m_g > c2.m_g;
	}
	else
		return c1.m_r > c2.m_r;
}

bool sb::operator !=(const color& c1, const color& c2) {
	return (c1.m_r != c2.m_r) || (c1.m_g != c2.m_g) || (c1.m_b != c2.m_b) || (c1.m_a != c2.m_a);
}

bool sb::aeq(const sb::color &c1, const sb::color &c2) {
	return aeq(c1.m_r, c2.m_r) && aeq(c1.m_b, c2.m_b) && aeq(c1.m_g, c2.m_g) && aeq(c1.m_a, c2.m_a);
}

color sb::operator +(const color& c1, const color& c2) {
	color _c = c1;
	_c += c2;
	return _c;
}
color sb::operator -(const color& c1, const color& c2) {
	color _c = c1;
	_c -= c2;
	return _c;
}
color sb::operator *(const color& c1, const color& c2) {
	color _c = c1;
	_c *= c2;
	return _c;
}
color sb::operator /(const color& c1, const color& c2) {
	color _c = c1;
	_c /= c2;
	return _c;
}
color sb::operator *(const color& c1, const float t) {
	color _c = c1;
	_c *= t;
	return _c;
}
color sb::operator *(const float t, const color& c2) {
	color _c = c2;
	_c *= t;
	return _c;
}
color sb::operator /(const color& c1, const float t) {
	color _c = c1;
	_c /= t;
	return _c;
}
color sb::operator -(const color& c1) {
	return color::fromRGBA(-c1.m_r, -c1.m_g, -c1.m_b, -c1.m_a);
}