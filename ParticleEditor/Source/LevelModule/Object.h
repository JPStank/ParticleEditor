//--------------------------------------------------------------------------------
//	Object class
//	Written by: Logan Stallman (LMS)
//	Date:		2/3/2016
//
//	Holds basic functions and data for all other objects.
//	Is not updated in main game loop.
//--------------------------------------------------------------------------------
/* For Windows 7 user- put into configuration properties\C/C++\commandline.
 /D _WIN32_WINNT=0x0601
*/
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include "IComponent.h"
#include "../LevelModule/Transform.h"
#include <DirectXMath.h>

class Camera;
class CLevel;

enum eObjectTag : unsigned int { eUNTAGGED = 0, eCAMERA, ePLAYER, eENEMY, eARMOR,
	eTERRAIN, ePROP, ePORTAL, eWEAPON, eTELL, ePARTICLE, eUI, eHUD, eNAVMESH, eLIGHT, eNUMTAGS };

class CObject
{
private:
	CObject();

protected:
	// Tags are used to compare objects, usually in collisions.
	const eObjectTag m_tTag;

	// If this object is active, it will be updated(if possible) and rendered.
	bool m_bActive;

	// The transform of an object defines its position, orientation, and scale.
	//XMFLOAT4X4 m_d3dTransform;

	// Set this to "child" an object to a skeleton joint
	// null means no parent, valid ptr will combine matrix with local transform
	DirectX::XMFLOAT4X4* m_pd3dSkeletonJoint;
	// used in conjunction with skeleton joint. aids in file loading
	int m_nJointIndex;

	CLevel* m_pCurLevel;

	// who owns this object. null if root
	CObject* m_pobjParent;

	// Objects can contain other objects.  I trust you know what this entails.
	std::vector<CObject*> m_vobjChildren;

	// all components this object has
	std::vector<IComponent*> m_vComponents;

	Transform* m_pTransform;

	Camera* m_pCamera;

	std::string m_szName;

public:
	//CObject(const CObject& _other);
	CObject(eObjectTag _tag, CObject* _prefab = nullptr);
	virtual ~CObject();

	// Initialize the object.  Can be used to reset the object.
	void Initialize();

	// Cleans up any dynamic memory the object has.
	void ShutDown();

	// Accessors

	virtual bool ResolveHit(CObject* _otherThing);
	bool HasChild(CObject* _inQuestion);
	const bool GetActive() { return m_bActive; }
	std::vector<IComponent*>& GetComponents() { return m_vComponents; }
	std::vector<IComponent*>& GetCompList() { return m_vComponents; }
	IComponent* GetComponent(IComponent::eComponentType type); // this should remove hassle of templating -JS
	std::vector<CObject*>& GetChildren() { return m_vobjChildren; }
	CObject* GetChild(eObjectTag _ChildType);
	CObject* GetParent() {return m_pobjParent;}
	const eObjectTag GetTag() { return m_tTag; }
	const DirectX::XMFLOAT3 GetRight() { return DirectX::XMFLOAT3(m_pTransform->GetWorldMatrix()._11, m_pTransform->GetWorldMatrix()._12, m_pTransform->GetWorldMatrix()._13); }
	const DirectX::XMFLOAT3 GetUp() { return DirectX::XMFLOAT3(m_pTransform->GetWorldMatrix()._21, m_pTransform->GetWorldMatrix()._22, m_pTransform->GetWorldMatrix()._23); }
	const DirectX::XMFLOAT3 GetForward() { return DirectX::XMFLOAT3(m_pTransform->GetWorldMatrix()._31, m_pTransform->GetWorldMatrix()._32, m_pTransform->GetWorldMatrix()._33); }
	const DirectX::XMFLOAT3 GetPosition() { return DirectX::XMFLOAT3(m_pTransform->GetWorldMatrix()._41, m_pTransform->GetWorldMatrix()._42, m_pTransform->GetWorldMatrix()._43); }
	Transform& GetTransform() { return *m_pTransform; }
	const std::string GetName() { return m_szName; }
	const DirectX::XMFLOAT4X4* GetSkeletonJoint() { return m_pd3dSkeletonJoint; }
	const int GetJointIndex() { return m_nJointIndex; }

	void SetActive(bool _toBe) { m_bActive = _toBe; }
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 _pos);
	//void SetPosition(XMMATRIX _pos);
	void SetUp(DirectX::XMFLOAT3 _up);
	void SetForward(DirectX::XMFLOAT3 _forward);
	void SetRight(DirectX::XMFLOAT3 _right);
	void SetParent(CObject* _newParent);
	virtual void SetCamera(Camera* _cam);
	void SetName(std::string _name) { m_szName = _name; }
	void SetSkeletonJoint(DirectX::XMFLOAT4X4* joint) { m_pd3dSkeletonJoint = joint; }
	void SetJointIndex(int index) { m_nJointIndex = index; }
	void SetLevel(CLevel* _level) { m_pCurLevel = _level; }

	virtual const bool IsLockedOn() { return false; }

	void AddComponent(IComponent* comp);
	void AddChild(CObject* _newChild);
};

#endif