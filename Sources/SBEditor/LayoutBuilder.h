/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace sb {
	class DXContext;
	class VertexItemDescription;

	class LayoutBuilder {
	public:
		//Constructor
		LayoutBuilder(const DXContext* ctx);
		//Creation
		ID3D11InputLayout* get(const VertexItemDescription* model, const VertexItemDescription* instance) const;
		//Release
		void releaseAll();
	private:
		void createLayout(const VertexItemDescription* model, const VertexItemDescription* instance) const;
	private:
		const DXContext* m_ctx;
		mutable std::map<std::pair<const VertexItemDescription*, const VertexItemDescription*>, Microsoft::WRL::ComPtr<ID3D11InputLayout>> m_layouts;
	};
}

