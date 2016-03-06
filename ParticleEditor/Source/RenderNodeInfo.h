#pragma once
#include <DirectXMath.h>

//These classes are used to let both the renderer and the gameplay objects know what is to be rendered without knowing about D3D11
struct PerLocation {
	DirectX::XMFLOAT4X4 mWorld;
};
struct PerType {
	unsigned int vs;
	unsigned int ps;
	unsigned int layout;
	unsigned int vertexBuffer;
	unsigned int vertexSize;
	unsigned int indexBuffer;
	unsigned int indexFormat;
	unsigned int indexCount;
};
struct PerObject :public PerLocation{
	PerType tRenderType;
};
struct PerFrame :public PerLocation {
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProjection;
};
