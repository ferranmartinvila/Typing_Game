#define _CRT_SECURE_NO_WARNINGS
#include "BlocksManager.h"

#include "Text_Block.h"

#include <time.h>
#include <stdlib.h>

#include "p2Log.h"
#include "j1App.h"
#include "j1Fonts.h"
#include "j1Console.h"
#include "j1Physics.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "Player.h"
#include"j1Scene.h"
#include "j1Audio.h"

//Constructors ----------------------------------
j1BlocksManager::j1BlocksManager()
{
	name.create("blocks_manager");
}

//Destructors -----------------------------------
j1BlocksManager::~j1BlocksManager()
{
}

//Game Loop -------------------------------------
bool j1BlocksManager::Awake(pugi::xml_node & config)
{
	//Load all strings XML data
	LOG("Loading strings.xml data...");
	uint str_size = LoadStringsXML(strings_doc);
	if (str_size == 0)return false;

	//Focus strings node
	pugi::xml_attribute node = strings_doc.first_child().child("scene_1").attribute("data");
	
	//Allocate a buffer for data
	char* buffer = (char*)node.value();

	//Tokenize buffer and push data to stings array
	char* str_seg = strtok(buffer, ",");
	while(str_seg != NULL)
	{
		strings.PushBack(str_seg);
		str_seg = strtok(NULL, ", ");
	}

	return true;
}

bool j1BlocksManager::Start()
{
	//Load audios
	block_break_fx = App->audio->LoadFx("audio/fx/block_break_fx.wav");

	default_font = App->font->default;
	default_target_color = { 180, 180, 80,255 };
	default_nontarget_color = { 0,0,0,255 };
	default_color_on = { 0,0,0,255 };
	default_color_off = { 255,255,255,255 };

	//Add Console Command
	App->console->AddCommand("reset", this);
	App->console->AddCommand("generate_block", this);

	//Set random seed
	srand(NULL);

	//Load default block texture
	default_block_texture = App->tex->Load("textures/block_texture.png");

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

		//Delete all item data
		ret = item->data->CleanUp();

		//Delete item from list
		text_blocks.del(item);
		item = item_prev;

		if (item_prev != nullptr)item_prev = item_prev->prev;
	}
	
		ret = strings.Delete_All();

	return ret;
}

int j1BlocksManager::LoadStringsXML(pugi::xml_document& data_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("strings.xml", &buf);
	pugi::xml_parse_result result = data_file.load_buffer(buf, size);
	RELEASE(buf);

	if (result == NULL)
	{
		LOG("Could not load strings_data xml file strings.xml pugi error: %s", result.description());
		return false;
	}

	LOG("strings_data from strings.xml loaded!");
	return size;
}

//Functionality ---------------------------------
TextBlock * j1BlocksManager::GetBlockTarget() const
{
	return target_block;
}

SDL_Color j1BlocksManager::GetDefaultColor() const
{
	return default_color_on;
}

_TTF_Font * j1BlocksManager::GetDefaultFont() const
{
	return default_font;
}

char * j1BlocksManager::GetRandomWord() const
{
	//Generate a rand number between 0 and strings array size
	int index = rand() % strings.Count();
	//Return string at random index
	return strings[index];
}

SDL_Texture * j1BlocksManager::GetDefaultBlockTexture() const
{
	return default_block_texture;
}

SDL_Color j1BlocksManager::GetTargetColor() const
{
	return default_target_color;
}

SDL_Color j1BlocksManager::GetNonTargetColor() const
{
	return default_nontarget_color;
}

TextBlock * j1BlocksManager::GetHigherBlock() const
{
	if (text_blocks.start == nullptr)return nullptr;
	
	int x, y;
	TextBlock*	target = nullptr;
	
	p2List_item<TextBlock*>* item = text_blocks.start;
	while (item)
	{
		item->data->GetPosition(x, y);

		if (y < App->scene->GetHeightLimit() && item->data->GetBornTime() < App->scene->GetSceneTimer() - App->scene->GetTimerMargin())
		{
			target = item->data;
			break;
		}
		item = item->next;
	}

	return target;
}

void j1BlocksManager::SetDefalutColor(const SDL_Color & new_color)
{
	default_color_on = new_color;
}

void j1BlocksManager::SetDefalutFont(_TTF_Font * def_font)
{
	default_font = def_font;
}

void j1BlocksManager::SetBlockTarget(TextBlock * target)
{
	if (target_block == target)return;
	target_block = target;
	target_block->GenerateTextureFromText();
}

TextBlock * j1BlocksManager::GenerateTextBlock(const char * text)
{
	if (text == nullptr || strlen(text) == 0)return nullptr;

	//Create the new text block
	TextBlock* new_block = new TextBlock(text, default_font, App->blocks_manager->default_color_off,App->blocks_manager->default_color_on);
	
	//Set new block listener
	new_block->GetBody()->listener = App->scene;

	//Add it to the manager list
	text_blocks.add(new_block);

	SetBlockTarget(text_blocks.start->data);

	return new_block;
}

TextBlock * j1BlocksManager::GenerateRandomTextBlock(uint x_margin, uint y_margin)
{
	//Create the new text block
	TextBlock* new_block = new TextBlock(GetRandomWord(), default_font, App->blocks_manager->default_color_off, App->blocks_manager->default_color_on,x_margin,y_margin);
	
	//Set new block listener
	new_block->GetBody()->listener = App->scene;
	
	//Add it to the manager list
	text_blocks.add(new_block);

	SetBlockTarget(text_blocks.start->data);

	return new_block;
}

void j1BlocksManager::DeleteTarget()
{
	//Add Target score to player score
	App->player->PlusScore(target_block->GetScore());
	App->scene->SetPlayerScoreText(App->player->GetCurrentScore());
	//Add Target score to player xp
	uint player_lvl = App->player->GetLevel();
	App->player->PlusLevel(target_block->GetScore());
	if (player_lvl != App->player->GetLevel())
	{
		App->scene->SetPlayerLevelText(App->player->GetLevel());
	}
	//Delete target block
	text_blocks.del(text_blocks.At(text_blocks.find(target_block)));
	App->physics->DeleteBody(target_block->GetBody());
	delete target_block;

	//Set new target block
	if (text_blocks.start != nullptr)SetBlockTarget(text_blocks.start->data);
}

bool j1BlocksManager::DeleteAllBlocks()
{
	bool ret = true;
	p2List_item<TextBlock*>* item = text_blocks.end;
	p2List_item<TextBlock*>* item_prev = nullptr;

	if (item != nullptr)item_prev = item->prev;
	while (item) {


		//Delete item body
		App->physics->DeleteBody(item->data->GetBody());

		//Delete all item data
		ret = item->data->CleanUp();

		//Delete item from list
		text_blocks.del(item);
		item = item_prev;
		if (item_prev != nullptr)item_prev = item_prev->prev;

	}
	return ret;
}

void j1BlocksManager::Console_Command_Input(Command * command, Cvar * cvar, p2SString * input)
{

	//Reset command
	if (*command->GetCommandStr() == "reset")
	{
		DeleteAllBlocks();
	}
	//Generate Block command
	if (*command->GetCommandStr() == "generate_block")
	{
		GenerateRandomTextBlock(5,2);
	}
}

void j1BlocksManager::PlayBlockBreakFx() const
{
	App->audio->PlayFx(block_break_fx);
}
