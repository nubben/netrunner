#ifndef COMMANDLINEPARAMS_H
#define COMMANDLINEPARAMS_H

#include <string>

void initCLParams(int argc, char *argv[]);

// USAGE: getCLParam("-example")
bool getCLParam(std::string const& param);

// 0 is the program name
std::string getCLParamByIndex(unsigned int i);

// USAGE: getRelativeCLParam("-example", 1) gives the param after -example
std::string getRelativeCLParam(std::string const& param, int i);

#endif
