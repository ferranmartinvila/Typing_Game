// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "p2SString.h"
#include "PugiXml\src\pugixml.hpp"

struct j1App;
struct UI_Element;
enum GUI_INPUT;
enum CONSOLE_COMMAND_TYPE;
struct Cvar;
struct Command;
struct PhysBody;

class j1Module
{
public:

	j1Module() : active(false)
	{}

	virtual ~j1Module()
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	//GUI functions -------------------
	virtual void GUI_Input(UI_Element* target, GUI_INPUT input)
	{

	}

	//Console functions ---------------
	virtual void Console_Cvar_Input(Cvar* cvar, Command* command, p2SString* input)
	{

	}
	virtual void Console_Command_Input(Command* command, Cvar* cvar, p2SString* input)
	{

	}

	//Physics functions -------------
	virtual void OnCollision(PhysBody* bodyA, PhysBody* bodyB)
	{

	}

	//Active/Deasctive module
	void Activate()
	{
		active = true;
	}
	void Desactivate()
	{
		active = false;
	}

public:

	p2SString	name;
	bool		active;

};

#endif // __j1MODULE_H__