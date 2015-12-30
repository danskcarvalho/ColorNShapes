/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace Concurrency {
	template<class _ReturnType>
	class task;
}

namespace sb {
	typedef Concurrency::task<void> ResourceLoaderTask;
	class ResourceLoader {
	public:
		ResourceLoader();
		virtual ~ResourceLoader();

		virtual ResourceLoaderTask load() = 0;
		virtual void unload() = 0;
		virtual bool hasLoaded() const = 0;
		virtual bool isLoading() const = 0;
	};
}

