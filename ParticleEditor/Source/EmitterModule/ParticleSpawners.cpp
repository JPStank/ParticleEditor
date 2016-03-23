/***********************************************
* Filename:           	ParticleSpawners.h
* Date :				02 / 09 / 2016
* Mod.Date :			03 / 08 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Define the different ways
that a particle can be spawned into
a given system
************************************************/
#include "ParticleSpawners.h"
#include "ParticleSystem.h"

CBoxSpawner::CBoxSpawner()
{
	m_d3dCenter = { 0, 0, 0 };
	m_d3dOffset = { 1, 1, 1 };
}
CBoxSpawner::CBoxSpawner(float3 center, float3 offset)
{
	m_d3dCenter = center;
	m_d3dOffset = offset;
}
CBoxSpawner::~CBoxSpawner()
{
}

void CBoxSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	float3 min = { m_d3dCenter.x - m_d3dOffset.x, m_d3dCenter.y - m_d3dOffset.y, m_d3dCenter.z - m_d3dOffset.z };
	float3 max = { m_d3dCenter.x + m_d3dOffset.x, m_d3dCenter.y + m_d3dOffset.y, m_d3dCenter.z + m_d3dOffset.z };

	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetPosition()[i] = float4{
			RAND_FLOAT(min.x, max.x),
			RAND_FLOAT(min.y, max.y),
			RAND_FLOAT(min.z, max.z),
			1.0f
		};
	}
}
CCircleSpawner::CCircleSpawner()
{
	m_d3dCenter = { 0, 0, 0 };
	m_fRadius = 1.0f;
	m_fSpeed = 1.0f;
	m_szAxis = "xy";
}
CCircleSpawner::~CCircleSpawner()
{

}
CCircleSpawner::CCircleSpawner(float3 center, float radius, float speed, std::string axis)
{
	m_d3dCenter = center;
	m_fRadius = radius;
	m_fSpeed = speed;
	m_szAxis = axis;
}
void CCircleSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	float angle = RAND_FLOAT(0, DirectX::XM_2PI);
	float s = sin(angle);
	float c = cos(angle);

	if (m_szAxis == "xy")
	{
		for (unsigned int i = start; i < end; ++i)
		{
			float radius = RAND_FLOAT(0, m_fRadius);
			sys->GetPosition()[i] = float4(
				(radius * s) + m_d3dCenter.x,
				(radius * c) + m_d3dCenter.y,
				m_d3dCenter.z,
				1.0f);

			sys->GetVelocity()[i].x = s * m_fSpeed;
			sys->GetVelocity()[i].y = c * m_fSpeed;
			sys->GetVelocity()[i].z = 0.0f;
		}
	}
	else if (m_szAxis == "yz")
	{
		for (unsigned int i = start; i < end; ++i)
		{
			float radius = RAND_FLOAT(0, m_fRadius);
			sys->GetPosition()[i] = float4(
				m_d3dCenter.x,
				(radius * s) + m_d3dCenter.y,
				(radius * c) + m_d3dCenter.z,
				1.0f);

			sys->GetVelocity()[i].x = 0.0f;
			sys->GetVelocity()[i].y = s * m_fSpeed;
			sys->GetVelocity()[i].z = c * m_fSpeed;
		}
	}
	else if (m_szAxis == "xz")
	{
		for (unsigned int i = start; i < end; ++i)
		{
			float radius = RAND_FLOAT(0, m_fRadius);
			sys->GetPosition()[i] = float4(
				(radius * s) + m_d3dCenter.x,
				m_d3dCenter.y,
				(radius * c) + m_d3dCenter.z,
				1.0f);

			sys->GetVelocity()[i].x = s * m_fSpeed;
			sys->GetVelocity()[i].y = 0.0f;
			sys->GetVelocity()[i].z = c * m_fSpeed;
		}
	}
}
CCircleRandSpeedSpawner::CCircleRandSpeedSpawner()
{
	m_d3dCenter = { 0, 0, 0 };
	m_fRadius = 1.0f;
	m_fMinSpeed = 0.1f;
	m_fMaxSpeed = 2.0f;
	m_szAxis = "xy";
}
CCircleRandSpeedSpawner::~CCircleRandSpeedSpawner()
{
}
CCircleRandSpeedSpawner::CCircleRandSpeedSpawner(float3 center, float radius, float minSpeed, float maxSpeed, std::string axis)
{
	m_d3dCenter = center;
	m_fRadius = radius;
	m_fMinSpeed = minSpeed;
	m_fMaxSpeed = maxSpeed;
	m_szAxis = axis;
}
void CCircleRandSpeedSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	float angle = RAND_FLOAT(0, DirectX::XM_2PI);
	float s = sin(angle);
	float c = cos(angle);

	if (m_szAxis == "xy")
	{
		for (unsigned int i = start; i < end; ++i)
		{
			float radius = RAND_FLOAT(0, m_fRadius);
			sys->GetPosition()[i] = float4(
				(radius * s) + m_d3dCenter.x,
				(radius * c) + m_d3dCenter.y,
				m_d3dCenter.z,
				1.0f);

			float speed = RAND_FLOAT(m_fMinSpeed, m_fMaxSpeed);

			sys->GetVelocity()[i].x = s * speed;
			sys->GetVelocity()[i].y = c * speed;
			sys->GetVelocity()[i].z = 0.0f;
		}
	}
	else if (m_szAxis == "yz")
	{
		for (unsigned int i = start; i < end; ++i)
		{
			float radius = RAND_FLOAT(0, m_fRadius);
			sys->GetPosition()[i] = float4(
				m_d3dCenter.x,
				(radius * s) + m_d3dCenter.y,
				(radius * c) + m_d3dCenter.z,
				1.0f);

			float speed = RAND_FLOAT(m_fMinSpeed, m_fMaxSpeed);

			sys->GetVelocity()[i].x = 0.0f;
			sys->GetVelocity()[i].y = s * speed;
			sys->GetVelocity()[i].z = c * speed;
		}
	}
	else if (m_szAxis == "xz")
	{
		for (unsigned int i = start; i < end; ++i)
		{
			float radius = RAND_FLOAT(0, m_fRadius);
			sys->GetPosition()[i] = float4(
				(radius * s) + m_d3dCenter.x,
				m_d3dCenter.y,
				(radius * c) + m_d3dCenter.z,
				1.0f);

			float speed = RAND_FLOAT(m_fMinSpeed, m_fMaxSpeed);

			sys->GetVelocity()[i].x = s * speed;
			sys->GetVelocity()[i].y = 0.0f;
			sys->GetVelocity()[i].z = c * speed;
		}
	}
}

