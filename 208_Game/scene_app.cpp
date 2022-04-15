#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include<input/input_manager.h>
#include<input/keyboard.h>
#include "load_texture.h"


b2Vec2 player_Position_;
b2Rot player_Rotation_;

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	audio_manager_(NULL),
	font_(NULL),
	input(NULL),
	world_(NULL)
{
}

void SceneApp::Init()
{

	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	// intialising input manager to use the keyboard input 
	input = gef::InputManager::Create(platform_);

	//setting the first game state as Init 
	game_state_.setCurrentState(State::Init);

	//initialising audio manger to use audio in the game
	audio_manager_ = gef::AudioManager::Create();

	
	screams_ = gef::AudioManager::Create();

	InitFont();
	SetupLights();

	// initialise the physics world
	b2Vec2 gravity(0.0f, 0.0f);
	world_ = new b2World(gravity);
	check_bullet_ = false;

	//initialising all the trees 
	tree.InitTrees(platform_);

	//loading both the guns 
	const char* shotgun = "shotgun_.scn";
	scene_assets_[0] = LoadSceneAssets(platform_, shotgun);
	if (scene_assets_[0])
	{
		gun_[0].set_mesh(GetMeshFromSceneAssets(scene_assets_[0]));
	}
	else
	{
		gef::DebugOut("Scene file %s failed to load\n", shotgun);
	}

	const char* ar = "smaller_ar.scn";
	scene_assets_[1] = LoadSceneAssets(platform_, ar);
	if (scene_assets_[1])
	{
		gun_[1].set_mesh(GetMeshFromSceneAssets(scene_assets_[1]));
	}
	else
	{
		gef::DebugOut("Scene file %s failed to load\n", ar);
	}


	//initialising player body 
	setPlayerBody();

	//loading game textures  
	mainscreen = CreateTextureFromPNG("TitleScreen.png", platform_);
	controlscreen = CreateTextureFromPNG("ControlsScreen.png", platform_);
	optionscreen = CreateTextureFromPNG("OptionsScreen.png", platform_);
	grass_ = CreateTextureFromPNG("grass.png", platform_);
	whompTexture_ = CreateTextureFromPNG("whomp.png", platform_);
	winscreen = CreateTextureFromPNG("win_screen.png", platform_);
	deathscreen = CreateTextureFromPNG("death_screen.png", platform_);
	//initialising gunType 
	gunType = "shotgun";

	//Menu select Items 
	selectedItemPauseScreen = 0;
	selectedItemMenuScreen = 0;

	//setting color for all the menu stuff 
	pause_[0]->setColor(0xFF0000FF);
	for (int i = 1; i <= 3; i++)
	{
		pause_[i]->setColor(0xffffffff);
	}
	menu_[0]->setColor(0xFF0000FF);
	for (int i = 1; i <= 2; i++)
	{
		menu_[i]->setColor(0xffffffff);
	}

	//loading all the game sounds 
	audio_manager_->LoadMusic("faded_menu.wav", platform_);
	audio_manager_->LoadSample("browsing.wav", platform_);
	audio_manager_->LoadSample("enter_.wav", platform_);
	audio_manager_->LoadSample("shotgun.wav", platform_);
	audio_manager_->LoadSample("ar_one_bullet.wav", platform_);
	audio_manager_->LoadSample("walking.wav", platform_);

	audio_manager_->SetMasterVolume(50);


	//initialising the gun on the player 
	camera.CameraInit(input, gun_,audio_manager_);

	//initialising material for the ground 
	for (int i = 0; i < 6; i++)
	{
		grass[i] = new gef::Material();
		grass[i]->set_texture(grass_);
	}
	//initialising the ground with the material 
	InitGround();

	for (int i = 0; i < 6; i++)
	{
		whompMaterial_[i] = new gef::Material();
		whompMaterial_[i]->set_texture(whompTexture_);
	}
	for (int i = 0; i < 5; i++) {
		enemy temp_enemies;
		temp_enemies.Init(world_, primitive_builder_, b2Vec2((5 + i), (5 * -i)),whompMaterial_);
		temp_enemies.setHealth(10);
		enemies_.push_back(temp_enemies);
	}
	//setting the player boundry
	setPlayerBoundry();

	
}
void SceneApp::CleanUp()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	CleanUpFont();

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete audio_manager_;
	audio_manager_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete input;
	input = NULL;
}

