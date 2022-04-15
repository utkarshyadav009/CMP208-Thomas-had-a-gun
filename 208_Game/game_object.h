#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>

enum ObjectType {

	PLAYER,
	ENEMY,
	BULLET
};
class GameObject : public gef::MeshInstance
{
public:
	//updates the simulation 
	void UpdateFromSimulation(const b2Body* body);
	ObjectType type_member;
	float y_pos = 0.f;
 
};

#endif // _GAME_OBJECT_H