#pragma once

#ifndef SbEpsilon
#define SbEpsilon 0.00005f
#endif

#ifndef SbPI
#define SbPI ((float)3.1415926535897932384626433832795028841971693993751058209749445923078164062)
#endif

#define SbToDegrees(x) (((float)(x) / (float)SbPI) * 180.0f)
#define SbToRadians(x) (((float)(x) / 180.0f) * (float)SbPI)

namespace sb {
	std::wstring fromString(const std::string& str);
	std::string toString(const std::wstring& str);

	inline bool aeq(float n1, float n2) {
		if (fabsf(n2 - n1) <= SbEpsilon)
			return true;
		if (fabsf(n2 - n1) <= SbEpsilon * fabsf(n2))
			return true;

		return false;
	}

	inline float clamp(float n, float min, float max) {
		return n < min ? min : (n > max ? max : n);
	}

	inline float sign(float n) {
		return n == 0 ? 0 : n / fabsf(n);
	}

	inline size_t bitwiseHash(const unsigned char *_First, size_t _Count) {
#if defined(_64BITS)
		static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
		const size_t _FNV_offset_basis = 14695981039346656037ULL;
		const size_t _FNV_prime = 1099511628211ULL;

#else
		static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
		const size_t _FNV_offset_basis = 2166136261U;
		const size_t _FNV_prime = 16777619U;
#endif

		size_t _Val = _FNV_offset_basis;
		for (size_t _Next = 0; _Next < _Count; ++_Next) {
			_Val ^= (size_t)_First[_Next];
			_Val *= _FNV_prime;
		}

#if defined(_64BITS)
		static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
		_Val ^= _Val >> 32;

#else
		static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif
		return (_Val);
	}
}
