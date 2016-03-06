#include "./Texture2D.h"
#include "./DXLog.h"
//--------------------------------------------------------------------------------
#pragma region TEXTURE_2D
Texture2D::Texture2D(ID3D11Device* d3dDevice,
	int width, int height, const char * _name, DXGI_FORMAT format,
	UINT bindFlags,
	int mipLevels) {
	InternalConstruct(d3dDevice, width, height, format, bindFlags, mipLevels, 1, 1, 0, _name, D3D11_RTV_DIMENSION_TEXTURE2D, D3D11_UAV_DIMENSION_TEXTURE2D, D3D11_SRV_DIMENSION_TEXTURE2D);
}
//--------------------------------------------------------------------------------
Texture2D::Texture2D(ID3D11Device* d3dDevice,
	int width, int height, const char * _name, DXGI_FORMAT format,
	UINT bindFlags,
	const DXGI_SAMPLE_DESC& sampleDesc) {
	// UAV's can't point to multisampled resources
	InternalConstruct(d3dDevice, width, height, format, bindFlags, 1, 1, sampleDesc.Count, sampleDesc.Quality, _name, D3D11_RTV_DIMENSION_TEXTURE2DMS, D3D11_UAV_DIMENSION_UNKNOWN, D3D11_SRV_DIMENSION_TEXTURE2DMS);
}
//--------------------------------------------------------------------------------
Texture2D::Texture2D(ID3D11Device* d3dDevice,
	int width, int height, const char * _name, DXGI_FORMAT format,
	UINT bindFlags,
	int mipLevels, int arraySize) {
	InternalConstruct(d3dDevice, width, height, format, bindFlags, mipLevels, arraySize, 1, 0, _name, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, D3D11_UAV_DIMENSION_TEXTURE2DARRAY, D3D11_SRV_DIMENSION_TEXTURE2DARRAY);
}
//--------------------------------------------------------------------------------
Texture2D::Texture2D(ID3D11Device* d3dDevice, ID3D11Texture2D* _texture, const char *_debugName) {
	mTexture = _texture;
	std::string name = _debugName;
	name += "Texture";
	DXName(mTexture, name.c_str());

	ID3D11RenderTargetView* renderTargetView = nullptr;
	DXCall(d3dDevice->CreateRenderTargetView(mTexture, nullptr, &renderTargetView));
	name = _debugName;
	name += " RTV";
	DXName(renderTargetView, name.c_str());
	mRenderTargetElements.push_back(renderTargetView);
}
//--------------------------------------------------------------------------------
void Texture2D::InternalConstruct(ID3D11Device* d3dDevice,
	int width, int height, DXGI_FORMAT format,
	UINT bindFlags, int mipLevels, int arraySize,
	int sampleCount, int sampleQuality, const char * _name,
	D3D11_RTV_DIMENSION rtvDimension,
	D3D11_UAV_DIMENSION uavDimension,
	D3D11_SRV_DIMENSION srvDimension) {
	// Initalize
	mShaderResource = 0;

	std::string name = _name;
	CD3D11_TEXTURE2D_DESC desc(
		format,
		width, height, arraySize, mipLevels,
		bindFlags,
		D3D11_USAGE_DEFAULT, 0,
		sampleCount, sampleQuality,
		// If they request mipmap levels, it's nice to be able to autogenerate them.
		(mipLevels != 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0));

	DXCall(d3dDevice->CreateTexture2D(&desc, 0, &mTexture));
	name += " Texture";
	DXName(mTexture, name.c_str());

	// Update with actual mip levels, etc.
	mTexture->GetDesc(&desc);

	if (bindFlags & D3D11_BIND_RENDER_TARGET) {
		for (int i = 0; i < arraySize; ++i) {
			CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(
				rtvDimension,
				format,
				0,          // Mips
				i, 1        // Array
				);

			ID3D11RenderTargetView* renderTargetView = nullptr;
			DXCall(d3dDevice->CreateRenderTargetView(mTexture, &rtvDesc, &renderTargetView));
			name = _name;
			name += " RTV";
			DXName(renderTargetView, name.c_str());
			mRenderTargetElements.push_back(renderTargetView);
		}
	}

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
		// UAV's can't point to multisampled resources!
		//assert(uavDimension != D3D11_UAV_DIMENSION_UNKNOWN);

		for (int i = 0; i < arraySize; ++i) {
			CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc(
				uavDimension,
				format,
				0,          // Mips
				i, 1        // Array
				);

			ID3D11UnorderedAccessView* unorderedAccessView;
			DXCall(d3dDevice->CreateUnorderedAccessView(mTexture, &uavDesc, &unorderedAccessView));
			mUnorderedAccessElements.push_back(unorderedAccessView);
			name = _name;
			name += " UAV";
			DXName(unorderedAccessView, name.c_str());
		}
	}

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
		// Whole resource
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(
			srvDimension,
			format,
			0, desc.MipLevels,  // Mips
			0, desc.ArraySize   // Array
			);

		DXCall(d3dDevice->CreateShaderResourceView(mTexture, &srvDesc, &mShaderResource));
		name = _name;
		name += " SR";
		DXName(mShaderResource, name.c_str());

		char buffer[32];
		// Single elements
		for (int i = 0; i < arraySize; ++i)
		{
			CD3D11_SHADER_RESOURCE_VIEW_DESC srvElementDesc(
				srvDimension,
				format,
				0, 1,  // Mips
				i, 1   // Array
				);

			ID3D11ShaderResourceView* shaderResourceView = nullptr;
			DXCall(d3dDevice->CreateShaderResourceView(mTexture, &srvElementDesc, &shaderResourceView));
			name = _name;
			name += " SRV ";
			name += _itoa_s(i, buffer, 10);
			DXName(shaderResourceView, name.c_str());
			mShaderResourceElements.push_back(shaderResourceView);
		}
	}
}
//--------------------------------------------------------------------------------
Texture2D::~Texture2D() {
	Release();
}
//--------------------------------------------------------------------------------
int Texture2D::Release() {
	for (std::size_t i = 0; i < mRenderTargetElements.size(); ++i) {
		SAFE_RELEASE(mRenderTargetElements[i]);
	}
	for (std::size_t i = 0; i < mUnorderedAccessElements.size(); ++i) {
		SAFE_RELEASE(mUnorderedAccessElements[i]);
	}
	for (std::size_t i = 0; i < mShaderResourceElements.size(); ++i) {
		SAFE_RELEASE(mShaderResourceElements[i]);
	}

	SAFE_RELEASE(mShaderResource);
	SAFE_RELEASE(mTexture);

	return 0;
}
#pragma endregion //Texture2D functions
//--------------------------------------------------------------------------------
#pragma region DEPTH2D
//--------------------------------------------------------------------------------
Depth2D::Depth2D(ID3D11Device* d3dDevice,
	int width, int height, const char * _name,
	UINT bindFlags,
	bool stencil) {
	InternalConstruct(d3dDevice, width, height, bindFlags, 1, 1, 0, _name, D3D11_DSV_DIMENSION_TEXTURE2D, D3D11_SRV_DIMENSION_TEXTURE2D, stencil);
}
//--------------------------------------------------------------------------------
Depth2D::Depth2D(ID3D11Device* d3dDevice,
	int width, int height, const char * _name,
	UINT bindFlags,
	const DXGI_SAMPLE_DESC& sampleDesc,
	bool stencil) {
	InternalConstruct(d3dDevice, width, height, bindFlags, 1, sampleDesc.Count, sampleDesc.Quality, _name, D3D11_DSV_DIMENSION_TEXTURE2DMS, D3D11_SRV_DIMENSION_TEXTURE2DMS, stencil);
}
//--------------------------------------------------------------------------------
Depth2D::Depth2D(ID3D11Device* d3dDevice,
	int width, int height, const char * _name,
	UINT bindFlags,
	int arraySize,
	bool stencil) {
	InternalConstruct(d3dDevice, width, height, bindFlags, arraySize, 1, 0, _name, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, stencil);
}
//--------------------------------------------------------------------------------
Depth2D::Depth2D(ID3D11Device* d3dDevice,
	int width, int height, const char * _name,
	UINT bindFlags,
	int arraySize,
	const DXGI_SAMPLE_DESC& sampleDesc,
	bool stencil) {
	InternalConstruct(d3dDevice, width, height, bindFlags, arraySize, sampleDesc.Count, sampleDesc.Quality, _name, D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY, D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY, stencil);
}
//--------------------------------------------------------------------------------
void Depth2D::InternalConstruct(ID3D11Device* d3dDevice,
	int width, int height,
	UINT bindFlags, int arraySize,
	int sampleCount, int sampleQuality, const char * _name,
	D3D11_DSV_DIMENSION dsvDimension,
	D3D11_SRV_DIMENSION srvDimension,
	bool stencil) {
	// Initalize
	mShaderResource = 0;

	CD3D11_TEXTURE2D_DESC desc(
		stencil ? DXGI_FORMAT_R32G8X24_TYPELESS : DXGI_FORMAT_R32_TYPELESS,
		width, height, arraySize, 1,
		bindFlags,
		D3D11_USAGE_DEFAULT, 0,
		sampleCount, sampleQuality);

	DXCall(d3dDevice->CreateTexture2D(&desc, 0, &mTexture));
	std::string name = _name;
	name += " Texture";
	DXName(mTexture, name.c_str());

	char buffer[32];
	if (bindFlags & D3D11_BIND_DEPTH_STENCIL) {
		for (int i = 0; i < arraySize; ++i) {
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc(
				dsvDimension,
				stencil ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT,
				0,          // Mips
				i, 1        // Array
				);

			ID3D11DepthStencilView* depthStencilView;
			DXCall(d3dDevice->CreateDepthStencilView(mTexture, &depthStencilDesc, &depthStencilView));
			mDepthStencilElements.push_back(depthStencilView);

			name = _name;
			name += " Stencil View";
			name += _itoa_s(i, buffer, 10);
			DXName(depthStencilView, name.c_str());
		}
	}

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
		CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc(
			srvDimension,
			stencil ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : DXGI_FORMAT_R32_FLOAT,
			0, 1,           // Mips
			0, arraySize    // Array
			);

		DXCall(d3dDevice->CreateShaderResourceView(mTexture, &shaderResourceDesc, &mShaderResource));

		std::string name = _name;
		name += " SRV";
		DXName(mShaderResource, name.c_str());
	}
}
//--------------------------------------------------------------------------------
Depth2D::~Depth2D() {
	Release();
}
//--------------------------------------------------------------------------------
int Depth2D::Release() {
	for (std::size_t i = 0; i < mDepthStencilElements.size(); ++i) {
		SAFE_RELEASE(mDepthStencilElements[i]);
	}
	SAFE_RELEASE(mShaderResource);
	SAFE_RELEASE(mTexture);

	return 0;
}
//--------------------------------------------------------------------------------
#pragma endregion //DEPTH2D functions

