#include "Projectiles.h"

#include <system/debug_log.h>

Projectiles::Projectiles(b2World* world, PrimitiveBuilder* primitive_builder)
{	
	world_ = world;
	primitive_builder_ = primitive_builder;
//	material->set_colour(0x00111100);
}
void Projectiles::Init()
{
	//set mesh of the projectile
	gef::Vector4 projectile_half_dimensions(0.1f, 0.1f, 0.1f);
	projectile_.set_mesh(primitive_builder_->CreateBoxMesh(projectile_half_dimensions));

	b2BodyUserData user_data_;
	user_data_.pointer = 5 ;
	//create a physics body for the projectiles
	b2BodyDef projectile_body_def;
	projectile_body_def.type = b2_dynamicBody;
	projectile_body_def.bullet = true;
	projectile_body_def.userData = user_data_;
	projectile_body_def.position = b2Vec2(1000, 8.0f);

	projectile_body_ = world_->CreateBody(&projectile_body_def);

	//create the shape for the projectile
	b2PolygonShape projectile_shape;
	projectile_shape.SetAsBox(0.05f, 0.05f);

	// Create Ficture 
	b2FixtureDef projectile_fixture_def;
	projectile_fixture_def.shape = &projectile_shape;
	projectile_fixture_def.density = 1.0f;

	// Create the fixture on the rigid body
	projectile_body_->CreateFixture(&projectile_fixture_def);

	//update visuals from simulation data 
	projectile_.UpdateFromSimulation(projectile_body_);
	projectile_.y_pos = 1.5f;
}
void Projectiles::InitShotgun()
{
	for (int i = 0; i < 2; i++)
	{

		gef::Vector4 projectile_half_dimensions(0.05f, 0.05f, 0.05f);
		shotgun_bullet_[i].set_mesh(primitive_builder_->CreateBoxMesh(projectile_half_dimensions));
		
		b2BodyUserData user_data_;
		user_data_.pointer = 5;
		//create a physics body for the projectiles
		b2BodyDef projectile_body_def;
		projectile_body_def.type = b2_dynamicBody;
		projectile_body_def.userData = user_data_;
		projectile_body_def.bullet = true;
		projectile_body_def.position = b2Vec2(1000+i , -5.0f);

		shotgun_bullet_body_[i] = world_->CreateBody(&projectile_body_def);

		//create the shape for the projectile
		b2PolygonShape projectile_shape;
		projectile_shape.SetAsBox(0.05f, 0.05f);

		// Create Ficture 
		b2FixtureDef projectile_fixture_def;
		projectile_fixture_def.shape = &projectile_shape;
		projectile_fixture_def.density = 1.0f;
		// Create the fixture on the rigid body
		shotgun_bullet_body_[i]->CreateFixture(&projectile_fixture_def);
		
		//update visuals from simulation data 
		shotgun_bullet_[i].UpdateFromSimulation(shotgun_bullet_body_[i]);
		shotgun_bullet_[i].y_pos = 1.5f;

	}
}
void Projectiles::shoot(float velocity,std::string type,b2Vec2 position)
{
	//shoot bullets from the player is currently in the game 
	gef::DebugOut("%f %f\n", position.x, position.y);
	position *= velocity;
	projectile_.UpdateFromSimulation(projectile_body_);
	if (type == "ar")
	{
		projectile_body_->ApplyLinearImpulseToCenter(position, true);
		projectile_.UpdateFromSimulation(projectile_body_);
	}
	if (type == "shotgun")
	{
		for (int i = 0; i < 2; i++)
		{
			shotgun_bullet_body_[i]->ApplyLinearImpulseToCenter(position, true);
			shotgun_bullet_[i].UpdateFromSimulation(shotgun_bullet_body_[i]);
			
		}
	}
}

void Projectiles::update()
{
	projectile_.UpdateFromSimulation(projectile_body_);
	for (int i = 0; i < 2; i++)
	{
		shotgun_bullet_[i].UpdateFromSimulation(shotgun_bullet_body_[i]);
	}

}

void Projectiles::render(gef::Renderer3D* render)
{
	renderer_3d_ = render;

	renderer_3d_->DrawMesh(projectile_);
	for (int i = 0; i < 2; i++)
	{
		renderer_3d_->DrawMesh(shotgun_bullet_[i]);
	}
}



void Projectiles::setPosition(b2Vec2 position,b2Rot rotation)
{
	//setting ar bullet position 
	projectile_body_->SetTransform(position,rotation.GetAngle());
	projectile_.UpdateFromSimulation(projectile_body_);
}

void Projectiles::setShotGunBulletsPos(b2Vec2 player_position,b2Rot rotation)
{
	//setting shotgun bullet position 

	for (int i = 0; i < 2; i++)
	{
		shotgun_bullet_body_[i]->SetTransform(player_position, rotation.GetAngle());
		shotgun_bullet_[i].UpdateFromSimulation(shotgun_bullet_body_[i]);
	}

}

