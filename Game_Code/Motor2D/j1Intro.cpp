#include "j1Intro.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1SceneManager.h"

//UI Elements
#include "UI_Button.h"

//Constructors ----------------------------------
j1Intro::j1Intro()
{
	name.create("intro");
}

//Destructors -----------------------------------
j1Intro::~j1Intro()
{
}

//Game Loop -------------------------------------
bool j1Intro::Start()
{
	//Load Intro scene background -----
	background = App->tex->Load("textures/intro_background.png");

	//UI Intro build ------------------
	LOG("Building intro UI...");
	intro_ui = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	intro_ui->SetInputTarget(this);

	//Intro start button build --------
	start_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	start_button->SetBox({ 220,350,260,60 });
	start_button->SetTexON({ 0,66,256,60 });
	start_button->SetTexOVER({ 0,136,256,60 });
	start_button->SetTexOFF({ 0,205,256,60 });
	start_button->SetInputTarget(this);
	intro_ui->AddChild(start_button);

	//Intro quit button build ---------
	quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	quit_button->SetBox({ 260,570,185,70 });
	quit_button->SetTexON({ 0,271,185,70 });
	quit_button->SetTexOVER({ 0,347,182,66 });
	quit_button->SetTexOFF({ 0,423,184,67 });
	quit_button->SetInputTarget(this);
	intro_ui->AddChild(quit_button);

	//Activate UI elements ------------
	start_button->Activate();
	intro_ui->Activate();

	App->gui->PushScreen(intro_ui);
	return true;
}

bool j1Intro::PreUpdate()
{
	return true;
}

bool j1Intro::Update(float dt)
{
	bool ret = true;

	App->gui->CalculateUpperElement(intro_ui);
	App->render->Blit(background, 0, 0);


	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		ret = false;
	}
	return ret;
}

bool j1Intro::PostUpdate()
{


	return true;
}

bool j1Intro::CleanUp()
{
	LOG("Freeing Intro UI");
	
	intro_ui->Desactivate();
	start_button->Desactivate();
	quit_button->Desactivate();

	return true;
}

void j1Intro::GUI_Input(UI_Element * target, GUI_INPUT input)
{
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
		if (target == quit_button)
		{
			App->SetQuit();
		}
		else if (target == start_button)
		{
			App->scene_manager->ChangeScene(1500);
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	case SUPR:
		break;
	case BACKSPACE:
		break;
	case ENTER:
		break;
	case TAB:
		break;
	default:
		break;
	}
}

void j1Intro::Activate()
{
	LOG("Intro Activated!");
	active = true;
	intro_ui->Activate();
	start_button->Activate();
	quit_button->Activate();
}

void j1Intro::Desactivate()
{
	LOG("Intro Desactivated!");
	active = false;
	intro_ui->Desactivate();
	start_button->Desactivate();
	quit_button->Desactivate();
}


//Funtionality ----------------------------------
