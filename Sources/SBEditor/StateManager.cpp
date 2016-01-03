/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#include "pch.h"
#include "StateManager.h"
#include "DXContext.h"
#include "BundleManager.h"
#include "BundleLoader.h"
#include "Option.h"
#include "Color.h"
#include "SamplerCache.h"
#include "BlendStateCache.h"
#include "RenderTargetCache.h"
#include "ConstantBufferCache.h"

using namespace sb;

namespace {
	class _ProgramId {
	public:
		size_t bundleId;
		size_t programId;
	};
	bool operator==(const _ProgramId& p1, const _ProgramId& p2) {
		return p1.bundleId == p2.bundleId && p1.programId == p2.programId;
	}
	bool operator!=(const _ProgramId& p1, const _ProgramId& p2) {
		return p1.bundleId != p2.bundleId || p1.programId != p2.programId;
	}
	class _TextureId {
	public:
		size_t bundleId;
		size_t textureId;
		RenderTargetInterfaces interfaces; //necessary if texture is a rendertarget too...
	};
	bool operator==(const _TextureId& t1, const _TextureId& t2) {
		return t1.bundleId == t2.bundleId && t1.textureId == t2.textureId;
	}
	bool operator!=(const _TextureId& t1, const _TextureId& t2) {
		return t1.bundleId != t2.bundleId || t1.textureId != t2.textureId;
	}
	class _RenderTarget {
	public:
		size_t textureId;
		RenderTargetInterfaces interfaces;
	};
	bool operator==(const _RenderTarget& r1, const _RenderTarget& r2) {
		return r1.textureId == r2.textureId;
	}
	bool operator!=(const _RenderTarget& r1, const _RenderTarget& r2) {
		return r1.textureId != r2.textureId;
	}
	class _ConstantBuffer {
	public:
		ID3D11Buffer* buffer;
		size_t id;
		size_t size;
	};
	bool operator==(const _ConstantBuffer& c1, const _ConstantBuffer& c2) {
		return c1.id == c2.id;
	}
	bool operator!=(const _ConstantBuffer& c1, const _ConstantBuffer& c2) {
		return c1.id != c2.id;
	}
	class RenderingState {
	public:
		std::unordered_map<size_t, Sampler> vsSamplers;
		std::unordered_map<size_t, Sampler> psSamplers;
		std::unordered_map<size_t, _TextureId> vsTextures;
		std::unordered_map<size_t, _TextureId> psTextures;
		Option<Blending> blending;
		Option<_ConstantBuffer> vsConstantBuffer;
		Option<_ConstantBuffer> psConstantBuffer;
		Option<_ProgramId> programId;
		Option<_RenderTarget> renderTarget;
		std::unordered_map<size_t, RenderTargetInterfaces> lockedRenderTargets;
		std::unordered_set<size_t> lockedConstantBuffers;
	};
}

class StateManager_Implementation {
public:
	StateManager_Implementation(DXContext* context, BundleManager* bundleManager) {
		m_context = context;
		m_bundleManager = bundleManager;

		m_blendStateCache = new DefaultBlendStateCache(m_context);
		m_constantBufferCache = new DefaultConstantBufferCache(m_context);
		m_renderTargetCache = new DefaultRenderTargetCache(m_context);
		m_samplerCache = new DefaultSamplerCache(m_context);
	}
	~StateManager_Implementation() {
		delete m_blendStateCache;
		delete m_constantBufferCache;
		delete m_renderTargetCache;
		delete m_samplerCache;
	}
	const DXContext* context() const {
		return m_context;
	}
	const BundleManager* bundleManager() const {
		return m_bundleManager;
	}
	BundleManager* bundleManager() {
		return m_bundleManager;
	}

	void pushState() {
		m_savedStates.push_back(m_currentState);
		m_currentState.lockedRenderTargets.clear();
		m_currentState.lockedConstantBuffers.clear();
		m_currentState.vsConstantBuffer = nullptr;
		m_currentState.psConstantBuffer = nullptr;
	}
	void popState() {
		assert(m_savedStates.size() > 0);
		for (auto& kvp : m_currentState.lockedRenderTargets)
			m_renderTargetCache->unlock(kvp.first);

		for (auto& id : m_currentState.lockedConstantBuffers)
			m_constantBufferCache->unlock(id);

		m_currentState = m_savedStates.back();
		m_savedStates.pop_back();
	}
	void commit() {
		assert(m_savedStates.size() > 0);

		//RENDER TARGET
		commitRenderTarget();

		//BLEND
		commitBlending();

		//SET Shaders
		commitProgram();

		//SET Constant Buffers
		commitConstantBuffers();

		//SET Samplers
		commitSamplers();

		//SET Textures
		commitTextures();
	}

