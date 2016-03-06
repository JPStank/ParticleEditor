/***********************************************
* Filename:            Texture.h
* Date:             02/06/2016
* Mod. Date:         02/06/2016
* Mod. Initials:  JS
* Author:           Joshua Stankiewicz
* Purpose:             Hold onto a collection of
*                shader resource views used by models
************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <string>

class CTexture
{
public:
	CTexture();
	~CTexture();

	// used to index into m_pd3dTextures
	enum eTextureType  { eDIFFUSE = 0, eNORMAL, eSPECULAR, eTEXTURE_TYPE_COUNT };

	void AssignTexture(eTextureType type, ID3D11ShaderResourceView* texture);
	void SetName(const char* name);

	ID3D11ShaderResourceView* GetDiffuse(void) const;
	ID3D11ShaderResourceView* GetNormal(void) const;
	ID3D11ShaderResourceView* GetSpecular(void) const;
	std::string GetName(void) const;

private:
	ID3D11ShaderResourceView* m_pd3dTextures[eTEXTURE_TYPE_COUNT];
	std::string m_szName;
};

#endif//_TEXTURE_H_