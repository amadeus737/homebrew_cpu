#include "cpu.h"
#include "archtag.h"
#include "directive.h"

cpu::cpu()
{
	registerOperations();
}

void cpu::registerOperations()
{
	registerDirective<includeDirective>(INCLUDE_STR);

	registerArchTag<archBitWidth>(INSTRUCTION_WIDTH_STR);
	registerArchTag<archBitWidth>(ADDRESS_WIDTH_STR);
	registerArchTag<archRom>(DECODER_ROM_STR);
	registerArchTag<archRom>(PROGRAM_ROM_STR);
	registerArchTag<archRegister>(REGISTER_STR);
	registerArchTag<archFlagDevice>(FLAG_STR);
	registerArchTag<archFlagDevice>(DEVICE_STR);
	registerArchTag<archControlLine>(CONTROL_STR);
	registerArchTag<archOpcode>(OPCODE_STR);
	registerArchTag<archOpcode>(OPCODE_ALIAS_STR);
	registerArchTag<archOpcodeSeq>(OPCODE_SEQ_STR);
	registerArchTag<archOpcodeSeq>(OPCODE_SEQ_IF_STR);
	registerArchTag<archOpcodeSeq>(OPCODE_SEQ_ELSE_STR);
}

void cpu::processCommand(assembler& a, std::optional<std::string> token, std::string remainder, int lineNum)
{
	if (parser::instance().is_command(token.value()))
	{
		/*	if (_symbols.count(token.value()) > 0)
{
	std::stringstream msg;
	msg << "Found a symbol at line <" << linenum << ">! Symbol [" << token.value() << "] already exists!";
	throw std::exception(msg.str().c_str());
}
else*/
		if (_archtags.count(token.value()) > 0)
		{
			_archtags[token.value()]->process(a, *this, token.value(), remainder, lineNum);
		}
	}
	else if (parser::instance().is_directive(token.value()))
	{
		// strip off the directive symbol
		token.value().erase(token.value().begin(), token.value().begin() + 1);

		// only handle registered directives
		if (_directives.count(token.value()) == 0)
		{
			std::stringstream msg;
			msg << "Unknown directive at line <" << lineNum << ">! Found [."
				<< token.value() << "]";
			throw std::exception(msg.str().c_str());
		}
		_directives[token.value()]->process(a, *this, token.value(), std::move(remainder), lineNum);
	}
}

void cpu::setAddress(int a)
{
	_address = a;

	if (_address > _max_address)
		_max_address = _address;
}

void cpu::addDecoderRom(bool write, int inputs, int outputs)
{
	_write_decode_rom = write;
	_in_bits_decode = inputs;
	_out_bits_decode = outputs;
}

void cpu::addProgramRom(bool write, int inputs, int outputs)
{
	int size = outputs * pow(2, inputs);

	_write_program_rom = write;
	_in_bits_program = inputs;
	_out_bits_program = outputs;
}

SymbolType cpu::getSymbolType(const std::string& n)
{
	std::map<std::string, symbol>::iterator i = _symbols.find(n);

	if (i != _symbols.end())
		return (i->second).getType();

	return SymbolType::None;
}

int cpu::getSymbolAddress(const std::string& n) const
{
	auto i = _symbols.find(n);
	return (i->second).getAddress();
}

const std::vector<int>& cpu::getSymbolAddresses(SymbolType t)
{
	switch (t)
	{
	case SymbolType::Constant:
		return _constantAddresses;

	case SymbolType::Variable:
		return _variableAddresses;

	case SymbolType::Label:
		return _labelAddresses;

	case SymbolType::Register:
		return _registerAddresses;

	case SymbolType::Flag:
		return _flagAddresses;

	case SymbolType::ControlLine:
		return _controlLineAddresses;
	}
}

void cpu::addConstant(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeConstant(n, a, l));
	_constantAddresses.push_back(a);
}

void cpu::addVariable(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeVariable(n, a, l));
	_variableAddresses.push_back(a);
}

void cpu::addLabel(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeLabel(n, a, l));
	_labelAddresses.push_back(a);
}

void cpu::addRegister(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeRegister(n, a, l));
	_registerAddresses.push_back(a);
}

void cpu::addFlag(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeFlag(n, a, l));
	_nFlags++;

	_flagAddresses.push_back(a);
}

void cpu::addControlLine(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeControlLine(n, a, l));

	_controlLineAddresses.push_back(a);

	if (a > _maxControlLineValue) _maxControlLineValue = a;
}
 
void cpu::addOpcode(int v, const opcode& oc)
{
	_lastOpcodeIndex = v;
	_opcodes.emplace(v, oc);

	if (v > _maxOpcodeValue) _maxOpcodeValue = v;

	_mnemonics.push_back(_opcodes[v].mnemonic());
	registerInstruction<archOpcode>(_opcodes[v].getUniqueString());
}

void cpu::addOpcodeAlias(int v, const opcode& oca)
{
	_opcode_aliases.emplace(v, oca);

	_mnemonics.push_back(_opcode_aliases[v].mnemonic());
	registerInstruction<archOpcode>(_opcode_aliases[v].getUniqueString());
}

void cpu::addNewControlPatternToCurrentOpcode(controlPattern cp)
{
	_opcodes[_lastOpcodeIndex].addNewControlPattern(cp);
	if (_opcodes[_lastOpcodeIndex].numCycles() > _maxNumCycles) _maxNumCycles = _opcodes[_lastOpcodeIndex].numCycles();
}

void cpu::addToLastControlPatternInCurrentOpcode(controlPattern cp)
{
	_opcodes[_lastOpcodeIndex].addToLastControlPattern(cp);
}

bool cpu::isAMnemonic(const std::string& s)
{
	return std::find(_mnemonics.begin(), _mnemonics.end(), s) != _mnemonics.end();
}

opcode& cpu::getOpcode(int v)
{
	return _opcodes[v];
}

int cpu::getValueByUniqueOpcodeString(const std::string& m)
{
	for (auto it = _opcodes.begin(); it != _opcodes.end(); ++it)
		if (it->second.getUniqueString() == m)
			return it->first;
}

int cpu::getValueByUniqueOpcodeAliasString(const std::string& m)
{
	for (auto it = _opcode_aliases.begin(); it != _opcode_aliases.end(); ++it)
		if (it->second.getUniqueString() == m)
			return it->first;
}

int cpu::numOpcodeCycles()
{
	return _opcodes[_lastOpcodeIndex].numArgs();
}

int cpu::lastOpcodeIndex()
{
	return _lastOpcodeIndex;
}