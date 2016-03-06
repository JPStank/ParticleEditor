#include "./BaseObject.h"
#include "../EventModule/EventManager.h"

using namespace Events;
//--------------------------------------------------------------------------------
BaseObject::BaseObject() :m_renderNode(nullptr) {}
//--------------------------------------------------------------------------------
BaseObject::~BaseObject() { }//delete m_renderNode; }
//--------------------------------------------------------------------------------
void BaseObject::SetRenderNode(RenderNode *_node) { m_renderNode = _node; }
//--------------------------------------------------------------------------------
void BaseObject::UpdateRenderNode() {
	EventManager()->SendEventInstant("AddRenderNode", new CGeneralEventArgs<RenderNode *>(m_renderNode));
}
//--------------------------------------------------------------------------------
