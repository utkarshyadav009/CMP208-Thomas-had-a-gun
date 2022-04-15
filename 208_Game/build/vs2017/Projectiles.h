#pragma once
#include<iostream>
#include"game_object.h"
#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/sprite.h>
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include<graphics/renderer_3d.h>
#include<input/input_manager.h>
#include<input/keyboard.h>
#include"CameraMov.h"

using std::cout;
class Projectiles
{
public:
	Projectiles() {};
	Projectiles(b2World*,PrimitiveBuilder*);// Contructor to initialise world and primitivebuilder parameters 
	void Init(); //initialising the ar bullets 
	void InitShotgun(); //initialising the shotgun bullets 
	void shoot(float,std::string,b2Vec2); //shooting the bodies 
	void update(); //updating the bodies 
	void render(gef::Renderer3D*); //rendering the bodies 
	void setPosition(b2Vec2,b2Rot); //setting position for the ar bullets	
	void setShotGunBulletsPos(b2Vec2,b2Rot); //setting position for the shotgun bullets	
	b2Vec2 getPosition() { return projectile_body_->GetTransform().p; } //getting position 
	b2Rot getRotation() { return projectile_body_->GetTransform().q; }//getting rotation 
	b2Body* getbulletbody() { return projectile_body_; } //getting ar bullet body for deletion 
	b2Body* getshotgunbulletbody1() { return shotgun_bullet_body_[0]; }
	b2Body* getshotgunbulletbody2() { return shotgun_bullet_body_[1]; }//getting both the shotgun bullets for deletion 


private:
	CameraMov FPScamera;
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	b2World* world_;
	GameObject projectile_;
	b2Body* projectile_body_;
	b2Body* shotgun_bullet_body_[2];
	GameObject shotgun_bullet_[2];
	gef::Mesh* projectile_mesh_;
	gef::Material* material;


};