void SceneApp::GameUpdate(float frame_time)
{
	fps_ = 1.0f / frame_time;
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	gef::TouchInputManager* touch_input = input->touch_manager();
	// update physics world
	float timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// update object visuals from simulation data

	camera.handleInput(frame_time, float(platform_.width()), float(platform_.height()), Player_body_);
	camera.update();

	Player_boundry_body_->SetTransform(Player_body_->GetPosition(), Player_body_->GetAngle());
	//pausing th game if ESCAPE is pressed 
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyDown(gef::Keyboard::KC_ESCAPE))
		{
			
			game_state_.setCurrentState(State::PAUSE);
		}
	}
	//updating all the enemies 
	for (int i = 0; i < enemies_.size(); i++) {
		enemies_[i].update();
	}
	//updating all teh bullets 
	for (Projectiles& bullet : bullets_)
		bullet.update();
	camera.setGunPositions(gunType);


	//shooting mechanic 
	if (keyboardManager)
	{		
		if (keyboardManager->IsKeyDown(gef::Keyboard::KC_SPACE) && timer_ <= 0.f)
		{
			b2Vec2 camera_pos(camera.getPosition().x(), camera.getPosition().z());
			b2Vec2 camera_fwd(camera.getForward().x(), camera.getForward().z());
			b2Vec2 bullet_pos = camera_fwd + camera_pos;

			Projectiles bullet(world_, primitive_builder_);			
			bullet.InitShotgun();
			bullet.Init();

			if (gunType == "ar")
			{
				    audio_manager_->PlaySample(3, false);
					timer_ = 0.1f;
					bullet.setPosition(bullet_pos, player_Rotation_);
					bullet.shoot(80, gunType, camera_fwd);
					bullets_.push_back(bullet);
			}
			if (gunType == "shotgun")
			{
					timer_ = 0.5f;
					audio_manager_->PlaySample(2, false);
					bullet.setShotGunBulletsPos(bullet_pos, player_Rotation_);
					bullet.shoot(50, gunType, camera_fwd);
					bullets_.push_back(bullet);
			
			}
		}
	}

	timer_ -= frame_time;
	//calling collision detection 
	CollisionDetection();
	//removing the bodies 
	remove_bodies();

	//updating death and credit screens 
	if (health <= 0)
	{
		game_state_.setCurrentState(State::DEATH);
	}
	if (enemies_.size() == 0)
	{
		game_state_.setCurrentState(State::CREDITS);
	}

}

