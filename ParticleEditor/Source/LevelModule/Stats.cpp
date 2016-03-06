#include "Stats.h"

Stats::Stats()
{

}

Stats::~Stats()
{

}

IComponent::eComponentType Stats::GetType()
{
	// TODO: Change the enum in IComponent to support stats
	return eComponentType::ePLAYER;
}

