#ifndef _BLOCKS_MANAGER_
#define _BLOCKS_MANAGER_

#include "j1Module.h"
#include "Text_Block.h"
#include "p2DynArray.h"

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

	SDL_Color			default_color_on;
	SDL_Color			default_color_off;
	SDL_Color			default_target_color;
	SDL_Color			default_nontarget_color;
	_TTF_Font*			default_font = nullptr;
	SDL_Texture*		default_block_texture = nullptr;

	pugi::xml_document	strings_doc;
	p2DynArray<char*>	strings;

public:
	
	

	//Load Strings Data -------------------------
	int LoadStringsXML(pugi::xml_document& data_file) const;

	//Get Functions -----------------------------
	TextBlock*		GetBlockTarget()const;
	SDL_Color		GetDefaultColor()const;
	_TTF_Font*		GetDefaultFont()const;
	char*			GetRandomWord()const;
	SDL_Texture*	GetDefaultBlockTexture()const;
	SDL_Color		GetTargetColor()const;
	SDL_Color		GetNonTargetColor()const;
	TextBlock*		GetHigherBlock(int time)const;

	//Set Functions -----------------------------
	void		SetDefalutColor(const SDL_Color& new_color);
	void		SetDefalutFont(_TTF_Font*	def_font);
	void		SetBlockTarget(TextBlock* target);

	//Factory -----------------------------------
	TextBlock*	GenerateTextBlock(const char* text);
	TextBlock*	GenerateRandomTextBlock(uint x_margin = 0, uint y_margin = 0);
	void		DeleteTarget();
	bool		DeleteAllBlocks();

	//Handle Console Input ----------------------
	void Console_Command_Input(Command* command, Cvar* cvar, p2SString* input);
};
#endif // _BLOCKS_MANAGER_
