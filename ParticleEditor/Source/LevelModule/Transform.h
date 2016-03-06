#pragma once
//--------------------------------------------------------------------------------
#include <DirectXMath.h>
//--------------------------------------------------------------------------------
class Transform {
public:
	Transform(void);
	Transform(const Transform& rhs);

	~Transform(){}

	const DirectX::XMFLOAT4X4& GetWorldMatrix(void);

	const DirectX::XMFLOAT4X4& GetLocalMatrix(void)const;
	void SetLocalMatrix(const DirectX::XMFLOAT4X4& matrix);

	Transform* GetParent(void);
	const Transform* GetParent(void)const;

	void SetParent(Transform* _parent);

	void DirtyTransform(void);

	bool wasModified(void);

	template<typename Func>
	static void ForEachTransformInHierarchy(Transform* transform, Func func) {
		func(transform);

		for (transform = transform->firstChild; transform != 0; transform = transform->nextSibling)
			ForEachTransformInHierarchy(transform, func);
	}

	Transform* clone(void)const {
		return new Transform(*this);
	}
	void SetChild(Transform* _child);

	const DirectX::XMFLOAT3 * GetPosition() const;
	const DirectX::XMFLOAT3 * GetYAxis() const;
	const DirectX::XMFLOAT3 * GetZAxis() const;

	void SetPosition(const DirectX::XMFLOAT3 pos);
	void SetXAxis(const DirectX::XMFLOAT3 xAxis);
	void SetYAxis(const DirectX::XMFLOAT3 yAxis);
	void SetZAxis(const DirectX::XMFLOAT3 zAxis);

private:
	void SetSibling(Transform* _sibling);
	void RemoveSibling(Transform* _sibling);
	Transform* parent;

	Transform* child;
	Transform* sibling;

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 localMatrix;

	void RemoveChild(Transform* child);
	void OnDestroy(void);

	int dirtied : 1;
	int modified : 1;
};
//--------------------------------------------------------------------------------
inline const DirectX::XMFLOAT3 * Transform::GetPosition() const { return reinterpret_cast<const DirectX::XMFLOAT3 *>(&const_cast<Transform *>(this)->GetWorldMatrix()._41); }
//--------------------------------------------------------------------------------
inline const DirectX::XMFLOAT3 * Transform::GetYAxis() const { return reinterpret_cast<const DirectX::XMFLOAT3 *>(&const_cast<Transform *>(this)->GetWorldMatrix()._21); }
//--------------------------------------------------------------------------------
inline const DirectX::XMFLOAT3 * Transform::GetZAxis() const { return reinterpret_cast<const DirectX::XMFLOAT3 *>(&const_cast<Transform *>(this)->GetWorldMatrix()._31); }
//--------------------------------------------------------------------------------