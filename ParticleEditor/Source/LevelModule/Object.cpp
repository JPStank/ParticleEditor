#include "Object.h"
#include "../Log.h"
#include "../LevelModule/Transform.h"
#include "Camera.h"

CObject::CObject() : m_tTag(eUNTAGGED),
m_pobjParent(nullptr),
m_bActive(true),
m_pCurLevel(nullptr),
m_pTransform(nullptr),
m_pd3dSkeletonJoint(nullptr),
m_nJointIndex(-1)
{
	Initialize();
}

CObject::CObject(eObjectTag _tag, CObject* _prefab) : m_tTag(_tag)
{
	if (_prefab)
	{
		SetName(_prefab->GetName());
	}
	Initialize();
}

CObject::~CObject()
{
	ShutDown();
}

void CObject::Initialize()
{
	m_bActive = true;

	m_pobjParent = nullptr;
	m_pCamera = nullptr;
	m_pd3dSkeletonJoint = nullptr;
	m_nJointIndex = -1;

	if (m_pTransform)
		delete m_pTransform;
	m_pTransform = nullptr;
	m_pTransform = new Transform();
	m_pTransform->SetLocalMatrix({1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1});

	if (m_tTag == eWEAPON || m_tTag == eARMOR)
		m_bActive = false;
}

void CObject::ShutDown()
{
	unsigned int i, size;
	size = m_vobjChildren.size();

	for (i = 0; i < size; i++)
	{
		//SAFE_SHUT(m_vobjChildren[i]);
		CObject* curChild = m_vobjChildren[i];
		if (curChild)
		{
			delete curChild;
			curChild = nullptr;
		}
	}
	m_vobjChildren.clear();
	size = m_vComponents.size();
	for (i = 0; i < size; i++)
	{
		IComponent* curComp = m_vComponents[i];
		if (curComp)
		{
			delete curComp;
			curComp = nullptr;
		}
	}
	m_vComponents.clear();

	if (m_pTransform)
	{
		delete m_pTransform;
		m_pTransform = nullptr;
	}
}

bool CObject::ResolveHit(CObject* _otherThing)
{
	if (_otherThing->GetActive() == false)
	{
		return false;
	}

	return true;
}

bool CObject::HasChild(CObject* _inQuestion)
{
	unsigned int i, size;
	size = m_vobjChildren.size();
	for (i = 0; i < size; i++)
	{
		if (m_vobjChildren[i] == _inQuestion)
			return true;
	}
	return false;
}

CObject* CObject::GetChild(eObjectTag _ChildType)
{
	unsigned int i, size;
	size = m_vobjChildren.size();
	for (i = 0; i < size; i++)
	{
		if (m_vobjChildren[i]->GetTag() == _ChildType)
			return m_vobjChildren[i];
	}
	return nullptr;
}

IComponent* CObject::GetComponent(IComponent::eComponentType type)
{
	for each(IComponent* comp in m_vComponents)
	{
		if (comp->GetType() == type)
		{
			return comp;
		}
	}

	return nullptr;
}

void CObject::AddComponent(IComponent* comp)
{
	m_vComponents.push_back(comp);
	comp->SetParent(this);
}

void CObject::AddChild(CObject* _newChild)
{
	m_vobjChildren.push_back(_newChild);
	_newChild->SetParent(this);
}

void CObject::SetPosition(float x, float y, float z)
{
	m_pTransform->SetPosition(DirectX::XMFLOAT3(x, y, z));
}

void CObject::SetPosition(DirectX::XMFLOAT3 _pos)
{
	m_pTransform->SetPosition(_pos);
}
	 
void CObject::SetUp(DirectX::XMFLOAT3 _up)
{
	m_pTransform->SetYAxis(_up);
}
void CObject::SetForward(DirectX::XMFLOAT3 _forward)
{
	m_pTransform->SetZAxis(_forward);
}
void CObject::SetRight(DirectX::XMFLOAT3 _right)
{
	m_pTransform->SetXAxis(_right);
}

void CObject::SetParent(CObject* _newParent)
{
	m_pobjParent = _newParent;
	m_pTransform->SetParent(&m_pobjParent->GetTransform());

	if (!m_pobjParent->HasChild(this))
	{
		m_pobjParent->AddChild(this);
		m_pobjParent->GetTransform().SetChild(m_pTransform);
	}
}

void CObject::SetCamera(Camera* _cam)
{
	if (_cam && m_tTag == ePLAYER)
	{
		m_pCamera = _cam;
	}
}
