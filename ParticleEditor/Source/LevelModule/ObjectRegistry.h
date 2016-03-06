#pragma once
//--------------------------------------------------------------------------------
#include <map>
#include <vector>

#include "../RenderNode.h"

//--------------------------------------------------------------------------------
struct __declspec(novtable) IBaseObject;
struct __declspec(novtable) IBaseModule;
//--------------------------------------------------------------------------------
class ObjectRegistry {
public:
	typedef IBaseObject* (*ObjectCreator)(void);
	typedef RenderNode * (*RenderNodeCreator)();
	typedef IBaseModule * (*ModuleCreator)();

	template<typename T>
	friend IBaseObject* objCreator(void){ return new T; }

	template<typename T>
	friend RenderNode* nodeCreator(void){ return new RenderData<T>; }

	template<typename T>
	friend IBaseModule* moduleCreator(void){ return new T; }

	template<typename T>
	unsigned int RegisterType(const char* identifier) {
		return RegisterType(identifier, &objCreator<T>);
	}
	template<typename T>
	unsigned int RegisterRenderType(const char* identifier) {
		return RegisterNode(identifier, &nodeCreator<T>);
	}
	template<typename T>
	unsigned int RegisterModule(const char* identifier) {
		return RegisterModule(identifier, &moduleCreator<T>);
	}
	IBaseObject * CreateObject(const char *_key);
	RenderNode * CreateNode(const char *_key);
	//Creates an object, but does not store the memory
	//The caller of this function is responible for the object and it's memory
	IBaseModule * CreateModule(const char *_key);
	void Update(float _DeltaTime);
	void ShutDown();
protected:
	unsigned int RegisterType(const char* identifier, ObjectCreator creator);
	unsigned int RegisterNode(const char* identifier, RenderNodeCreator creator);
	unsigned int RegisterModule(const char* identifier, ModuleCreator creator);
private:
	std::vector<IBaseObject *> Objects;
	std::vector<RenderNode *> Nodes;
	//TODO get all major modules that can get overriden into one interface base class
	std::vector<IBaseModule *> modules;
	std::map<unsigned int, ObjectCreator> creatorMap;
	std::map<unsigned int, RenderNodeCreator> NodeMap;
	std::map<unsigned int, ModuleCreator> ModuleMap;
	std::map<unsigned int, std::string> stringMap;
	typedef std::map<unsigned int, ObjectCreator>::iterator objItr;
	typedef std::map<unsigned int, RenderNodeCreator>::iterator nodeItr;
	typedef std::map<unsigned int, ModuleCreator>::iterator moduleItr;
};
//--------------------------------------------------------------------------------