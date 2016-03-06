/***********************************************
* Filename:           	AssetManager.cpp
* Date:  	         	02/08/2016
* Mod. Date:    	   	02/10/2016
* Mod. Initials:		SH
* Author:	         	Samuel Hotchkiss
* Purpose:            	Loads in assets from exterior files
************************************************/

#include <fstream>
#include "AssetManager.h"
#include "Texture.h"
#include <DirectXMath.h>
#include <d3d11.h> // for device things
#include "DDSTextureLoader.h"
#include "../Log.h"
#include "../DXLog.h"
#include "../LevelModule/CLevel.h"
#include "../RendererModule/RenderComponent.h"
#include "../EmitterModule/ParticleSystem.h"
#include "../EmitterModule/ParticleSpawners.h"
#include "../EmitterModule/ParticleUpdaters.h"
#include "tinyxml.h"

#include "../EventModule/EventManager.h"
#include "../EventModule/MessageSystem.h"
using namespace Events;

CAssetManager::CAssetManager()
{
	m_pd3dDevice = nullptr;

}

CAssetManager::~CAssetManager()
{
	ShutDown();
}

bool CAssetManager::Initialize()
{
	return true;
}

bool CAssetManager::ShutDown()
{
	std::map<std::string, CTexture*>::iterator textureIter = m_mpTextures.begin();
	std::map<std::string, CObject*>::iterator objIter = m_mpPrefabs.begin();
	std::map<std::string, CParticleSystem*>::iterator sysIter = m_mpParticleSystems.begin();

	while (sysIter != m_mpParticleSystems.end())
	{
		CParticleSystem* byebye = (*sysIter).second;

		for (int i = 0; i < (int)byebye->GetSpawners().size(); ++i)
		{
			delete byebye->GetSpawners()[i];
		}
		byebye->GetSpawners().clear();

		for (int i = 0; i < (int)byebye->GetUpdaters().size(); ++i)
		{
			delete byebye->GetUpdaters()[i];
		}
		byebye->GetUpdaters().clear();

		sysIter++;
	}
	m_mpParticleSystems.clear();

	while (textureIter != m_mpTextures.end())
	{
		CTexture* garbage = (*textureIter).second;
		delete garbage;
		garbage = nullptr;
		textureIter++;
	}
	m_mpTextures.clear();

	while (objIter != m_mpPrefabs.end())
	{
		CObject* garbage = (*objIter).second;
		delete garbage;
		garbage = nullptr;
		objIter++;
	}
	m_mpPrefabs.clear();

	return true;
}

CTexture* CAssetManager::GetTexture(std::string _name/*, CTexture* _outTexture*/)
{
	if (MapContains(m_mpTextures, _name))
	{
		return m_mpTextures[_name];
	}
	else
	{
		std::string error = _name + " NOT LOADED";
		Log(error.c_str());
		return nullptr;
	}
}
void CAssetManager::GetParticleSystem(std::string _name, CParticleSystem** _outSystem)
{
	if (MapContains(m_mpParticleSystems, _name))
	{
		CParticleSystem* original = m_mpParticleSystems[_name];
		CParticleSystem* sys = new CParticleSystem(original->GetMax(), original->GetRate());

		sys->GetSpawners() = original->GetSpawners();
		sys->GetUpdaters() = original->GetUpdaters();

		*_outSystem = sys;
	}
	else
	{
		Log(_name, " NOT LOADED");
	}
}
void CAssetManager::GetPrefab(std::string _name, CObject** _outFab)
{
	if (MapContains(m_mpPrefabs, _name))
	{

		unsigned int tag = m_mpPrefabs[_name]->GetTag();
		CObject* fab;
		fab = new CObject((eObjectTag)tag, m_mpPrefabs[_name]);
		for each(IComponent* comp in m_mpPrefabs[_name]->GetCompList())
		{
			IComponent* addThis = nullptr;
			switch (comp->GetType())
			{
			case IComponent::eRENDER:
			{
				addThis = new CRenderComponent(nullptr, ((CRenderComponent*)comp)->GetRenderType());
				if (((CRenderComponent*)comp)->GetTexture())
				{
					((CRenderComponent*)addThis)->SetTexture(((CRenderComponent*)comp)->GetTexture());
				}
				if ((((CRenderComponent*)comp)->GetTextures()))
				{
					for (size_t i = 0; i < ((CRenderComponent*)comp)->GetTextures()->size(); i++)
					{
						((CRenderComponent*)addThis)->AddToTextureArray(((CRenderComponent*)comp)->GetTextureInArray(i));
					}
				}
				fab->AddComponent(addThis);
				break;
			}
			case IComponent::eEMITTER:
			{
				CParticleSystem* original = (CParticleSystem*)comp;

				addThis = new CParticleSystem(original->GetMax(), original->GetRate());

				((CParticleSystem*)addThis)->GetSpawners() = original->GetSpawners();
				((CParticleSystem*)addThis)->GetUpdaters() = original->GetUpdaters();

				fab->AddComponent(addThis);
				break;
			}
			default:
				break;
			}
		}

		CObject* addMe;
		unsigned int i, size;
		size = m_mpPrefabs[_name]->GetChildren().size();
		for (i = 0; i < size; i++)
		{
			CObject* currentObject = m_mpPrefabs[_name]->GetChildren()[i];
			GetPrefab(currentObject->GetName(), &addMe);

			//addMe = new CObject(currentObject->GetTag());
			addMe->SetPosition(currentObject->GetPosition());

			fab->AddChild(addMe);
		}

		(*_outFab) = fab;
	}
	else
	{
		std::string error = _name + " NOT LOADED";
		Log(error.c_str());
	}
}

