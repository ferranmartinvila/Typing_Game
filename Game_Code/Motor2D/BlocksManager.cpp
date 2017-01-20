#define _CRT_SECURE_NO_WARNINGS
#include "BlocksManager.h"

#include <time.h>
#include <stdlib.h>

#include "p2Log.h"
#include "j1App.h"
#include "j1Fonts.h"
#include "j1Console.h"
#include "j1Physics.h"
#include "j1FileSystem.h"

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
	default_font = App->font->default;
	default_color_on = { 155,55,255,180 };
	default_color_off = { 255,105,255,255 };

	//Add Console Command
	App->console->AddCommand("reset", this);
	App->console->AddCommand("generate_block", this);

	//Set random seed
	srand(NULL);

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

	strings.Delete_All();

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

void j1BlocksManager::SetDefalutColor(const SDL_Color & new_color)
{
	default_color_on = new_color;
}

void j1BlocksManager::SetDefalutFont(_TTF_Font * def_font)
{
	default_font = def_font;
}

TextBlock * j1BlocksManager::GenerateTextBlock(const char * text)
{
	if (text == nullptr || strlen(text) == 0)return nullptr;

	//Create the new text block
	TextBlock* new_block = new TextBlock(text, default_font, App->blocks_manager->default_color_off,App->blocks_manager->default_color_on);
	
	//Add it to the manager list
	text_blocks.add(new_block);

	//Update target block
	target_block = text_blocks.start->data;

	return new_block;
}

TextBlock * j1BlocksManager::GenerateRandomTextBlock()
{
	//Create the new text block
	TextBlock* new_block = new TextBlock(GetRandomWord(), default_font, App->blocks_manager->default_color_off, App->blocks_manager->default_color_on);

	//Add it to the manager list
	text_blocks.add(new_block);

	//Update target block
	target_block = text_blocks.start->data;

	return new_block;
}

void j1BlocksManager::DeleteTarget()
{
	text_blocks.del(text_blocks.At(text_blocks.find(target_block)));
	App->physics->DeleteBody(target_block->GetBody());
	delete target_block;
	if (text_blocks.end != nullptr)target_block = text_blocks.end->data;
}

void j1BlocksManager::Console_Command_Input(Command * command, Cvar * cvar, p2SString * input)
{
	//Reset command
	if (*command->GetCommandStr() == "reset")
	{
		p2List_item<TextBlock*>* item = text_blocks.start;
		while (item)
		{
			item->data->ResetCharIndex();
			item = item->next;
		}
	}
	//Generate Block command
	if (*command->GetCommandStr() == "generate_block")
	{
		GenerateTextBlock("new_block");
	}
}
