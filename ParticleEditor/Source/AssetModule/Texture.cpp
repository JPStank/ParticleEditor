/***********************************************
* Filename:           	Texture.cpp
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/06/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:            	Hold onto a collection of
*               	shader resource views used by models
************************************************/
#include "Texture.h"
#include "../DXLog.h"

CTexture::CTexture()
{
	for (int i = 0; i < eTEXTURE_TYPE_COUNT; ++i)
	{
		m_pd3dTextures[i] = nullptr;
	}
	m_szName = "";
}
CTexture::~CTexture()
{
	for (int i = 0; i < eTEXTURE_TYPE_COUNT; ++i)
	{
		SAFE_RELEASE(m_pd3dTextures[i]);
	}
}
void CTexture::AssignTexture(eTextureType type, ID3D11ShaderResourceView* texture)
{
	m_pd3dTextures[type] = texture;
}
void CTexture::SetName(const char* name)
{
	m_szName = name;
}

ID3D11ShaderResourceView* CTexture::GetDiffuse(void) const
{
	return m_pd3dTextures[eDIFFUSE];
}
ID3D11ShaderResourceView* CTexture::GetNormal(void) const
{
	return m_pd3dTextures[eNORMAL];
}
ID3D11ShaderResourceView* CTexture::GetSpecular(void) const
{
	return m_pd3dTextures[eSPECULAR];
}
std::string CTexture::GetName(void) const
{
	return m_szName;
}

