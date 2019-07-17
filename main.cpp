//
//  main.cpp
//  Ska interpreter
//
//  Created by Maxwell Guppy on 15/7/19.
//  Copyright © 2019 Dockdev. All rights reserved.
//

#include "cxxopts.hpp"

#include <cstdio>

#include <fstream>
#include <iostream>
#include <stack>
#include <unordered_map>

std::string UnicodeToUTF8(unsigned int codepoint)
{
	std::string out;
	
	if (codepoint <= 0x7f)
		out.append(1, static_cast<char>(codepoint));
	else if (codepoint <= 0x7ff)
	{
		out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
		out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
	else if (codepoint <= 0xffff)
	{
		out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
		out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
	else
	{
		out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
		out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
		out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
	return out;
}

class input {
public:
	virtual ~input() {}
	
	virtual char get() {return 0;};
	virtual bool good() {return false;}
};

class fsinput : public input {
	std::ifstream *stream;
public:
	fsinput(std::ifstream *stream) : stream(stream) {}
	~fsinput() override {delete stream;}
	
	char get() override
	{
		return stream->get();
	}
	
	bool good() override
	{
		return stream->good();
	}
};

class cinput : public input {
public:
	char get() override
	{
		return std::cin.get();
	}
	
	bool good() override
	{
		return std::cin.good();
	}
};

class fninput : public input {
	std::string function;
	int pointer = 0;
public:
	fninput(std::string data) : function(data) {}
	
	char get() override
	{
		return function[pointer++]; // Yes, I am aware that this is a postfix increment
	}
	
	bool good() override
	{
		return pointer < function.length();
	}
};

void error(int line, int col, std::string msg)
{
	std::cerr << "Ska (" << line << ":" << col << "): " << msg << std::endl;
	throw "this ball";
}

int main(int argc, char *argv[])
{
	cxxopts::Options parser = cxxopts::Options("Ska interpreter");
	
	parser.add_options()
	("d,debug", "Enable debug mode")
	("f,file", "Name of the file to be run", cxxopts::value<std::string>(), "filename")
	("h,help", "Shows a help message")
	;
	
	auto options = parser.parse(argc, argv);
	
	if (options.count("help")) {
		std::cout << parser.help();
		return EXIT_SUCCESS;
	}
	
	std::stack<input*> inputStack;
	
	if (options.count("file")) {
		std::ifstream *ifs = new std::ifstream(options["file"].as<std::string>());
		if (ifs->fail()) {
			delete ifs;
			std::cerr << "Could not open file \"" << options["file"].as<std::string>() << "\"" << std::endl;
			return EXIT_FAILURE;
		}
		inputStack.push(new fsinput(ifs));
	} else {
		inputStack.push(new cinput());
	}
	
	bool DEBUGF = options.count("debug");
	
#define in inputStack.top()
	
	//Declaration of interpreter variables
	//- Data about position and token
	char c = in->get();
	unsigned int line = 1, col = 1;
	
	std::stack<std::pair<unsigned int, unsigned int>> locStack;
	std::stack<std::pair<std::string, std::pair<unsigned int, unsigned int>>> loopStack;
	
	//Memory
	std::unordered_map<std::string, long> memory;
	std::unordered_map<std::string, std::pair<std::string, std::pair<unsigned int, unsigned int>>> functions;
	
	//"Registers"
	long accumulator = 0;
	long counter = 0;
	
	std::string stringSrc;
	std::string stringDest;
	
	std::vector<std::string> stringList(argc);
	std::vector<std::string> listBackup;
	for (int i = 0; i < argc; i++) {
		stringList[i] = argv[i];
	}
	
	try {
		while (!inputStack.empty()) {
			if (!in->good()) {
				delete in;
				inputStack.pop();
				if (inputStack.empty()) break;
				std::pair<int, int> newLoc = locStack.top();
				line = newLoc.first;
				col = newLoc.second;
				locStack.pop();
			}
			switch (c) {
				//Memory storage
				case 's': // Stores the accumulator into the location specified by stringDest
					memory[stringDest] = accumulator;
					break;
				case 'l': // Loads the value at stringSrc into the accumulator
					try {
						accumulator = memory.at(stringSrc);
					} catch (std::out_of_range) {
						error(line, col, "No memory at specified location");
					}
					break;
				
				//Numeric values
				case '0': // Multiplies the counter by 10, then adds 0
				case '1': // --------------------------------------- 1
				case '2': // --------------------------------------- 2
				case '3': // --------------------------------------- 3
				case '4': // --------------------------------------- 4
				case '5': // --------------------------------------- 5
				case '6': // --------------------------------------- 6
				case '7': // --------------------------------------- 7
				case '8': // --------------------------------------- 8
				case '9': // --------------------------------------- 9
					counter *= 10;
					counter += c - 48;
					break;
					
				//Input
				case 'P':
					{
						char c;
						std::cin >> c;
						accumulator = c;
					}
					break;
				case 'O':
					std::cin >> accumulator;
					break;
				case 'I':
					std::getline(std::cin, stringDest);
					break;
					
				//Output
				case 'p': // Prints the current value of stringSrc
					std::cout << stringSrc << std::flush;
					break;
				case 'o': // Prints the current value of the accumulator
					std::cout << accumulator << std::flush;
					break;
				case 'i': // Prints a newline
					std::cout << std::endl;
					break;
				
				//Strings
				case '"': // Loads a string into stringSrc
					{
						std::string buffer;
						do {
							c = in->get();
							col++;
							switch (c) {
								case '\\':
									c = in->get();
									col++;
									switch (c) {
										case 'a':
											buffer += '\a';
											break;
										case 'b':
											buffer += '\b';
											break;
										case 'e':
											buffer += '\e';
											break;
										case 'f':
											buffer += '\f';
											break;
										case 'n':
											buffer += '\n';
											break;
										case 'r':
											buffer += '\r';
											break;
										case 't':
											buffer += '\t';
											break;
										case 'u':
											try {
												int unicode = stoi(std::string()+={in->get(), in->get(), in->get(), in->get()}, nullptr, 16);
												std::string utf8 = UnicodeToUTF8(unicode);
												buffer += utf8;
											} catch (std::invalid_argument) {
												error(line, col, "Invalid hexadecimal value for \\u");
											}
											col += 4;
											break;
										case 'U':
											try {
												int unicode = stoi(std::string()+={in->get(), in->get(), in->get(), in->get(), in->get(), in->get(), in->get(), in->get()}, nullptr, 16);
												std::string utf8 = UnicodeToUTF8(unicode);
												buffer += utf8;
											} catch (std::invalid_argument) {
												error(line, col, "Invalid hexadecimal value for \\U");
											}
											col += 8;
											break;
										case 'v':
											buffer += '\v';
											break;
										case 'x':
											try {
												int unicode = stoi(std::string()+={in->get(), in->get()}, nullptr, 16);
												std::string utf8 = UnicodeToUTF8(unicode);
												buffer += utf8;
											} catch (std::invalid_argument) {
												error(line, col, "Invalid hexadecimal value for \\x");
											}
											col += 2;
											break;
										case '\\':
											buffer += '\\';
											break;
										case '"':
											buffer += '"';
											break;
										default:
											error(line, col, "Invalid escape sequence");
									}
									break;
								case '"':
									break;
								case '\n':
									error(line, col, "Linefeed in string literal");
								default:
									buffer += c;
							}
						} while (c != '"' && in->good());
						stringSrc = buffer;
					}
					break;
				case '\'': // Loads a string into stringDest
					{
					std::string buffer;
					do {
						c = in->get();
						col++;
						switch (c) {
							case '\\':
								c = in->get();
								col++;
								switch (c) {
									case 'a':
										buffer += '\a';
										break;
									case 'b':
										buffer += '\b';
										break;
									case 'e':
										buffer += '\e';
										break;
									case 'f':
										buffer += '\f';
										break;
									case 'n':
										buffer += '\n';
										break;
									case 'r':
										buffer += '\r';
										break;
									case 't':
										buffer += '\t';
										break;
									case 'u':
										try {
											int unicode = stoi(std::string()+={in->get(), in->get(), in->get(), in->get()}, nullptr, 16);
											std::string utf8 = UnicodeToUTF8(unicode);
											buffer += utf8;
										} catch (std::invalid_argument) {
											error(line, col, "Invalid hexadecimal value for \\u");
										}
										col += 4;
										break;
									case 'U':
										try {
											int unicode = stoi(std::string()+={in->get(), in->get(), in->get(), in->get(), in->get(), in->get(), in->get(), in->get()}, nullptr, 16);
											std::string utf8 = UnicodeToUTF8(unicode);
											buffer+= utf8;
										} catch (std::invalid_argument) {
											error(line, col, "Invalid hexadecimal value for \\U");
										}
										col += 8;
										break;
									case 'v':
										buffer += '\v';
										break;
									case 'x':
										try {
											int unicode = stoi(std::string()+={in->get(), in->get()}, nullptr, 16);
											std::string utf8 = UnicodeToUTF8(unicode);
											buffer += utf8;
										} catch (std::invalid_argument) {
											error(line, col, "Invalid hexadecimal value for \\x");
										}
										col += 2;
										break;
									case '\\':
										buffer += '\\';
										break;
									case '\'':
										buffer += '\'';
										break;
									default:
										error(line, col, "Invalid escape sequence");
								}
								break;
							case '\'':
								break;
							case '\n':
								error(line, col, "Linefeed in string literal");
							default:
								buffer += c;
						}
					} while (c != '\'' && in->good());
					stringDest = buffer;
				}
					break;
				case 't': // Swaps stringSrc and stringDest
					{
						std::string temp = stringSrc;
						stringSrc = stringDest;
						stringDest = temp;
					}
					break;
				case 'j':
					try {
						accumulator = stringSrc[counter];
					} catch (std::out_of_range) {
						error(line, col, "String index out of range");
					}
					break;
				case 'k':
					while (stringDest.length() <= counter) {
						stringDest += " ";
					}
					stringDest[counter] = accumulator;
					break;
				case 'e':
					stringDest += stringSrc;
					
				//Lists
				case 'w':
					accumulator = stringList.size();
					break;
				case 'y':
					{
						size_t last = 0;
						size_t next = 0;
						while ((next = stringSrc.find(stringDest, last)) != std::string::npos) {
							stringList.push_back(stringSrc.substr(last, next-last));
							last = next + 1;
						}
						stringList.push_back(stringSrc.substr(last, stringSrc.length()));
					}
					break;
				case 'v':
					for (auto item : listBackup) {
						stringList.push_back(item);
					}
					break;
				case 'u':
					{
						std::vector<std::string> temp = stringList;
						stringList = listBackup;
						listBackup = temp;
					}
					break;
				case 'b':
					try {
						stringDest = stringList[counter];
					} catch (std::out_of_range) {
						error(line, col, "List index out of range");
					}
					break;
				case 'f':
					while (stringList.size() <= counter) {
						stringList.push_back("");
					}
					stringList[counter] = stringSrc;
					break;
				case 'Z':
					stringList.erase(stringList.begin(), stringList.end());
					break;
					
				//Arithmetic
				case 'a':
					accumulator += counter;
					break;
				case 'm':
					accumulator -= counter;
					break;
				case 'x':
					accumulator *= counter;
					break;
				case 'd':
					accumulator /= counter;
					break;
				case 'r':
					accumulator %= counter;
					break;
				case 'z':
					counter = 0;
					break;
				case 'c':
					{
						long temp = counter;
						counter = accumulator;
						accumulator = temp;
					}
					break;
					
				//Flow control
				case '[':
					{
						std::string lpdata;
						std::pair<unsigned int, unsigned int> location = std::make_pair(line, col);
						while (c != ']') {
							c = in->get();
							if (c == '\n') {
								line++;
								col = 0;
							}
							col++;
							lpdata += c;
						}
						locStack.push(std::make_pair(line, col));
						loopStack.push(std::make_pair(lpdata, location));
						inputStack.push(new fninput(lpdata));
						
						line = location.first;
						col = location.second;
					}
					break;
				case ']':
					if (loopStack.empty()) error(line, col, "Unmatched closing bracket");
					if (accumulator != 0) {
						inputStack.push(new fninput(loopStack.top().first));
						locStack.push(std::make_pair(line, col));
						line = loopStack.top().second.first;
						col = loopStack.top().second.second;
					} else {
						loopStack.pop();
					}
					break;
				case '{':
					{
						if (functions.count(stringDest)) error(line, col, "Function declared twice");
						c = in->get();
						col++;
						std::string fndata;
						std::pair<int, int> location = std::make_pair(line, col - 1);
						while (c != '}') {
							fndata += c;
							c = in->get();
							col++;
						}
						functions[stringDest] = std::make_pair(fndata, location);
					}
					break;
				case '}':
					error(line, col, "Unmatched closing brace");
					break;
				case 'n': // acc == 0
					if (accumulator != 0) {
						in->get();
						col++;
					}
					break;
				case 'N': // acc != 0
					if (accumulator == 0) {
						c = in->get();
						col++;
					}
					break;
				case 'g': // acc > 0
					if (accumulator <= 0) {
						c = in->get();
						col++;
					}
					break;
				case 'G': // acc <= 0 (!<)
					if (accumulator > 0) {
						c = in->get();
						col++;
					}
					break;
				case 'h': // acc < 0
					if (accumulator >= 0) {
						c = in->get();
						col++;
					}
					break;
				case 'H': // acc >= 0 (!>)
					if (accumulator < 0) {
						c = in->get();
						col++;
					}
					break;
				case 'q':
					try {
						auto f = functions.at(stringSrc);
						inputStack.push(new fninput(f.first));
						locStack.push(std::make_pair(line, col));
						
						line = f.second.first;
						col = f.second.second;
					} catch (std::out_of_range) {
						error(line, col, "No function with specified name");
					}
					break;
				
				//Miscellaneous
				case '(':
					while ((c = in->get()) != ')') {
						if (c == '\n') {
							line++;
							col = 0;
						}
						col++;
					}
					break;
				case '\t':
				case ' ':
					break;
				case '\n':
					line++;
					col = 0;
					break;
				default:
					error(line, col, "Unrecognised token");
			}
			if (DEBUGF && c != '\n' && c != ' ' && c != '\t') {
				printf("Character: %c\nAccum Count \n%5ld %5ld\nSrc: \"%s\"\nDest: \"%s\"\nList:\n", c, accumulator, counter, stringSrc.c_str(), stringDest.c_str());
				for (std::string item : stringList) {
					std::cout << '\t' << (item == "" ? "(Empty)" : item) << std::endl;
				}
				for (auto pair : memory) {
					std::cout << "\t\"" << pair.first << "\": " << pair.second;
				}
				std::cin.get();
			}
			c = in->get();
			col++;
		}
	} catch (const char*) {}
	
	while (!inputStack.empty()) {
		delete in;
		inputStack.pop();
	}
}
