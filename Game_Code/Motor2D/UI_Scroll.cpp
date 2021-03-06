#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "p2Log.h"
#include "UI_Scroll.h"
#include "j1Render.h"

//Constructors ============================================
UI_Scroll::UI_Scroll(const SDL_Rect& box, const SDL_Rect& ContentWindow, const UI_Image& ScrollItem, const UI_Image& ScrollBack, SCROLL_TYPE Scroll_Type, int MaxValue) :UI_Element(box, SCROLL), ContentWindow(ContentWindow), ScrollItem(ScrollItem), ScrollBack(ScrollBack), Scroll_Type(Scroll_Type), MaxValue(MaxValue) {}

UI_Scroll::UI_Scroll(const UI_Scroll* copy) : UI_Element(copy->box, SCROLL), ContentWindow(copy->ContentWindow), ScrollItem(copy->ScrollItem), ScrollBack(copy->ScrollBack), Scroll_Type(copy->Scroll_Type), MaxValue(copy->MaxValue) {}

UI_Scroll::UI_Scroll() : UI_Element({ 0,0,0,0 }, SCROLL), ContentWindow({0,0,0,0}),ScrollItem(),ScrollBack() {}


//Destructors =============================================
UI_Scroll::~UI_Scroll()
{

}

//Game Loop ===============================================
void UI_Scroll::Draw(bool debug)
{
	//Draw Scroll & Content View Area Qwad -
	if (debug)
	{
		App->render->DrawQuad({ box.x - App->render->camera.x, box.y - App->render->camera.y, box.w, box.h }, 150, 150, 0);
		App->render->DrawQuad({ ContentWindow.x + box.x - App->render->camera.x,ContentWindow.y + box.y - App->render->camera.y,ContentWindow.w,ContentWindow.h }, 0, 50, 0);
		App->render->DrawQuad({ box.x + ScrollBack.GetBox()->x - App->render->camera.x,box.y + ScrollBack.GetBox()->y - App->render->camera.y,ScrollBack.GetBox()->w, ScrollBack.GetBox()->h }, 90, 20, 0);
		App->render->DrawQuad({ box.x + ScrollItem.GetBox()->x - App->render->camera.x,box.y + ScrollItem.GetBox()->y - App->render->camera.y,ScrollItem.GetBox()->w, ScrollItem.GetBox()->h }, 90, 80, 110);
	}

	//Draw the scroll img ------------------
	ScrollBack.DrawAt(box.x,box.y);
	ScrollItem.DrawAt(box.x, box.y);
	
	//Draw scroll items --------------------
	SDL_Rect view_port = { ContentWindow.x + box.x, ContentWindow.y + box.y, ContentWindow.w,ContentWindow.h };
	SDL_RenderSetViewport(App->render->renderer,&view_port);
	
	p2List_item<UI_Element*>* item = Items.start;
	while (item) {

		item->data->Draw(debug);

		item = item->next;
	}
	SDL_RenderSetViewport(App->render->renderer,NULL);
	
	//Draw Scroll Childs -------------------
	DrawChilds(debug);
}

bool UI_Scroll::CleanUp()
{
	bool ret = true;
	p2List_item<UI_Element*>* item = Items.end;
	p2List_item<UI_Element*>* item_prev = nullptr;

	if (item != nullptr)item_prev = item->prev;
	while (item) {

		//CleanUp the item childs
		ret = item->data->CleanUp();
		//Delete all item data
		Items.del(item);

		item = item_prev;
		if (item_prev != nullptr)item_prev = item_prev->prev;

	}
	return ret;
}

void UI_Scroll::SetScrollableItem(const iPoint position, const SDL_Rect rect, int id)
{
	ScrollItem = UI_Image({ position.x,position.y,rect.w,rect.h }, rect, id);
}

void UI_Scroll::SetScrollableBack(const iPoint position, const SDL_Rect rect, int id)
{
	ScrollBack = UI_Image({ position.x,position.y,rect.w,rect.h }, rect, id);
}

