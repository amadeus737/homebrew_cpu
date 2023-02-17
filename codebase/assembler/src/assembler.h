#pragma once

#include "cpu.h"
#include "filestack.h"
#include "command.h"

#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <assert.h>

class assembler
{
public:
	assembler(const std::string& startFile, cpu& c);
	
	void assemble();
	void assembly_pass0();

	void pushFileToStack(const std::string& filename);
	void processFile();

	// Echo stuff
	void setEcho(unsigned char e);
	bool echoArchitecture() { return _echo_architecture; }
	bool echoMajorTasks() { return _echo_major_tasks; }
	bool echoMinorTasks() { return _echo_minor_tasks; }
	bool echoWarnings() { return _echo_warnings; }
	bool echoSource() { return _echo_source; }
	bool echoParsedMajor() { return _echo_parsed_major; }
	bool echoParsedMinor() { return _echo_parsed_minor; }
	bool echoRomData() { return _echo_rom_data; }

private:
	cpu& _cpu;

	std::ifstream _file;
	filestack _filestack;
	std::string _startFile;
	int _lineNumber = -1;

	std::vector<std::string> _cmds;

	// echo stuff
	bool _echo_architecture = false;
	bool _echo_major_tasks = false;
	bool _echo_minor_tasks = false;
	bool _echo_warnings = false;
	bool _echo_source = false;
	bool _echo_parsed_major = false;
	bool _echo_parsed_minor = false;
	bool _echo_rom_data = false;
};