	size_t lockRenderTarget(size_t width, size_t height) {
		assert(m_savedStates.size() > 0); //must have called pushState before

		size_t id;
		auto rt = m_renderTargetCache->get(RenderTargetConfiguration(width, height), &id);
		m_currentState.lockedRenderTargets.insert({id, rt});
		return id;
	}

	void setProgram(size_t bundleId, size_t programId) {
		assert(m_savedStates.size() > 0); //must have called pushState before

		m_currentState.programId = { bundleId, programId };
	}
	void setBlending(Blending b) {
		assert(m_savedStates.size() > 0); //must have called pushState before

		m_currentState.blending = b;
	}
	void setRenderTarget(size_t id) {
		assert(m_savedStates.size() > 0); //must have called pushState before
		assert(m_currentState.lockedRenderTargets.count(id));

		m_currentState.renderTarget = {id, m_currentState.lockedRenderTargets.at(id)};
	}
	void setVSSampler(const Sampler& sampler, size_t slot = 0) {
		assert(m_savedStates.size() > 0); //must have called pushState before

		m_currentState.vsSamplers[slot] = sampler;
	}
	void setPSSampler(const Sampler& sampler, size_t slot = 0) {
		assert(m_savedStates.size() > 0); //must have called pushState before

		m_currentState.psSamplers[slot] = sampler;
	}
	void setVSTexture(size_t bundleId, size_t textureId, size_t slot = 0) {
		assert(m_savedStates.size() > 0); //must have called pushState before
		if (bundleId == SbRenderTargetBundle)
			assert(m_currentState.lockedRenderTargets.count(textureId));

		m_currentState.vsTextures[slot] = {
			bundleId, 
			textureId, 
			bundleId == SbRenderTargetBundle ? m_currentState.lockedRenderTargets.at(textureId) : RenderTargetInterfaces()};
	}
	void setPSTexture(size_t bundleId, size_t textureId, size_t slot = 0) {
		assert(m_savedStates.size() > 0); //must have called pushState before
		if (bundleId == SbRenderTargetBundle)
			assert(m_currentState.lockedRenderTargets.count(textureId));

		m_currentState.psTextures[slot] = {
			bundleId,
			textureId,
			bundleId == SbRenderTargetBundle ? m_currentState.lockedRenderTargets.at(textureId) : RenderTargetInterfaces() };
	}
	void setVSConstantBuffer(const void* pBuffer, size_t szBuffer) {
		assert(m_savedStates.size() > 0); //must have called pushState before
		assert(pBuffer);
		assert(szBuffer > 0);

		if (m_currentState.vsConstantBuffer && 
			m_currentState.vsConstantBuffer->size == szBuffer) {
			m_context->deviceContext()->UpdateSubresource(
				m_currentState.vsConstantBuffer->buffer,
				0,
				nullptr,
				pBuffer,
				0,
				0);
			return;
		}

		size_t id = 0;
		auto buffer = m_constantBufferCache->get(ConstantBufferSize(szBuffer), &id);

		m_context->deviceContext()->UpdateSubresource(
			buffer,
			0,
			nullptr,
			pBuffer,
			0,
			0);

		m_currentState.vsConstantBuffer = { buffer, id, szBuffer };
		m_currentState.lockedConstantBuffers.insert(id);
	}
	void setPSConstantBuffer(const void* pBuffer, size_t szBuffer) {
		assert(m_savedStates.size() > 0); //must have called pushState before
		assert(pBuffer);
		assert(szBuffer > 0);

		if (m_currentState.psConstantBuffer && m_currentState.psConstantBuffer->size == szBuffer) {
			m_context->deviceContext()->UpdateSubresource(
				m_currentState.psConstantBuffer->buffer,
				0,
				nullptr,
				pBuffer,
				0,
				0);
			return;
		}

		size_t id = 0;
		auto buffer = m_constantBufferCache->get(ConstantBufferSize(szBuffer), &id);

		m_context->deviceContext()->UpdateSubresource(
			buffer,
			0,
			nullptr,
			pBuffer,
			0,
			0);

		m_currentState.psConstantBuffer = { buffer, id, szBuffer };
		m_currentState.lockedConstantBuffers.insert(id);
	}
private:
	//Private methods
	void commitRenderTarget() {
		if (m_commitedState.renderTarget != m_currentState.renderTarget) {
			m_commitedState.renderTarget = m_currentState.renderTarget;
			if (m_commitedState.renderTarget)
				m_context->deviceContext()->OMSetRenderTargets(
					1,
					&m_commitedState.renderTarget->interfaces.renderTargetView,
					nullptr);
			else
				m_context->restoreDefaultRenderTarget();
		}
	}
	void commitBlending() {
		if (m_commitedState.blending != m_currentState.blending) {
			m_commitedState.blending = m_currentState.blending;
			ID3D11BlendState* bState = nullptr;
			if (m_commitedState.blending)
				bState = m_blendStateCache->get(*m_commitedState.blending);

			m_context->deviceContext()->OMSetBlendState(bState, nullptr, 0xFFFFFFFF);
		}
	}
	void commitProgram() {
		if (m_commitedState.programId != m_currentState.programId) {
			m_commitedState.programId = m_currentState.programId;
			if (m_commitedState.programId) {
				auto pShader = m_bundleManager
					->bundle(m_commitedState.programId->bundleId)
					->pixelShader(m_commitedState.programId->programId);
				auto vShader = m_bundleManager
					->bundle(m_commitedState.programId->bundleId)
					->vertexShader(m_commitedState.programId->programId);
				m_context->deviceContext()->VSSetShader(vShader, nullptr, 0);
				m_context->deviceContext()->PSSetShader(pShader, nullptr, 0);
			}
			else {
				m_context->deviceContext()->VSSetShader(nullptr, nullptr, 0);
				m_context->deviceContext()->PSSetShader(nullptr, nullptr, 0);
			}
		}
	}
	void commitConstantBuffers() {
		if (m_commitedState.vsConstantBuffer != m_currentState.vsConstantBuffer) {
			m_commitedState.vsConstantBuffer = m_currentState.vsConstantBuffer;
			if (m_commitedState.vsConstantBuffer)
				m_context->deviceContext()->VSSetConstantBuffers(0, 1, &m_commitedState.vsConstantBuffer->buffer);
			else {
				ID3D11Buffer* cBuffer = nullptr;
				m_context->deviceContext()->VSSetConstantBuffers(0, 1, &cBuffer);
			}
		}

		if (m_commitedState.psConstantBuffer != m_currentState.psConstantBuffer) {
			m_commitedState.psConstantBuffer = m_currentState.psConstantBuffer;
			if (m_commitedState.psConstantBuffer)
				m_context->deviceContext()->PSSetConstantBuffers(0, 1, &m_commitedState.psConstantBuffer->buffer);
			else {
				ID3D11Buffer* cBuffer = nullptr;
				m_context->deviceContext()->VSSetConstantBuffers(0, 1, &cBuffer);
			}
		}
	}
	void commitSamplers() {
		if (m_commitedState.vsSamplers != m_currentState.vsSamplers ||
			m_commitedState.psSamplers != m_currentState.psSamplers) {
			ID3D11SamplerState* samplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
			ZeroMemory(&samplers[0], sizeof(samplers));
			m_context->deviceContext()->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, samplers);
			m_context->deviceContext()->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, samplers);
		}
		if (m_commitedState.vsSamplers != m_currentState.vsSamplers) {
			m_commitedState.vsSamplers = m_currentState.vsSamplers;

			for (auto& kvp : m_commitedState.vsSamplers) {
				auto s = m_samplerCache->get(kvp.second);
				m_context->deviceContext()->VSSetSamplers((UINT)kvp.first, 1, &s);
			}
		}
		if (m_commitedState.psSamplers != m_currentState.psSamplers) {
			m_commitedState.psSamplers = m_currentState.psSamplers;

			for (auto& kvp : m_commitedState.psSamplers) {
				auto s = m_samplerCache->get(kvp.second);
				m_context->deviceContext()->PSSetSamplers((UINT)kvp.first, 1, &s);
			}
		}
	}
	void commitTextures() {
		if (m_commitedState.vsTextures != m_currentState.vsTextures ||
			m_commitedState.psTextures != m_currentState.psTextures) {
			ID3D11ShaderResourceView* views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
			ZeroMemory(&views[0], sizeof(views));
			m_context->deviceContext()->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, views);
			m_context->deviceContext()->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, views);
		}
		if (m_commitedState.vsTextures != m_currentState.vsTextures) {
			m_commitedState.vsTextures = m_currentState.vsTextures;

			for (auto& kvp : m_commitedState.vsTextures) {
				if (kvp.second.bundleId == SbRenderTargetBundle) {
					m_context->deviceContext()->VSSetShaderResources((UINT)kvp.first, 1, &kvp.second.interfaces.shaderResourceView);
				}
				else {
					auto t = m_bundleManager->bundle(kvp.second.bundleId)->textureView(kvp.second.textureId);
					m_context->deviceContext()->VSSetShaderResources((UINT)kvp.first, 1, &t);
				}
			}
		}
		if (m_commitedState.psTextures != m_currentState.psTextures) {
			m_commitedState.psTextures = m_currentState.psTextures;

			for (auto& kvp : m_commitedState.psTextures) {
				if (kvp.second.bundleId == SbRenderTargetBundle) {
					m_context->deviceContext()->PSSetShaderResources((UINT)kvp.first, 1, &kvp.second.interfaces.shaderResourceView);
				}
				else {
					auto t = m_bundleManager->bundle(kvp.second.bundleId)->textureView(kvp.second.textureId);
					m_context->deviceContext()->PSSetShaderResources((UINT)kvp.first, 1, &t);
				}
			}
		}
	}
	//Fields
	DXContext* m_context;
	DefaultBlendStateCache* m_blendStateCache;
	DefaultConstantBufferCache* m_constantBufferCache;
	DefaultRenderTargetCache* m_renderTargetCache;
	DefaultSamplerCache* m_samplerCache;
	BundleManager* m_bundleManager;
	std::vector<RenderingState> m_savedStates;
	RenderingState m_currentState;
	RenderingState m_commitedState;
};

