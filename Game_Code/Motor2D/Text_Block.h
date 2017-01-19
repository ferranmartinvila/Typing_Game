#ifndef _TEXT_BLOCK_
#define _TEXT_BLOCK_

#include "p2List.h"
#include "p2SString.h"
#include "SDL/include/SDL.h"

struct PhysBody;
struct SDL_Texture;
struct _TTF_Font;

class TextBlock
{
public:

	TextBlock(const char* text, _TTF_Font*	font, const SDL_Color& off_color = {255,255,255,255}, const SDL_Color& on_color = { 255,105,255,255 });
	~TextBlock();

private:

	p2SString				text;
	_TTF_Font*				text_font;
	SDL_Color				text_color_off;
	SDL_Color				text_color_on;
	
	uint					char_index = 0;

	PhysBody*				body = nullptr;
	SDL_Texture*			texture = nullptr;

public:

	//Game Loop ------------
	bool	Update();
	void	Draw();
	bool	CleanUp();

	//Functionality ---------
	bool			GenerateTextureFromText();
	bool			GenerateBodyFromTexture();

	PhysBody*		GetBody()const;
	void			GetPosition(int& x, int &y) const;
	float			GetRotation() const;
	SDL_Texture*	GetTexture()const;
	char			GetTextCharTarget()const;

	void			SetText(char* new_text);
	void			SetPosition(int x, int y);

	void			PlusCharIndex();
	void			ResetCharIndex();
};
#endif
