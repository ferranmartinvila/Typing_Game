#include "Player.h"
#include "j1Scene.h"
#include "p2Log.h"

//Constructors ----------------------------------
j1Player::j1Player()
{
}

//Destructors -----------------------------------
j1Player::~j1Player()
{
}

//Game Loop -------------------------------------
bool j1Player::Awake(pugi::xml_node & config)
{
	return true;
}


void j1Player::EndParty()
{
	alive = false;
	if (current_score > max_score)max_score = current_score;
	current_score = 0;
	LOG("You Lose :(");
}

bool j1Player::GetPlayerState() const
{
	return alive;
}

//Functionality ---------------------------------
uint j1Player::PlusScore(uint plus)
{
	return current_score += plus;;
}

uint j1Player::PlusLevel(uint plus)
{
	xp += plus;
	while (xp > lvl * 25)
	{
		xp -= lvl * 25;
		lvl++;
	}
	return lvl;
}

uint j1Player::GetCurrentScore() const
{
	return current_score;
}

uint j1Player::GetMaxScore() const
{
	return max_score;
}

uint j1Player::GetLevel() const
{
	return lvl;
}

uint j1Player::GetXp() const
{
	return xp;
}