sb::StateManager::StateManager(DXContext * context, BundleManager* manager) {
	m_impl = new StateManager_Implementation(context, manager);
}

sb::StateManager::~StateManager() {
	delete m_impl;
}

const DXContext * sb::StateManager::context() const {
	return m_impl->context();
}

const BundleManager * sb::StateManager::bundleManager() const {
	return m_impl->bundleManager();
}

BundleManager * sb::StateManager::bundleManager() {
	return m_impl->bundleManager();
}

void sb::StateManager::pushState() {
	m_impl->pushState();
}

void sb::StateManager::popState() {
	m_impl->popState();
}

void sb::StateManager::commit() {
	m_impl->commit();
}

size_t sb::StateManager::lockRenderTarget(size_t width, size_t height) {
	return m_impl->lockRenderTarget(width, height);
}

void sb::StateManager::setProgram(size_t bundleId, size_t programId) {
	m_impl->setProgram(bundleId, programId);
}

void sb::StateManager::setBlending(Blending b) {
	m_impl->setBlending(b);
}

void sb::StateManager::setRenderTarget(size_t id) {
	m_impl->setRenderTarget(id);
}

void sb::StateManager::setVSSampler(const Sampler & sampler, size_t slot) {
	m_impl->setVSSampler(sampler, slot);
}

void sb::StateManager::setPSSampler(const Sampler & sampler, size_t slot) {
	m_impl->setPSSampler(sampler, slot);
}

void sb::StateManager::setVSTexture(size_t bundleId, size_t textureId, size_t slot) {
	m_impl->setVSTexture(bundleId, textureId, slot);
}

void sb::StateManager::setPSTexture(size_t bundleId, size_t textureId, size_t slot) {
	m_impl->setPSTexture(bundleId, textureId, slot);
}

void sb::StateManager::setVSConstantBuffer(const void * pBuffer, size_t szBuffer) {
	m_impl->setVSConstantBuffer(pBuffer, szBuffer);
}

void sb::StateManager::setPSConstantBuffer(const void * pBuffer, size_t szBuffer) {
	m_impl->setPSConstantBuffer(pBuffer, szBuffer);
}
