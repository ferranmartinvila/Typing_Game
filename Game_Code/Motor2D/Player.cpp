#include "Player.h"

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


//Functionality ---------------------------------
uint j1Player::PlusScore(uint plus)
{
	return current_score += plus;;
}

uint j1Player::PlusLevel(uint plus)
{
	xp += plus;
	while (xp > lvl * 100)
	{
		xp -= lvl * 100;
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
