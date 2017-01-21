#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"

//UI_Elements
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll.h"
#include "UI_String.h"
#include "UI_Text_Box.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	//Load Atlas
	atlas = App->tex->Load(atlas_file_name.GetString());

	//Load other textures

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool ret = true;

	p2List_item<UI_Element*>* item = screens.start;
	while (item) {

		if (item->data->GetActiveState())
			ret = item->data->Update();

		item = item->next;
	}

	return ret;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	//Debug Mode ------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)debug = !debug;

	//Tab Input
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN && screens.start != nullptr)
	{
		if (ItemSelected == NULL)ItemSelected = GetTabElement(screens.start->data, 1);
		else if (ItemSelected->GetInputTarget() != (j1Module*)App->console)
		{
			ItemSelected = GetTabElement(screens.start->data, ItemSelected->GetTabNum() + 1);
		}

	}

	// Update & draw the UI screens
	p2List_item<UI_Element*>* item = screens.start;
	while (item) {

		if (item->data->GetActiveState())
			item->data->Draw(debug);

		item = item->next;
	}

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	bool ret = true;
	p2List_item<UI_Element*>* item = screens.end;
	p2List_item<UI_Element*>* item_prev = nullptr;

	if (item != nullptr)item_prev = item->prev;
	while (item) {

		//CleanUp the item childs
		ret = item->data->CleanUp();
		//Delete all item data
		screens.del(item);

		item = item_prev;
		if (item_prev != nullptr)item_prev = item_prev->prev;

	}

	return ret;
}




// =================================================================
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

SDL_Texture * j1Gui::Get_UI_Texture(uint tex_id)
{
	if (tex_id > ui_textures.count() - 1)return nullptr;

	return ui_textures.At(tex_id)->data;
}

uint j1Gui::GetTabNumber() const
{
	return tabable_elements;
}

void j1Gui::SetTabNumber(uint new_tab_num)
{
	tabable_elements = new_tab_num;
}

UI_Element * j1Gui::GetTabElement(UI_Element* screen, uint index) const
{
	UI_Element* tab_element = nullptr;

	p2List_item<UI_Element*>* item = screen->childs.start;

	while (item) {

		tab_element = GetTabElement(item->data, index);

		if (item->data->GetTabNum() == index && item->data->GetActiveState())
		{
			tab_element = item->data;
			break;
		}
		item = item->next;

	}
	return tab_element;
}

uint j1Gui::PushScreen(const UI_Element* new_screen)
{
	screens.add((UI_Element*)new_screen);
	return screens.count();
}

bool j1Gui::DeleteScreen(UI_Element * target_screen)
{
	uint index = screens.find(target_screen);
	if (index == -1)return false;

	target_screen->CleanUp();
	screens.del(screens.At(index));

	return true;
}

UI_Element* j1Gui::GenerateUI_Element(UI_TYPE element_type)
{
	UI_Element* new_element = nullptr;
	switch (element_type)
	{
	case UNDEFINED:		
		new_element = new UI_Element();		
	break;

	case BUTTON:	
		new_element = new UI_Button();
		break;

	case TEXT_BOX:
		new_element = new UI_Text_Box();
		break;

	case STRING:
		new_element = new UI_String();
		break;

	case IMG:
		new_element = new UI_Image();
		break;

	case SCROLL:
		new_element = new UI_Scroll();
		break;
	}
	
	return new_element;
}

UI_Element * j1Gui::GetActiveScreen() const
{
	p2List_item<UI_Element*>* item = screens.start;
	while (item)
	{
		if (item->data->GetActiveState())return item->data;
		item = item->next;
	}
	return nullptr;
}

uint j1Gui::CalculateUpperElement(const UI_Element* parent, uint layer) const
{

	p2List_item<UI_Element*>* item = parent->childs.start;

	while (item) {

		layer = CalculateUpperElement(item->data, layer);

		if (item->data->GetLayer() > layer && item->data->GetActiveState() && item->data->MouseIsIn())layer = item->data->GetLayer();

		item = item->next;

	}
	upper_element = layer;
	return uint(layer);
}

void j1Gui::SetDefaultInputTarget(j1Module * target)
{
	default_input_target = target;
}

j1Module * j1Gui::GetDefaultInputTarget() const
{
	if (default_input_target == nullptr)return nullptr;
	return default_input_target;
}

