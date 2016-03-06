#pragma once
//--------------------------------------------------------------------------------
#include <stdint.h>

#include "./Interface.h"
#include "./RenderNodeInfo.h"
//--------------------------------------------------------------------------------
CREATE_INTERFACE(RenderNode) {
	enum class DataType :int8_t {WOLRD, VIEWPROJECTION, MAX_TYPES};
	bool SetType(DataType _type);
	DataType GetType() const;
private:
	DataType type;
};
//--------------------------------------------------------------------------------
template <typename NodeData>
class RenderData :public RenderNode {
public:
	RenderData();
	void SetData(NodeData &_data);
	NodeData & GetData();
	const NodeData & GetData() const;
private:
	NodeData data;
};
//--------------------------------------------------------------------------------
inline bool RenderNode::SetType(DataType _type) { if (DataType::MAX_TYPES > _type) { type = _type; return true; } return false; }
inline RenderNode::DataType RenderNode::GetType() const { return type; }
template <typename NodeData> NodeData & RenderData<NodeData>::GetData() { return data; }
template <typename NodeData> const NodeData & RenderData<NodeData>::GetData() const { return data; }

template<> inline RenderData<PerObject>::RenderData() { SetType(RenderNode::DataType::WOLRD); };
template<> inline void RenderData<PerObject>::SetData(PerObject & _data) { data.mWorld = _data.mWorld; };
template<> inline RenderData<PerFrame>::RenderData() { SetType(RenderNode::DataType::VIEWPROJECTION); };
template<> inline void RenderData<PerFrame>::SetData(PerFrame & _data) { data.mProjection = _data.mProjection; data.mView = _data.mView; };
//--------------------------------------------------------------------------------