#include "CLevel.h"
#include "../GameModule/Game.h"
#include "../AssetModule/AssetManager.h"
#include "../Log.h"
#include "Object.h"
#include "../RendererModule/Renderer.h"
#include "../RendererModule/RenderComponent.h"
#include "../EmitterModule/EmitterManager.h"
#include "../EmitterModule/ParticleSystem.h"
#include "../EmitterModule/ParticleSpawners.h"
#include "../EmitterModule/ParticleUpdaters.h"
#include "../EventModule/MessageSystem.h"
#include "../EventModule/EventManager.h"
#include "Camera.h"

#include <AntTweakBar.h>
#include <assert.h>

extern float g_ClearColor[4];
DirectX::XMFLOAT4 g_ParticleQuaternion;

/******************
* Tweak Bar Globals
*******************/
TwBar* g_bar = nullptr;
TwType g_float3Type;

CLevel::CLevel()
{

}

CLevel::~CLevel()
{
	//ShutDown();
}

void CLevel::ShutDown()
{
	TwTerminate();
	UnLoadLevel();
	ClearObjects();
}

void CLevel::ClearObjects()
{
	unsigned int i, size;
	size = m_vobjObjects.size();
	for (i = 0; i < size; i++)
	{
		CObject* curObject = m_vobjObjects[i];
		if (curObject)
		{
			delete curObject;
			curObject = nullptr;
		}
	}

	m_vobjObjects.clear();
}

vector<CObject*>& CLevel::GetSceneObjects()
{
	return m_vobjObjects;
}

vector<CObject*> CLevel::GetSceneObjectsWithTag(eObjectTag _tag)
{
	vector<CObject*> ret;
	unsigned int i, size;
	size = m_vobjObjects.size();
	for (i = 0; i < size; i++)
	{
		CObject* curObj = m_vobjObjects[i];
		if (curObj->GetTag() == _tag)
		{
			ret.push_back(curObj);
		}
	}
	return ret;
}

