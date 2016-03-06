#pragma once
#include <string>

class RenderType {
public:
	RenderType() {};
	virtual ~RenderType() {};
private:
	std::string m_ps;
	std::string m_vs;
	std::string m_geomerty;
};