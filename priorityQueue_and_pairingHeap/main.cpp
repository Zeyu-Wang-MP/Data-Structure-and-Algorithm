#include "constant.h" //for some constant
#include "getMode.h" //for getMode()
#include "Hunt.h" //for HuntClass 
#include <stdint.h>
#include <string>
#include <iostream>


using namespace std;

int main(int argc, char* argv[]){
    Mode userChoice = getMode(argc, argv);
    HuntClass hunt(userChoice);
    hunt.startHunt();
}



