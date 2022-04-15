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
#include<graphics/mesh.h>
class enemy
{
public:

	void Init(b2World*, PrimitiveBuilder*,b2Vec2,gef::Material**); //Initialising the enemy body with the right materials	
	void update();   //updating the enemy body 
	void render(gef::Renderer3D*); //rendering the enemy body 
	b2Body* getBody() { return enemy_body_; } //getting the body for deletion 
	int getHealth() { return enemy_health; } //health
	void setHealth(int h) { enemy_health = h; }

private:


	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	b2World* world_;
	GameObject enemy_;
	b2Body* enemy_body_;
	gef::Mesh* enemy_mesh_;
	int enemy_health;
};

