#include "getMode.h"
#include <iostream>
#include <getopt.h>
#include <string>
#include <unordered_set>
using namespace std;

void printHelp(char *argv[]) {
    cout << "Usage: " << argv[0] << " -option " << '\n';
    cout << "--captain or -c <queue|stack>  (default: stack)" << '\n';
    cout << "--first-mate or -f <queue|stack>  (default: queue)" << '\n';
    cout << "--hunt-order or -o <order>  (default: nesw)" << '\n';
    cout << "--verbose or -v " << '\n';
    cout << "--stats or -s " << '\n';
    cout << "--show-path or -p <M|L> " << '\n';
    cout << "--help or -h" << '\n';
} // printHelp()

bool checkHuntOrder(const std::string& str){
    if (str.size() != 4) return false;
    unordered_set<char> validChar = {'n', 'e', 's', 'w'};
    for(const char& eachChar : str){
        //if we can't find this char
        if(validChar.find(eachChar) == validChar.end()) return false;
        else{
            validChar.erase(eachChar);
        }
    } 
    return true;
}

ostream& operator<<(ostream& os, const Mode& rhs){
    os << "captain mode: " << static_cast<uint16_t>(rhs.captainMode) << '\n';
    os << "fisrt-mate mode: " << static_cast<uint16_t>(rhs.firstMateMode) << '\n';
    os << "hunt-order: " << rhs.huntOrder << '\n';
    os << "whether verbose: " << rhs.whetherVerbose << '\n';
    os << "whether stats: " << rhs.whetherStats << '\n';
    os << "whether show path: " << rhs.whetherShowPath << '\n';
    if(rhs.whetherShowPath){
        os << "show path mode: " << rhs.showPathMode << '\n';
    } 
    return os;
}

Mode getMode(int argc, char * argv[]) {
    Mode userMode;
    bool whetherGetPath = false;

    // These are used with getopt_long()
    opterr = true; // Give us help with errors
    int choice;
    int option_index = 0;
    
    // these are all options we have, for each option, we need to specify four entries
    option long_options[] = {
        // "mode is long version", "m" is short version
        { "captain", required_argument, nullptr, 'c'},
        { "first-mate", required_argument, nullptr, 'f'},
        { "hunt-order", required_argument, nullptr, 'o'},
        { "verbose", no_argument, nullptr, 'v'},
        { "stats", no_argument, nullptr, 's'},
        { "show-path", required_argument, nullptr, 'p'},
        { "help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, '\0'}
    };

    // while choice(return from getopt_long()) != -1, continue. 
    // getopt_long will return the letter that the user specify
    // m: indicates that m need argument
    while ((choice = getopt_long(argc, argv, "c:f:o:vsp:h", long_options, &option_index)) != -1) {
        switch (choice) {
        case 'h':{
            printHelp(argv);
            exit(0);
        }
        
        case 'c':{
            // optarg is option m's argument
            string captainMode(optarg);
            if (captainMode != "stack" && captainMode != "queue") {
                cerr << "Error: invalid captain mode " << captainMode << '\n';
                exit(1);
            }
            if (captainMode == "queue") userMode.captainMode = QUEUE;
            else userMode.captainMode = STACK;
        }
        break;
        case 'f':{
            string firstMateMode(optarg);
            if (firstMateMode != "stack" && firstMateMode != "queue"){
                cerr << "Error: invalid fisrt-mate mode " << firstMateMode << '\n';
                exit(1);
            }
            if (firstMateMode == "stack") userMode.firstMateMode = STACK;
            else userMode.firstMateMode = QUEUE;
        }
        break;
        case 'o':{
            string huntOrder(optarg);
            if ( !checkHuntOrder(huntOrder) ){
                cerr << "Error: invalid hunt-order " << huntOrder << '\n';
                exit(1);
            }
            userMode.huntOrder = huntOrder;
        }
        break;
        case 'v':{
            userMode.whetherVerbose = true;
        }
        break;
        case 's':{
            userMode.whetherStats = true;
        }
        break;
        case 'p':{
            if (whetherGetPath){
                cerr << "Error: show-path is specified more than once!" << '\n';
                exit(1);
            }
            string pathMode(optarg);
            if (pathMode.size() != 1){
                cerr << "Error: invalid show-path length " << pathMode.size() << '\n';
                exit(1); 
            }
            if (pathMode[0] != 'M' && pathMode[0] != 'L'){
                cerr << "Error: invalid show-path mode " << pathMode << '\n';
                exit(1);
            }
            whetherGetPath = true;
            userMode.whetherShowPath = true;
            userMode.showPathMode = pathMode[0];
        }
        break;
        default:
            cerr << "Error: invalid option" << '\n';
            exit(1);
        } // switch
    } // while

    return userMode;
} // getMode()