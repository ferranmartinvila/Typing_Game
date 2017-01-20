#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2DynArray.h"
#include "j1Timer.h"

struct SDL_Texture;
struct UI_String;
struct UI_Image;
struct PhysBody;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Recieve UI input and work with it
	void GUI_Input(UI_Element* target, GUI_INPUT input);

private:

	//Scene UI ------------------------
	UI_String*	player_score;
	UI_String*	player_max_score;
	UI_String*	player_lvl;
	UI_Image*	player_data_marks;

	//Background ----------------------
	SDL_Texture*		background;
	PhysBody*			background_collide_mark;

	//Timming ----------------------------
	j1Timer				label_generate_timer;
	uint				label_rate = 1000;

public:

	//Functionality -----------------------------
	void	SetPlayerScoreText(uint score_value);
};

#endif // __j1SCENE_H__