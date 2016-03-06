#include <hash_map>

#include "./ObjectRegistry.h"
#include "../Log.h"
#include "./IBaseObject.h"
#include "../RenderNode.h"
//--------------------------------------------------------------------------------
unsigned int ObjectRegistry::RegisterType(const char* identifier, ObjectCreator creator) {

	unsigned int hash = (unsigned int)std::hash<std::string>()(identifier);
	objItr itr = creatorMap.find(hash);
	if (itr == creatorMap.end()) {
		creatorMap.insert(std::make_pair(hash, creator) );
		stringMap.insert(std::make_pair(hash, identifier) );
	}
	else {
		Log("Name collision in ObjectRegistry::RegisterType ", identifier, " matches registered object ", stringMap[hash]);
	}
	
	return hash;
}
unsigned int ObjectRegistry::RegisterNode(const char* identifier, RenderNodeCreator creator) {
	unsigned int hash = (unsigned int)std::hash<std::string>()(identifier);
	nodeItr itr = NodeMap.find(hash);
	if (itr == NodeMap.end()) {
		NodeMap.insert(std::make_pair(hash, creator));
		stringMap.insert(std::make_pair(hash, identifier));
	}
	else {
		Log("Name collision in ObjectRegistry::RegisterNode ", identifier, " matches registered object ", stringMap[hash]);
	}

	return hash;
}
unsigned int ObjectRegistry::RegisterModule(const char* identifier, ModuleCreator creator) {
	unsigned int hash = (unsigned int)std::hash<std::string>()(identifier);
	moduleItr itr = ModuleMap.find(hash);
	if (itr == ModuleMap.end()) {
		ModuleMap.insert(std::make_pair(hash, creator));
		stringMap.insert(std::make_pair(hash, identifier));
	}
	else {
		itr->second = creator;
	}

	return hash;
}
//--------------------------------------------------------------------------------
IBaseObject *ObjectRegistry::CreateObject(const char *_key) {
	unsigned int hash = (unsigned int)std::hash<std::string>()(_key);
	objItr itr = creatorMap.find(hash);
	if (itr == creatorMap.end()) {
		Log("No Objects registered with the name ", _key);
		return nullptr;
	}
	unsigned int index = Objects.size();
	Objects.push_back(itr->second() );

	return Objects[index];
}
//--------------------------------------------------------------------------------
RenderNode *ObjectRegistry::CreateNode(const char *_key) {
	unsigned int hash = (unsigned int)std::hash<std::string>()(_key);
	nodeItr itr = NodeMap.find(hash);
	if (itr == NodeMap.end()) {
		Log("No Render Nodes registered with the name ", _key);
		return nullptr;
	}
	unsigned int index = Nodes.size();
	Nodes.push_back(itr->second());

	return Nodes[index];
}
IBaseModule * ObjectRegistry::CreateModule(const char *_key) {
	unsigned int hash = (unsigned int)std::hash<std::string>()(_key);
	moduleItr itr = ModuleMap.find(hash);
	if (itr == ModuleMap.end()) {
		Log("No Render Nodes registered with the name ", _key);
		return nullptr;
	}
	return itr->second();
}
//--------------------------------------------------------------------------------
void ObjectRegistry::Update(float _DeltaTime) {
	unsigned int numObjects = Objects.size();
	for (unsigned int i = 0; i < numObjects; ++i) {
		Objects[i]->Update(_DeltaTime);
	}
}
//--------------------------------------------------------------------------------
void ObjectRegistry::ShutDown() {
	unsigned int numObjects = Objects.size();
	for (unsigned int i = 0; i < numObjects; ++i) {
		Objects[i]->DestroyObject();
		delete Objects[i];
	}
	Objects.clear();

	numObjects = Nodes.size();
	for (unsigned int i = 0; i < numObjects; ++i) {
		delete Nodes[i];
	}
	Nodes.clear();

	creatorMap.clear();
	NodeMap.clear();
	ModuleMap.clear();
	stringMap.clear();
}
//--------------------------------------------------------------------------------
