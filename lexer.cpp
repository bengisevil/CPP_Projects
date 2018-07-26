#include "lexer.hpp"


//Checks if a number is even. 
//Helper function to keep track of matching
//Quotations and parathesizes.
bool isEven(int number)
{
	if ((number % 2) == 0)
		return true;
	else
		return false;
}
TokenList tokenize(std::istream & ins)
{

	TokenList tokens;

	//Keep track of matchin paren and quotes
	int paren = 0;
	int quotes = 0;
	int line = 1; //line number

	bool isMeaningful = false;
	bool errorFound = false;
	char c;
	std::string temp;
	while (ins.get(c)) {

		switch (c) {
		case ' ':
			if (isEven(quotes)) {
				if (!temp.empty()) {
					isMeaningful = true;
					tokens.emplace_back(STRING, line, temp);
					temp.clear();
				}
			}
			else {
				temp.push_back(c);
			}
			break;
		case ',':
			if (isEven(quotes)) {
				if (!temp.empty()) {
					isMeaningful = true;
					tokens.emplace_back(STRING, line, temp);
					temp.clear();
				}
				isMeaningful = true;
				tokens.emplace_back(SEP, line);
			}
			else {
				temp.push_back(c);
			}
			break;
		case '\n':
			if (paren == 0 && isEven(quotes)) {
				if (!temp.empty()) {
					tokens.emplace_back(STRING, line, temp);
					temp.clear();
				}
				if (tokens.size() != 0)
				{
					if (isMeaningful)
					{
						tokens.emplace_back(EOL, line);
					}
					isMeaningful = false;
				}
				line++;
			}
			else
			{
				errorFound = true;
			}
			break;
		case '(':
			if (isEven(quotes)) {
				paren++;
				if (!temp.empty())
				{
					isMeaningful = true;
					tokens.emplace_back(STRING, line, temp);
					temp.clear();
				}
				isMeaningful = true;
				tokens.emplace_back(OPEN_PAREN, line);
			}
			else
				temp.push_back(c);
			break;
		case ')':
			if (isEven(quotes)) {
				paren--;
				if (!temp.empty())
				{
					isMeaningful = true;
					tokens.emplace_back(STRING, line, temp);
					temp.clear();
				}
				isMeaningful = true;
				tokens.emplace_back(CLOSE_PAREN, line);
			}
			else
				temp.push_back(c);
			break;
		case '"':
			quotes++;
			if (isEven(quotes) || !temp.empty())
			{
				isMeaningful = true;
				tokens.emplace_back(STRING, line, temp);
				temp.clear();
			}
			isMeaningful = true;
			tokens.emplace_back(STRING_DELIM, line);
			break;
		case '=':
			if (isEven(quotes)) {
				if (!temp.empty())
				{
					tokens.emplace_back(STRING, line, temp);
					temp.clear();
				}
				isMeaningful = true;
				tokens.emplace_back(EQUAL, line);
			}
			else
				temp.push_back(c);
			break;
		case '\t':
			if (isEven(quotes)) {
				if (!temp.empty()) {
					isMeaningful = true;
					tokens.emplace_back(STRING, line, temp);
					temp.clear();
				}
			}
			else {
				temp.push_back(c);
			}
			break;
		case '\r':
			//Do nothing, ignore
			break;
		case '#':
			while (ins.get(c))
			{
				if (c == '\n')
				{
					if (isMeaningful)
					{
						line++;
						tokens.emplace_back(EOL, line);
						isMeaningful = false;
					}
					else
					{
						line++;
					}
					break;
				}
			}
			break;
		default:
			isMeaningful = true;
			temp.push_back(c);
		}
		if (errorFound)
			break;
	}
	if (!temp.empty())
	{
		tokens.emplace_back(STRING, line, temp);
		temp.clear();
		tokens.emplace_back(EOL, line);
		isMeaningful = false;
	}
	if (errorFound)
	{
		tokens.emplace_back(ERROR, line, "Error: A string must consist of matching quotations and cannot span multiple lines.");
		temp.clear();
	}
	return tokens;
}



