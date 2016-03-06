//--------------------------------------------------------------------------------
// Entity3D
// Written By: Justin Murphy
//
// very simple entity class holding a transform to represent 
// position and orientation of an object in 3D space
//--------------------------------------------------------------------------------
#ifndef Entity3D_h
#define Entity3D_h
//--------------------------------------------------------------------------------
#include <stdint.h>

#include "./IBaseObject.h"
//--------------------------------------------------------------------------------
class Entity3D :public IBaseObject {
	
public:
	enum class UpdateTypes : int8_t {ROTATE, SPIN, NUM_UPDATE_TYPES};
	Entity3D( );
	virtual ~Entity3D( );

	void Update(const float fDeltaTime) override;
	void DestroyObject() override;
	 

	bool SetUpdateType(UpdateTypes _type);
	void UpdateRenderNode() override;
protected:
	typedef IBaseObject super;
private:
	
	float totalTime;
	typedef void(Entity3D::*UpdateFuncs)(const float fDeltaTime);
	void UpdateSpin(const float fDeltaTime);
	void UpdateRotate(const float fDeltaTime);

	UpdateTypes type;
	UpdateFuncs func[static_cast<unsigned int>(UpdateTypes::NUM_UPDATE_TYPES)];
};
//--------------------------------------------------------------------------------
#endif // Entity3D_h
//--------------------------------------------------------------------------------