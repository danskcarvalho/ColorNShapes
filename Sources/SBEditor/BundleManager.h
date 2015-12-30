/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace sb {
	class DXContext;
	class BundleLoader;
	class BundleManager {
	public:
		BundleManager(const DXContext* context);
		virtual ~BundleManager();

		const BundleLoader* bundle(size_t id) const;
		void load(size_t id);
		void unload(size_t id);
	protected:
		void addBundle(size_t id, BundleLoader* loader) const;
		virtual void loadBundles() const = 0;
	private:
		mutable std::unordered_map<size_t, BundleLoader*> m_loaders;
		const DXContext* m_context;
		mutable bool m_isDescribing;
	};
}
