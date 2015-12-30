/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include <ppltasks.h>
#include "BundleManager.h"
#include "DXContext.h"
#include "BundleLoader.h"

using namespace sb;

BundleManager::BundleManager(const DXContext* context) : m_context(context) {
	assert(context);
	m_isDescribing = false;
}

BundleManager::~BundleManager() {
	for (auto& kvp : m_loaders)
		delete kvp.second;
}

const BundleLoader * sb::BundleManager::bundle(size_t id) const {
	if (m_loaders.size() == 0) {
		m_isDescribing = true;
		loadBundles();
		m_isDescribing = false;
	}
	assert(m_loaders.count(id));
	return m_loaders.at(id);
}

void sb::BundleManager::load(size_t id) {
	if (m_loaders.size() == 0) {
		m_isDescribing = true;
		loadBundles();
		m_isDescribing = false;
	}
	assert(m_loaders.count(id));
	m_loaders.at(id)->load();
}

void sb::BundleManager::unload(size_t id) {
	if (m_loaders.size() == 0) {
		m_isDescribing = true;
		loadBundles();
		m_isDescribing = false;
	}
	assert(m_loaders.count(id));
	m_loaders.at(id)->unload();
}

void sb::BundleManager::addBundle(size_t id, BundleLoader * loader) const {
	assert(m_isDescribing);
	assert(!m_loaders.count(id));
	assert(loader);
	m_loaders.insert({ id, loader });
}