void UI_Scroll::SetContentWindow(const SDL_Rect rect)
{
	ContentWindow = rect;
}

void UI_Scroll::SetScrollMaxValue(int maxvalue)
{
	MaxValue = maxvalue;
}

void UI_Scroll::SetScrollValue(float scrollvalue)
{
	Value = scrollvalue;
}

// Functionality =========================================
iPoint UI_Scroll::CalculateScrollDesp()
{
	float loc = 0, desp_x = 0, desp_y = 0;

	switch (Scroll_Type)
	{
	case VERTICAL:

		loc = -(ScrollItem.GetBox()->y - ScrollBack.GetBox()->y);
		if (loc == 0) break;
		desp_y = (loc * ContentLenght) / (ScrollBack.GetBox()->h - ScrollItem.GetBox()->h);
		break;

	case VERTICAL_INV:

		loc = (ScrollItem.GetBox()->y - ScrollBack.GetBox()->y);
		if (loc == 0) break;
		desp_y = (loc * ContentLenght) / (ScrollBack.GetBox()->h - ScrollItem.GetBox()->h);
		break;

	case LATERAL:

		loc = (ScrollItem.GetBox()->x - ScrollBack.GetBox()->x);
		if (loc == 0) break;
		desp_x = -(loc * ContentLenght) / (ScrollBack.GetBox()->w - ScrollItem.GetBox()->w);
		break;

	case LATERAL_INV:

		loc = (ScrollItem.GetBox()->x - ScrollBack.GetBox()->x);
		if (loc == 0) break;
		desp_x = (loc * ContentLenght) / (ScrollBack.GetBox()->w - ScrollItem.GetBox()->w);
		break;

	case FREE_DIRECTION:
		break;
	}

	desp_x = (desp_x - floor(desp_x) > 0.5) ? ceil(desp_x) : floor(desp_x);
	desp_y = (desp_y - floor(desp_y) > 0.5) ? ceil(desp_y) : floor(desp_y);

	return iPoint(desp_x, desp_y);
}

void UI_Scroll::MoveScrollItems()
{
	//Move the scroll items --
	iPoint desp = CalculateScrollDesp();
	p2List_item<UI_Element*>* item = Items.start;
	p2List_item<iPoint>* item_location = Items_location.start;
	while (item_location)
	{
		item->data->SetBoxPosition(item_location->data.x + desp.x, item_location->data.y + desp.y);
		item = item->next;
		item_location = item_location->next;
	}
}

uint UI_Scroll::UpdateContentLenght(UI_Element * new_item)
{
	int length = 0;
	
	switch (Scroll_Type)
	{
	case VERTICAL:
		length = (new_item->GetBox()->y + new_item->GetBox()->h) - (ContentWindow.h);
		break;

	case VERTICAL_INV:
		length = -new_item->GetBox()->y;
		break;

	case LATERAL:
		length = (new_item->GetBox()->x + new_item->GetBox()->w) - (ContentWindow.w);
		break;
	
	case LATERAL_INV:
		length = -new_item->GetBox()->x;
		break;
	
	case FREE_DIRECTION:
		break;
	}

	if (length > 0 && length > ContentLenght)ContentLenght = length;
	return uint(length);
}

void UI_Scroll::SetScrollType(SCROLL_TYPE type)
{
	Scroll_Type = type;
}

