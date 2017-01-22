#include "Player.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "j1App.h"
#include "BlocksManager.h"

//Constructors ----------------------------------
j1Player::j1Player()
{
	name.create("player");
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

bool j1Player::Load(pugi::xml_node &load_node)
{
	//Load Player Score
	max_score = load_node.child("score").attribute("value").as_int();
	App->scene->SetPlayerMaxScoreText(max_score);

	return true;
}

bool j1Player::Save(pugi::xml_node &save_node)const
{
	//Save Player Score
	pugi::xml_node new_node = save_node.append_child("score");

	new_node.append_attribute("value") = max_score;

	return true;
}


void j1Player::EndParty()
{
	alive = false;
	if (current_score > max_score)max_score = current_score;
	current_score = 0;
	lvl = 1;
	App->scene->SetPlayerLevelText(lvl);
	App->scene->SetPlayerMaxScoreText(max_score);
	App->scene->SetPlayerScoreText(current_score);
	App->blocks_manager->DeleteAllBlocks();
	LOG("You Lose :(");
}

void j1Player::StartParty()
{
	alive = true;
	LOG("Party Started!");
}

bool j1Player::GetPlayerState() const
{
	return alive;
}

//Functionality ---------------------------------
uint j1Player::PlusScore(uint plus)
{
	return current_score += plus;
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
