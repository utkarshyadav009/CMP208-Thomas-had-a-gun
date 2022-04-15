#include "Guns.h"
#include<input/input_manager.h>
#include <input/touch_input_manager.h>
#include<input/keyboard.h>

void Guns::ShootPistol(b2Vec2 position)
{
	Projectiles bullet_;
	//bullet_.shoot(position, pistol_bulletSpeed, "pistol");
	bullets_.push_back(bullet_);
	Sleep(1000);
}

void Guns::ShootShotgun(b2Vec2 position)
{

}

void Guns::ShootAR(b2Vec2 position)
{
	
}

void Guns::handleGunInput(gef::TouchInputManager* mouseManager,b2Vec2 position)
{
	if (mouseManager)
	{
		if (mouseManager->is_button_down(0))
		{
			ShootPistol(position);
		}
	}
	

}
