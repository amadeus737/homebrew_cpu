#pragma once

#include "assembler.h"
#include "cpu.h"
#include "command.h"
#include "parser.h"

#include <iostream>
#include <sstream>

class includeDirective : public command
{
public:
	void process(assembler& a, cpu& cpu, const std::string& d, std::string remainder, int line) const override
	{
		auto token = parser::instance().extract_token_str(remainder);
		if (!token.has_value())
		{
			// no data
			std::stringstream msg;
			msg << "Processing include directive ." << d << " at line <" << line << ">! No data!";
			throw std::exception(msg.str().c_str());
		}

		// check for garbage after directive
		if (!remainder.empty())
		{
			std::stringstream msg;
			msg << "Processing include directive ." << d << " at line <" << line << ">! Does not include : [";
			msg << remainder << "]!!";
			throw std::exception(msg.str().c_str());
		}

		std::string tokenString = token.has_value() ? std::string(token.value()) : "";
		parser::instance().trim_ws(tokenString);

		if (!tokenString.empty())
		{
			//if (a.echoMajorTasks())
				std::cout << "          *** Processing include directive for file: " << tokenString << "\n";

			a.pushFileToStack("code\\" + tokenString);
			a.processFile();
		}
		else
		{
			// bad value
			std::stringstream msg;
			msg << "Processing include directive " << d << " at line <" << line << ">! Does not include : [";
			msg << tokenString << "]!!";
			throw std::exception(msg.str().c_str());
		}
	}
};