void SceneApp::CollisionDetection()
{
	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	for (int contact_num = 0; contact_num < contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{

			auto fA = contact->GetFixtureA();
			auto fB = contact->GetFixtureB();

			auto bA = fA->GetBody();
			auto bB = fB->GetBody();

			b2BodyUserData budA = bA->GetUserData();
			b2BodyUserData budB = bB->GetUserData();

			//if bullet collides with the wall
			if ((budA.pointer == 10 && budB.pointer == 5) || (budA.pointer == 5 && budB.pointer == 10))
			{
				if (budA.pointer == 5)
				{
					remove_bullets.push_back(bA);
				}
				if (budB.pointer == 5)
				{
					remove_bullets.push_back(bB);

				}
			}

			//if enemy collides with player boundry move towards player
			if (budA.pointer == 3 && budB.pointer == 23 || budA.pointer == 23 && budB.pointer == 3) {

				if (budA.pointer == 3)
				{
					b2Vec2 velocity = Player_body_->GetPosition() - bA->GetPosition();
					velocity.Normalize();
					velocity *= 2.f;
					bA->SetLinearVelocity(velocity);
				}
				if (budB.pointer == 3)
				{
					b2Vec2 velocity = Player_body_->GetPosition() - bB->GetPosition();
					velocity.Normalize();
					velocity *= 2.f;
					bB->SetLinearVelocity(velocity);
				}
			}
			//if enemy collide with player body decrease player health 
			if (budA.pointer == 3 && budB.pointer == 22 || budA.pointer == 22 && budB.pointer == 3) {
				health -= 1;
			}
			//if player body collides with gun do nothing or stop physics 
			if (budA.pointer == 5 && budB.pointer == 22 || budA.pointer == 22 && budB.pointer == 5) {
				contact->SetEnabled(false);
			}

			//checking collision between both the enemy and bullets for the gun types 
			if (gunType == "ar")
			{
				if ((budA.pointer == 3 && budB.pointer == 5) || (budA.pointer == 5 && budB.pointer == 3))
				{
					if (budA.pointer == 3)
					{
						remove_enemies.push_back(bA);
					}
					if (budB.pointer == 3)
					{
						remove_enemies.push_back(bB);
					}
					if (budA.pointer == 5)
					{
						remove_bullets.push_back(bA);
					}
					if (budB.pointer == 5)
					{
						remove_bullets.push_back(bB);
					}
				}
			}
			if (gunType == "shotgun")
			{
				if ((budA.pointer == 3 && budB.pointer == 5) || (budA.pointer == 5 && budB.pointer == 3))
				{
					if (budA.pointer == 3)
					{
						remove_enemies.push_back(bA);
					}
					if (budB.pointer == 3)
					{
						remove_enemies.push_back(bB);
					}
					if (budA.pointer == 5)
					{
						remove_bullets.push_back(bA);
					}
					if (budB.pointer == 5)
					{
						remove_bullets.push_back(bB);
					}
				}

			}

		}

		// Get next contact point
		contact = contact->GetNext();
	}

}
void SceneApp::remove_bodies() {

	//removing enemy bodies 
	for (size_t i = 0; i < remove_enemies.size(); i++) {
		for (size_t j = 0; j <enemies_.size(); j++) {
			if (enemies_[j].getBody() == remove_enemies[i]) {
				int health = enemies_[j].getHealth() - 1;
				enemies_[j].setHealth(health);

				if (enemies_[j].getHealth() <= 0) {
					enemies_[j].getBody()->GetWorld()->DestroyBody(enemies_[j].getBody());
					enemies_.erase(enemies_.begin() + j);
				}
			}
			
		}
	}
	//removing bullet bodies for both the bodies 
	if (gunType == "ar") {
		for (size_t i = 0; i < remove_bullets.size(); i++) {
			for (size_t j = 0; j < bullets_.size(); j++) {
				if (bullets_[j].getbulletbody() == remove_bullets[i]) {
					bullets_[j].getbulletbody()->GetWorld()->DestroyBody(bullets_[j].getbulletbody());
					bullets_.erase(bullets_.begin() + j);
				}
			}
		}
	}
	if (gunType == "shotgun") {
		for (size_t i = 0; i < remove_bullets.size(); i++) {
			for (size_t j = 0; j < bullets_.size(); j++) {
				if (bullets_[j].getshotgunbulletbody1() == remove_bullets[i] || bullets_[j].getshotgunbulletbody2() == remove_bullets[i]) {
					bullets_[j].getshotgunbulletbody1()->GetWorld()->DestroyBody(bullets_[j].getshotgunbulletbody1());
					bullets_[j].getshotgunbulletbody2()->GetWorld()->DestroyBody(bullets_[j].getshotgunbulletbody2());
					bullets_.erase(bullets_.begin() + j);
				}
			}
		}	
	}
	//clearing both the vectors
	remove_enemies.clear();
	remove_bullets.clear();
}
void SceneApp::GameRender() {
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(camera.getPosition().x(), camera.getPosition().y(), camera.getPosition().z());
	gef::Vector4 camera_lookat(camera.getLookAt().x(), camera.getLookAt().y(), camera.getLookAt().z());
	gef::Vector4 camera_up(camera.getUp().x(), camera.getUp().y(), camera.getUp().z());
	
	
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	
	renderer_3d_->set_view_matrix(view_matrix);



	// draw 3d geometry
	renderer_3d_->Begin();
		//rendering guns 
		camera.render(renderer_3d_);
		//renderign trees
		tree.render(renderer_3d_);

		//draw enemies 
		for (int i = 0; i < enemies_.size(); i++) {
			enemies_[i].render(renderer_3d_);
		}
		// draw ground
		renderer_3d_->DrawMesh(ground_);
		//draw bullet
		for (Projectiles& bullet : bullets_)
			bullet.render(renderer_3d_);
	renderer_3d_->End();
	

	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}
