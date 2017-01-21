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

	TextBlock(const char* text, _TTF_Font*	font, const SDL_Color& off_color, const SDL_Color& on_color, uint x_margin = 0, uint y_margin = 0);
	~TextBlock();

private:

	p2SString				text;
	_TTF_Font*				text_font;
	SDL_Color				text_color_off;
	SDL_Color				text_color_on;
	
	uint					char_index = 0;
	uint					score = 0;
	PhysBody*				body = nullptr;
	SDL_Texture*			texture = nullptr;
	int						x_margin = 0;
	int						y_margin = 0;

	uint					born_time = 0;

public:

	//Game Loop ------------
	bool	Update();
	void	Draw();
	bool	CleanUp();

	//Functionality ---------
	bool			GenerateTextureFromText();
	bool			GenerateBodyFromTexture();
	void			CalculateBlockScore();

	PhysBody*		GetBody()const;
	void			GetPosition(int& x, int &y) const;
	float			GetRotation() const;
	SDL_Texture*	GetTexture()const;
	char			GetTextCharTarget()const;
	uint			GetScore()const;
	int				GetBornTime()const;
	void			SetText(char* new_text);
	void			SetPosition(int x, int y);
	void			SetBornTime(uint time);

	void			PlusCharIndex();
	void			ResetCharIndex();

};
#endif
