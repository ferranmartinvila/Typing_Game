#ifndef _PLAYER_
#define _PLAYER_

#include "j1Module.h"

class j1Player :public j1Module
{
public:

	j1Player();
	~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	/*// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();*/

private:

	uint lvl = 1;
	uint xp = 0;
	uint current_score = 0;
	uint max_score = 0;

	bool alive = true;

public:

	//Functionality
	void	EndParty();
	bool	GetPlayerState()const;

	uint	PlusScore(uint plus);
	uint	PlusLevel(uint plus);
	
	uint	GetCurrentScore()const;
	uint	GetMaxScore()const;
	uint	GetLevel()const;
	uint	GetXp()const;

};

#endif // _PLAYER_
