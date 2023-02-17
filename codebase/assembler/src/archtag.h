#pragma once

#include "assembler.h"
#include "cpu.h"
#include "command.h"
#include "parser.h"

#include <iostream>
#include <sstream>

enum Operation { None, OR };

class archBitWidth : public command
{
public:
	void process(assembler& assembler, cpu& cpu, const std::string& label, std::string remainder, int line) const override
	{
		auto sizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
		{
			if (label == INSTRUCTION_WIDTH_STR)
				std::cout << "          *** Instruction Width set to " << std::string(sizeToken.value()) << "\n\n";

			if (label == ADDRESS_WIDTH_STR)
				std::cout << "          *** Address Width set to " << std::string(sizeToken.value()) << "\n\n";
		}

		if (label == INSTRUCTION_WIDTH_STR)
			cpu.setInstructionWidth(stoi(std::string(sizeToken.value())));

		if (label == ADDRESS_WIDTH_STR)
			cpu.setAddressWidth(stoi(std::string(sizeToken.value())));
	}
};

class archRom : public command
{
public:
	void process(assembler& assembler, cpu& cpu, const std::string& label, std::string remainder, int line) const override
	{
		auto writeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!writeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid write token!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(writeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << writeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		auto inSizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!inSizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid input size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(inSizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << inSizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		auto outSizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!outSizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid output size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(outSizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << outSizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		bool write = std::stoi(std::string(writeToken.value())) == 1;

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
		{
			if (label == DECODER_ROM_STR)
				std::cout << "          *** Decoder Rom with " << std::string(inSizeToken.value()) << " inputs and " << std::string(outSizeToken.value()) << " outputs (";

			if (label == PROGRAM_ROM_STR)
				std::cout << "          *** Program Rom with " << std::string(inSizeToken.value()) << " inputs and " << std::string(outSizeToken.value()) << " outputs (";

			if (write)
				std::cout << "write)\n";
			else
				std::cout << "non-write)\n";

			std::cout << "\n";
		}

		if (label == DECODER_ROM_STR)
			cpu.addDecoderRom(write, stoi(inSizeToken.value()), stoi(outSizeToken.value()));

		if (label == PROGRAM_ROM_STR)
			cpu.addProgramRom(write, stoi(inSizeToken.value()), stoi(outSizeToken.value()));
	}
};

class archRegister : public command
{
public:
	void process(assembler& assembler, cpu& cpu, const std::string& label, std::string remainder, int line) const override
	{
		auto sizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nameToken = parser::instance().extract_token_ws_comma(remainder);
			if (nameToken.has_value())
			{
				std::string nameTokenString = std::string(nameToken.value());

				if (assembler.echoParsedMajor() && assembler.echoArchitecture())
					std::cout << "          *** Adding " << std::string(sizeToken.value()) << "-bit Register [" << nameTokenString << "]\n";

				cpu.addRegister(nameTokenString, stoi(std::string(sizeToken.value())), line);
			}
			else
			{
				tokensRemain = false;
			}
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
			std::cout << "\n";
	}
};

class archFlagDevice : public command
{
public:
	void process(assembler& assembler, cpu& cpu, const std::string& label, std::string remainder, int line) const override
	{
		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nameToken = parser::instance().extract_token_ws_comma(remainder);
			if (nameToken.has_value())
			{
				std::string nameTokenString = std::string(nameToken.value());

				if (assembler.echoParsedMajor() && assembler.echoArchitecture())
				{
					if (label == FLAG_STR)
						std::cout << "          *** Adding flag [" << nameTokenString << "]\n";

					if (label == DEVICE_STR)
						std::cout << "          *** Adding device [" << nameTokenString << "]\n";
				}

				if (label == FLAG_STR)
					cpu.addFlag(nameTokenString, cpu.getFlagCount() + 1, line);
			}
			else
			{
				tokensRemain = false;
			}
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
			std::cout << "\n";
	}
};

class archControlLine : public command
{
public:
	void process(assembler& assembler, cpu& cpu, const std::string& label, std::string remainder, int line) const override
	{
		auto nameToken = parser::instance().extract_token_ws(remainder);
		if (!nameToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! No label provided for control line!";
			throw std::exception(msg.str().c_str());
		}

		if (std::string(nameToken.value()) == "fetch")
			std::cout << "";

		bool tokensRemain = true;
		int firstNum = -1;
		int op = 0;
		int secondNum = -1;
		while (tokensRemain)
		{
			// Get the rest of 
			auto nextToken = parser::instance().extract_token_ws_comma(remainder);
			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());
				LiteralNumType type = parser::instance().get_num_type(tokenString);

				if (type != LiteralNumType::None)
				{
					int num = parser::instance().parse_literal_num(tokenString, type);
					if (num != -1)
					{
						if (op == 0) firstNum = num;
						else         secondNum = num;
					}
					else
					{
						std::stringstream msg;
						msg << "Assembling command " << label << " at line <" << line << ">! Bad int literal parse type!";
						throw std::exception(msg.str().c_str());
					}
				}
				else
				{
					if (tokenString[0] == '<' && tokenString[1] == '<') op = -1;
					else if (tokenString[0] == '>' && tokenString[1] == '>') op = 1;
					else
					{
						if (!isdigit(tokenString.c_str()[0]))
						{
							if (tokenString[0] == '|')
							{
								op = Operation::OR;
							}
							else if (tokenString[0] == '=')
							{
								continue;
							}
							else
							{
								if (firstNum == -1)
									firstNum = 0;

								if (op == Operation::OR)
								{
									if (tokenString[0] == '_')
										firstNum = firstNum ^ cpu.getSymbolAddress(tokenString);
									else
										firstNum = firstNum | cpu.getSymbolAddress(tokenString);

									op = Operation::None;
								}
								else
								{
									firstNum = cpu.getSymbolAddress(tokenString);
								}
							}
						}
						else
						{
							std::stringstream msg;
							msg << "Assembling command " << label << " at line <" << line << ">! Expected a symbol reference -- found !" << tokenString;
							throw std::exception(msg.str().c_str());
						}
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		int finalNum = -1;
		if (op != 0)
		{
			if (op == -1) finalNum = firstNum << secondNum;
			if (op == 1)  finalNum = firstNum >> secondNum;
		}
		else
		{
			finalNum = firstNum;
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
		{
			std::cout << "          *** Saving control line = ";
			std::cout << " = $" << hex8 << finalNum;

			if (assembler.echoParsedMinor())
				std::cout << " = %" << std::bitset<sizeof(int) * 8>(finalNum);

			std::cout << "\n\n";
		}

		cpu.addControlLine(std::string(nameToken.value()), finalNum, line);
	}
};

class archOpcode : public command
{
public:
	virtual void process(assembler& assembler, cpu& cpu, const std::string& label, std::string remainder, int line) const override
	{
		opcode opcode;

		auto valueToken = parser::instance().extract_token_ws_comma(remainder);
		if (!valueToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Opcode is not assigned a valid value!";
			throw std::exception(msg.str().c_str());
		}

		std::string valueString = std::string(valueToken.value());
		int parsedValue = parser::instance().parse_literal_num(valueString);

		opcode.setValue(parsedValue);

		auto nameToken = parser::instance().extract_token_ws_comma(remainder);
		if (!nameToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Opcode is not assigned a valid label!";
			throw std::exception(msg.str().c_str());
		}

		opcode.setMnemonic(std::string(nameToken.value()));

		bool tokensRemain = true;
		Operation op = Operation::None;
		int seq = 0;
		int num = -1;
		while (tokensRemain)
		{
			auto nextToken = parser::instance().extract_token_ws_comma(remainder);

			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());

				bool isAddress = parser::instance().try_strip_indirect(tokenString);

				if (tokenString[0] == '|' && label != OPCODE_ALIAS_STR)
				{
					op = Operation::OR;
				}
				else if (tokenString[0] == '=' && label != OPCODE_ALIAS_STR)
				{
					continue;
				}
				else if (tokenString[0] == '#')
				{
					opcode::arg newArg;
					if (isAddress)
					{
						newArg._type = ArgType::DerefNum;
						newArg._string = "[#]";
					}
					else
					{
						newArg._type = ArgType::Numeral;
						newArg._string = "#";
					}

					opcode.addArgument(newArg);

					if (assembler.echoParsedMinor() && assembler.echoArchitecture())
					{
						if (!isAddress)
							std::cout << "					*** Adding an immediate value argument = " << newArg._string << "\n";
						else
							std::cout << "					*** Adding a dereferenced value argument = " << newArg._string << "\n";
					}
				}
				else if (cpu.getSymbolType(tokenString) == SymbolType::Register)
				{
					opcode::arg newArg;
					if (isAddress)
					{
						newArg._type = ArgType::DerefReg;
						newArg._string = "[" + tokenString + "]";
					}
					else
					{
						newArg._type = ArgType::Register;
						newArg._string = tokenString;
					}

					opcode.addArgument(newArg);

					if (assembler.echoParsedMinor() && assembler.echoArchitecture())
					{
						if (!isAddress)
							std::cout << "					*** Adding a register value argument = " << newArg._string << "\n";
						else
							std::cout << "					*** Adding a dereferenced register value argument = " << newArg._string << "\n";
					}
				}
				else if (label != OPCODE_ALIAS_STR)
				{
					if (num == -1)
						num = 0;

					if (op == Operation::OR)
					{
						if (tokenString[0] == '_')
							num = num ^ cpu.getSymbolAddress(tokenString);
						else
							num = num | cpu.getSymbolAddress(tokenString);

						op = Operation::None;
					}
					else
					{
						num = cpu.getSymbolAddress(tokenString);
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		cpu.addOpcode(parsedValue, opcode);

		if (num != -1 && label != OPCODE_ALIAS_STR)
		{
			controlPattern cp;
			cp.pattern = num;

			for (int i = 0; i < pow(2, cpu.getFlagCount()); i++)
			{
				cp.flags.push_back(i);
				cp.type = PatternType::Seq;
			}

			opcode.addNewControlPattern(cp);
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
		{
			std::cout << "          *** Saving opcode " << std::string(nameToken.value()) << " ";

			for (int i = 0; i < opcode.numArgs(); i++)
			{
				std::cout << opcode.getArg(i)._string;

				if (i != opcode.numArgs() - 1)
					std::cout << ", ";
			}

			if (label != OPCODE_ALIAS_STR)
				std::cout << " -- val = $";
			else
				std::cout << " -- to existing opcode with val = $";

			std::cout << hex2 << opcode.value();

			if (label != OPCODE_ALIAS_STR)
			{
				std::cout << ", control sequence : ";
				for (int i = 0; i < opcode.numCycles(); i++)
				{
					for (int j = 0; j < opcode.getPatterns(i).cpattern[0].flags.size(); i++)
					{
						controlPattern p = opcode.getPatterns(i).cpattern[0];
						std::cout << "              " << dec << i << ": $" << hex8 << p.pattern << " and flag pattern = " << p.flags[j] << "\n";
					}
				}
			}

			std::cout << ", unique_str = " << opcode.getUniqueString() << "\n";
		}
	}
};

class archOpcodeSeq : public command
{
public:
	void process(assembler& assembler, cpu& cpu, const std::string& label, std::string remainder, int line) const override
	{
		bool tokensRemain = true;
		bool colonFound = false;
		Operation op = Operation::None;

		cpu.lastAddedFlags.clear();

		controlPattern cp;
		int num = 0;
		while (tokensRemain)
		{
			auto nextToken = parser::instance().extract_token_ws_comma(remainder);

			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());

				if (tokenString[0] == ':')
				{
					colonFound = true;
				}
				else if (tokenString[0] == '|')
				{
					op = Operation::OR;
				}
				else if (tokenString[0] == '=')
				{
					continue;
				}
				else
				{
					if (!colonFound && label == OPCODE_SEQ_IF_STR)
					{
						int nFlags = cpu.getFlagCount();
						for (int i = 0; i < pow(2, nFlags); i++)
						{
							std::string currFlag = std::bitset<5>(i).to_string();

							bool patternMatch = true;
							for (int j = 0; j < nFlags; j++)
							{
								bool digit_j_matches = tokenString[j] == 'x' || tokenString[j] == currFlag[j];
								patternMatch = patternMatch && digit_j_matches;
							}

							if (patternMatch)
							{
								cp.flags.push_back(i);
								cpu.lastAddedFlags.push_back(i);
							}
						}
					}
					else if (op == Operation::OR)
					{
						if (tokenString[0] == '_')
							num = num ^ cpu.getSymbolAddress(tokenString);
						else
							num = num | cpu.getSymbolAddress(tokenString);

						op = Operation::None;
					}
					else
					{
						num = cpu.getSymbolAddress(tokenString);
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		cp.pattern = num;

		if (label == OPCODE_SEQ_STR) cp.type = PatternType::Seq;
		if (label == OPCODE_SEQ_IF_STR) cp.type = PatternType::Seq_If;
		if (label == OPCODE_SEQ_ELSE_STR) cp.type = PatternType::Seq_Else;

		if (label != OPCODE_SEQ_IF_STR)
		{
			for (int i = 0; i < pow(2, cpu.getFlagCount()); i++)
			{
				cp.flags.push_back(i);
			}
		}

		cpu.addNewControlPatternToCurrentOpcode(cp);

		opcode opcode = cpu.getOpcode(cpu.lastOpcodeIndex());

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
		{
			for (int i = 0; i < cp.flags.size(); i++)
				std::cout << "              *** new cycle added = $" << hex8 << num << " with flag pattern = " << cp.flags[i] << "\n";
		}
	}
};