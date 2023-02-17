#pragma once

#include "symbol.h"
#include "opcode.h"
#include "assembler.h"
#include "command.h"

#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <memory>
#include <optional>

class cpu
{
public:
	// constructor
	cpu();

	// bitwidth stuff
	void setInstructionWidth(int i) { _instructionWidth = i; }
	void setAddressWidth(int a) { _addressWidth = a; }
	int getInstructionWidth() { return _instructionWidth; }
	int getAddressWidth() { return _addressWidth; }

	void registerOperations(); 
	void processCommand(assembler& a, std::optional<std::string> token, std::string remainder, int lineNum);

	// flag stuff
	int getFlagCount() { return _nFlags; }
	std::vector<int> lastAddedFlags;

	// symbol stuff
	SymbolType getSymbolType(const std::string& n);
	int getSymbolAddress(const std::string& n) const;
	const std::vector<int>& getSymbolAddresses(SymbolType t);
	void addLabel(const std::string& n, int a, int l);
	void addConstant(const std::string& n, int a, int l);
	void addVariable(const std::string& n, int a, int l);
	void addFlag(const std::string& n, int a, int l);
	void addRegister(const std::string& n, int a, int l);
	void addControlLine(const std::string& n, int a, int l);
	void addOpcode(int v, const opcode& oc);
	void addOpcodeAlias(int v, const opcode& oca);
	void addNewControlPatternToCurrentOpcode(controlPattern cp);
	void addToLastControlPatternInCurrentOpcode(controlPattern cp);

	// opcode stuff
	bool isAMnemonic(const std::string& s);
	int getValueByUniqueOpcodeString(const std::string& s);
	int getValueByUniqueOpcodeAliasString(const std::string& s);
	int numOpcodeCycles();
	int lastOpcodeIndex();
	opcode& getOpcode(int v);

	// addressing stuff
	void setAddress(int a);
	int getAddress() const { return _address; }
	
	// Decoder Rom stuff
	void addDecoderRom(bool write, int inputs, int outputs);
	void writeDecoderRom();

	// ProgramRom stuff
	void setActiveSegment(int i) { _activeSegmentIndex = i; }
	void addProgramRom(bool write, int inputs, int outputs);
	void addByteToProgramRom(int8_t byte, int address = -1);
	void writeProgramRom();

private:
private:
	template <class d>
	void registerDirective(std::string name)
	{
		assert(_directives.count(name) == 0);
		_directives.emplace(name, std::make_unique<d>());
	}

	template <class a>
	void registerArchTag(std::string name)
	{
		assert(_archtags.count(name) == 0);
		_archtags.emplace(name, std::make_unique<a>());
	}

	template <class i>
	void registerInstruction(std::string name)
	{
		assert(_instructions.count(name) == 0);
		_instructions.emplace(name, std::make_unique<i>());
	}

private:
	// bitwidth stuff
	int _instructionWidth = 0;
	int _addressWidth = 0;

	// flag stuff
	int _nFlags = 0;

	// symbol stuff
	std::map<std::string, symbol> _symbols;
	std::vector<int> _constantAddresses;
	std::vector<int> _variableAddresses;
	std::vector<int> _labelAddresses;
	std::vector<int> _registerAddresses;
	std::vector<int> _flagAddresses;
	std::vector<int> _controlLineAddresses;

	// opcode stuff
	std::map<int, opcode> _opcodes;
	std::map<int, opcode> _opcode_aliases;
	std::vector<std::string> _mnemonics;
	int _lastOpcodeIndex = -1;

	// addressing stuff
	int _address = 0;
	int _last_address = -1;
	int _max_address = 0;

	// token identifier stuff
	std::map<std::string, std::unique_ptr<command>>  _directives;
	std::map<std::string, std::unique_ptr<command>>  _archtags;
	std::map<std::string, std::unique_ptr<command>>  _instructions;

	// decode rom stuff
	bool _write_decode_rom = false;
	int _maxControlLineValue = -1;
	int _maxOpcodeValue = -1;
	int _maxNumCycles = -1;
	int _in_bits_decode;
	int _out_bits_decode;

	// program rom stuff
	int _activeSegmentIndex;
	bool _write_program_rom = false;
	int _in_bits_program;
	int _out_bits_program;
};