bool UI_Scroll::MoveScroll(int mouse_x_motion, int mouse_y_motion)
{
	//Select the Scroll Item ----------
	if (ScrollItem.MouseIsIn(box.x,box.y) || ScrollSelected)
	{
		ScrollSelected = true;
	}

	//Drag the Scroll Item ------------
	if (ScrollSelected)
	{
		if (Scroll_Type == VERTICAL || Scroll_Type == VERTICAL_INV)
		{
			mouse_x_motion = 0;

			if (ScrollItem.RectIsIn(ScrollBack.GetBox(), mouse_x_motion, mouse_y_motion) == false)
			{
				//Set at limits
				if (mouse_y_motion < 0)
				{
					mouse_y_motion = ScrollBack.GetBox()->y - ScrollItem.GetBox()->y;
				}
				else if (mouse_y_motion > 0)
				{
					mouse_y_motion = ScrollBack.GetBox()->y + ScrollBack.GetBox()->h - (ScrollItem.GetBox()->h + ScrollItem.GetBox()->y);
				}
			}

			//Update scroll position
			ScrollPosition += mouse_y_motion;

			//Update scroll value
			Value = ((ScrollItem.GetBox()->y - ScrollBack.GetBox()->y) * MaxValue) / (float)(ScrollBack.GetBox()->h - ScrollItem.GetBox()->h);
		}
		else if (Scroll_Type == LATERAL || Scroll_Type == LATERAL_INV)
		{
			mouse_y_motion = 0;

			if (ScrollItem.RectIsIn(ScrollBack.GetBox(), mouse_x_motion, mouse_y_motion, true) == false)
			{
				if (mouse_x_motion < 0)
				{
					mouse_x_motion = -(ScrollItem.GetBox()->x - ScrollBack.GetBox()->x);
				}
				else if (mouse_x_motion > 0)
				{
					mouse_x_motion = (ScrollBack.GetBox()->x + ScrollBack.GetBox()->w) - (ScrollItem.GetBox()->x + ScrollItem.GetBox()->w);
				}
			}

			//Update scroll position
			ScrollPosition += mouse_x_motion;

			//Update scroll value
			Value = ((ScrollItem.GetBox()->x - ScrollBack.GetBox()->x) * MaxValue) / (float)(ScrollBack.GetBox()->w - ScrollItem.GetBox()->w);
		}

		//Move the scroll Item ---
		ScrollItem.MoveBox(mouse_x_motion, mouse_y_motion);
		
		//Move the scroll items --
		MoveScrollItems();
	}

	return ScrollSelected;
}

void UI_Scroll::GoBottom()
{
	//Update the scroll pos --
	UpdateContentLenght(Items.end->data);

	//Move the scroll Item ---
	ScrollItem.MoveBox(0, ScrollBack.GetBox()->y + ScrollBack.GetBox()->h - (ScrollItem.GetBox()->h + ScrollItem.GetBox()->y));

	//Move the scroll items --
	CalculateScrollDesp();
	MoveScrollItems();

	//Update scroll value ----
	Value = ((ScrollItem.GetBox()->y - ScrollBack.GetBox()->y) * MaxValue) / (float)(ScrollBack.GetBox()->h - ScrollItem.GetBox()->h);
}

void UI_Scroll::UnselectScroll()
{
	ScrollSelected = false;
}

void UI_Scroll::AddScrollItem(UI_Element* new_item)
{
	//Calculate the size for the scroll
	UpdateContentLenght(new_item);

	//Set item layer
	new_item->SetLayer(this->layer + 1);

	//Add the new item to the list of items
	this->Items.add(new_item);
	this->Items_location.add(iPoint(new_item->GetBox()->x, new_item->GetBox()->y));
}

void UI_Scroll::AddScrollItemAtBottom(UI_Element * new_item)
{
	//Locat item at the bottom of the scroll
	if(Items.end != nullptr)new_item->SetBoxPosition(0, Items_location.end->data.y + Items.end->data->GetBox()->h);
	else new_item->SetBoxPosition(0, 0);
	
	//Update the scroll length
	UpdateContentLenght(new_item);
	
	//Set item layer
	new_item->SetLayer(this->layer + 1);

	//Add the new item to the list of items
	this->Items.add(new_item);
	this->Items_location.add(iPoint(new_item->GetBox()->x, new_item->GetBox()->y));
}

uint UI_Scroll::GetScrollItemsNum() const
{
	return uint(Items.count());
}

UI_Element * UI_Scroll::GetScrollItem(uint index)
{
	return this->Items.At(index)->data;
}

