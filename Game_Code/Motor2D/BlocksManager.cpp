#include "BlocksManager.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Fonts.h"

//Constructors ----------------------------------
j1BlocksManager::j1BlocksManager()
{
}

//Destructors -----------------------------------
j1BlocksManager::~j1BlocksManager()
{
}

//Game Loop -------------------------------------
bool j1BlocksManager::Awake(pugi::xml_node & config)
{
	return true;
}

bool j1BlocksManager::Start()
{
	default_font = App->font->default;
	default_color = { 155,55,255,180 };
	return true;
}

bool j1BlocksManager::PreUpdate()
{
	return true;
}

bool j1BlocksManager::Update(float dt)
{


	return true;
}

bool j1BlocksManager::PostUpdate()
{
	p2List_item<TextBlock*>* item = text_blocks.start;
	while (item)
	{
		item->data->Draw();
		item = item->next;
	}
	return true;
}

bool j1BlocksManager::CleanUp()
{
	LOG("Freeing BlocksManager");
	bool ret = true;
	p2List_item<TextBlock*>* item = text_blocks.end;
	p2List_item<TextBlock*>* item_prev = nullptr;

	if (item != nullptr)item_prev = item->prev;
	while (item) {

		//CleanUp the item childs
		ret = item->data->CleanUp();
		//Delete all item data
		text_blocks.del(item);

		item = item_prev;
		if (item_prev != nullptr)item_prev = item_prev->prev;

	}

	return ret;
}


//Functionality ---------------------------------
SDL_Color j1BlocksManager::GetDefaultColor() const
{
	return default_color;
}

_TTF_Font * j1BlocksManager::GetDefaultFont() const
{
	return default_font;
}

void j1BlocksManager::SetDefalutColor(const SDL_Color & new_color)
{
	default_color = new_color;
}

void j1BlocksManager::SetDefalutFont(_TTF_Font * def_font)
{
	default_font = def_font;
}

TextBlock * j1BlocksManager::GenerateTextBlock(const char * text)
{
	if (text == nullptr || strlen(text) == 0)return nullptr;

	//Create the new text block
	TextBlock* new_block = new TextBlock(text, default_font, default_color);
	
	//Add it to the manager list
	text_blocks.add(new_block);

	return new_block;
}
