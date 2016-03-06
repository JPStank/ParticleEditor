//--------------------------------------------------------------------------------
// Camera
// Written By: Justin Murphy
// basic debug camera to fly through scene, uses mouse move for rotation and wasd for movement
//--------------------------------------------------------------------------------
#ifndef Camera_h
#define Camera_h
//--------------------------------------------------------------------------------
#include "./Entity3D.h"
#include "../EventModule/EventArgs.h"
//--------------------------------------------------------------------------------
class Camera : public Entity3D {
public:
	void LookAt(const DirectX::XMFLOAT3 eye, const DirectX::XMFLOAT3 at, const DirectX::XMFLOAT3 up);
	void SetSpeed(float s);
	virtual void SetLens(float fov, float aspect, float nearZ, float farZ);
	void Update(const float fDeltaTime) override;

	const DirectX::XMFLOAT4X4 GetView() const;
	const DirectX::XMFLOAT4X4 GetProjection() const;
	const DirectX::XMFLOAT4X4 GetViewProjection() const;
	void DestroyObject() override;

	Camera();
	~Camera();
	void UpdateRenderNode() override;
private:
	DirectX::XMFLOAT4X4	m_mtxView;
	DirectX::XMFLOAT4X4	m_mtxProj;
	DirectX::XMFLOAT4X4	m_mtxViewProj;

	float m_fNear;
	float m_fFar;
	float m_fFOV;
	float m_fAspectRatio;

	DirectX::XMFLOAT3 m_vec3RightW;
	DirectX::XMFLOAT3 m_vec3UpW;
	DirectX::XMFLOAT3 m_vec3LookW;
	DirectX::XMFLOAT3 m_vec3Dir;

	float m_fMouseDX;
	float m_fMouseDY;
	float m_fSpeed;

	void BuildView();

	//event handlers
	void OnMoveForwardKeyDown(const Events::CGeneralEventArgs<float>& args);
	void OnMoveBackKeyDown(const Events::CGeneralEventArgs<float>& args);
	void OnMoveLeftKeyDown(const Events::CGeneralEventArgs<float>& args);
	void OnMoveRightKeyDown(const Events::CGeneralEventArgs<float>& args);
	void OnMouseLDown(const Events::CGeneralEventArgs<float>& args);
	void SpeedUp(const Events::CBaseEventArgs&);
	void SlowDown(const Events::CBaseEventArgs&);
};
//--------------------------------------------------------------------------------
inline const DirectX::XMFLOAT4X4 Camera::GetView() const { return m_mtxView; }
//--------------------------------------------------------------------------------
inline const DirectX::XMFLOAT4X4 Camera::GetProjection() const { return m_mtxProj; }
//--------------------------------------------------------------------------------
inline const DirectX::XMFLOAT4X4 Camera::GetViewProjection() const { return m_mtxViewProj; }
//--------------------------------------------------------------------------------
#endif // Camera_h
//--------------------------------------------------------------------------------