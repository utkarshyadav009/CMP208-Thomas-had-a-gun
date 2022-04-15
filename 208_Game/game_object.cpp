#include "game_object.h"

//
// UpdateFromSimulation
// 
// Update the transform of this object from a physics rigid body
//
void GameObject::UpdateFromSimulation(const b2Body* body)
{
	if (body)
	{
		// setup object rotation to Y axis 
		gef::Matrix44 object_rotation;
		object_rotation.RotationY(body->GetAngle());

		// setup the object translation making y constant to not move in that direction 
		gef::Vector4 object_translation(body->GetPosition().x, y_pos, body->GetPosition().y);

		// build object transformation matrix
		gef::Matrix44 object_transform = object_rotation;
		object_transform.SetTranslation(object_translation);
		set_transform(object_transform);
	}
}
