#include "Text_Block.h"

#include "p2Log.h"
#include "j1App.h"
#include "BlocksManager.h"
#include "j1Physics.h"
#include "j1Fonts.h"
#include "j1Render.h"

// Constructors ---------------------------------
TextBlock::TextBlock(const char * text, _TTF_Font* font, const SDL_Color& off_color, const SDL_Color& on_color, uint x_margin, uint y_margin) :text(text),text_font(font),text_color_off(off_color), text_color_on(on_color),x_margin(x_margin),y_margin(y_margin)
{
	if (!GenerateTextureFromText())LOG("Error Generating TextBlock Textures");
	if (!GenerateBodyFromTexture())LOG("Error Generating TextBlock Body");
	CalculateBlockScore();
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
	delete body;
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
	int total_w = 0;
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
	texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, total_w + (x_margin * 2), h + (y_margin * 2));
	SDL_SetRenderTarget(App->render->renderer, texture);

	//Texture blit
	App->render->Blit(App->blocks_manager->GetDefaultBlockTexture(), 0, 0);

	//Text blit
	if(on_texture != nullptr)App->render->Blit(on_texture, x_margin, y_margin);
	if(off_texture != nullptr)App->render->Blit(off_texture, total_w - w + x_margin, y_margin);

	//Mark blit
	SDL_Color color;
	if (App->blocks_manager->GetBlockTarget() == this)color = App->blocks_manager->GetTargetColor();
	else color = App->blocks_manager->GetNonTargetColor();
	App->render->DrawQuad({ 0, 0, x_margin, h +  x_margin * 2}, color.r, color.g, color.b, color.a);
	App->render->DrawQuad({ total_w + x_margin, 0, x_margin, h + x_margin * 2 }, color.r, color.g, color.b, color.a);
	App->render->DrawQuad({ 0, 0, total_w + x_margin * 2, y_margin}, color.r, color.g, color.b, color.a);
	App->render->DrawQuad({ 0, h + y_margin, total_w + x_margin * 2, y_margin }, color.r, color.g, color.b, color.a);

	//Set blend mode
	SDL_SetTextureBlendMode(texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	//Set viewport to screen
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
	body = App->physics->CreateRectangle(350, -20, w, h, TEXT_BLOCK);

	return true;
}

void TextBlock::CalculateBlockScore()
{
	score = text.Length();
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

uint TextBlock::GetScore() const
{
	return score;
}

int TextBlock::GetBornTime() const
{
	return born_time;
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

void TextBlock::SetBornTime(uint time)
{
	born_time = time;
}

void TextBlock::PlusCharIndex()
{
	char_index++;
	if (char_index == text.Length())
	{
		App->blocks_manager->DeleteTarget();
		return;
	}
	GenerateTextureFromText();
}

void TextBlock::ResetCharIndex()
{
	char_index = 0;
	GenerateTextureFromText();
}