bool SceneApp::Update(float frame_time)
{
	//update gamestate 
	UpdateState(frame_time);
	
	return true;

}




void SceneApp::Render()
{
	
}


														/*GAMESTATE FUNCTIONS*/

void SceneApp::ControlsStateUpdate(float frame_time)
{
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_ESCAPE))
		{
			game_state_.setCurrentState(State::Init);
		}
	}
}

void SceneApp::ControlsStateRender() {
	sprite_renderer_->Begin();
	gef::Sprite controls;
	controls.set_texture(controlscreen);
	controls.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	controls.set_height(610.0f);
	controls.set_width(960.0f);
	sprite_renderer_->DrawSprite(controls);

	//DrawHUD();

	sprite_renderer_->End();

}

void SceneApp::moveUpMenu() {
	if (selectedItemMenuScreen - 1 >= 0)
	{
		menu_[selectedItemMenuScreen]->setColor(0xffffffff);
		selectedItemMenuScreen--;
		menu_[selectedItemMenuScreen]->setColor(0xFF0000FF);
	}

}
void SceneApp::moveDownMenu() {
	if (selectedItemMenuScreen + 1 < 3)
	{
		menu_[selectedItemMenuScreen]->setColor(0xffffffff);
		selectedItemMenuScreen++;
		menu_[selectedItemMenuScreen]->setColor(0xFF0000FF);
	}
}
void SceneApp::InitStateUpdate(float frame_time)
{
	static bool runOnce = true;
	{
		if (runOnce)
		{
			audio_manager_->PlayMusic();
			runOnce = false;
		}
	}
	
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_UP))
		{
			moveUpMenu();
			audio_manager_->PlaySample(0, false);
		}
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_DOWN))
		{
			moveDownMenu();
			audio_manager_->PlaySample(0, false);
		}
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_RETURN))
		{
			audio_manager_->PlaySample(1, false);
			switch (getPressedItemMenu())
			{
			case 0:
				game_state_.setCurrentState(State::OPTIONS);
				break;
			case 1:

				game_state_.setCurrentState(State::CONTROLS);
				break;
			case 2:
				exit(0);
				break;
			}
		}
	}
}

