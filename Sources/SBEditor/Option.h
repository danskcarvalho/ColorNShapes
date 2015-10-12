/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace sb {
	template<class _Data>
	union OptionData {
		typedef _Data Data;
		Data data;
		inline OptionData() {}
		inline ~OptionData() {}
	};

	template<class _Data>
	struct Option {
	public:
		/*Typedefs*/
		typedef _Data Data;
		/*Constructors*/
		inline Option() {
			m_hasValue = false;
		}
		inline Option(std::nullptr_t) {
			m_hasValue = false;
		}

		inline Option(const Data& data) {
			new (&m_data.data) Data(data);
			m_hasValue = true;
		}
		inline Option(Data&& data) {
			new (&m_data.data) Data(std::move(data));
			m_hasValue = true;
		}
		inline Option(const Option& other) {
			if (other.m_hasValue) {
				new (&m_data.data) Data(other.m_data.data);
				m_hasValue = true;
			}
			else {
				m_hasValue = false;
			}
		}
		inline Option(Option&& other) {
			if (other.m_hasValue) {
				new (&m_data.data) Data(std::move(other.m_data.data));
				other.m_data.data.~Data();
				other.m_hasValue = false;
				m_hasValue = true;
			}
			else {
				m_hasValue = false;
			}
		}
		/*Operator =*/
		inline Option& operator=(std::nullptr_t) {
			if (!m_hasValue)
				return *this;

			m_data.data.~Data();
			m_hasValue = false;

			return *this;
		}
		inline Option& operator=(const Data& data) {
			if (m_hasValue)
				m_data.data = data;
			else {
				new (&m_data.data) Data(data);
				m_hasValue = true;
			}

			return *this;
		}
		inline Option& operator=(Data&& data) {
			if (m_hasValue)
				m_data.data = std::move(data);
			else {
				new (&m_data.data) Data(std::move(data));
				m_hasValue = true;
			}
			return *this;
		}
		inline Option& operator=(const Option& other) {
			if (this == &other)
				return *this;

			if (other.m_hasValue) {
				if (m_hasValue)
					m_data.data = other.m_data.data;
				else {
					new (&m_data.data) Data(other.m_data.data);
					m_hasValue = true;
				}
			}
			else {
				if (m_hasValue) {
					m_data.data.~Data();
					m_hasValue = false;
				}
			}

			return *this;
		}
		inline Option& operator=(Option&& other) {
			if (this == &other)
				return *this;

			if (other.m_hasValue) {
				if (m_hasValue)
					m_data.data = std::move(other.m_data.data);
				else {
					new (&m_data.data) Data(std::move(other.m_data.data));
					m_hasValue = true;
				}
				other = nullptr;
			}
			else {
				if (m_hasValue) {
					m_data.data.~Data();
					m_hasValue = false;
				}
			}

			return *this;
		}
		/*Methods*/
		inline operator bool() const {
			return m_hasValue;
		}
		inline bool hasValue() const {
			return m_hasValue;
		}
		inline Data& operator*() {
			assert(hasValue());
			return m_data.data;
		}
		inline const Data& operator*() const {
			assert(hasValue());
			return m_data.data;
		}
		inline Data& value() {
			assert(hasValue());
			return m_data.data;
		}
		inline const Data& value() const {
			assert(hasValue());
			return m_data.data;
		}
		inline Data* operator->() {
			assert(hasValue());
			return &m_data.data;
		}
		inline const Data* operator->() const {
			assert(hasValue());
			return &m_data.data;
		}
	private:
		OptionData<Data> m_data;
		bool m_hasValue;
	};

	template<class T>
	inline bool operator==(const Option<T>& o1, const Option<T>& o2) {
		if (!o1 && !o2)
			return true;
		if (!o1 || !o2)
			return false;
		return *o1 == *o2;
	}
	template<class T>
	inline bool operator!=(const Option<T>& o1, const Option<T>& o2) {
		if (!o1 && !o2)
			return false;
		if (!o1 || !o2)
			return true;
		return *o1 != *o2;
	}
	template<class T>
	inline bool operator==(const Option<T>& o1, const T& o2) {
		if (!o1)
			return false;
		return *o1 == o2;
	}
	template<class T>
	inline bool operator==(const T& o1, const Option<T>& o2) {
		if (!o2)
			return false;
		return o1 == *o2;
	}
	template<class T>
	inline bool operator!=(const Option<T>& o1, const T& o2) {
		if (!o1)
			return true;
		return *o1 != o2;
	}
	template<class T>
	inline bool operator!=(const T& o1, const Option<T>& o2) {
		if (!o2)
			return true;
		return o1 != *o2;
	}

	template<class T>
	inline bool operator==(const Option<T>& o1, const std::nullptr_t& o2) {
		if (!o1)
			return false;
		return !o1.hasValue();
	}
	template<class T>
	inline bool operator==(const std::nullptr_t& o1, const Option<T>& o2) {
		if (!o2)
			return false;
		return !o2.hasValue();
	}
	template<class T>
	inline bool operator!=(const Option<T>& o1, const std::nullptr_t& o2) {
		if (!o1)
			return true;
		return o1.hasValue();
	}
	template<class T>
	inline bool operator!=(const std::nullptr_t& o1, const Option<T>& o2) {
		if (!o2)
			return true;
		return o2.hasValue();
	}

	template<class T, class Y>
	inline bool operator==(const Option<T>& o1, const Option<Y>& o2) {
		if (!o1 && !o2)
			return true;
		if (!o1 || !o2)
			return false;
		return *o1 == *o2;
	}
	template<class T, class Y>
	inline bool operator==(const Option<Y>& o1, const Option<T>& o2) {
		if (!o1 && !o2)
			return true;
		if (!o1 || !o2)
			return false;
		return *o1 == *o2;
	}
	template<class T, class Y>
	inline bool operator!=(const Option<T>& o1, const Option<Y>& o2) {
		if (!o1 && !o2)
			return false;
		if (!o1 || !o2)
			return true;
		return *o1 != *o2;
	}
	template<class T, class Y>
	inline bool operator!=(const Option<Y>& o1, const Option<T>& o2) {
		if (!o1 && !o2)
			return false;
		if (!o1 || !o2)
			return true;
		return *o1 != *o2;
	}

	template<class T, class Y>
	inline bool operator==(const Option<T>& o1, const Y& o2) {
		if (!o1)
			return false;
		return *o1 == o2;
	}
	template<class T, class Y>
	inline bool operator==(const T& o1, const Option<Y>& o2) {
		if (!o2)
			return false;
		return o1 == *o2;
	}
	template<class T, class Y>
	inline bool operator!=(const Option<T>& o1, const Y& o2) {
		if (!o1)
			return true;
		return *o1 != o2;
	}
	template<class T, class Y>
	inline bool operator!=(const T& o1, const Option<Y>& o2) {
		if (!o2)
			return true;
		return o1 != *o2;
	}
}

namespace std {
	template<class T>
	class hash<sb::Option<T>> {
	public:
		size_t operator()(const sb::Option<T>& o) const {
			std::hash<T> hs;
			if (!o)
				return 0;
			return hs(*o);
		}
	};
}

