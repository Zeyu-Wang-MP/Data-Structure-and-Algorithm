#include "Drone.h"
#include <getopt.h>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

Mode getMode(int argc, char * argv[]) {
    bool modeSpecified = false;
    string mode;

    // These are used with getopt_long()
    opterr = true; // Give us help with errors
    int choice;
    int option_index = 0;
    
    // these are all options we have, for each option, we need to specify four entries
    option long_options[] = {
        // "mode is long version", "m" is short version
        { "mode", required_argument, nullptr, 'm'},
        { "help", no_argument,       nullptr, 'h'},
        { nullptr, 0,                nullptr, '\0'}
    };

    // while choice(return from getopt_long()) != -1, continue. 
    // getopt_long will return the letter that the user specify
    // m: indicates that m need argument
    while ((choice = getopt_long(argc, argv, "m:h", long_options, &option_index)) != -1) {
        switch (choice) {
        case 'h':
            cout << "usage: -m | --mode < MST|FASTTSP|OPTTSP" << endl;
            exit(0);

        case 'm':
            // optarg is option m's argument
            mode = optarg;
            if (mode != "MST" && mode != "FASTTSP" && mode != "OPTTSP") {
                cerr << "Error: invalid mode " << mode << endl;
                exit(1);
            } // if
            modeSpecified = true;
            break;

        default:
            cerr << "Error: invalid option" << endl;
            exit(1);
        } // switch
    } // while

    if (!modeSpecified) {
        cerr << "Error: no mode specified" << endl;
        exit(1);
    } // if
    if(mode == "MST") return Mode::MST;
    else if(mode == "FASTTSP") return Mode::FASTTSP;
    else return Mode::OPTTSP;
} // getMode()

int main(int argc, char* argv[]){
    cout << std::setprecision(2);
    cout << std::fixed;
    Mode userMode = getMode(argc, argv);
    Drone program(userMode);
    return program.run();
}

