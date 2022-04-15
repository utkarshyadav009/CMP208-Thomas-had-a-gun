#include "CameraMov.h"
#include <system/platform.h>
#include<Windows.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <maths/math_utils.h>
#include<input/input_manager.h>
#include <input/touch_input_manager.h>
#include<input/keyboard.h>
#include<maths/quaternion.h>

static const float mov_speed = 1000.f;
static const float rotate_speed = 30.f;

void CameraMov::update()
{
	float cosR, cosP, cosY;  //temp values for sin/cos from 
	float sinR, sinP, sinY;

	// Roll, Pitch and Yallare variables stored by the camera
	// handle rotation
	// Only want to calculate these values once, when rotation changes, not every frame. 
	cosY = cosf(Yaw * 3.1415 / 180);
	cosP = cosf(Pitch * 3.1415 / 180);
	cosR = cosf(Roll * 3.1415 / 180);
	sinY = sinf(Yaw * 3.1415 / 180);
	sinP = sinf(Pitch * 3.1415 / 180);
	sinR = sinf(Roll * 3.1415 / 180);

	//This using the parametric equation of a sphere
	// Calculate the three vectors to put into glu Lookat
	// Look direction, position and the up vector
	// This function could also calculate the right vector
	forward.set_x(sinY * cosP);
	forward.set_y(0.f);
	//forward.set_y(sinP);
	forward.set_z(cosP * -cosY);



	// Look At Point
	// To calculate add Forward Vector to Camera position.
	
	// Up Vector
	up.set_value(0.f, 1.f, 0.f);


	// Side Vector (right)
	// this is a cross product between the forward and up vector.
	// If you don’t need to calculate this, don’t do it. 
	right = forward.CrossProduct(up);

	right.Normalise();
	LookAt = position + forward;

}
void CameraMov::render(gef::Renderer3D* renderer) {

	renderer->DrawMesh(shotgun);
	renderer->DrawMesh(argun);
}
void CameraMov::handleInput(float dt, float width, float height,b2Body* player_body)
{
	timer = dt;
	timer_ -= dt;

	gef::Keyboard* keyboardManager = input->keyboard();
	const gef::TouchInputManager* touch_input = input->touch_manager();
	

	gef::Vector4 newpos = gef::Vector4::kZero;

	//camera movement 
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyDown(gef::Keyboard::KC_A))
		{
			newpos += right * (-mov_speed * dt);
		}
		if (keyboardManager->IsKeyDown(gef::Keyboard::KC_D))
		{

			newpos += right * (mov_speed * dt);
		}
		if (keyboardManager->IsKeyDown(gef::Keyboard::KC_W))
		{

			newpos += forward * (mov_speed * dt);
		}
		if (keyboardManager->IsKeyDown(gef::Keyboard::KC_S))
		{

			newpos += forward * (-mov_speed * dt);
		}
		if ((keyboardManager->IsKeyDown(gef::Keyboard::KC_A) || keyboardManager->IsKeyDown(gef::Keyboard::KC_D) || keyboardManager->IsKeyDown(gef::Keyboard::KC_S) || keyboardManager->IsKeyDown(gef::Keyboard::KC_W) )&& timer_ <= 0.f) 
		{
			audio_manager_->PlaySample(4, false);
			timer_ = 0.5f;
		}

		//setting player body 
		player_body->SetLinearVelocity({newpos.x(), newpos.z()});
		position.set_x(player_body->GetPosition().x);
		position.set_z(player_body->GetPosition().y);
	}

	//rotation for player 
	if (touch_input)
	{
		ShowCursor(false);
		mousex = touch_input->mouse_rel().x();

		Yaw += mousex * 0.08f;
	}

}
void CameraMov::setPostion(gef::Vector4 pos)
{
	position.set_value(pos.x(), pos.y(), pos.z());
}

void CameraMov::setGunPositions(std::string s )
{

	//setting gun position right infront of the camera 
	if (s == "shotgun")
	{
		shotgunposition = position + gef::Vector4(0.0f, -1.8f, 0.f) + (forward * 1.8f) + (right * 0.1f);
		gef::Matrix44 guntransform, guntranslation, gunrotation;
		gunrotation.SetIdentity();
		gunrotation.RotationY(-300.f + gef::DegToRad(-Yaw));
		guntranslation.SetIdentity();
		guntranslation.SetTranslation(shotgunposition);

		shotgun.set_transform(gunrotation * guntranslation);
	}
	if (s == "ar")
	{
		argunposition = position + gef::Vector4(0.0f, -2.f, 0.f) + (forward * 1.8f) + (right * 0.1f);
		gef::Matrix44 guntransform, guntranslation, gunrotation;
		gunrotation.SetIdentity();
		guntransform.SetIdentity();
		guntranslation.SetIdentity();
		guntranslation.Scale(gef::Vector4(0.1f, 0.1f, 0.1f));
		gunrotation.RotationY(-600.f+ gef::DegToRad(-Yaw));	
		guntranslation.SetTranslation(argunposition);
		argun.set_transform(gunrotation * guntranslation);
	}

}
