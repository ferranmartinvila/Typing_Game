#define _CRT_SECURE_NO_WARNINGS
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Physics.h"
#include "Player.h"

//UI Elements
#include "UI_String.h"
#include "UI_Image.h"

//Text Blocks
#include "BlocksManager.h"


j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{



	return true;
}

// Called before the first frame
bool j1Scene::Start()
{

	//Background mark collide build
	int background_points[18] = {
		621, 3,
		620, 870,
		28, 871,
		28, 7,
		1, 7,
		1, 892,
		648, 892,
		647, 4,
		628, 4
	};
	background_collide_mark = App->physics->CreateChain(0, 0, background_points, 18, MAP);

	//Load Scene background -------------------------------
	background = App->tex->Load("textures/background.png");

	//UI Scene build --------------------------------------
	LOG("Building scene UI...");
	screen_ui = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	screen_ui->SetInputTarget(this);

	//UI Player score title build -----
	player_score_title = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	player_score_title->SetBoxPosition(420, 20);
	player_score_title->SetFont(App->font->default);
	player_score_title->SetColor({ 80,80,80,255 });
	player_score_title->SetInputTarget(this);
	player_score_title->SetString("Score:");
	player_score_title->GenerateTexture();
	screen_ui->AddChild(player_score_title);

	//UI Player Score build -----------
	player_score = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	player_score->SetBoxPosition(500, 22);
	player_score->SetFont(App->font->default);
	player_score->SetColor({ 80,80,80,255 });
	player_score->SetInputTarget(this);
	SetPlayerScoreText(App->player->GetCurrentScore());
	screen_ui->AddChild(player_score);

	//UI Player max Score title build -
	player_max_score_title = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	player_max_score_title->SetBoxPosition(420, 50);
	player_max_score_title->SetFont(App->font->default);
	player_max_score_title->SetColor({ 80,80,80,255 });
	player_max_score_title->SetInputTarget(this);
	player_max_score_title->SetString("Record:");
	screen_ui->AddChild(player_max_score_title);

	//UI Player max Score build -------
	player_max_score = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	player_max_score->SetBoxPosition(515, 52);
	player_max_score->SetFont(App->font->default);
	player_max_score->SetColor({ 80,80,80,255 });
	player_max_score->SetInputTarget(this);
	SetPlayerMaxScoreText(App->player->GetMaxScore());
	screen_ui->AddChild(player_max_score);

	//UI Player Level title build -----
	player_lvl_title = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	player_lvl_title->SetBoxPosition(35, 20);
	player_lvl_title->SetFont(App->font->default);
	player_lvl_title->SetColor({ 80,80,80,255 });
	player_lvl_title->SetInputTarget(this);
	player_lvl_title->SetString("Level:");
	player_lvl_title->GenerateTexture();
	screen_ui->AddChild(player_lvl_title);

	//UI Player Level build -----------
	player_lvl = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	player_lvl->SetBoxPosition(110, 20);
	player_lvl->SetFont(App->font->default);
	player_lvl->SetColor({ 80,80,80,255 });
	player_lvl->SetInputTarget(this);
	SetPlayerLevelText(App->player->GetLevel());
	screen_ui->AddChild(player_lvl);

	//UI Scene Height Limit build -----
	height_limit = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	height_limit->SetBox({ 28,550,594,56 });
	height_limit->ChangeTextureRect({ 0,0,594,56 });
	screen_ui->AddChild(height_limit);
	
	App->gui->PushScreen(screen_ui);
	// ----------------------------------------------------

	// Active scene UI ------------------------------------
	screen_ui->Activate();
	player_score_title->Activate();
	player_score->Activate();
	// ----------------------------------------------------

	//Timer Start -----------------------------------------
	label_generate_timer.Start();
	scene_time.Start();

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	App->render->Blit(background, 0, 0);

	// Gui Upper Element ---------------------------
	//App->gui->CalculateUpperElement(scene_1_screen);

	
	if (App->player->GetPlayerState())
	{
		//Check label generate timer
		if (label_generate_timer.Read() > label_rate) {
			TextBlock* item = App->blocks_manager->GenerateRandomTextBlock(5, 2);
			item->SetBornTime(scene_time.ReadSec());
			label_generate_timer.Start();
		}

		//Check blocks height
		if (App->blocks_manager->GetHigherBlock(scene_time.ReadSec() - timer_margin) != nullptr)
		{
			App->player->EndParty();
			App->blocks_manager->GetHigherBlock(scene_time.ReadSec() - timer_margin);
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		App->ChangeScene();

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	return true;
}

void j1Scene::GUI_Input(UI_Element* target, GUI_INPUT input)
{
	int x, y;
	App->input->GetMouseMotion(x, y);
	switch (input)
	{
	case UP_ARROW:
		break;
	case DOWN_ARROW:
		break;
	case LEFT_ARROW:
		break;
	case RIGHT_ARROW:
		break;
	case MOUSE_LEFT_BUTTON_DOWN:
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case BACKSPACE:
		break;
	case SUPR:
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	case ENTER:
		break;
	}
}

void j1Scene::Activate()
{
	LOG("Scene Activated!");
	active = true;
	screen_ui->Activate();
	player_score_title->Activate();
	player_score->Activate();
	player_max_score_title->Activate();
	player_max_score->Activate();
	player_lvl_title->Activate();
	player_lvl->Activate();
	height_limit->Activate();
	scene_time.Start();
	label_generate_timer.Start();
	App->physics->Activate();
	App->blocks_manager->Activate();
}

void j1Scene::Desactive()
{
	LOG("Scene Desactivated!");
	active = false;
	screen_ui->Desactivate();
	player_score_title->Desactivate();
	player_score->Desactivate();
	player_max_score_title->Desactivate();
	player_max_score->Desactivate();
	player_lvl_title->Desactivate();
	player_lvl->Desactivate();
	height_limit->Desactivate();
	App->blocks_manager->DeleteAllBlocks();
	App->blocks_manager->Desactivate();
	App->physics->Desactivate();
}

//Functionality -----------------------------
void j1Scene::SetPlayerScoreText(uint score_value)
{
	char* str = new char[10];
	_itoa(score_value, str, 10);
	player_score->SetString(str);
	player_score->GenerateTexture();
	delete str;
}

void j1Scene::SetPlayerMaxScoreText(uint max_value)
{
	char* str = new char[10];
	_itoa(max_value, str, 10);
	player_max_score->SetString(str);
	player_max_score->GenerateTexture();
	delete str;
}

void j1Scene::SetPlayerLevelText(uint level_value)
{
	char* str = new char[10];
	_itoa(level_value, str, 10);
	player_lvl->SetString(str);
	player_lvl->GenerateTexture();
	delete str;
}

void j1Scene::SetLabelRate(uint rate)
{
	label_rate = rate;
}

uint j1Scene::GetHeightLimit() const
{
	return (height_limit->GetBox()->y + height_limit->GetBox()->h);
}
