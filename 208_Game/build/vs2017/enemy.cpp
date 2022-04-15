#include "enemy.h"

void enemy::Init(b2World* world, PrimitiveBuilder* builder,b2Vec2 position,gef::Material** whomp)
{
	world_ = world;
	primitive_builder_ = builder;
	

	gef::Vector4 enemy_half_dimensions(2.0f, 2.5f, 1.0f);

	enemy_mesh_ = primitive_builder_->CreateBoxMesh(enemy_half_dimensions,gef::Vector4::kZero,whomp);
	enemy_.set_mesh(enemy_mesh_);

	b2BodyUserData enemy_data;
	enemy_data.pointer = 3;
	// create a physics body for the player
	b2BodyDef enemy_body_def;
	enemy_body_def.type = b2_dynamicBody;
	enemy_body_def.userData = enemy_data;
	enemy_body_def.position = position;

	enemy_body_ = world_->CreateBody(&enemy_body_def);

	// create the shape for the player
	b2PolygonShape enemy_shape;
	enemy_shape.SetAsBox(enemy_half_dimensions.x(), enemy_half_dimensions.z());

	// create the fixture
	b2FixtureDef enemy_fixture_def;
	enemy_fixture_def.shape = &enemy_shape;
	enemy_fixture_def.density = 1.f;

	// create the fixture on the rigid body
	enemy_body_->CreateFixture(&enemy_fixture_def);

	// update visuals from simulation data
	enemy_.UpdateFromSimulation(enemy_body_);
	enemy_.y_pos = 3.0f;

}

void enemy::update()
{

	enemy_.UpdateFromSimulation(enemy_body_);

}

void enemy::render(gef::Renderer3D* render)
{
	renderer_3d_ = render;
	renderer_3d_->DrawMesh(enemy_);

}
