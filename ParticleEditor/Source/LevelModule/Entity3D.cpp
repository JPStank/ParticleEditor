//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "./Entity3D.h"
#include "../Log.h"
#include "../RenderNode.h"
#include "../EventModule/EventManager.h"
//--------------------------------------------------------------------------------
using namespace DirectX;
//--------------------------------------------------------------------------------
Entity3D::Entity3D() : type(UpdateTypes::ROTATE), totalTime(0.f) {
	func[static_cast<unsigned int>(UpdateTypes::ROTATE)] = &Entity3D::UpdateRotate;
	func[static_cast<unsigned int>(UpdateTypes::SPIN)] = &Entity3D::UpdateSpin;
}
//--------------------------------------------------------------------------------
bool Entity3D::SetUpdateType(UpdateTypes _type) {
	if (_type < UpdateTypes::NUM_UPDATE_TYPES) {
		type = _type;
		return true;
	}
	Log("Could not set update Type, invalid argument");
	return false;
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateSpin(const float fDeltaTime) {
	XMMATRIX mSpin = XMMatrixRotationZ(-totalTime);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	//the matrix *= operator adds an addition pointer dereference.  Below is better than calling ... World = mScale * mSpin * mTranslate;
	XMFLOAT4X4 newLocal;
	//store the entities new location 
	XMStoreFloat4x4(&newLocal, XMMatrixMultiply(XMMatrixMultiply(mScale, mSpin), mTranslate) );
	//Wrote the above way for clarity, this should be called like :
	//XMStoreFloat4x4(&newLocal, XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(0.3f, 0.3f, 0.3f), XMMatrixRotationZ(-totalTime)), XMMatrixTranslation(-4.0f, 0.0f, 0.0f)));
	SetLocalMatrix(newLocal);
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateRotate(const float fDeltaTime) {
	XMFLOAT4X4 newLocal;
	//store the entities new location
	XMStoreFloat4x4(&newLocal, XMMatrixMultiply(XMMatrixRotationY(totalTime), XMMatrixTranslation(0.f, 0.f, 0.5f * totalTime) ) );
	SetLocalMatrix(newLocal);
}
//--------------------------------------------------------------------------------
Entity3D::~Entity3D() { }
//--------------------------------------------------------------------------------
void Entity3D::DestroyObject() {
	Events::CEventManager * em = Events::CEventManager::GetInstance();
	if (!em) {
		Log(__FUNCTION__, " Could not call UnRegisterListener, no event manager");
		return;
	}
	em->SendEventInstant("UnRegisterEntity", new Events::CGeneralEventArgs<const Entity3D*>(this));
}
//--------------------------------------------------------------------------------
void Entity3D::Update(const float fDeltaTime) {
	totalTime += fDeltaTime;
	(this->*func[static_cast<unsigned int>(type)])(fDeltaTime);
	UpdateRenderNode();
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateRenderNode() {
	PerObject data;
	data.mWorld = GetWorldMatrix();

	static_cast<RenderData<PerObject>*>(m_renderNode)->SetData(data);

	super::UpdateRenderNode();
}
//--------------------------------------------------------------------------------