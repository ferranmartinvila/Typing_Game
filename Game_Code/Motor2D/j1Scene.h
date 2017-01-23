#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2DynArray.h"
#include "j1Timer.h"

struct SDL_Texture;
struct UI_String;
struct UI_Image;
struct UI_Button;
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

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Recieve UI input and work with it
	void GUI_Input(UI_Element* target, GUI_INPUT input);

	// Recieve Physics Collisions
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	//Active/Deasctive scene
	void Activate();
	void Desactivate();

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
	UI_Button*	restart_button = nullptr;

	//Background ----------------------
	SDL_Texture*		background = nullptr;
	PhysBody*			background_collide_mark = nullptr;

	//Timming ----------------------------
	j1Timer				label_generate_timer;
	uint				base_label_rate = 3000;

	j1Timer				scene_time;
	j1Timer				margin_timer;
	uint				timer_margin = 3500;
	bool				timer_started = false;

	//Audio ---------------------------
	uint	box_contact_fx;
	uint	scene_quit_fx;
	uint	game_lose_fx;
	uint	restart_fx;
	uint	limit_alarm_fx;

public:

	//Functionality -----------------------------
	void	SetPlayerScoreText(uint score_value);
	void	SetPlayerMaxScoreText(uint max_value);
	void	SetPlayerLevelText(uint level_value);

	void	SetLabelRate(uint rate);

	uint	GetHeightLimit()const;
	uint	GetSceneTimer()const;
	uint	GetTimerMargin()const;

};

#endif // __j1SCENE_H__