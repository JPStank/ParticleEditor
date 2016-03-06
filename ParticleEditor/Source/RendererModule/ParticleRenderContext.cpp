/***********************************************
* Filename:           	ParticleRenderContext.h
* Date:  	         	02/10/2016
* Mod. Date:    	   	02/11/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Define the pipeline for drawing
				particles to the scene. Will also update
				appropriate vertex buffers of relevant data
************************************************/
#include "ParticleRenderContext.h"
#include "RenderComponent.h"
#include "../EmitterModule/ParticleSystem.h"
#include "../LevelModule/Transform.h"
#include "../AssetModule/Texture.h"

#include "Shaders/ParticleVS.csh"
#include "Shaders/ParticleGS.csh"
#include "Shaders/ParticlePS.csh"

CParticleRenderContext::CParticleRenderContext()
{
	m_pd3dDevice = nullptr;
	m_pd3dDeviceContext = nullptr;
	m_pd3dInputLayout = nullptr;
	m_pd3dVertexShader = nullptr;
	m_pd3dPixelShader = nullptr;
	m_pd3dGeometryShader = nullptr;
}
CParticleRenderContext::~CParticleRenderContext()
{
	for each (CRenderObject* r in m_vpParticles)
	{
		SAFE_RELEASE(r->m_pd3dMatrixBuffer);
		SAFE_RELEASE(r->m_pd3dIndexBuffer);
		SAFE_RELEASE(r->m_pd3dVertexBuffer);
		if (r->m_ptParticleData)
		{
			delete r->m_ptParticleData;
			r->m_ptParticleData = nullptr;
		}
		delete r;
	}
	m_vpParticles.clear();

	//SAFE_RELEASE(m_pd3dDevice);
	//SAFE_RELEASE(m_pd3dDeviceContext);
	SAFE_RELEASE(m_pd3dVertexShader);
	SAFE_RELEASE(m_pd3dGeometryShader);
	SAFE_RELEASE(m_pd3dPixelShader);
	SAFE_RELEASE(m_pd3dInputLayout);
}
void CParticleRenderContext::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CTexture* defaultTexture)
{
	m_pd3dDevice = device;
	m_pd3dDeviceContext = deviceContext;

	CreateShadersAndInputLayout();
}
void CParticleRenderContext::Render(void)
{
	m_pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout);
	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, NULL);
	m_pd3dDeviceContext->GSSetShader(m_pd3dGeometryShader, NULL, NULL);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, NULL);

	int numObjects = (int)m_vpParticles.size();

	DXDrawName(L"Particles");

	// draw all da particles
	for (int i = 0; i < numObjects; ++i)
	{
		// map world matrix
		D3D11_MAPPED_SUBRESOURCE data;
		m_pd3dDeviceContext->Map(m_vpParticles[i]->m_pd3dMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &data);
		memcpy(data.pData, &m_vpParticles[i]->m_pobjOwner->GetTransform().GetWorldMatrix(), sizeof(float4x4));
		m_pd3dDeviceContext->Unmap(m_vpParticles[i]->m_pd3dMatrixBuffer, 0);

		m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_vpParticles[i]->m_pd3dMatrixBuffer);
		m_pd3dDeviceContext->GSSetConstantBuffers(0, 1, &m_vpParticles[i]->m_pd3dMatrixBuffer);

		// update and map vertex buffer
		CParticleSystem* particle = (CParticleSystem*)m_vpParticles[i]->m_pobjOwner->GetComponent(IComponent::eEMITTER);

		UINT drawCount = particle->GetAlive();
		for (UINT j = 0; j < drawCount; ++j)
		{
			m_vpParticles[i]->m_ptParticleData[j].m_d3dPosition = particle->GetPosition()[j];
			m_vpParticles[i]->m_ptParticleData[j].m_d3dColor = particle->GetColor()[j];
		}
		
		m_pd3dDeviceContext->Map(m_vpParticles[i]->m_pd3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &data);
		memcpy(data.pData, &m_vpParticles[i]->m_ptParticleData[0], sizeof(TParticleVert) * particle->GetMax());
		m_pd3dDeviceContext->Unmap(m_vpParticles[i]->m_pd3dVertexBuffer, 0);

		UINT stride = sizeof(TParticleVert);
		UINT offset = 0;
		m_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_vpParticles[i]->m_pd3dVertexBuffer, &stride, &offset);

		m_pd3dDeviceContext->Draw(drawCount, 0);
	}

	m_pd3dDeviceContext->GSSetShader(NULL, NULL, NULL);
}
void CParticleRenderContext::AddRenderComponent(CObject* obj)
{
	CRenderObject* addMe = new CRenderObject();
	addMe->m_pobjOwner = obj;

	CRenderComponent* rend = (CRenderComponent*)obj->GetComponent(IComponent::eRENDER);
	CParticleSystem* particle = (CParticleSystem*)obj->GetComponent(IComponent::eEMITTER);

	if (particle == nullptr)
	{
		Log("Object tagged as Particle given to Renderer without Particle System Component");
		delete addMe;
		addMe = nullptr;
		return;
	}

	// create initial particle verts
	//TParticleVert* tempVerts;
	int numVerts;
	numVerts = (int)particle->GetPosition().size();
	addMe->m_ptParticleData = new TParticleVert[numVerts];
	for (int i = 0; i < numVerts; ++i)
	{
		addMe->m_ptParticleData[i].m_d3dPosition = particle->GetPosition()[i];
		addMe->m_ptParticleData[i].m_d3dColor = particle->GetColor()[i];
		// add more init data as needed
	}

	// create vertex buffer
	D3D11_BUFFER_DESC vb;
	ZeroMemory(&vb, sizeof(vb));
	vb.ByteWidth = sizeof(TParticleVert) * numVerts;
	vb.Usage = D3D11_USAGE_DYNAMIC;
	vb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sub;
	ZeroMemory(&sub, sizeof(sub));
	sub.pSysMem = addMe->m_ptParticleData;

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

	m_vpParticles.push_back(addMe);
}
void CParticleRenderContext::RemoveRenderComponent(CObject* obj)
{
	auto iter = m_vpParticles.begin();
	auto end = m_vpParticles.end();

	while (iter != end)
	{
		CRenderObject* r = *iter;
		if (r->m_pobjOwner == obj)
		{
			SAFE_RELEASE(r->m_pd3dIndexBuffer);
			SAFE_RELEASE(r->m_pd3dVertexBuffer);
			SAFE_RELEASE(r->m_pd3dMatrixBuffer);
			if (r->m_ptParticleData)
			{
				delete r->m_ptParticleData;
				r->m_ptParticleData = nullptr;
			}
			delete r;
			m_vpParticles.erase(iter);
			return;
		}
		++iter;
	}

	Log("Tried to remove an object in ParticleRenderContext that did not exist");
}
void CParticleRenderContext::CreateShadersAndInputLayout()
{
	DXCall(m_pd3dDevice->CreateVertexShader(ParticleVS, sizeof(ParticleVS), NULL, &m_pd3dVertexShader));
	DXName(m_pd3dVertexShader, "Particle Vertex Shader");
	DXCall(m_pd3dDevice->CreateGeometryShader(ParticleGS, sizeof(ParticleGS), NULL, &m_pd3dGeometryShader));
	DXName(m_pd3dGeometryShader, "Particle Geometry Shader");
	DXCall(m_pd3dDevice->CreatePixelShader(ParticlePS, sizeof(ParticlePS), NULL, &m_pd3dPixelShader));
	DXName(m_pd3dPixelShader, "Particle Pixel Shader");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DXCall(m_pd3dDevice->CreateInputLayout(layout, 2, ParticleVS, sizeof(ParticleVS), &m_pd3dInputLayout));
	DXName(m_pd3dInputLayout, "Particle Input Layout");
}
void CParticleRenderContext::ClearContainer(void)
{
	for each (CRenderObject* r in m_vpParticles)
	{
		SAFE_RELEASE(r->m_pd3dMatrixBuffer);
		SAFE_RELEASE(r->m_pd3dIndexBuffer);
		SAFE_RELEASE(r->m_pd3dVertexBuffer);
		if (r->m_ptParticleData)
		{
			delete r->m_ptParticleData;
			r->m_ptParticleData = nullptr;
		}
		delete r;
	}
	m_vpParticles.clear();
}
