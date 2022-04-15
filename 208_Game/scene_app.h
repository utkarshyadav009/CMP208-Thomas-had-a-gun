#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/sprite.h>
#include <graphics/mesh_instance.h>
#include "graphics/scene.h"
#include <platform/win32/system/window_win32.h>
#include <box2d/Box2D.h>
#include<graphics/texture.h>
#include<graphics/image_data.h>
#include "game_object.h"
#include<iostream>
#include<vector>
#include"CameraMov.h"
#include"Projectiles.h"
#include"Trees.h"
#include"GameState.h"
#include<audio/audio_manager.h>
#include<enemy.h>
# define M_PI           3.14159265358979323846  /* pi */
#define thresh			 0.000010
using std::cout;
using std::cin;
using namespace std;
// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
	class WindowWin32;
}


class SceneApp : public gef::Application
{
public:
	//Game loop functions 
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
		
	/*Game State functions */
	void ControlsStateUpdate(float frame_time);
	void ControlPauseStateupdate(float fram_time);
	void ControlPauseStaterender();
	void ControlsStateRender();
	void UpdateState(float);
	void GameUpdate(float frame_time);
	void GameRender();
	void InitStateUpdate(float frame_time);
	void OptionsStateUpdate(float frame_time);
	void OptionsStateRender();
	void moveUpMenu();
	void moveDownMenu();
	void moveUpPause();
	void moveDownPause();
	void PauseStateUpdate(float frame_time);
	void PauseStateRender();
	void DeathStateUpdate(float);
	void DeathStateRender();
	void CreditStateUpdate(float dt);
	void CreditStateRender();
	void InitStateRender();
	int getPressedItemPause() { return selectedItemPauseScreen; }
	int getPressedItemMenu() { return selectedItemMenuScreen; }

	
	void remove_bodies();//removing the bodies	
	void CollisionDetection(); //detecting collisions 
	void setPlayerBody(); //setting player body
	void setPlayerBoundry(); //setting player boundry body 
	void InitGround(); //setting the ground bodies with walls 
	void InitFont(); //initialising all the font variables 
	void CleanUpFont();//deleting the font variable 
	void DrawHUD();//drawing fps 
	void SetupLights();//lights setup 

	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename); 
	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);// object loading  

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;
	

	//textures
	PrimitiveBuilder* primitive_builder_;

	//input
	gef::InputManager* input;

	// create the physics world
	b2World* world_;

	//Player 
	b2Body* Player_body_;
	b2Body* Player_boundry_body_;
	
	// ground variables
	gef::Mesh* ground_mesh_;

	//ground body is also initialising walls for the game so that player doesn't walk off the edge
	GameObject ground_;
	b2Body* ground_body_;

	//Angle of the body
	float angle;
	
	//Camera Movement class Object 
	CameraMov camera;
	gef::Matrix44 view_matrix;

	bool check_bullet_ = false;

	//bullet class object
	//Projectiles* bullets;
	vector<Projectiles> bullets_;

	//
	float timer_ = 0;
	//fps
	float fps_;
	//tree  
	Trees tree;
	//gef::MeshInstance mesh_instance_;
	gef::MeshInstance gun_[2];
	gef::Scene* scene_assets_[2];

	gef::Mesh* mesh_;

	//Gamestate Object
	GameState game_state_;

	//enemies
	vector<enemy> enemies_;
	//gun string
	string gunType;
	//Textures 
	gef::Texture* mainscreen;
	gef::Texture* controlscreen;
	gef::Texture* optionscreen;
	gef::Texture* deathscreen;
	gef::Texture* winscreen;

	//font for the menu and pause menu 
	gef::Font* pause_[4];
	gef::Font* menu_[3];

	//selected item in the menu 
	 int selectedItemPauseScreen;
	 int selectedItemMenuScreen;

	//audio management 
	 gef::AudioManager* audio_manager_;
	 gef::AudioManager* screams_;

	//Material 
	 gef::Texture* grass_;
	 gef::Material* grass[6];

	 gef::Texture* whompTexture_;
	 gef::Material* whompMaterial_[6];

	 //removing objects  
	 vector<b2Body*> remove_enemies;
	 vector<b2Body*> remove_bullets;

	 //player health 
	 int health = 200;
};

#endif // _SCENE_APP_H