CConeSpawner::CConeSpawner()
{
	m_d3dCenter = { 0, 0, 0 };
	m_fHalfAngle = 30.0f;
	m_fSpeed = 1.0f;
}
CConeSpawner::~CConeSpawner()
{

}
CConeSpawner::CConeSpawner(float3 center, float halfAngle, float speed)
{
	m_d3dCenter = center;
	m_fHalfAngle = halfAngle;
	m_fSpeed = speed;
}

void CConeSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	using namespace DirectX;
	float theta, r, rad, length;
	rad = XMConvertToRadians(m_fHalfAngle);

	// TODO: look into making this a custom variale to allow spawning anywhere in cone, not just at origin point
	length = 1.0f;
	r = length * tanf(rad);
	for (unsigned int i = start; i < end; ++i)
	{
		theta = RAND_FLOAT(-XM_PI, XM_PI);

		sys->GetPosition()[i] = float4(
			m_d3dCenter.x,
			m_d3dCenter.y,
			m_d3dCenter.z,
			1.0f);

		float3 pointOnCircle = float3(
			m_d3dCenter.x + sinf(theta)*r,
			m_d3dCenter.y + cosf(theta)*r,
			m_d3dCenter.z + length);

		float3 vel;
		XMStoreFloat3(&vel, XMVECTOR{ { m_fSpeed, m_fSpeed, m_fSpeed, 0.0f } } *XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&pointOnCircle), XMLoadFloat3(&m_d3dCenter))));


		sys->GetVelocity()[i].x = vel.x;
		sys->GetVelocity()[i].y = vel.y;
		sys->GetVelocity()[i].z = vel.z;
	}
}
CConeRandSpeedSpawner::CConeRandSpeedSpawner()
{
	m_d3dCenter = { 0, 0, 0 };
	m_fHalfAngle = 30.0f;
	m_fMinSpeed = 0.5f;
	m_fMaxSpeed = 3.0f;
}
CConeRandSpeedSpawner::~CConeRandSpeedSpawner()
{
}
CConeRandSpeedSpawner::CConeRandSpeedSpawner(float3 center, float halfAngle, float minSpeed, float maxSpeed)
{
	m_d3dCenter = center;
	m_fHalfAngle = halfAngle;
	m_fMinSpeed = minSpeed;
	m_fMaxSpeed = maxSpeed;
}
void CConeRandSpeedSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	using namespace DirectX;
	float theta, r, rad, length;
	rad = XMConvertToRadians(m_fHalfAngle);

	// TODO: look into making this a custom variale to allow spawning anywhere in cone, not just at origin point
	length = 1.0f;
	r = length * tanf(rad);
	for (unsigned int i = start; i < end; ++i)
	{
		theta = RAND_FLOAT(-XM_PI, XM_PI);

		sys->GetPosition()[i] = float4(
			m_d3dCenter.x,
			m_d3dCenter.y,
			m_d3dCenter.z,
			1.0f);

		float3 pointOnCircle = float3(
			m_d3dCenter.x + sinf(theta)*r,
			m_d3dCenter.y + cosf(theta)*r,
			m_d3dCenter.z + length);

		float3 vel;
		float speed = RAND_FLOAT(m_fMinSpeed, m_fMaxSpeed);
		XMStoreFloat3(&vel, XMVECTOR{ { speed, speed, speed, 0.0f } } *XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&pointOnCircle), XMLoadFloat3(&m_d3dCenter))));


		sys->GetVelocity()[i].x = vel.x;
		sys->GetVelocity()[i].y = vel.y;
		sys->GetVelocity()[i].z = vel.z;
	}

}

