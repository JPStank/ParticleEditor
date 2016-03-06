#include "./iBaseObject.h"
#include "../EventModule/EventManager.h"

//--------------------------------------------------------------------------------
IBaseObject::IBaseObject() :m_renderNode(nullptr) {}
//--------------------------------------------------------------------------------
IBaseObject::~IBaseObject() { }//delete m_renderNode; }
//--------------------------------------------------------------------------------
void IBaseObject::SetRenderNode(RenderNode *_node) { m_renderNode = _node; }
//--------------------------------------------------------------------------------
void IBaseObject::UpdateRenderNode() {
	Events::EventManager()->SendEventInstant("AddRenderNode", new Events::CGeneralEventArgs<RenderNode *>(m_renderNode));
}
//--------------------------------------------------------------------------------
