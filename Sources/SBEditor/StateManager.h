/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

#define SbRenderTargetBundle ((size_t)-1)

class StateManager_Implementation;

namespace sb {
	class DXContext;
	class BundleManager;
	class Color;
	struct Sampler;
	template<class _Value>
	struct Option;
	enum class Blending;

	class StateManager {
	public:
		StateManager(DXContext* context, BundleManager* manager);
		~StateManager();

		const DXContext* context() const;
		const BundleManager* bundleManager() const;
		BundleManager* bundleManager();

		void pushState();
		void popState();
		void commit();

		size_t lockRenderTarget(size_t width, size_t height);
		void setProgram(size_t bundleId, size_t programId);
		void setBlending(Blending b);
		void setRenderTarget(size_t id);
		void setVSSampler(const Sampler& sampler, size_t slot = 0);
		void setPSSampler(const Sampler& sampler, size_t slot = 0);
		void setVSTexture(size_t bundleId, size_t textureId, size_t slot = 0);
		void setPSTexture(size_t bundleId, size_t textureId, size_t slot = 0);
		void setVSConstantBuffer(const void* pBuffer, size_t szBuffer);
		void setPSConstantBuffer(const void* pBuffer, size_t szBuffer);
	private:
		StateManager_Implementation* m_impl;
	};
}