bool CLevel::Initialize(string _levelname, CGame* _theGame)
{
	m_pTheGame = _theGame;
	m_pTheAssMan = m_pTheGame->GetAssetManager();
	m_pTheRenderer = m_pTheGame->GetRenderer();

	// init tweak bar here since this holds onto both renderer and emitters
	if (!TwInit(TW_DIRECT3D11, m_pTheRenderer->GetDevice()))
	{
		MessageBoxA(m_pTheGame->GetHandle(), TwGetLastError(), "AntTweakBar initialization failed", MB_OK | MB_ICONERROR);
		return false;
	}
	TwWindowSize(m_pTheGame->GetWindWidthDirty(), m_pTheGame->GetWindHeightDirty());

	// create the bar
	g_bar = TwNewBar("myBar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar into a DirectX11 application.' "); // Message added to the help bar.
	int barSize[2] = { 224, 520 };
	TwSetParam(g_bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	// add some variables
	TwAddVarRW(g_bar, "ClearColor", TW_TYPE_COLOR3F, &g_ClearColor, " colormode=rgb group=ParticleSystem ");

	// define structs
	TwStructMember float3Members[] = {
		{ "x", TW_TYPE_FLOAT, offsetof(DirectX::XMFLOAT3, x), " step=0.05 " },
		{ "y", TW_TYPE_FLOAT, offsetof(DirectX::XMFLOAT3, y), " step=0.05 " },
		{ "z", TW_TYPE_FLOAT, offsetof(DirectX::XMFLOAT3, z), " step=0.05 " }
	};

	g_float3Type = TwDefineStruct("float3", float3Members, 3, sizeof(DirectX::XMFLOAT3), NULL, NULL);

	return LoadLevel(_levelname);
}

bool CLevel::UnLoadLevel()
{
	for each (CObject* obj in m_vobjObjects)
	{
		UnregisterComponents(obj);
	}
	return true;
}

bool CLevel::LoadLevel(string _levelname)
{
	ClearObjects();

	vector<SpawnObject> managerObjects;
	// get leveldata from asset manager
	if (m_pTheAssMan->LoadLevel(&_levelname[0], managerObjects) == false)
		return false; // something went wrong.  Double check the name maybe?

	// fill out our vectors
	unsigned int i, size;
	size = managerObjects.size();
	for (i = 0; i < size; i++)
	{
		CObject* pushMe;
		m_pTheAssMan->GetPrefab(managerObjects[i].sObjName, &pushMe);
		pushMe->SetPosition(managerObjects[i].d3dPosition.x, managerObjects[i].d3dPosition.y, managerObjects[i].d3dPosition.z);
		pushMe->SetLevel(this);
		m_vobjObjects.push_back(pushMe);
		RegisterComponents(pushMe);
	}

	// there should only be one object in the scene, and it should have an emitter
	assert(m_vobjObjects.size() == 1);
	assert(m_vobjObjects[0]->GetComponent(IComponent::eEMITTER));

	CParticleSystem* particle = (CParticleSystem*)m_vobjObjects[0]->GetComponent(IComponent::eEMITTER);

	TwAddVarRW(g_bar, "Emission Rate", TW_TYPE_FLOAT, &particle->m_fEmitRate, std::string(" min=0 max=" + std::to_string(particle->m_unMaxCount) + " step=10.0 group=ParticleSystem ").c_str());

	// init the quaternion for rotating the particle system
	using namespace DirectX;
	XMVECTOR trans, rot, scale;
	XMMatrixDecompose(&scale, &rot, &trans, XMLoadFloat4x4(&m_vobjObjects[0]->GetTransform().GetWorldMatrix()));
	XMStoreFloat4(&g_ParticleQuaternion, rot);
	TwAddVarRW(g_bar, "Rotation", TW_TYPE_QUAT4F, &g_ParticleQuaternion, " group=ParticleSystem ");

	// add customizer for each spawner type

	for each (IParticleSpawners* spawner in particle->GetSpawners())
	{
		if (dynamic_cast<CBoxSpawner*>(spawner))
		{
			CBoxSpawner* box = (CBoxSpawner*)spawner;
			TwAddVarRW(g_bar, "Center", g_float3Type, &box->m_d3dCenter, " group=PosSpawner ");
			TwAddVarRW(g_bar, "Offset", g_float3Type, &box->m_d3dOffset, " group=PosSpawner ");
		}
		else if (dynamic_cast<CSphereRandomVelocitySpawner*>(spawner))
		{
			CSphereRandomVelocitySpawner* vel = (CSphereRandomVelocitySpawner*)spawner;
			TwAddVarRW(g_bar, "Min Speed", TW_TYPE_FLOAT, &vel->m_fMinSpeed, " group=VelSpawner step=0.01 ");
			TwAddVarRW(g_bar, "Max Speed", TW_TYPE_FLOAT, &vel->m_fMaxSpeed, " group=VelSpawner step=0.01 ");
		}
		else if (dynamic_cast<CSphereVelocitySpawner*>(spawner))
		{
			CSphereVelocitySpawner* vel = (CSphereVelocitySpawner*)spawner;
			TwAddVarRW(g_bar, "Speed", TW_TYPE_FLOAT, &vel->m_fSpeed, " group=VelSpawner step=0.01 ");
		}
		else if (dynamic_cast<CUniformTimeSpawner*>(spawner))
		{
			CUniformTimeSpawner* time = (CUniformTimeSpawner*)spawner;
			TwAddVarRW(g_bar, "Lifetime", TW_TYPE_FLOAT, &time->m_fDuration, " group=TimeSpawner min=0 step=0.01 ");
		}
		else if (dynamic_cast<CUniformScaleSpawner*>(spawner))
		{
			CUniformScaleSpawner* scale = (CUniformScaleSpawner*)spawner;
			TwAddVarRW(g_bar, "Scale", TW_TYPE_FLOAT, &scale->m_fScale, " group=ScaleSpawner min=0.005 step=0.005 ");
		}
		else if (dynamic_cast<CUniformRotationSpawner*>(spawner))
		{
			CUniformRotationSpawner* rot = (CUniformRotationSpawner*)spawner;
			TwAddVarRW(g_bar, "Rotation", TW_TYPE_FLOAT, &rot->m_fRotation, " group=RotationSpawner min=0 max=360 step=0.1 ");
		}
		else if (dynamic_cast<CRandomTimeSpawner*>(spawner))
		{
			CRandomTimeSpawner* time = (CRandomTimeSpawner*)spawner;
			TwAddVarRW(g_bar, "Min Lifetime", TW_TYPE_FLOAT, &time->m_fMinDuration, " group=TimeSpawner min=0 step=0.01 ");
			TwAddVarRW(g_bar, "Max Lifetime", TW_TYPE_FLOAT, &time->m_fMaxDuration, " group=TimeSpawner min=0 step=0.01 ");
		}
		else if (dynamic_cast<CRandomScaleSpawner*>(spawner))
		{
			CRandomScaleSpawner* scale = (CRandomScaleSpawner*)spawner;
			TwAddVarRW(g_bar, "Min Scale", TW_TYPE_FLOAT, &scale->m_fMinScale, " group=ScaleSpawner min=0.005 step=0.005 ");
			TwAddVarRW(g_bar, "Max Scale", TW_TYPE_FLOAT, &scale->m_fMaxScale, " group=ScaleSpawner min=0.005 step=0.005 ");
		}
		else if (dynamic_cast<CRandomRotationSpawner*>(spawner))
		{
			CRandomRotationSpawner* rot = (CRandomRotationSpawner*)spawner;
			TwAddVarRW(g_bar, "Min Rotation", TW_TYPE_FLOAT, &rot->m_fMinRotation, " group=RotationSpawner min=0 max=360 step=0.1 ");
			TwAddVarRW(g_bar, "Max Rotation", TW_TYPE_FLOAT, &rot->m_fMaxRotation, " group=RotationSpawner min=0 max=360 step=0.1 ");
		}
		else if (dynamic_cast<CCircleSpawner*>(spawner))
		{
			CCircleSpawner* circle = (CCircleSpawner*)spawner;
			TwAddVarRW(g_bar, "Center", g_float3Type, &circle->m_d3dCenter, " group=PosSpawner ");
			TwAddVarRW(g_bar, "Radius", TW_TYPE_FLOAT, &circle->m_fRadius, " group=PosSpawner min=0 step=0.01 ");
			TwAddVarRW(g_bar, "Speed", TW_TYPE_FLOAT, &circle->m_fSpeed, " group=VelSpawner step=0.01 ");
		}
		else if (dynamic_cast<CCircleRandSpeedSpawner*>(spawner))
		{
			CCircleRandSpeedSpawner* circle = (CCircleRandSpeedSpawner*)spawner;
			TwAddVarRW(g_bar, "Center", g_float3Type, &circle->m_d3dCenter, " group=PosSpawner ");
			TwAddVarRW(g_bar, "Radius", TW_TYPE_FLOAT, &circle->m_fRadius, " group=PosSpawner min=0 step=0.01 ");
			TwAddVarRW(g_bar, "Min Speed", TW_TYPE_FLOAT, &circle->m_fMinSpeed, " group=VelSpawner step=0.01 ");
			TwAddVarRW(g_bar, "Max Speed", TW_TYPE_FLOAT, &circle->m_fMaxSpeed, " group=VelSpawner step=0.01 ");
		}
		else if (dynamic_cast<CConeSpawner*>(spawner))
		{
			CConeSpawner* cone = (CConeSpawner*)spawner;
			TwAddVarRW(g_bar, "Center", g_float3Type, &cone->m_d3dCenter, " group=PosSpawner ");
			TwAddVarRW(g_bar, "Half Angle", TW_TYPE_FLOAT, &cone->m_fHalfAngle, " group=VelSpawner min=0.1 max=89.9 step=0.05 ");
			TwAddVarRW(g_bar, "Speed", TW_TYPE_FLOAT, &cone->m_fSpeed, " group=VelSpawner step=0.01 ");
		}
		else if (dynamic_cast<CConeRandSpeedSpawner*>(spawner))
		{
			CConeRandSpeedSpawner* cone = (CConeRandSpeedSpawner*)spawner;
			TwAddVarRW(g_bar, "Center", g_float3Type, &cone->m_d3dCenter, " group=PosSpawner ");
			TwAddVarRW(g_bar, "Half Angle", TW_TYPE_FLOAT, &cone->m_fHalfAngle, " group=VelSpawner min=0.1 max=89.9 step=0.05 ");
			TwAddVarRW(g_bar, "Min Speed", TW_TYPE_FLOAT, &cone->m_fMinSpeed, " group=VelSpawner step=0.01 ");
			TwAddVarRW(g_bar, "Max Speed", TW_TYPE_FLOAT, &cone->m_fMaxSpeed, " group=VelSpawner step=0.01 ");
		}
	}

	// add updaters

	for each (IParticleUpdater* updater in particle->GetUpdaters())
	{
		if (dynamic_cast<CEulerUpdater*>(updater))
		{
			CEulerUpdater* euler = (CEulerUpdater*)updater;
			TwAddVarRW(g_bar, "Acceleration", g_float3Type, &euler->m_d3dAcceleration, " group=PosUpdater ");
		}
		else if (dynamic_cast<CColorUpdater*>(updater))
		{
			CColorUpdater* color = (CColorUpdater*)updater;
			TwAddVarRW(g_bar, "Start Color", TW_TYPE_COLOR4F, &color->m_d3dStartColor, " group=ColorUpdater ");
			TwAddVarRW(g_bar, "End Color", TW_TYPE_COLOR4F, &color->m_d3dEndColor, " group=ColorUpdater ");
		}
		else if (dynamic_cast<CScaleUpdater*>(updater))
		{
			CScaleUpdater* scale = (CScaleUpdater*)updater;
			TwAddVarRW(g_bar, "Start Scale", TW_TYPE_FLOAT, &scale->m_fStartScale, " group=ScaleUpdater min=0.001 step=0.001 ");
			TwAddVarRW(g_bar, "End Scale", TW_TYPE_FLOAT, &scale->m_fEndScale, " group=ScaleUpdater min=0.001 step=0.001 ");
		}
		else if (dynamic_cast<CRotationUpdater*>(updater))
		{
			CRotationUpdater* rot = (CRotationUpdater*)updater;
			TwAddVarRW(g_bar, "Speed", TW_TYPE_FLOAT, &rot->m_fSpeed, " group=RotationUpdater step=0.1 ");
		}
	}
	return true;
}

void CLevel::RegisterComponents(CObject* _obj)
{
	IComponent* toRegister;
	toRegister = _obj->GetComponent(IComponent::eRENDER);
	if (toRegister)
		m_pTheRenderer->RegisterComponent(toRegister);

	toRegister = _obj->GetComponent(IComponent::eEMITTER);
	if (toRegister)
	{
		m_pTheGame->GetEmitterManager()->RegisterParticleSystem((CParticleSystem*)toRegister);
	}

	unsigned int i, size;
	size = _obj->GetChildren().size();
	for (i = 0; i < size; i++)
	{
		RegisterComponents(_obj->GetChildren()[i]);
	}
}
void CLevel::UnregisterComponents(CObject* _obj)
{
	IComponent* toRegister;
	toRegister = _obj->GetComponent(IComponent::eRENDER);
	if (toRegister)
		m_pTheRenderer->UnregisterComponent(toRegister);

	toRegister = _obj->GetComponent(IComponent::eEMITTER);
	if (toRegister)
	{
		m_pTheGame->GetEmitterManager()->UnregisterParticleSystem((CParticleSystem*)toRegister);
	}

	if (_obj)
	{
		unsigned int i, size;
		size = _obj->GetChildren().size();
		for (i = 0; i < size; i++)
		{
			UnregisterComponents(_obj->GetChildren()[i]);
		}
	}

}
void CLevel::AddCamera(Camera* _cam)
{
	m_pCamera = _cam;
}

void CLevel::Update(float _dt)
{
	using namespace DirectX;
	float4x4 world;
	XMStoreFloat4x4(&world, XMMatrixRotationQuaternion(XMLoadFloat4(&g_ParticleQuaternion)));
	m_vobjObjects[0]->GetTransform().SetLocalMatrix(world);
}
