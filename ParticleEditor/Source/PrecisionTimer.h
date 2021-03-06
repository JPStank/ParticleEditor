//--------------------------------------------------------------------------------
// PrecisionTimer
// Written By: Justin Murphy
//
// uses the QueryPerformanceCounter for a highly accurate time representation
// supports time modulation
//--------------------------------------------------------------------------------
#ifndef PrecisionTimer_h
#define PrecisionTimer_h
//--------------------------------------------------------------------------------
#include <Windows.h>
//--------------------------------------------------------------------------------
class CPrecisionTimer {
public:
	CPrecisionTimer();
	~CPrecisionTimer(){}

	// initializes the timer system (applies for all timers created after this call)
	static void Init();

	static void GlobalUpdate();

	// returns the amount of time in seconds between each GlobalUpdate() call
	static inline float GetDeltaTime(const bool bUnmodulate = false);
	static inline void SetModulation(const float fModulation);
	static inline void SetTemporaryModulation(const float fModulation, const float fDur);

	void StartTimer();
	void PauseTimer();

	// by forcing timer query performance drastically decreases
	float GetElapsedTime(const bool bForceTimerQuery) const;
	inline float GetElapsedTime() const;
private:
	static LARGE_INTEGER m_liFreq;
	static LARGE_INTEGER m_liCurTime;

	static float m_fDeltaTime;
	static float m_fModulatedDeltaTime;

	static float m_fModulation;
	static bool  m_bTimedMod;
	static float m_fModDur;

	float m_fElapsedTime;
	bool m_bPaused;

	LARGE_INTEGER m_liStartTime;
};
//--------------------------------------------------------------------------------
inline float CPrecisionTimer::GetElapsedTime() const { return (m_bPaused ? m_fElapsedTime : (m_fElapsedTime + (float)(m_liCurTime.QuadPart - m_liStartTime.QuadPart) / m_liFreq.QuadPart) ); }
//--------------------------------------------------------------------------------
inline void CPrecisionTimer::SetModulation(const float fModulation) { m_fModulation = fModulation; }
//--------------------------------------------------------------------------------
inline void CPrecisionTimer::SetTemporaryModulation(const float fModulation, const float fDur) { m_fModulation = fModulation; m_fModDur = fDur; m_bTimedMod = true; }
//--------------------------------------------------------------------------------
inline float CPrecisionTimer::GetDeltaTime(const bool bUnmodulate) { return (bUnmodulate ? m_fDeltaTime : m_fModulatedDeltaTime); }
//--------------------------------------------------------------------------------
#endif // PrecisionTimer_h
//--------------------------------------------------------------------------------