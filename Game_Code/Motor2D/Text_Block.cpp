#include "Text_Block.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Physics.h"
#include "j1Fonts.h"
#include "j1Render.h"

// Constructors ---------------------------------
TextBlock::TextBlock(const char * text, _TTF_Font* font, const SDL_Color& off_color, const SDL_Color& on_color) :text(text),text_font(font),text_color_off(off_color), text_color_on(on_color)
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

	//Data to place textures
	int w = 0, h = 0;
	uint total_w = 0;
	SDL_Texture* on_texture = nullptr;
	SDL_Texture* off_texture = nullptr;
	//Generate completed textblock texture
	if (char_index > 0)
	{
		on_texture = App->font->Print(text.StringSegment(0, char_index), text_color_on, text_font);
		if (on_texture != NULL)SDL_QueryTexture(on_texture, NULL, NULL, &w, &h);
		total_w += w;
	}
	//Generate incompleted textblock texture
	if (char_index < text.Length())
	{
		off_texture = App->font->Print(text.StringSegment(char_index, strlen(text.GetString())), text_color_off, text_font);
		SDL_QueryTexture(off_texture, NULL, NULL, &w, &h);
		total_w += w;
	}

	//Generate a texture with the two parts
	texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, total_w, h);
	SDL_SetRenderTarget(App->render->renderer, texture);
	if(on_texture != nullptr)App->render->Blit(on_texture, 0, 0);
	if(off_texture != nullptr)App->render->Blit(off_texture, total_w - w, 0);
	SDL_SetTextureBlendMode(texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(App->render->renderer, NULL);

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

char TextBlock::GetTextCharTarget() const
{
	return text.GetString()[char_index];
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

void TextBlock::PlusCharIndex()
{
	char_index++;
	if (char_index > text.Length())return;

	GenerateTextureFromText();
}

void TextBlock::ResetCharIndex()
{
	char_index = 0;
	GenerateTextureFromText();
}
