#ifndef  _COMMAND_
#define _COMMAND_

#include "j1Module.h"

class Command
{
public:

	Command(const char* command_str, j1Module* module_target) :command_str(command_str), module_target(module_target) {}
	~Command() {}

private:

	p2SString	command_str;
	j1Module*	module_target;

public:

	const p2SString* GetCommandStr()const
	{
		return &command_str;
	}

	j1Module* GetModuleTarget()const
	{
		return module_target;
	}

};
#endif // _COMMAND_
