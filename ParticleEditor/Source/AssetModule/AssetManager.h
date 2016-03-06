/***********************************************
* Filename:           	AssetManager.h
* Date:  	         	02/08/2016
* Mod. Date:    	   	02/10/2016
* Mod. Initials:		SH
* Author:	         	Samuel Hotchkiss
* Purpose:            	Loads in assets from exterior files
************************************************/

#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include <string>
//#include <fstream>
#include <map>
#include <vector>

//forward declarations
class CTexture;
class CObject;
struct SpawnObject;
class CRenderer;
class CParticleSystem;
struct ID3D11Device;

class CAssetManager
{
	friend CRenderer;	//with benefits

	CAssetManager(const CAssetManager&);
	CAssetManager& operator=(const CAssetManager&);				//private to help prevent making multiple instances of this class

	//asset containers
	std::map<std::string, CTexture*>		m_mpTextures;
	std::map<std::string, CObject*>			m_mpPrefabs;
	std::map<std::string, CParticleSystem*>	m_mpParticleSystems;

	ID3D11Device*	m_pd3dDevice;

	unsigned int m_unScreenWidth;
	unsigned int m_unScreenHeight;

	bool MapContains(std::map<std::string, CTexture*>& _map, std::string _name);
	bool MapContains(std::map<std::string, CObject*>& _map, std::string _name);
	bool MapContains(std::map<std::string, CParticleSystem*>& _map, std::string _name);

	bool LoadTexture(const wchar_t* _path, CTexture** _texture);					//loads in texture file located at _path, true if loaded successfully, false if not
	bool LoadLevelFile(const char* _path);										//loads in level data locatated at _path, true if successful, false if not
	bool LoadPrefab(std::string _objName, CObject** _object);
	bool LoadParticleSystem(const char* _path, CParticleSystem** _system);

	std::wstring ConvertToWide(const std::string& str);

public:
	CAssetManager();
	~CAssetManager();

	bool Initialize();
	bool ShutDown();

	CTexture*	GetTexture(std::string _name/*, CTexture* _outTexture*/);					//returns a copy of a specified texture if it exists
	void		GetPrefab(std::string _name, CObject** _outObject);					//returns a copy of a specified prefab if it exists
	void		GetParticleSystem(std::string _name, CParticleSystem** _outSystem);	//returns a copy of a specified particle system if it exists

	CTexture*		AccessTexture(std::string _name);

	bool LoadLevel(std::string _lvlName, std::vector<SpawnObject>& _spawns);

	void SetDevice(ID3D11Device* device) { m_pd3dDevice = device; }

	void SetWindowSize(unsigned int width, unsigned int height) { m_unScreenWidth = width; m_unScreenHeight = height; }
};
#endif //_ASSETMANAGER_H_