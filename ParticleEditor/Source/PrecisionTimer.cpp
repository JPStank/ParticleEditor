//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "./PrecisionTimer.h"
//--------------------------------------------------------------------------------
LARGE_INTEGER CPrecisionTimer::m_liFreq = LARGE_INTEGER();
LARGE_INTEGER CPrecisionTimer::m_liCurTime = LARGE_INTEGER();
float CPrecisionTimer::m_fDeltaTime = 0;
float CPrecisionTimer::m_fModulatedDeltaTime = 0;
float CPrecisionTimer::m_fModulation = 1.0f;
bool CPrecisionTimer::m_bTimedMod = false;
float CPrecisionTimer::m_fModDur = 0.0f;
//--------------------------------------------------------------------------------
CPrecisionTimer::CPrecisionTimer() {
	m_fElapsedTime = 0;
	m_bPaused = true;
}
//--------------------------------------------------------------------------------
void CPrecisionTimer::Init() {
	QueryPerformanceFrequency(&m_liFreq);
	QueryPerformanceCounter(&m_liCurTime);
}
//--------------------------------------------------------------------------------
void CPrecisionTimer::GlobalUpdate() {
	LONGLONG llPrevTime = m_liCurTime.QuadPart;
	QueryPerformanceCounter(&m_liCurTime);
	m_fDeltaTime = (float(m_liCurTime.QuadPart-llPrevTime) / m_liFreq.QuadPart);

	// if modulating for a limited time
	if (m_bTimedMod) {
		// update the time
		m_fModDur-= m_fDeltaTime;

		// out of time?
		if (m_fModDur <= 0) {
			// set modulation back to normal speed
			m_fModulation = 1.0f;
			m_bTimedMod = false;
		}
	}

	m_fModulatedDeltaTime = m_fDeltaTime * m_fModulation;
}
//--------------------------------------------------------------------------------
void CPrecisionTimer::StartTimer() {
	QueryPerformanceCounter(&m_liStartTime);
	m_bPaused = false;
}
//--------------------------------------------------------------------------------
void CPrecisionTimer::PauseTimer() {
	m_fElapsedTime = GetElapsedTime(true);
	QueryPerformanceCounter(&m_liStartTime);
}
//--------------------------------------------------------------------------------
float CPrecisionTimer::GetElapsedTime(bool bForceTimerQuery) const {
	if (bForceTimerQuery)
		QueryPerformanceCounter(&m_liCurTime);

	return (m_bPaused ? m_fElapsedTime : (m_fElapsedTime + (float)(m_liCurTime.QuadPart - m_liStartTime.QuadPart)/m_liFreq.QuadPart) );
}
//--------------------------------------------------------------------------------