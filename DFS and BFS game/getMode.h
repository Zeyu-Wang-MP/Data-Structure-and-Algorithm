#ifndef _GETMODE_
#define _GETMODE_

#include "constant.h"
#include <string>
#include <stdint.h>
#include <iostream>


struct Mode{
    uint8_t captainMode = STACK;
    uint8_t firstMateMode = QUEUE;
    std::string huntOrder = "nesw";
    bool whetherVerbose = false;
    bool whetherStats = false;
    bool whetherShowPath = false;
    char showPathMode = '\0';
}; 

std::ostream& operator<<(std::ostream& os, const Mode& rhs);

void printHelp(char *argv[]);

Mode getMode(int argc, char * argv[]);

bool checkHuntOrder(const std::string& str);

#endif