CSphereVelocitySpawner::CSphereVelocitySpawner()
{
	m_fSpeed = 2.0f;
}
CSphereVelocitySpawner::CSphereVelocitySpawner(float speed)
{
	m_fSpeed = speed;
}
CSphereVelocitySpawner::~CSphereVelocitySpawner()
{
}

void CSphereVelocitySpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	float phi, theta, r;
	for (unsigned int i = start; i < end; ++i)
	{
		phi = RAND_FLOAT(-DirectX::XM_PI, DirectX::XM_PI);
		theta = RAND_FLOAT(-DirectX::XM_PI, DirectX::XM_PI);
		r = m_fSpeed * sinf(phi);

		sys->GetVelocity()[i].x = m_fSpeed * cosf(phi);
		sys->GetVelocity()[i].y = r * cosf(theta);
		sys->GetVelocity()[i].z = r * sinf(theta);
	}
}
CSphereRandomVelocitySpawner::CSphereRandomVelocitySpawner()
{
	m_fMinSpeed = 0.5f;
	m_fMaxSpeed = 2.0f;
}
CSphereRandomVelocitySpawner::~CSphereRandomVelocitySpawner()
{
}
CSphereRandomVelocitySpawner::CSphereRandomVelocitySpawner(float minSpeed, float maxSpeed)
{
	m_fMinSpeed = minSpeed;
	m_fMaxSpeed = maxSpeed;
}
void CSphereRandomVelocitySpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	float phi, theta, r, speed;
	for (unsigned int i = start; i < end; ++i)
	{
		phi = RAND_FLOAT(-DirectX::XM_PI, DirectX::XM_PI);
		theta = RAND_FLOAT(-DirectX::XM_PI, DirectX::XM_PI);
		speed = RAND_FLOAT(m_fMinSpeed, m_fMaxSpeed);
		r = speed * sinf(phi);

		sys->GetVelocity()[i].x = speed * cosf(phi);
		sys->GetVelocity()[i].y = r * cosf(theta);
		sys->GetVelocity()[i].z = r * sinf(theta);
	}

}