void SceneApp::InitStateRender()
{
	sprite_renderer_->Begin();
	gef::Sprite mainmenu;
	mainmenu.set_texture(mainscreen);
	mainmenu.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	mainmenu.set_height(625.0f);
	mainmenu.set_width(1000.0f);
	sprite_renderer_->DrawSprite(mainmenu);	
	DrawHUD();
	sprite_renderer_->End();

	menu_[0]->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 100.0f, -0.99f),
		1.0f,
		0xFF0000FF,
		gef::TJ_CENTRE,
		"Play");
	menu_[1]->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 70.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Controls");
	menu_[2]->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 40.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Quit");

}
void SceneApp::ControlPauseStateupdate(float fram_time) {
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_ESCAPE))
		{
			game_state_.setCurrentState(State::PAUSE);
		}
	}
}
void SceneApp::ControlPauseStaterender() {
	sprite_renderer_->Begin();
	gef::Sprite controls;
	controls.set_texture(controlscreen);
	controls.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	controls.set_height(610.0f);
	controls.set_width(960.0f);
	sprite_renderer_->DrawSprite(controls);

	//DrawHUD();

	sprite_renderer_->End();


}
void SceneApp::OptionsStateUpdate(float frame_time) {
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_1))
		{
			gunType = "ar";
			game_state_.setCurrentState(State::LEVEL);
		}
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_2))
		{
			gunType = "shotgun";
			game_state_.setCurrentState(State::LEVEL);
		}
	}
	if (gunType == "ar")
	{
		for (int i = 0; i < 5; i++)
		{
			enemies_[i].setHealth(4);
		}
	}
	if (gunType == "shotgun")
	{
		for (int i = 0; i < 5; i++)
		{
			enemies_[i].setHealth(2);
		}
	}

}
void SceneApp::OptionsStateRender() {

	sprite_renderer_->Begin();
	gef::Sprite options;
	options.set_texture(optionscreen);
	options.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	options.set_height(610.0f);
	options.set_width(960.0f);
	sprite_renderer_->DrawSprite(options);

	//DrawHUD();

	sprite_renderer_->End();

}
void SceneApp::moveUpPause()
{
	if (selectedItemPauseScreen - 1 >= 0)
	{
		pause_[selectedItemPauseScreen]->setColor(0xffffffff);
		selectedItemPauseScreen--;
		pause_[selectedItemPauseScreen]->setColor(0xFF0000FF);
	}


}
void SceneApp::moveDownPause() {
	if (selectedItemPauseScreen + 1 < 4)
	{
		pause_[selectedItemPauseScreen]->setColor(0xffffffff);
		selectedItemPauseScreen++;
		pause_[selectedItemPauseScreen]->setColor(0xFF0000FF);
	}
}
void SceneApp::PauseStateUpdate(float frame_time) {
	static bool runOnce = true;
	{
		if (runOnce)
		{
			audio_manager_->PlayMusic();
			runOnce = false;
		}
	}
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	if (selectedItemPauseScreen < 0) {
		selectedItemPauseScreen = 3;
	}



	if (keyboardManager)
	{

		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_UP))
		{
			moveUpPause();
			audio_manager_->PlaySample(0, false);

		}
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_DOWN))
		{
			moveDownPause();
			audio_manager_->PlaySample(0, false);

		}
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_ESCAPE))
		{
			game_state_.setCurrentState(State::LEVEL);
		}
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_RETURN))
		{
			audio_manager_->PlaySample(1, false);
			switch (getPressedItemPause())
			{
			case 0:
				game_state_.setCurrentState(State::LEVEL);
				break;
			case 1:
				
				game_state_.setCurrentState(State::PAUSE_CONTROLS);
				break;
			case 2:
				game_state_.setCurrentState(State::Init);
				break;
			case 3:
				exit(0);
				break;
			}
		}
	}



}
void SceneApp::PauseStateRender() {
	sprite_renderer_->Begin();
	gef::Sprite mainmenu;
	mainmenu.set_texture(mainscreen);
	mainmenu.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	mainmenu.set_height(625.0f);
	mainmenu.set_width(1000.0f);
	sprite_renderer_->DrawSprite(mainmenu);


	sprite_renderer_->End();
	pause_[0]->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 100.0f, -0.99f),
		1.0f,
		0xFF0000FF,
		gef::TJ_CENTRE,
		"Resume");
	pause_[1]->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 70.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Controls");
	pause_[2]->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 40.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Main Menu");
	pause_[3]->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 10.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Quit");

}
void SceneApp::DeathStateUpdate(float dt)
{
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_ESCAPE))
		{
			exit(0);
		}
	}

}
void SceneApp::DeathStateRender() {

	sprite_renderer_->Begin();
	gef::Sprite death_sprite;
	death_sprite.set_texture(deathscreen);
	death_sprite.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	death_sprite.set_height(560.0f);
	death_sprite.set_width(960.0f);
	sprite_renderer_->DrawSprite(death_sprite);

	sprite_renderer_->End();

}


void SceneApp::CreditStateUpdate(float dt) {
	input->Update();
	gef::Keyboard* keyboardManager = input->keyboard();
	if (keyboardManager)
	{
		if (keyboardManager->IsKeyPressed(gef::Keyboard::KC_ESCAPE))
		{
			exit(0);
		}
	}



}
void SceneApp::CreditStateRender() {
	sprite_renderer_->Begin();
	gef::Sprite win_sprite;
	win_sprite.set_texture(winscreen);
	win_sprite.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	win_sprite.set_height(550.0f);
	win_sprite.set_width(960.0f);
	sprite_renderer_->DrawSprite(win_sprite);

	sprite_renderer_->End();
}
void SceneApp::UpdateState(float dt)
{
	
	switch (game_state_.getCurrentState()) {
	case State::Init:
		InitStateUpdate(dt);
		InitStateRender();
		break;
	case State::CONTROLS:
		ControlsStateUpdate(dt);
		ControlsStateRender();
		break;
	case State::OPTIONS:
		OptionsStateUpdate(dt);
		OptionsStateRender();
		break;
	case State::LEVEL:
		GameUpdate(dt);
		GameRender();
		break;
	case State::PAUSE:
		PauseStateRender();
		PauseStateUpdate(dt);
		break;
	case State::PAUSE_CONTROLS:
		ControlPauseStateupdate(dt);
		ControlPauseStaterender();
		break;
	case State::DEATH:
		DeathStateUpdate(dt);
		DeathStateRender();
		break;
	case State::CREDITS:
		CreditStateUpdate(dt);
		CreditStateRender();
		break;
	}

}



