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
extern float g_ClearColor[4];


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
	TwBar *bar = TwNewBar("myBar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar into a DirectX11 application.' "); // Message added to the help bar.
	int barSize[2] = { 224, 320 };
	TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	// add some variables
	TwAddVarRW(bar, "ClearColor", TW_TYPE_COLOR3F, &g_ClearColor, "colormode=rgb");

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

}
