#include "Text_Block.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Physics.h"
#include "j1Fonts.h"
#include "j1Render.h"

// Constructors ---------------------------------
TextBlock::TextBlock(const char * text, _TTF_Font* font, const SDL_Color& color) :text(text),text_font(font),text_color(color)
{
	if (!GenerateTextureFromText())LOG("Error Generating TextBlock Textures");
	if(!GenerateBodyFromTexture())LOG("Error Generating TextBlock Body");
}

//Destructors -----------------------------------
TextBlock::~TextBlock()
{
}

//Game Loop -------------------------------------
bool TextBlock::Update()
{
	return true;
}

void TextBlock::Draw()
{
	int x, y;
	body->GetPosition(x, y);
	App->render->Blit(texture, x, y,NULL,1.0f, body->GetRotation());
}

bool TextBlock::CleanUp()
{
	return true;
}

// Functionality --------------------------------
bool TextBlock::GenerateTextureFromText()
{
	if (text.Length() == 0 || text_font == nullptr)
	{
		LOG("Invalid TextBlock text");
		return false;
	}

	texture = App->font->Print(text.GetString(), text_color, text_font);

	return true;
}

bool TextBlock::GenerateBodyFromTexture()
{
	if (texture == nullptr)
	{
		LOG("Invalid TextBlock texture");
		return false;
	}
	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	body = App->physics->CreateRectangle(350, 0, w, h, TEXT_BLOCK);

	return true;
}

PhysBody * TextBlock::GetBody() const
{
	return body;
}

void TextBlock::GetPosition(int & x, int & y) const
{
	body->GetPosition(x,y);
}

float TextBlock::GetRotation() const
{
	return body->GetRotation();
}

SDL_Texture * TextBlock::GetTexture() const
{
	return texture;
}

void TextBlock::SetText(char * new_text)
{
	if (strlen(new_text) == 0)return;

	text.Clear();
	text.create(new_text);
}

void TextBlock::SetPosition(int x, int y)
{
	body->SetPosition(x, y);
}
