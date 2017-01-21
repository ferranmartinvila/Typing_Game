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

	//Active/Deasctive scene
	void Activate();
	void Desactive();

private:

	//Scene UI ------------------------
	UI_Element*	screen_ui = nullptr;
	UI_String*	player_score_title = nullptr;
	UI_String*	player_score = nullptr;
	UI_String*	player_max_score_title = nullptr;
	UI_String*	player_max_score = nullptr;
	UI_String*	player_lvl_title = nullptr;
	UI_String*	player_lvl = nullptr;
	UI_Image*	height_limit = nullptr;

	//Background ----------------------
	SDL_Texture*		background = nullptr;
	PhysBody*			background_collide_mark = nullptr;

	//Timming ----------------------------
	j1Timer				label_generate_timer;
	uint				label_rate = 1000;
	
	j1Timer				scene_time;
	uint				timer_margin = 3;

public:

	//Functionality -----------------------------
	void	SetPlayerScoreText(uint score_value);
	void	SetPlayerMaxScoreText(uint max_value);
	void	SetPlayerLevelText(uint level_value);

	void	SetLabelRate(uint rate);

	uint	GetHeightLimit()const;
};

#endif // __j1SCENE_H__