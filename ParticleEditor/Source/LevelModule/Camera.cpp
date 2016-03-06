//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "./Camera.h"
#include "../RenderNode.h"
#include "../EventModule/MessageSystem.h"
#include "../EventModule/EventManager.h"
#include "../Log.h"
//--------------------------------------------------------------------------------
using namespace DirectX;
using namespace Events;
//--------------------------------------------------------------------------------
Camera::Camera() :
m_vec3RightW(1.0f, 0.0f, 0.0f),
m_vec3UpW(0.0f, 1.0f, 0.0f),
m_vec3LookW(0.0f, 0.0f, 1.0f),
m_vec3Dir(0.0f, 0.0f, 0.0f),
m_fSpeed(5),
m_fMouseDX(0.f),
m_fMouseDY(0.f),
m_fNear(0.f),
m_fFar(0.f),
m_fFOV(0.f),
m_fAspectRatio(0.f) {
	XMStoreFloat4x4(&m_mtxView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mtxProj, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mtxViewProj, XMMatrixIdentity());

	// WASD input registering
	MessageSystem *ms = MessageSystem::GetInstance();
	if (!ms) {
		Log("Calling Message system before it was created");
		return;
	}
	//down
	EventManager()->RegisterClient(ms->Call<const EVENTID&, const EVENTID>("GetKeyDown", "DirForward"), this, &Camera::OnMoveForwardKeyDown);
	EventManager()->RegisterClient(ms->Call<const EVENTID&, const EVENTID>("GetKeyDown", "DirBack"), this, &Camera::OnMoveBackKeyDown);
	EventManager()->RegisterClient(ms->Call<const EVENTID&, const EVENTID>("GetKeyDown", "DirLeft"), this, &Camera::OnMoveLeftKeyDown);
	EventManager()->RegisterClient(ms->Call<const EVENTID&, const EVENTID>("GetKeyDown", "DirRight"), this, &Camera::OnMoveRightKeyDown);
	EventManager()->RegisterClient(ms->Call<const EVENTID&, const EVENTID>("GetKeyDown", "MouseLButton"), this, &Camera::OnMouseLDown);
	EventManager()->RegisterClient(ms->Call<const EVENTID&, const EVENTID>("GetKeyDown", "DebugSpeedUp"), this, &Camera::SpeedUp);
	//Up
	EventManager()->RegisterClient(ms->Call<const EVENTID&, const EVENTID>("GetKeyUp", "DebugSpeedDown"), this, &Camera::SlowDown);
}
//--------------------------------------------------------------------------------
Camera::~Camera() {
	// unregister events and deactivate keys
	EventManager()->UnregisterClient("DirForward", this, &Camera::OnMoveForwardKeyDown);
	EventManager()->UnregisterClient("DirBack", this, &Camera::OnMoveBackKeyDown);
	EventManager()->UnregisterClient("DirLeft", this, &Camera::OnMoveLeftKeyDown);
	EventManager()->UnregisterClient("DirRight", this, &Camera::OnMoveRightKeyDown);
	EventManager()->UnregisterClient("MouseLButton", this, &Camera::OnMouseLDown);

	MessageSystem *ms = MessageSystem::GetInstance();
	if (ms) {
		ms->Call<bool, EVENTID>("RemoveKey", "DirForward");
		ms->Call<bool, EVENTID>("RemoveKey", "DirBack");
		ms->Call<bool, EVENTID>("RemoveKey", "DirLeft");
		ms->Call<bool, EVENTID>("RemoveKey", "DirRight");
		ms->Call<bool, EVENTID>("RemoveKey", "MouseLButton");
		ms->Call<bool, EVENTID>("RemoveKey", "DebugSpeedUp");
		ms->Call<bool, EVENTID>("RemoveKey", "DebugSpeedDown");
	}
}
//--------------------------------------------------------------------------------
void Camera::DestroyObject() {
	Events::CEventManager * em = Events::CEventManager::GetInstance();
	if (!em) {
		Log(__FUNCTION__, " Could not call UnRegisterListener, no event manager");
		return;
	}
	em->SendEventInstant("UnRegisterListener", new Events::CGeneralEventArgs<const Entity3D*>(this));
}
//--------------------------------------------------------------------------------
void Camera::SetLens(float fov, float aspect, float nearZ, float farZ) {
	m_fNear = nearZ;
	m_fFar = farZ;
	m_fFOV = fov;
	m_fAspectRatio = aspect;
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ));
	XMStoreFloat4x4(&m_mtxViewProj, XMLoadFloat4x4(&m_mtxView)*XMLoadFloat4x4(&m_mtxProj));
}
//--------------------------------------------------------------------------------
void Camera::SpeedUp(const CBaseEventArgs&) {
	m_fSpeed = m_fSpeed * 10.f;
}
//--------------------------------------------------------------------------------
void Camera::SlowDown(const CBaseEventArgs&) {
	m_fSpeed = m_fSpeed / 10.f;
}
//--------------------------------------------------------------------------------
void Camera::SetSpeed(float s) {
	m_fSpeed = s;
}
//--------------------------------------------------------------------------------
void Camera::Update(const float fDeltaTime) {
	// move at the set speed along net direction.
	XMStoreFloat3(&m_vec3Dir, XMVector3Normalize(XMLoadFloat3(&m_vec3Dir)));

	XMFLOAT3 pos(0.f, 0.f, 0.f);
	XMStoreFloat3(&pos, XMVectorAdd(XMLoadFloat3(GetPosition()), XMVectorScale(XMLoadFloat3(&m_vec3Dir), m_fSpeed * fDeltaTime)));
	SetPosition(pos);

	// reset direction and mouse distance changes
	m_vec3Dir = XMFLOAT3(0, 0, 0);
	m_fMouseDX = m_fMouseDY = 0.0f;

	BuildView();
	UpdateRenderNode();
}
//--------------------------------------------------------------------------------
void Camera::OnMoveForwardKeyDown(const CGeneralEventArgs<float>& args) {
	XMStoreFloat3(&m_vec3Dir, XMLoadFloat3(&m_vec3Dir) + XMLoadFloat3(&m_vec3LookW));
}
//--------------------------------------------------------------------------------
void Camera::OnMoveBackKeyDown(const CGeneralEventArgs<float>& args) {
	XMStoreFloat3(&m_vec3Dir, XMLoadFloat3(&m_vec3Dir) - XMLoadFloat3(&m_vec3LookW));
}
//--------------------------------------------------------------------------------
void Camera::OnMoveLeftKeyDown(const CGeneralEventArgs<float>& args) {
	XMStoreFloat3(&m_vec3Dir, XMLoadFloat3(&m_vec3Dir) - XMLoadFloat3(&m_vec3RightW));
}
//--------------------------------------------------------------------------------
void Camera::OnMoveRightKeyDown(const CGeneralEventArgs<float>& args) {
	XMStoreFloat3(&m_vec3Dir, XMLoadFloat3(&m_vec3Dir) + XMLoadFloat3(&m_vec3RightW));
}
//--------------------------------------------------------------------------------
void Camera::OnMouseLDown(const CGeneralEventArgs<float>& args) {
	MessageSystem *ms = MessageSystem::GetInstance();
	if (!ms) {
		Log("No Message system");
		return;
	}
	float x = 0.f;
	float y = 0.f;
	float px = 0.f;
	float py = 0.f;
	ms->Call<void, float &, float&>("GetMousePos", x, y);
	ms->Call<void, float &, float&>("GetMousePrevPos", px, py);
	float pitch = (y - py) / 250.3f;
	float yAngle = (x - px) / 250.3f;

	// Rotate camera's look and up vectors around the camera's right vector.
	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat3(&m_vec3RightW), pitch);
	XMStoreFloat3(&m_vec3LookW, XMVector3TransformNormal(XMLoadFloat3(&m_vec3LookW), r));
	XMStoreFloat3(&m_vec3UpW, XMVector3TransformNormal(XMLoadFloat3(&m_vec3UpW), r));

	// Rotate camera axis about the world's y-axis.
	r = XMMatrixRotationY(yAngle);
	XMStoreFloat3(&m_vec3RightW, XMVector3TransformNormal(XMLoadFloat3(&m_vec3RightW), r));
	XMStoreFloat3(&m_vec3UpW, XMVector3TransformNormal(XMLoadFloat3(&m_vec3UpW), r));
	XMStoreFloat3(&m_vec3LookW, XMVector3TransformNormal(XMLoadFloat3(&m_vec3LookW), r));
}
//--------------------------------------------------------------------------------
void Camera::BuildView() {
	// Keep camera's axes orthogonal to each other and of unit length.
	XMStoreFloat3(&m_vec3LookW, XMVector3Normalize(XMLoadFloat3(&m_vec3LookW)));

	XMStoreFloat3(&m_vec3UpW, XMVector3Cross(XMLoadFloat3(&m_vec3LookW), XMLoadFloat3(&m_vec3RightW)));
	XMStoreFloat3(&m_vec3UpW, XMVector3Normalize(XMLoadFloat3(&m_vec3UpW)));

	XMStoreFloat3(&m_vec3RightW, XMVector3Cross(XMLoadFloat3(&m_vec3UpW), XMLoadFloat3(&m_vec3LookW)));
	XMStoreFloat3(&m_vec3RightW, XMVector3Normalize(XMLoadFloat3(&m_vec3RightW)));

	float x, y, z;
	XMStoreFloat(&x, -XMVector3Dot(XMLoadFloat3(GetPosition()), XMLoadFloat3(&m_vec3RightW)));
	XMStoreFloat(&y, -XMVector3Dot(XMLoadFloat3(GetPosition()), XMLoadFloat3(&m_vec3UpW)));
	XMStoreFloat(&z, -XMVector3Dot(XMLoadFloat3(GetPosition()), XMLoadFloat3(&m_vec3LookW)));

	m_mtxView(0, 0) = m_vec3RightW.x;
	m_mtxView(1, 0) = m_vec3RightW.y;
	m_mtxView(2, 0) = m_vec3RightW.z;
	m_mtxView(3, 0) = x;

	m_mtxView(0, 1) = m_vec3UpW.x;
	m_mtxView(1, 1) = m_vec3UpW.y;
	m_mtxView(2, 1) = m_vec3UpW.z;
	m_mtxView(3, 1) = y;

	m_mtxView(0, 2) = m_vec3LookW.x;
	m_mtxView(1, 2) = m_vec3LookW.y;
	m_mtxView(2, 2) = m_vec3LookW.z;
	m_mtxView(3, 2) = z;

	m_mtxView(0, 3) = 0.0f;
	m_mtxView(1, 3) = 0.0f;
	m_mtxView(2, 3) = 0.0f;
	m_mtxView(3, 3) = 1.0f;

	//Update the local transform
	SetXAxis(m_vec3RightW);
	SetYAxis(m_vec3UpW);
	SetZAxis(m_vec3LookW);

	XMStoreFloat4x4(&m_mtxViewProj, XMLoadFloat4x4(&m_mtxView)*XMLoadFloat4x4(&m_mtxProj));
}
//--------------------------------------------------------------------------------
void Camera::LookAt(const XMFLOAT3 pos, const XMFLOAT3 target, const XMFLOAT3 up)
{
	XMVECTOR L = XMLoadFloat3(&target) - XMLoadFloat3(&pos);
	L = XMVector3Normalize(L);

	XMVECTOR R = XMVector3Cross(XMLoadFloat3(&up), L);
	R = XMVector3Normalize(R);

	XMVECTOR U = XMVector3Cross(L, R);
	U = XMVector3Normalize(U);

	XMStoreFloat3(&m_vec3RightW, R);
	XMStoreFloat3(&m_vec3UpW, U);
	XMStoreFloat3(&m_vec3LookW, L);
	SetPosition(pos);

	BuildView();
}
//--------------------------------------------------------------------------------
void Camera::UpdateRenderNode() {
	PerFrame data;

	data.mProjection = GetProjection();
	data.mView = GetView();

	static_cast<RenderData<PerFrame>*>(m_renderNode)->SetData(data);

	super::UpdateRenderNode();
}
//--------------------------------------------------------------------------------