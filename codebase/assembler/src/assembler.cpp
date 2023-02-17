#include "assembler.h"
#include "config.h"
#include "parser.h"
#include "directive.h"
#include "archtag.h"

#include <iostream>
#include <sstream>

assembler::assembler(const std::string& filename, cpu &c)
	:
	_cpu(c)
{
	// set file exceptions, save the start file, add to file stack
	_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	_startFile = filename;
}

void assembler::assemble()
{
	assembly_pass0();
}

void assembler::assembly_pass0()
{
	pushFileToStack(_startFile);

	_cmds.clear();
	_cmds.insert(_cmds.end(), { ".include", INSTRUCTION_WIDTH_STR, ADDRESS_WIDTH_STR,
		                        DECODER_ROM_STR, PROGRAM_ROM_STR, REGISTER_STR,
								FLAG_STR, DEVICE_STR, CONTROL_STR, OPCODE_STR, OPCODE_ALIAS_STR,
								OPCODE_SEQ_STR, OPCODE_SEQ_IF_STR, OPCODE_SEQ_ELSE_STR });

	processFile();
}

void assembler::pushFileToStack(const std::string& filename)
{
	_filestack.push(filename);
}

void assembler::processFile()
{
	if (_file.is_open()) _file.close();
	std::string currname = _filestack.currName();
	_file.open(currname);

	if (_echo_major_tasks)
		std::cout << "\n-- processing file: " << currname << "\n";

	std::string line;
	_lineNumber = 0;

	try
	{
		while (std::getline(_file, line))
		{
			int startLine = _filestack.getLine();
			if (startLine < _lineNumber && _filestack.processingNewFile())
				_lineNumber = startLine;

			if (startLine != 0 && _lineNumber < startLine)
			{
				_lineNumber++;
				continue;
			}

			if (_echo_source)
				std::cout << "     ==> source line #" << _lineNumber << " = " << line << "\n";
			
			// remove any comments and extract token
			parser::instance().strip_comment(line);
			auto token = parser::instance().extract_token_ws(line);
			std::string tokenString = "";

			if (token.has_value())
				tokenString = std::string(token.value());
			
			for (std::string command : _cmds)
			{
				if (tokenString == command)
					_cpu.processCommand(*this, token, line, _lineNumber);
			}

			_lineNumber++;
		}
	}
	catch (const std::ifstream::failure& e)
	{
		if (!_file.eof())
		{
			throw e;
		}
		else
		{
			if (_file.is_open()) _file.close();

			if (!_filestack.hasNullParent())
			{
				_filestack.makeParentActive();
				processFile();
			}
		}
	}
}

void assembler::setEcho(unsigned char e)
{
	_echo_architecture = (e & 0x80) == 0x80; // $1000 0000
	_echo_major_tasks = (e & 0x40) == 0x40;  // $0100 0000
	_echo_minor_tasks = (e & 0x20) == 0x20;  // $0010 0000
	_echo_warnings = (e & 0x10) == 0x10;     // $0001 0000
	_echo_parsed_major = (e & 0x08) == 0x08; // $0000 1000
	_echo_parsed_minor = (e & 0x04) == 0x04; // $0000 0100
	_echo_source = (e & 0x02) == 0x02;       // $0000 0010
	_echo_rom_data = (e & 0x01) == 0x01;     // $0000 0001
}