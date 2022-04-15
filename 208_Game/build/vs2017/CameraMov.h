#pragma once
#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include<input/input_manager.h>
#include<input/keyboard.h>
#include <graphics/mesh_instance.h>
#include <graphics/renderer_3d.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include<iostream>
#include "primitive_builder.h"
#include<audio/audio_manager.h>

using std::cout;
using std::cin;



class CameraMov
{
private:
	//vectors for camera movement 
	gef::Vector4 position;
	gef::Vector4 rotation;
	gef::Vector4 LookAt;
	gef::Vector4 up;
	gef::Vector4 forward;
	gef::Vector4 right;
	gef::Vector4 move;
	

	//mouse movement 
	float mousex, mousey;
	float Pitch = 0, Yaw = 0, Roll = 0;
	float centre[2];
	Int32 active_touch_id_;
	gef::Vector2 touch_position_;
	gef::InputManager* input;

	//shotgun object 
	GameObject shotgun;
	gef::Vector4 shotgunposition;

	//ar object
	GameObject argun;
	gef::Vector4 argunposition;

	//fot playing the sounds properly
	float timer = 0;
	float timer_ = 0;
	
	//managing meshes and audio
	PrimitiveBuilder* primitive_builder_;
	gef::AudioManager* audio_manager_;



	
public:
	
	CameraMov() {

		 position = { 0.f, 3.f, 25.f };
		 LookAt= gef::Vector4::kZero;

		 up = { 0.f, 1.f, 0.f };
		 forward = { 0.f, 0.f, 1.f };
		 right= { 1.f, 0.f, 0.f };

	}
	//updating camera poistion based on user inut 
	void update();
	//rendering the guns 
	void render(gef::Renderer3D* renderer);

	//initialising the meshes and input and audio variables 
	void CameraInit(gef::InputManager* in, gef::MeshInstance model[2],gef::AudioManager* audio_) {
		input = in;
		gef::Matrix44 rotate,translate;
		//rotate.SetIdentity();
		translate.SetIdentity();
		//rotate.RotationY(-300.f);
		translate.SetTranslation(gef::Vector4(1000, 100, 0));
		shotgun.set_mesh(model[0].mesh());
		shotgun.set_transform(translate);

		gef::Matrix44 scale;
		scale.SetIdentity();
		//scale.Scale(gef::Vector4(0.1f, 0.1f, 0.1f));
		scale.SetTranslation(gef::Vector4(1000, 100, 0));
		argun.set_mesh(model[1].mesh());
		argun.set_transform(scale);

		audio_manager_ = audio_;

		
	}
	void setPostion(gef::Vector4);
	void calRotation();
	void calLookAt();
	void setGunPositions(std::string);

	const gef::Vector4 &getPosition() { return position; };
	const gef::Vector4 &getLookAt() { return LookAt; };
	const gef::Vector4 &getUp() { return up; };
	const gef::Vector4 &getForward() { return forward; };
	const gef::Vector4 &getRight() { return right; };

	//handeling all the user input
	void handleInput(float dt, float, float,b2Body*);

};

