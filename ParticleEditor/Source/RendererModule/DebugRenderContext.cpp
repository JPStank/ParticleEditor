/***********************************************
* Filename:           	DebugRenderContext.cpp
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/06/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Draw transform lines, unit grid,
					collider shapes and anything else
					that can be represented with lines
************************************************/
#include "DebugRenderContext.h"
#include "../AssetModule/Texture.h"
#include "RenderComponent.h"
#include "../LevelModule/Transform.h"

#include "Shaders/ColorVS.csh"
#include "Shaders/ColorPS.csh"

CDebugRenderContext::CDebugRenderContext()
{
	m_pd3dDevice = nullptr;
	m_pd3dDeviceContext = nullptr;
	m_pd3dInputLayout = nullptr;
	m_pd3dPixelShader = nullptr;
	m_pd3dVertexShader = nullptr;
}
CDebugRenderContext::~CDebugRenderContext()
{
#ifdef RENDER_GRID
	DestroyGrid();
#endif
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pd3dDeviceContext);
	SAFE_RELEASE(m_pd3dInputLayout);
	SAFE_RELEASE(m_pd3dPixelShader);
	SAFE_RELEASE(m_pd3dVertexShader);

	for each (CRenderObject* r in m_vDebugObjects)
	{
		SAFE_RELEASE(r->m_pd3dIndexBuffer);
		SAFE_RELEASE(r->m_pd3dVertexBuffer);
		SAFE_RELEASE(r->m_pd3dMatrixBuffer);
		delete r;
	}
	m_vDebugObjects.clear();
}
void CDebugRenderContext::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CTexture* defaultTexture)
{
	m_pd3dDevice = device;
	m_pd3dDeviceContext = deviceContext;

	CreateShadersAndInputLayout();

#ifdef RENDER_GRID
	CreateGrid(20, 20);
	AddRenderComponent(m_pobjGrid);
#endif
}
void CDebugRenderContext::Render(void)
{
	m_pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout);
	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, NULL);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, NULL);

	int numObjects = (int)m_vDebugObjects.size();

	DXDrawName(L"Debug Stuff");

	// draw all debug objects
	for (int i = 0; i < numObjects; ++i)
	{
		// map world matrix
		D3D11_MAPPED_SUBRESOURCE data;
		m_pd3dDeviceContext->Map(m_vDebugObjects[i]->m_pd3dMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &data);
		memcpy(data.pData, &m_vDebugObjects[i]->m_pobjOwner->GetTransform().GetWorldMatrix(), sizeof(float4x4));
		m_pd3dDeviceContext->Unmap(m_vDebugObjects[i]->m_pd3dMatrixBuffer, 0);

		m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_vDebugObjects[i]->m_pd3dMatrixBuffer);

		// vertex buffer
		UINT stride = sizeof(TColorVert);
		UINT offset = 0;
		m_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_vDebugObjects[i]->m_pd3dVertexBuffer, &stride, &offset);
		
		CRenderComponent* rend = (CRenderComponent*)m_vDebugObjects[i]->m_pobjOwner->GetComponent(IComponent::eRENDER);

		m_pd3dDeviceContext->Draw(6, 0);// not magic, just how many verts in 3 lines
	}

}
void CDebugRenderContext::AddRenderComponent(CObject* obj)
{
	// make sure oject has a render component
	CRenderComponent* rend = (CRenderComponent*)obj->GetComponent(IComponent::eRENDER);
	if (rend == nullptr)
	{
		Log("Object added to Debug RC without a RenderComponent");
		return;
	}

	// create the render object and link to the object
	CRenderObject* addMe = new CRenderObject();
	addMe->m_pobjOwner = obj;

	// create color verts
	TColorVert* tempVerts;
	int numVerts;
	{
		numVerts = 6;
		
		tempVerts = new TColorVert[numVerts];

		tempVerts[0].m_d3dColor = (float4)DirectX::Colors::Red;
		tempVerts[1].m_d3dColor = (float4)DirectX::Colors::Red;
		tempVerts[2].m_d3dColor = (float4)DirectX::Colors::Green;
		tempVerts[3].m_d3dColor = (float4)DirectX::Colors::Green;
		tempVerts[4].m_d3dColor = (float4)DirectX::Colors::Blue;
		tempVerts[5].m_d3dColor = (float4)DirectX::Colors::Blue;

		tempVerts[0].m_d3dPosition = float3(0.0f, 0.0f, 0.0f);
		tempVerts[1].m_d3dPosition = float3(1.0f, 0.0f, 0.0f);

		tempVerts[2].m_d3dPosition = float3(0.0f, 0.0f, 0.0f);
		tempVerts[3].m_d3dPosition = float3(0.0f, 1.0f, 0.0f);

		tempVerts[4].m_d3dPosition = float3(0.0f, 0.0f, 0.0f);
		tempVerts[5].m_d3dPosition = float3(0.0f, 0.0f, 1.0f);

	}

	// create vertex buffer
	D3D11_BUFFER_DESC vb;
	ZeroMemory(&vb, sizeof(vb));
	vb.ByteWidth = sizeof(TColorVert) * numVerts;
	vb.Usage = D3D11_USAGE_IMMUTABLE;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sub;
	ZeroMemory(&sub, sizeof(sub));
	sub.pSysMem = tempVerts;

	DXCall(m_pd3dDevice->CreateBuffer(&vb, &sub, &addMe->m_pd3dVertexBuffer));

	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	cb.ByteWidth = sizeof(float4x4);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ZeroMemory(&sub, sizeof(sub));
	sub.pSysMem = &obj->GetTransform().GetWorldMatrix();

	DXCall(m_pd3dDevice->CreateBuffer(&cb, &sub, &addMe->m_pd3dMatrixBuffer));

	delete[] tempVerts;

	m_vDebugObjects.push_back(addMe);

}
void CDebugRenderContext::RemoveRenderComponent(CObject* obj)
{
	auto iter = m_vDebugObjects.begin();
	auto end = m_vDebugObjects.end();

	while (iter != end)
	{
		CRenderObject* r = *iter;
		if (r->m_pobjOwner == obj)
		{
			SAFE_RELEASE(r->m_pd3dIndexBuffer);
			SAFE_RELEASE(r->m_pd3dVertexBuffer);
			SAFE_RELEASE(r->m_pd3dMatrixBuffer);
			delete r;
			m_vDebugObjects.erase(iter);
			break;
		}

		++iter;
	}
}
void CDebugRenderContext::CreateShadersAndInputLayout()
{
	DXCall(m_pd3dDevice->CreateVertexShader(ColorVS, sizeof(ColorVS), NULL, &m_pd3dVertexShader));
	DXCall(m_pd3dDevice->CreatePixelShader(ColorPS, sizeof(ColorPS), NULL, &m_pd3dPixelShader));

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DXCall(m_pd3dDevice->CreateInputLayout(layout, 2, ColorVS, sizeof(ColorVS), &m_pd3dInputLayout));
}
void CDebugRenderContext::ClearContainer(void)
{
	for each (CRenderObject* r in m_vDebugObjects)
	{
		SAFE_RELEASE(r->m_pd3dIndexBuffer);
		SAFE_RELEASE(r->m_pd3dVertexBuffer);
		SAFE_RELEASE(r->m_pd3dMatrixBuffer);
		delete r;
	}
	m_vDebugObjects.clear();
}