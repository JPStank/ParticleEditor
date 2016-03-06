//--------------------------------------------------------------------------------
//	CLevel class
//	Written by: Logan Stallman (LMS)
//	Date:		2/5/2016
//
//	Contains and updates all objects present in the current level.
//	Also handles events that may happen during a level.
//--------------------------------------------------------------------------------
/* For Windows 7 user- put into configuration properties\C/C++\commandline.
 /D _WIN32_WINNT=0x0601
*/
#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <vector>
#include <string>
#include <DirectXMath.h>
#include "../EventModule/EventArgs.h"
#include "Object.h"

using std::vector;
using std::string;

class CAssetManager;
class CGame;
class CRenderer;
class Camera;

struct SpawnObject
{
	string sObjName;
	DirectX::XMFLOAT3 d3dPosition;
};

class CLevel
{
private:
	// the master container of all objects in this level.
	vector<CObject*> m_vobjObjects;

	//List of objects to delete
	vector<std::pair<CObject*, float>> m_vobjFiringLine;

	CGame* m_pTheGame;

	CAssetManager* m_pTheAssMan;

	CRenderer* m_pTheRenderer;

	Camera* m_pCamera;

	void ClearObjects();
	void RegisterComponents(CObject* _obj);
	void UnregisterComponents(CObject* _obj);

public:
	CLevel();
	~CLevel();

	//	Accessors

	vector<CObject*>& GetSceneObjects();
	vector<CObject*> GetSceneObjectsWithTag(eObjectTag _tag);

	//	Logical functions

	//	Initialize can be used to reset the level.
	bool Initialize(string _levelname, CGame* _theGame);
	//	Load the leveldata from the AssetManager.
	bool LoadLevel(string _levelname);
	//	Unload the levedata
	bool UnLoadLevel();
	//	Update all the update-able objects.
	void Update(float _dt);
	//	Does the releasing of the memories.
	void ShutDown();

	void AddCamera(Camera* _cam);
};

#endif