#pragma once

#include "config.h"
#include "util.h"
#include "cpu.h"

#include <string>

class command
{
public:
	virtual ~command() {};
	virtual void process(class assembler& a, class cpu& c, const std::string& d, std::string remainder, int line) const {}
	virtual void process(class assembler& a, class cpu& c, std::string& line, int value0, int value1, int startAddress) const {}
};

class commandAlias : public command
{
public:
	commandAlias(const command* c)
		:
		_command(c)
	{}

	virtual void process(class assembler& a, class cpu& c, const std::string& d, std::string r, int l) const override
	{
		_command->process(a, c, d, r, l);
	}

	virtual void process(class assembler& a, class cpu& c, std::string& il, int iv0, int iv1, int sa) const override
	{
		_command->process(a, c, il, iv0, iv1, sa);
	}

private:
	const command* const _command;
};
