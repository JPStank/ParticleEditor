#include "./Transform.h"
//--------------------------------------------------------------------------------
using namespace DirectX;
//--------------------------------------------------------------------------------
Transform::Transform(void) : parent(0), child(0), sibling(0), dirtied(true) {
	using namespace DirectX;
	XMStoreFloat4x4(&localMatrix, XMMatrixIdentity());
}
//--------------------------------------------------------------------------------
Transform::Transform(const Transform& rhs) : child(nullptr), sibling(nullptr), parent(nullptr), dirtied(false), modified(false) {
	localMatrix = rhs.localMatrix;
	worldMatrix = rhs.worldMatrix;
}
//--------------------------------------------------------------------------------
void Transform::OnDestroy(void) {
	for (Transform* current = child; current != 0; current = current->sibling) {
		//remove this object
	}
}
//--------------------------------------------------------------------------------
const DirectX::XMFLOAT4X4& Transform::GetWorldMatrix(void) {
	using namespace DirectX;
	if (dirtied) {
		if (nullptr != parent) {
			XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMLoadFloat4x4(&parent->GetWorldMatrix())));
		}
		else
			worldMatrix = localMatrix;

		dirtied = false;
	}

	return worldMatrix;
}
//--------------------------------------------------------------------------------
const DirectX::XMFLOAT4X4& Transform::GetLocalMatrix(void) const {
	return localMatrix;
}
//--------------------------------------------------------------------------------
void Transform::SetLocalMatrix(const DirectX::XMFLOAT4X4& matrix) {
	localMatrix = matrix;

	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::DirtyTransform(void) {
	modified = true;

	if (!dirtied) {
		dirtied = true;

		Transform* current = child;

		while (nullptr != current) {
			current->DirtyTransform();
			current = current->sibling;
		}
	}
}
//--------------------------------------------------------------------------------
Transform* Transform::GetParent(void) {
	return parent;
}
//--------------------------------------------------------------------------------
const Transform* Transform::GetParent(void)const {
	return parent;
}
//--------------------------------------------------------------------------------
void Transform::SetParent(Transform* newParent) {
	if (newParent == this)
		return;

	if (parent == newParent)
		return;

	if (nullptr != parent)
		parent->RemoveChild(this);

	parent = newParent;

	if (nullptr != newParent) {
		using namespace DirectX;

		XMFLOAT4X4 newLocal;
		//world = local * worldParent is how we normally get our world
		//Adjust the local to keep the transform in the same world space position
		XMStoreFloat4x4(&newLocal, XMMatrixMultiply(XMLoadFloat4x4(&GetWorldMatrix()), XMMatrixInverse(nullptr, XMLoadFloat4x4(&parent->GetWorldMatrix()))));
		SetLocalMatrix(newLocal);
		parent->SetChild(this);
	}
}
//--------------------------------------------------------------------------------
void Transform::SetChild(Transform* _child) {
	if (nullptr == child) {
		child = _child;
		return;
	}

	child->SetSibling(_child);
}
//--------------------------------------------------------------------------------
void Transform::SetSibling(Transform* _sibling) {
	if (nullptr == sibling) {
		sibling = _sibling;
		return;
	}
	sibling->SetSibling(_sibling);
}
//--------------------------------------------------------------------------------
void Transform::RemoveChild(Transform* _child) {
	if (nullptr == child || child->parent != this)
		return;

	if (child == _child) {
		child->SetLocalMatrix(child->GetWorldMatrix());
		child = child->sibling;
		child->parent = nullptr;
		child->sibling = nullptr;
		return;
	}
	child->RemoveSibling(_child);
}
//--------------------------------------------------------------------------------
void Transform::RemoveSibling(Transform* _sibling) {
	if (nullptr == sibling)
		return;

	if (sibling == _sibling) {
		sibling->SetLocalMatrix(sibling->GetWorldMatrix());
		sibling = sibling->sibling;
		sibling->parent = nullptr;
		sibling->sibling = nullptr;
		return;
	}
	sibling->RemoveSibling(_sibling);
}
//--------------------------------------------------------------------------------
void Transform::SetPosition(const XMFLOAT3 pos) {
	localMatrix._41 = pos.x;
	localMatrix._42 = pos.y;
	localMatrix._43 = pos.z;
	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::SetXAxis(const XMFLOAT3 xAxis) {
	localMatrix._11 = xAxis.x;
	localMatrix._12 = xAxis.y;
	localMatrix._13 = xAxis.z;
	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::SetYAxis(const XMFLOAT3 yAxis) {
	localMatrix._21 = yAxis.x;
	localMatrix._22 = yAxis.y;
	localMatrix._23 = yAxis.z;
	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::SetZAxis(const XMFLOAT3 zAxis) {
	localMatrix._31 = zAxis.x;
	localMatrix._32 = zAxis.y;
	localMatrix._33 = zAxis.z;
	DirtyTransform();
}