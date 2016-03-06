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

CLevel::CLevel()
{

}

CLevel::~CLevel()
{
	//ShutDown();
}

void CLevel::ShutDown()
{
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
