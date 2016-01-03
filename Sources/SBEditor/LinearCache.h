/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

#define SbGenerateLinearCacheCtor(TypeName, Input, Output)						TypeName(const DXContext* ctx) : ::sb::LinearCache<_CacheSize, Input, Output>(ctx) { }
#define SbGenerateLinearCacheCtorWithMapper(TypeName, Input, Output, Mapper)	TypeName(const DXContext* ctx) : ::sb::LinearCache<_CacheSize, Input, Output, Mapper>(ctx) { }
#define SbDirectXMapperIsAFriend()												template<class _InputType, class _OutputType> friend class DirectXMapper

namespace sb {
	class DXContext;

	template<
		class _InputType,
		class _OutputType>
	class DirectXMapper;

	template<
		size_t _CacheSize,
		class _InputType,
		class _OutputType,
		class _MapperType = DirectXMapper<_InputType, _OutputType>>
	class LinearCache {
	public:
		enum { CacheSize = _CacheSize };
		typedef _InputType Input;
		typedef _OutputType Output;
		typedef _MapperType Mapper;
	public:
		LinearCache(const DXContext* context) : m_context(context) {
			assert(context);
			m_seed = 1;
		}
		~LinearCache() {
			releaseAll(true);
		}
		Output get(const Input& input, size_t* lock = nullptr) {
			for (size_t i = 0; i < m_mapped.size(); ++i) {
				if (!m_mapped[i].locked &&
					 m_mapped[i].input == input) {

					if (!m_mapped[i].initialized)
						 m_mapped[i].output = map(input);

					m_mapped[i].locked = lock != nullptr;
					if (lock)
						*lock = m_mapped[i].id;
					m_mapped[i].counter++;

					return m_mapped[i].output;
				}
			}

			// if not found
			auto _new = map(input);
			m_mapped.push_back({
				input,           //generator
				_new,            //result
				lock != nullptr, //is locked?
			    true,            //is initialized
				1,               //counter
				m_seed           //id
			});

			if (lock)
				*lock = m_seed;

			m_seed++;
			return _new;
		}
		void releaseAll(bool forced = false) {
			for (size_t i = 0; i < m_mapped.size(); ++i) {
				if (m_mapped[i].initialized && (!m_mapped[i].locked || forced)) {
					destroy(m_mapped[i].output);

					m_mapped[i].initialized = false;
					m_mapped[i].locked      = false;
				}
			}

			m_mapped.erase(
				std::remove_if(
					m_mapped.begin(), 
					m_mapped.end(),
					[](const Object& obj) {
						return !obj.initialized;
					}), 
				m_mapped.end());
		}
		void compress() {
			ptrdiff_t _initCount = std::count_if(
				m_mapped.begin(),
				m_mapped.end(),
				[](const Object& obj){
					return obj.initialized;
				});

			if (_initCount <= CacheSize)
				return;

			std::sort(m_mapped.begin(), m_mapped.end(), [](const auto& o1, const auto& o2) {
				return o1.counter < o2.counter;
			});

			for (size_t i = 0; i < m_mapped.size(); ++i) {
				if ( m_mapped[i].initialized &&
					!m_mapped[i].locked) {

					destroy(m_mapped[i].output);

					m_mapped[i].initialized = false;
					m_mapped[i].locked = false;

					_initCount--;

					if (_initCount <= CacheSize)
						return;
				}
			}
		}
		void unlock(size_t id) {
			for (size_t i = 0; i < m_mapped.size(); ++i) {
				if (m_mapped[i].id == id)
					m_mapped[i].locked = false;
			}
		}
	private:
		Output map(const Input& input) {
			const Mapper _m;
			return _m(input, m_context);
		}
		static void destroy(const Output& output) {
			const Mapper _m;
			_m.destroy(output);
		}
	private:
		struct Object {
			Input                       input;
			Output                      output;
			bool                        locked;
			bool                        initialized;
			size_t                      counter;
			size_t                      id;
		};
		const DXContext*                m_context;
		std::vector<Object>             m_mapped;
		size_t                          m_seed;
	};

	template<
		class _InputType,
		class _OutputType>
	class DirectXMapper {
	public:
		_OutputType operator()(const _InputType& input, const DXContext* ctx) const {
			return input.build(ctx);
		}
		void destroy(_OutputType output) const {
			output->Release();
		}
	};
}