//setting player body 
void SceneApp::setPlayerBody()
{
	b2BodyUserData player_userData;
	player_userData.pointer = 22;
	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.userData = player_userData;
	player_body_def.position = b2Vec2(0, 0);

	Player_body_ = world_->CreateBody(&player_body_def);

	//player_body_->GetUserData()
	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(1.0f, 1.0f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.f;
	// create the fixture on the rigid body
	Player_body_->CreateFixture(&player_fixture_def);
}


//setting player boundry
void SceneApp::setPlayerBoundry()
{
	b2BodyUserData player_userData;
	player_userData.pointer = 23;

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.userData = player_userData;
	player_body_def.position = b2Vec2(0, 0);
	

	Player_boundry_body_ = world_->CreateBody(&player_body_def);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(30.f, 30.f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.f;
	player_fixture_def.isSensor = true;
	// create the fixture on the rigid body
	Player_boundry_body_->CreateFixture(&player_fixture_def);
}


//initialising the ground and the walls 
void SceneApp::InitGround()
{
	// ground dimensions
	gef::Vector4 ground_half_dimensions(50.0f, 0.5f, 50.0f);
	b2BodyUserData ground_data_;
	ground_data_.pointer = 10;
	// setup the mesh for the ground
	ground_mesh_ = primitive_builder_->CreateBoxMesh(ground_half_dimensions, gef::Vector4::kZero, grass);
	ground_.set_mesh(ground_mesh_);

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.userData = ground_data_;
	body_def.position = b2Vec2(0.0f, -7.0f);

	ground_body_ = world_->CreateBody(&body_def);

	b2Vec2 positions[4]{
		{ -51.f, 0.f },
		{ 51.f, 0.f },
		{ 0.f, -51.f },
		{ 0.f, 51.f }
	};
	b2Vec2 sizes[4]{
		{ 1.f, 50.f },
		{ 1.f, 50.f },
		{ 50.f, 1.f },
		{ 50.f, 1.f }
	};

	for (int i = 0; i < 4; ++i) {
		b2PolygonShape shape;
		shape.SetAsBox(sizes[i].x, sizes[i].y, positions[i], 0.f);
		b2FixtureDef fix;
		fix.shape = &shape;
		// create the fixture on the rigid body
		ground_body_->CreateFixture(&fix);
	}

	// update visuals from simulation data
	ground_body_->SetTransform(ground_body_->GetPosition(), 0);
	
	ground_.UpdateFromSimulation(ground_body_);

//	ground_body_->SetTransform(ground_body_->GetPosition(), 90);
}


//initialising the fonts 

void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
	font_->setColor(0xFF0000FF);
	for (int i = 0; i < 4; i++) {
		pause_[i] = new gef::Font(platform_);
		pause_[i]->Load("comic_sans");
	}
	for (int i = 0; i < 3; i++) {
		menu_[i] = new gef::Font(platform_);
		menu_[i]->Load("comic_sans");
	}
	
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
	for (int i = 0; i < 4; i++) {
		delete pause_[i];
		pause_[i] = NULL;
	}
	for (int i = 0; i < 3; i++) {
		delete menu_[i];
		menu_[i] = NULL;
	}
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(1.0f, 1.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
		font_->RenderText(sprite_renderer_, gef::Vector4(1.0f, 20.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Health: %.1d", (health/2));

	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

//loading objects 
gef::Scene* SceneApp::LoadSceneAssets(gef::Platform& platform, const char* filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		// if scene file loads successful
		// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

gef::Mesh* SceneApp::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}
