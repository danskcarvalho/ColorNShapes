/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

namespace sb {
	class DXContext {
	public:
		virtual ID3D11Device2* device() const = 0;
		virtual ID3D11DeviceContext2* deviceContext() const = 0;
		virtual const std::vector<byte>& currentShaderBytecode() const = 0;
		virtual ~DXContext();
	};
}

