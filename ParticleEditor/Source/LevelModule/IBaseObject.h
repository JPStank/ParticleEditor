#pragma once
//--------------------------------------------------------------------------------
#include "../Interface.h"
#include "./Transform.h"
//--------------------------------------------------------------------------------
struct RenderNode;
//--------------------------------------------------------------------------------
CREATE_INTERFACE(IBaseObject) : public Transform {

	IBaseObject();
	virtual ~IBaseObject();
	
	void SetRenderNode(RenderNode *_node);
	virtual void UpdateRenderNode();
	virtual void Update(const float fDeltaTime) = 0;
	virtual void DestroyObject() {}

protected:
	RenderNode *m_renderNode;
};
//--------------------------------------------------------------------------------