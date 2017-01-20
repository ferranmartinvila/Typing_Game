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
	App->physics->CreateChain(0, 0, background_points, 18, MAP);

	//Load Scene background -------------------------------
	background = App->tex->Load("textures/background.png");


	//UI Scene build --------------------------------------
	UI_Element* screen_1_ui = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	screen_1_ui->Activate();
	screen_1_ui->SetInputTarget(this);

	//UI Player Score build
	player_score = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	player_score->SetBoxPosition(450, 125);
	player_score->SetFont(App->font->default);
	player_score->SetColor({ 80,80,80,255 });
	player_score->Activate();
	player_score->SetInputTarget(this);
	screen_1_ui->AddChild(player_score);


	App->gui->PushScreen(screen_1_ui);
	// ----------------------------------------------------


	//Timer Start -----------------------------------------
	label_generate_timer.Start();

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

	//Check label generate timer
	if (label_generate_timer.Read() > label_rate) {
		TextBlock* item = App->blocks_manager->GenerateRandomTextBlock(5,2);
		label_generate_timer.Start();
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

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

//Functionality -----------------------------
void j1Scene::SetPlayerScoreText(uint score_value)
{
	char* str = new char[10];
	_itoa(score_value, str, 10);
	player_score->SetString(str);
	player_score->GenerateTexture();
	delete str;
}
