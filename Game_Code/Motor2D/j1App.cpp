#define _CRT_SECURE_NO_WARNINGS

#include <iostream>		
#include <sstream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Physics.h"
#include "j1Scene.h"
#include "j1FileSystem.h"
#include "j1Fonts.h"

#include "j1Gui.h"
#include "j1Console.h"
#include "BlocksManager.h"
#include "Player.h"

#include "j1App.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{

	PERF_START(ptimer);

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	physics = new j1Physics();
	scene = new j1Scene();
	fs = new j1FileSystem();
	font = new j1Fonts();
	gui = new j1Gui();
	console = new j1Console();
	blocks_manager = new j1BlocksManager();
	player = new j1Player();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(fs);
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(font);
	AddModule(blocks_manager);

	AddModule(physics);
	
	// scene last
	AddModule(scene);
	AddModule(player);
	AddModule(gui);
	AddModule(console);

	// render last to swap buffer
	AddModule(render);

	

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	//Load App config data
	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if(cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	//Call modules Awake method
	if(ret == true)
	{
		p2List_item<j1Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	//Load Cvars 
	pugi::xml_node module_node = app_config;
	while (module_node != NULL)
	{

		pugi::xml_node cvar = module_node.child("c_vars").first_child();
		while (cvar != NULL)
		{
			//Load CVar data
			p2SString name = cvar.attribute("name").value();
			p2SString description = cvar.attribute("description").value();
			p2SString value = cvar.attribute("value").value();
			
			const p2SString type = cvar.attribute("type").value();
			C_VAR_TYPE cv_type = App->console->StringtoCvarType(&type);
			
			const p2SString module = module_node.name();
			j1Module* cv_module = App->GetModule(&module);

			bool only_read = cvar.attribute("only_read").as_bool();
			
			//Build CVar
			Cvar* cv = App->console->LoadCvar(name.GetString(), description.GetString(), value.GetString(), cv_type, cv_module,only_read);

			LOG("-- %s -- CVar added at %s", cv->GetCvarName()->GetString(), module.GetString());
			
			//Next cvar
			cvar = cvar.next_sibling();
		}

		//Next module
		module_node = module_node.next_sibling();
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{

	PERF_START(ptimer);
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	//Add Console Commands
	console->AddCommand("quit", nullptr);
	console->AddCommand("save", nullptr);
	console->AddCommand("load", nullptr);

	//Add Console Cvars
	save_dir = App->console->AddCvar("save_dir", "Directory where game data is saved", "game_save.xml", C_VAR_TYPE::CHAR_VAR, nullptr, false);
	load_dir = App->console->AddCvar("load_dir", "Directory from app load data", "game_save.xml", C_VAR_TYPE::CHAR_VAR, nullptr, false);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();

	// Framerate calculations --

	if(last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %lu ",
			  avg_fps, last_frame_ms, frames_on_last_update, dt, seconds_since_startup, frame_count);
	App->win->SetTitle(title);

	if(capped_ms > 0 && last_frame_ms < capped_ms)
	{
		j1PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	ret = !want_to_quit;

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(ptimer);
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	p2List_item<p2SString*>* str_item = saved_games.end;
	p2List_item<p2SString*>* item_prev = nullptr;

	if (str_item != nullptr)item_prev = str_item->prev;
	while (str_item) {

		//CleanUp the item childs
		ret = item->data->CleanUp();
		//Delete all item data
		saved_games.del(str_item);

		str_item = item_prev;
		if (item_prev != nullptr)item_prev = item_prev->prev;

	}

	PERF_PEEK(ptimer);
	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
float j1App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void j1App::LoadGame(const char* file)
{
	bool ret = false;
	p2List_item<p2SString*>* saved_game = saved_games.start;
	while (saved_game != NULL)
	{
		if (*saved_game->data == file)
		{
			ret = true;
			break;
		}
		saved_game = saved_game->next;
	}
	if (ret)
	{
		want_to_load = true;
		load_game.create("%s%s", fs->GetSaveDirectory(), file);
	}
	else LOG("Load Directory is no available!");
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	p2List_item<p2SString*>* saved_game = saved_games.start;
	bool exist = false;
	while (saved_game != NULL)
	{
		if (*saved_game->data == file)exist = true;
		saved_game = saved_game->next;
	}
	if (!exist)
	{
		p2SString* new_file_str = new p2SString(file);
		saved_games.add(new_file_str);

	}

	want_to_save = true;
	save_game.create(file);
}

// ---------------------------------------
void j1App::GetSaveGames(p2List<p2SString*>& list_to_fill) const
{
	list_to_fill = saved_games;
}

bool j1App::IsXMLdir(const char * dir) const
{
	uint len = strlen(dir);
	if (len < 4)return false;
	bool format_zone = false;
	char* temp = new char[strlen(dir)];
	uint j = 0;
	for (uint k = 0; k < len; k++)
	{
		if (dir[k] == '.')format_zone = true;
		else if (format_zone)
		{
			temp[j] = dir[k];
			j++;
		}

	}
	temp[j] = '\0';
	p2SString str = temp;
	delete temp;
	if (str == "xml")return true;

	return false;
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	char* buffer;
	uint size = fs->Load(load_game.GetString(), &buffer);

	if(size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if(result != NULL)
		{
			LOG("Loading new Game State from %s...", load_game.GetString());

			root = data.child("game_state");

			p2List_item<j1Module*>* item = modules.start;
			ret = true;

			while(item != NULL && ret == true)
			{
				ret = item->data->Load(root.child(item->data->name.GetString()));
				item = item->next;
			}

			data.reset();
			if(ret == true)
				LOG("...finished loading");
			else
				LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
		}
		else
			LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());
	}
	else
		LOG("Could not load game state xml file %s", load_game.GetString());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	p2List_item<j1Module*>* item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if(ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save(save_game.GetString(), stream.str().c_str(), stream.str().length());
		LOG("... finished saving", save_game.GetString());
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}

j1Module * j1App::GetModule(const p2SString* module_name) const
{
	p2List_item<j1Module*>* item = modules.start;
	
	while (item)
	{
		if (*module_name == item->data->name.GetString())
		{
			return item->data;
		}

		item = item->next;
	}
	return nullptr;
}

uint j1App::GetModulesNum() const
{
	return modules.count();
}

j1Module * j1App::GetModuleAt(uint index) const
{
	return modules.At(index)->data;
}

pugi::xml_node j1App::GetConfigXML() const
{
	return config_node;
}

void j1App::Console_Command_Input(Command * command, Cvar * cvar, p2SString * input)
{
	if (*command->GetCommandStr() == "quit")
	{
		SetQuit();
	}
	else if (*command->GetCommandStr() == "save")
	{
		SaveGame(save_dir->GetValueString()->GetString());
		
	}
	else if (*command->GetCommandStr() == "load")
	{
		LoadGame(load_dir->GetValueString()->GetString());
	}
}

void j1App::Console_Cvar_Input(Cvar * cvar, Command* command_type, p2SString * input)
{
	//Set command
	if (*command_type->GetCommandStr() == "set")
	{
		//Maxfps cvar
		if (*cvar->GetCvarName() == "maxfps")
		{
			if (cvar->GetValueAsNum() < 1)cvar->SetValue("-1");
			else if (cvar->GetValueAsNum() > 120)cvar->SetValue("120");

			//Set cvar value
			cvar->SetValue(input->GetString());

			//Set new ms delay
			capped_ms = 1000 / cvar->GetValueAsNum();

		}
		//Save_dir cvar
		else if (*cvar->GetCvarName() == "save_dir")
		{
			if (strlen(input->GetString()) > 5 && IsXMLdir(input->GetString()))
			{
				//Set new save directory
				App->save_game.create(input->GetString());
				
				//Set cvar value
				cvar->SetValue(input->GetString());
			}
			else App->console->GenerateConsoleLabel("Invalid Save Directory: %s", input->GetString(),cvar->GetCvarName()->GetString());

		}

		else if (*cvar->GetCvarName() == "load_dir")
		{
			if (strlen(input->GetString()) > 5 && IsXMLdir(input->GetString()))
			{
				//Set new save directory
				App->load_game.create(input->GetString());

				//Set cvar value
				cvar->SetValue(input->GetString());
			}
			else App->console->GenerateConsoleLabel("Invalid Load Directory: %s", input->GetString(), cvar->GetCvarName()->GetString());

		}
		//Unknown cvar
		else
		{
			App->console->GenerateConsoleLabel("Cvar id Error at module Render");
			return;
		}

	}
}

void j1App::SetQuit()
{
	want_to_quit = true;
}