CUniformTimeSpawner::CUniformTimeSpawner()
{
	m_fDuration = 5.0f;
}
CUniformTimeSpawner::CUniformTimeSpawner(float duration)
{
	m_fDuration = duration;
}
CUniformTimeSpawner::~CUniformTimeSpawner()
{
}

void CUniformTimeSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetTime()[i].x = sys->GetTime()[i].y = m_fDuration;
		sys->GetTime()[i].z = 0.0f; // will be used to keep interpolated value
		sys->GetTime()[i].w = 1.0f / m_fDuration; // used to calculate
	}
}
CRandomTimeSpawner::CRandomTimeSpawner()
{
	m_fMinDuration = 0.5f;
	m_fMaxDuration = 2.0f;
}
CRandomTimeSpawner::~CRandomTimeSpawner()
{

}
CRandomTimeSpawner::CRandomTimeSpawner(float minDuration, float maxDuration)
{
	m_fMinDuration = minDuration;
	m_fMaxDuration = maxDuration;
}

void CRandomTimeSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	for (unsigned int i = start; i < end; ++i)
	{
		float duration = RAND_FLOAT(m_fMinDuration, m_fMaxDuration);

		sys->GetTime()[i].x = sys->GetTime()[i].y = duration;
		sys->GetTime()[i].z = 0.0f; // will be used to keep interpolated value
		sys->GetTime()[i].w = 1.0f / duration; // used to calculate
	}
}

CUniformScaleSpawner::CUniformScaleSpawner()
{
	m_fScale = 0.1f;
}
CUniformScaleSpawner::~CUniformScaleSpawner()
{
}
CUniformScaleSpawner::CUniformScaleSpawner(float scale)
{
	m_fScale = scale;
}
void CUniformScaleSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetScale()[i] = m_fScale;
	}
}
CRandomScaleSpawner::CRandomScaleSpawner()
{
	m_fMinScale = 0.1f;
	m_fMaxScale = 1.0f;
}
CRandomScaleSpawner::~CRandomScaleSpawner()
{
}
CRandomScaleSpawner::CRandomScaleSpawner(float minScale, float maxScale)
{
	m_fMinScale = minScale;
	m_fMaxScale = maxScale;
}
void CRandomScaleSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetScale()[i] = RAND_FLOAT(m_fMinScale, m_fMaxScale);
	}
}

CUniformRotationSpawner::CUniformRotationSpawner()
{
	m_fRotation = 0.0f;
}
CUniformRotationSpawner::~CUniformRotationSpawner()
{

}
CUniformRotationSpawner::CUniformRotationSpawner(float degrees)
{
	m_fRotation = degrees;
}
void CUniformRotationSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetRotation()[i] = m_fRotation;
	}
}
CRandomRotationSpawner::CRandomRotationSpawner()
{
	m_fMinRotation = 0.0f;
	m_fMaxRotation = 360.0f;
}
CRandomRotationSpawner::~CRandomRotationSpawner()
{
}
CRandomRotationSpawner::CRandomRotationSpawner(float minRot, float maxRot)
{
	m_fMinRotation = minRot;
	m_fMaxRotation = maxRot;
}
void CRandomRotationSpawner::Spawn(CParticleSystem* sys, unsigned int start, unsigned int end)
{
	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetRotation()[i] = RAND_FLOAT(m_fMinRotation, m_fMaxRotation);
	}
}
