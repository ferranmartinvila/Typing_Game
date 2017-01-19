#ifndef _BLOCKS_MANAGER_
#define _BLOCKS_MANAGER_

#include "j1Module.h"
#include "Text_Block.h"

class j1BlocksManager : public j1Module
{
public:

	j1BlocksManager();
	~j1BlocksManager();

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

private:

	p2List<TextBlock*>	text_blocks;
	TextBlock*			target_block = nullptr;

	SDL_Color			default_color;
	_TTF_Font*			default_font = nullptr;

public:
	
	//Get Functions -----------------------------
	TextBlock*	GetBlockTarget()const;
	SDL_Color	GetDefaultColor()const;
	_TTF_Font*	GetDefaultFont()const;

	//Set Functions -----------------------------
	void		SetDefalutColor(const SDL_Color& new_color);
	void		SetDefalutFont(_TTF_Font*	def_font);

	//Factory -----------------------------------
	TextBlock*	GenerateTextBlock(const char* text);

	//Handle Console Input ----------------------
	void Console_Command_Input(Command* command, Cvar* cvar, p2SString* input);
};
#endif // _BLOCKS_MANAGER_
