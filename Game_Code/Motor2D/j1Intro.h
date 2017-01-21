#ifndef _INTRO_
#define _INTRO_

#include "j1Module.h"

struct SDL_Texture;
struct UI_Button;
struct UI_Scroll;

class j1Intro : public j1Module
{
public:

	j1Intro();

	// Destructor
	virtual ~j1Intro();

	// Called before render is available
	//bool Awake(pugi::xml_node& config);

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

	//Active/Deasctive intro
	void Activate();
	void Desactivate();

private:

	//Scene UI ------------------------
	UI_Element*	intro_ui = nullptr;
	UI_Button*	start_button = nullptr;
	UI_Button*	quit_button = nullptr;

	//Background ----------------------
	SDL_Texture*		background = nullptr;

public:

	//Functionality -----------------------------

};

#endif // !_INTRO_
