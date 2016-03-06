/***********************************************
* Filename:           	ParticleUpdaters.h
* Date :				02 / 09 / 2016
* Mod.Date :			02 / 09 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Define the different updates
				that a given system will apply to particles
************************************************/
#include "ParticleUpdaters.h"
#include "ParticleSystem.h"

CEulerUpdater::CEulerUpdater()
{
	m_d3dAcceleration = { 0.0f, 1.0f, 0.0f };
}
CEulerUpdater::CEulerUpdater(float3 acceleration)
{
	m_d3dAcceleration = acceleration;
}
CEulerUpdater::~CEulerUpdater()
{
}
void CEulerUpdater::Update(float dt, CParticleSystem* sys)
{
	float3 currAcc = { m_d3dAcceleration.x * dt, m_d3dAcceleration.y * dt, m_d3dAcceleration.z * dt };

	unsigned int end = sys->GetAlive();
	for (unsigned int i = 0; i < end; ++i)
	{
		sys->GetVelocity()[i].x += currAcc.x;
		sys->GetVelocity()[i].y += currAcc.y;
		sys->GetVelocity()[i].z += currAcc.z;

		sys->GetPosition()[i].x += sys->GetVelocity()[i].x * dt;
		sys->GetPosition()[i].y += sys->GetVelocity()[i].y * dt;
		sys->GetPosition()[i].z += sys->GetVelocity()[i].z * dt;
	}
}
CColorUpdater::CColorUpdater()
{
	m_d3dStartColor = { 0, 1, 0, 1 };
	m_d3dEndColor = { 1, 0, 0, 1 };
}
CColorUpdater::CColorUpdater(float4 startColor, float4 endColor)
{
	m_d3dStartColor = startColor;
	m_d3dEndColor = endColor;
}
CColorUpdater::~CColorUpdater()
{
}
void CColorUpdater::Update(float dt, CParticleSystem* sys)
{
	unsigned int end = sys->GetAlive();
	for (unsigned int i = 0; i < end; ++i)
	{
		sys->GetColor()[i] = float4{
			LERP(m_d3dStartColor.x, m_d3dEndColor.x, sys->GetTime()[i].z),
			LERP(m_d3dStartColor.y, m_d3dEndColor.y, sys->GetTime()[i].z),
			LERP(m_d3dStartColor.z, m_d3dEndColor.z, sys->GetTime()[i].z),
			LERP(m_d3dStartColor.w, m_d3dEndColor.w, sys->GetTime()[i].z)
		};
	}
}
CTimeUpdater::CTimeUpdater()
{
}
CTimeUpdater::~CTimeUpdater()
{
}
void CTimeUpdater::Update(float dt, CParticleSystem* sys)
{
	unsigned int end = sys->GetAlive();
	for (unsigned int i = 0; i < end; ++i)
	{
		sys->GetTime()[i].x -= dt;
		// generate interpolator value for .z using 1/max which is .w
		sys->GetTime()[i].z = 1.0f - (sys->GetTime()[i].x * sys->GetTime()[i].w);

		if (sys->GetTime()[i].x < 0.0f)
		{
			sys->Kill(i);
			end = sys->GetAlive();
		}
	}

}
