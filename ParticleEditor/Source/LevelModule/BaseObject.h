#pragma once
//--------------------------------------------------------------------------------
#include "../Interface.h"
#include "../RenderNode.h"
#include "./Transform.h"
//--------------------------------------------------------------------------------
CREATE_INTERFACE(BaseObject) {
	Transform m_location;

	BaseObject();
	virtual ~BaseObject();
	
	void SetRenderNode(RenderNode *_node);
	virtual void UpdateRenderNode();
	virtual void Update(const float fDeltaTime) = 0;
	virtual void DestroyObject() {}

protected:
	RenderNode *m_renderNode;
};
//--------------------------------------------------------------------------------