#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2DynArray.h"
#include "j1Timer.h"

struct SDL_Texture;
struct UI_Button;
struct UI_String;
struct UI_Interactive_String;
struct UI_Image;
struct UI_Element;
struct UI_Text_Box;
struct UI_Scroll;
struct PhysBody;
struct TextBlock;

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

	//Background ----------------------
	SDL_Texture*		background;
	PhysBody*			background_collide_mark;

	//Timming ----------------------------
	j1Timer				label_generate_timer;
	uint				label_rate = 1000;
};

#endif // __j1SCENE_H__