bool CAssetManager::LoadTexture(const WCHAR* _path, CTexture** _texture)
{
	CTexture* t = new CTexture();

	HRESULT result;
	ID3D11Resource* temp = nullptr;
	ID3D11ShaderResourceView* diffuse = nullptr;
	result = DirectX::CreateDDSTextureFromFile(m_pd3dDevice, _path, &temp, &diffuse);
	SAFE_RELEASE(temp);

	if (FAILED(result))
	{
		delete t;
		SAFE_RELEASE(diffuse);
		Log(_path, " could not be loaded");
		return false;
	}

	t->AssignTexture(CTexture::eDIFFUSE, diffuse);
	*_texture = t;
	// TODO: add multi texture support
	return false;
}

bool CAssetManager::LoadPrefab(std::string _objName, CObject** _object) //pass in map[name]
{
	std::string path = "Resources/XML_Files/" + _objName + ".xml";
	TiXmlDocument doc(path.c_str());
	if (!doc.LoadFile())
	{
		std::string error = "PREFAB FILE NOT FOUND AT ";
		error = error + _objName + ".xml";
		Log(error.c_str());

		return false;
	}

	TiXmlElement* pRoot = doc.RootElement();
	//error check

	TiXmlElement* pElem = pRoot->FirstChildElement(); //pElem is now at "base"
	int tag = 0;
	pElem->Attribute("Tag", &tag);

	*_object = new CObject((eObjectTag)tag);
	(*_object)->SetName(_objName);

	pElem = pElem->NextSiblingElement(); //pElem is now on "components"
	//int numComponents = 0;
	//pElem->Attribute("NumComps", &numComponents);
	TiXmlElement* pChild = pElem->FirstChildElement(); //child is the first component

	int compType = 0;
	while (pChild)
	{
		pChild->Attribute("CompType", &compType); //what type of component?

		switch (compType)
		{
		case IComponent::eRENDER: //if render component
		{
			std::string meshName;
			if (pChild->Attribute("Mesh"))
			{
				Log("Mesh currently not supported with emitter editor.");
			}
			int renderType = 0;
			pChild->Attribute("Type", &renderType);
			CRenderComponent* rend = new CRenderComponent(nullptr, (CRenderComponent::eRenderType)renderType);
			if (pChild->Attribute("Texture"))
			{
				std::string textureName = pChild->Attribute("Texture");
				if (!MapContains(m_mpTextures, textureName))
				{
					std::wstring wName = ConvertToWide(textureName); // i fucking hate wide strings
					std::wstring path = L"Resources/Texture/" + wName + L".dds";
					LoadTexture(path.c_str(), &m_mpTextures[textureName]);
					m_mpTextures[textureName]->SetName(textureName.c_str());
				}
				rend->SetTexture(m_mpTextures[textureName]);
			}
			else
			{
				TiXmlElement* pTextures = pChild->FirstChildElement();
				if (pTextures)
				{
					pTextures = pTextures->FirstChildElement();
					while (pTextures)
					{
						std::string textureName = pTextures->Attribute("Texture");
						if (!MapContains(m_mpTextures, textureName))
						{
							std::wstring wName = ConvertToWide(textureName); // i fucking hate wide strings
							std::wstring path = L"Resources/Texture/" + wName + L".dds";
							LoadTexture(path.c_str(), &m_mpTextures[textureName]);
							m_mpTextures[textureName]->SetName(textureName.c_str());
						}
						rend->AddToTextureArray(m_mpTextures[textureName]);
						pTextures = pTextures->NextSiblingElement();
					}
					rend->SetTexture(rend->GetTextureInArray(0));
				}
			}
			(*_object)->AddComponent(rend);
			break;
		}
		case IComponent::eEMITTER:
		{
			std::string particleName;

			if (pChild->Attribute("ParticleFile"))
			{
				particleName = pChild->Attribute("ParticleFile");
				if (!MapContains(m_mpParticleSystems, particleName))
				{
					std::string path = "Resources/XML_Files/" + particleName + ".xml";
					LoadParticleSystem(path.c_str(), &m_mpParticleSystems[particleName]); // calls new
					(*_object)->AddComponent(m_mpParticleSystems[particleName]);
				}
			}
			break;
		}
		default:
			break;
		}


		pChild = pChild->NextSiblingElement(); //next component
	}

	pElem = pElem->NextSiblingElement(); //pElem is now on "children"
	if (pElem)
	{
		pChild = pElem->FirstChildElement(); //child is the first... child.

		while (pChild)
		{
			SpawnObject s;
			s.sObjName = pChild->Attribute("Name");
			double tempX, tempY, tempZ;
			pChild->Attribute("posX", &tempX);
			pChild->Attribute("posY", &tempY);
			pChild->Attribute("posZ", &tempZ);

			s.d3dPosition.x = (float)tempX;
			s.d3dPosition.y = (float)tempY;
			s.d3dPosition.z = (float)tempZ;

			//_spawns.push_back(s);
			if (!MapContains(m_mpPrefabs, s.sObjName))
			{
				LoadPrefab(s.sObjName, &m_mpPrefabs[s.sObjName]);
			}

			CObject* child;
			GetPrefab(s.sObjName, &child);
			child->SetName(s.sObjName);
			child->SetPosition(s.d3dPosition);
			(*_object)->AddChild(child);

			pChild = pChild->NextSiblingElement();
		}
	}

	return true;
}
bool CAssetManager::LoadParticleSystem(const char* _path, CParticleSystem** _system)
{
	TiXmlDocument doc(_path);
	if (!doc.LoadFile())
	{
		Log(_path, " NOT FOUND!");
		return false;
	}

	TiXmlElement* pRoot = doc.RootElement(); // ParticleSystem
	TiXmlElement* pElem = pRoot->FirstChildElement(); // ParticleSpawners

	if (!pElem)
	{
		Log(_path, " must contain at least one spawner!");
		return false;
	}

	double max, rate;

	pRoot->Attribute("maxCount", &max);
	pRoot->Attribute("emissionRate", &rate);

	CParticleSystem* sys = new CParticleSystem((unsigned int)max, (float)rate);

	TiXmlElement* pSpawn = pElem->FirstChildElement(); // first spawner type
	while (pSpawn)
	{
		std::string type = pSpawn->Attribute("type");

		if (type == "boxPos")
		{
			double cX, cY, cZ, oX, oY, oZ; // center and offset
			pSpawn->Attribute("centerX", &cX);
			pSpawn->Attribute("centerY", &cY);
			pSpawn->Attribute("centerZ", &cZ);
			pSpawn->Attribute("offsetZ", &oX);
			pSpawn->Attribute("offsetY", &oY);
			pSpawn->Attribute("offsetZ", &oZ);

			sys->GetSpawners().push_back(new CBoxSpawner(float3((float)cX, (float)cY, (float)cZ), float3((float)oX, (float)oY, (float)oZ)));
		}
		else if (type == "sphereVel")
		{
			double speed;
			pSpawn->Attribute("speed", &speed);

			sys->GetSpawners().push_back(new CSphereVelocitySpawner((float)speed));
		}
		else if (type == "uniformTime")
		{
			double duration;
			pSpawn->Attribute("duration", &duration);

			sys->GetSpawners().push_back(new CUniformTimeSpawner((float)duration));
		}
		else
		{
			Log(type, " is an unknown spawner type");
		}

		pSpawn = pSpawn->NextSiblingElement();
	}

	TiXmlElement* pUpdate = pElem->NextSiblingElement()->FirstChildElement(); // ParticleUpdaters
	if (!pUpdate)
	{
		Log(_path, " needs at least one updater!");
		delete sys;
		return false;
	}

	while (pUpdate)
	{
		std::string type = pUpdate->Attribute("type");

		if (type == "eulerPos")
		{
			double aX, aY, aZ; // acceleration
			pUpdate->Attribute("accX", &aX);
			pUpdate->Attribute("accY", &aY);
			pUpdate->Attribute("accZ", &aZ);

			sys->GetUpdaters().push_back(new CEulerUpdater(float3((float)aX, (float)aY, (float)aZ)));
		}
		else if (type == "lerpColor")
		{
			double sR, sG, sB, sA, eR, eG, eB, eA; // start and end color
			pUpdate->Attribute("startR", &sR);
			pUpdate->Attribute("startG", &sG);
			pUpdate->Attribute("startB", &sB);
			pUpdate->Attribute("startA", &sA);
			pUpdate->Attribute("endR", &eR);
			pUpdate->Attribute("endG", &eG);
			pUpdate->Attribute("endB", &eB);
			pUpdate->Attribute("endA", &eA);

			sys->GetUpdaters().push_back(new CColorUpdater(float4((float)sR, (float)sG, (float)sB, (float)sA), float4((float)eR, (float)eG, (float)eB, (float)eA)));
		}
		else if (type == "defaultTime")
		{
			sys->GetUpdaters().push_back(new CTimeUpdater());
		}
		else
		{
			Log(type, " unknown updater type");
		}

		pUpdate = pUpdate->NextSiblingElement();
	}

	*_system = sys;

	return true;
}
bool CAssetManager::LoadLevel(std::string _lvlName, std::vector<SpawnObject>& _spawns)
{
	std::string path = "Resources/XML_Files/" + _lvlName + ".xml";

	TiXmlDocument doc(path.c_str());
	if (!doc.LoadFile())
	{
		std::string error = _lvlName + ".xml NOT FOUND";
		Log(error.c_str());
		return false;
	}

	TiXmlElement* pRoot = doc.RootElement(); //<Level>
	TiXmlElement* pElem = pRoot->FirstChildElement(); //<object>
	while (pElem)
	{
		SpawnObject s;
		s.sObjName = pElem->Attribute("Name");
		double tempX, tempY, tempZ;
		pElem->Attribute("posX", &tempX);
		pElem->Attribute("posY", &tempY);
		pElem->Attribute("posZ", &tempZ);

		s.d3dPosition.x = (float)tempX;
		s.d3dPosition.y = (float)tempY;
		s.d3dPosition.z = (float)tempZ;

		_spawns.push_back(s);
		if (!MapContains(m_mpPrefabs, s.sObjName))
		{
			//m_mpPrefabs[s.sObjName] = new CObject();
			LoadPrefab(s.sObjName, &m_mpPrefabs[s.sObjName]);
		}

		pElem = pElem->NextSiblingElement();
	}

	return true;
}


CTexture* CAssetManager::AccessTexture(std::string _name)
{
	if (MapContains(m_mpTextures, _name))
	{
		return m_mpTextures[_name];
	}

	return nullptr;
}
std::wstring CAssetManager::ConvertToWide(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);

	std::vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &buffer[0], (int)buffer.size());

	return std::wstring(&buffer[0], charsConverted);

}

bool CAssetManager::MapContains(std::map<std::string, CTexture*>& _map, std::string _name)
{
	if (_map.empty())
		return false;

	std::map<std::string, CTexture*>::iterator iter = _map.find(_name);
	if (iter != _map.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CAssetManager::MapContains(std::map<std::string, CObject*>& _map, std::string _name)
{
	if (_map.empty())
		return false;

	std::map<std::string, CObject*>::iterator iter = _map.find(_name);
	if (iter != _map.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CAssetManager::MapContains(std::map<std::string, CParticleSystem*>& _map, std::string _name)
{
	if (_map.empty())
	{
		return false;
	}

	auto iter = _map.find(_name);
	if (iter != _map.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}
