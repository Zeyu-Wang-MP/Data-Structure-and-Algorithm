#include "Hunt.h"
#include "getMode.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

HuntClass::HuntClass(const Mode& userMode){
    this->success = false;
    this->landLocationCount = 0;
    this->waterLocationCount = 0;
    this->ashoreCount = 0;

    this->captainContainer = userMode.captainMode;
    this->firstMateContainer = userMode.firstMateMode;
    this->huntOrder = userMode.huntOrder;
    this->whetherVerbose = userMode.whetherVerbose;
    this->whetherStats = userMode.whetherStats;
    this->whetherShowPath = userMode.whetherShowPath;
    this->pathOption = userMode.showPathMode;

    string line;
    do getline(cin, line);
    while(line[0] == '#');

    cin >> this->mapsize;
    // resize the 2D vector map to default square
    this->map.resize(this->mapsize);
    for(vector<Square>& each: this->map){
        each.resize(this->mapsize);
    }
    // if the map format is map
    if(line[0] == 'M') this->setByMap();
    else this->setByList();
}

void HuntClass::setByMap(){
    string line;
    //skip the newline followed by mapsize
    getline(cin, line);
    // i is which row
    for(unsigned int i = 0; i < this->mapsize; ++i){
        getline(cin, line);
        // j is which column
        for(unsigned int j = 0; j < line.size(); ++j){
            this->map[i][j].terrain = line[j];
            if(line[j] == '@') {
                this->startPoint = Location(i, j);
                this->map[i][j].terrain = '.';
            }
            if(line[j] == '$') {
                this->treasure = Location(i, j);
                this->map[i][j].terrain = 'o';
            }
        }
    }
}

void HuntClass::setByList(){
    unsigned int row, column;
    char terrain;
    while(cin >> row >> column >> terrain){
        this->map[row][column].terrain = terrain;
        if(terrain == '@') {
            this->startPoint = Location(row, column);
            this->map[row][column].terrain = '.';
        }
        if(terrain == '$') {
            this->treasure = Location(row, column);
            this->map[row][column].terrain = 'o';
        }
    }
}

void HuntClass::startHunt(){
    deque<Location> sailContainer;
    if(this->whetherVerbose){
        cout << "Treasure hunt started at: " << this->startPoint.first << ',';
        cout << this->startPoint.second << '\n'; 
    }
    sailContainer.push_back(this->startPoint);
    //update the discovered status for startPoint
    this->map[this->startPoint.first][this->startPoint.second].discovered = true;
    Location currLocation;
    while((!sailContainer.empty()) && (!this->success)){
        //get the current location
        if(this->captainContainer == STACK) {
            // every time we pop a location, we arrive this location, 
            // so add the water location count
            this->waterLocationCount++;
            currLocation = sailContainer.back();
            sailContainer.pop_back();
        }
        else {
            this->waterLocationCount++;
            currLocation = sailContainer.front();
            sailContainer.pop_front();
        }
        //according to hunt order, explore one direction each time
        for(unsigned int i = 0; i < this->huntOrder.size(); ++i){
            //new location for this time
            Location newLocation;
            switch(this->huntOrder[i]){
                // if current direction is north
                case 'n':{
                    if(currLocation.first == 0) continue;
                    newLocation = Location(currLocation.first - 1, currLocation.second);
                }
                break;
                case 'e':{
                    if(currLocation.second == this->mapsize - 1) continue;
                    newLocation = Location(currLocation.first, currLocation.second + 1);
                }
                break;
                case 's':{
                    if(currLocation.first == this->mapsize - 1) continue;
                    newLocation = Location(currLocation.first + 1, currLocation.second);
                }
                break;
                case 'w':{
                    if(currLocation.second == 0) continue;
                    newLocation = Location(currLocation.first, currLocation.second - 1);
                }
                break;
            }
            // now we already ensure that newlocation is in bounds
            // we need to check if the new location is already discovered or not
            if(this->map[newLocation.first][newLocation.second].discovered) continue;
            // if not discovered yet, change it to discovered
            else this->map[newLocation.first][newLocation.second].discovered = true;

            // then we need to check if the new location is impassable
            if(this->map[newLocation.first][newLocation.second].terrain == '#') continue;

            // then we check if the new location is water location
            if(this->map[newLocation.first][newLocation.second].terrain == '.'){
                //before we add this newlocation to deque, we update it's comefrom status in map
                //to current location
                this->map[newLocation.first][newLocation.second].comeFrom = currLocation;
                sailContainer.push_back(newLocation);
                continue;
            }

            // then we check if the new location is land location
            if(this->map[newLocation.first][newLocation.second].terrain == 'o'){
                this->map[newLocation.first][newLocation.second].comeFrom = currLocation;
                this->success = this->searchLand(newLocation);
                // if this land search find the treasure, we don't need to explore next direction
                if(this->success){
                    if(this->whetherVerbose){
                        cout << "Searching island... party found treasure at ";
                        cout << this->treasure.first << ',' << this->treasure.second << ".\n";
                    }
                    break;
                }
                else{
                    if(this->whetherVerbose){
                        cout << "Searching island... party returned with no treasure.\n"; 
                    }
                }
            }
        } 
    }
    
    if(this->success){
        unsigned int pathLength = this->backTrack();
        if(this->whetherStats) this->printStats(pathLength);
        if(this->whetherShowPath && this->pathOption == 'M') this->printMap();
        if(this->whetherShowPath && this->pathOption == 'L') this->printListPath();
        cout << "Treasure found at " << this->treasure.first << ',' << this->treasure.second;
        cout << " with path length " << pathLength << '.' << endl; 
    }
    else{
        if(this->whetherVerbose){
            cout << "Treasure hunt failed\n";
        }
        if(this->whetherStats) this->printStats(0);
        cout << "No treasure found after investigating ";
        cout << this->landLocationCount + this->waterLocationCount << " locations." << endl;
    }
}

bool HuntClass::searchLand(const Location& landLocation){
    this->ashoreCount++;
    if(this->whetherVerbose){
        cout << "Went ashore at: " << landLocation.first << ',' << landLocation.second << '\n';
    }
    // before we add this location to searchContainer, check if it's treasure
    if(landLocation == this->treasure){
        this->landLocationCount++;
        return true;
    }
    
    deque<Location> searchContainer;
    searchContainer.push_back(landLocation);
    Location currLocation;
    while(!searchContainer.empty()){
        if(this->firstMateContainer == STACK){
            // every time we pop a location, we arrive this location, 
            // so add the land location count
            this->landLocationCount++;
            currLocation = searchContainer.back();
            searchContainer.pop_back();
        }
        else{
            this->landLocationCount++;
            currLocation = searchContainer.front();
            searchContainer.pop_front();
        }
        //according to hunt order, explore one direction each time
        for(unsigned int i = 0; i < this->huntOrder.size(); ++i){
            //new location for this time
            Location newLocation;
            switch(this->huntOrder[i]){
                // if current direction is north
                case 'n':{
                    if(currLocation.first == 0) continue;
                    newLocation = Location(currLocation.first - 1, currLocation.second);
                }
                break;
                case 'e':{
                    if(currLocation.second == this->mapsize - 1) continue;
                    newLocation = Location(currLocation.first, currLocation.second + 1);
                }
                break;
                case 's':{
                    if(currLocation.first == this->mapsize - 1) continue;
                    newLocation = Location(currLocation.first + 1, currLocation.second);
                }
                break;
                case 'w':{
                    if(currLocation.second == 0) continue;
                    newLocation = Location(currLocation.first, currLocation.second - 1);
                }
                break;
            }
            // after bound check, we need to check the type of newlocation
            if(this->map[newLocation.first][newLocation.second].terrain == '.') continue;

            if(this->map[newLocation.first][newLocation.second].discovered) continue;
            // if not discovered yet, change it to discovered
            else this->map[newLocation.first][newLocation.second].discovered = true;

            if(this->map[newLocation.first][newLocation.second].terrain == '#') continue;

            if(this->map[newLocation.first][newLocation.second].terrain == 'o'){
                if(newLocation == this->treasure){
                    // if new location is treasure, we don't add it to search container,
                    // but we need to add land location count
                    this->landLocationCount++;
                    // update treasure's comefrom status in map
                    this->map[newLocation.first][newLocation.second].comeFrom = currLocation;
                    return true;
                }
                else{
                    this->map[newLocation.first][newLocation.second].comeFrom = currLocation;
                    searchContainer.push_back(newLocation);
                }
            }
        }
    }
    return false;
}

unsigned int HuntClass::backTrack(){
    unsigned int pathLength = 0;
    Location currLocation = this->treasure;

    if(this->whetherShowPath && this->pathOption == 'L'){
        this->path.push_front(currLocation);
    }
    // as long as current location is not come from start point
    while(this->map[currLocation.first][currLocation.second].comeFrom != this->startPoint){
        Location lastLocation = this->map[currLocation.first][currLocation.second].comeFrom;
        // if path option is L, we can't change map, 
        // instead, we need to store every location in path deque
        if(this->whetherShowPath && this->pathOption == 'L'){
            this->path.push_front(lastLocation);
        }
        // if path option is M, we do need to change map
        else if(this->whetherShowPath && this->pathOption == 'M'){
            Location llLocation = this->map[lastLocation.first][lastLocation.second].comeFrom;
            if(currLocation.second == lastLocation.second && lastLocation.second == llLocation.second){
                this->map[lastLocation.first][lastLocation.second].terrain = '|';
            }
            else if(currLocation.first == lastLocation.first && lastLocation.first == llLocation.first){
                this->map[lastLocation.first][lastLocation.second].terrain = '-';
            }
            else this->map[lastLocation.first][lastLocation.second].terrain = '+';
        }
        currLocation = lastLocation;
        ++pathLength; 
    }
    // when user choose show-path with M, we need recover the start point and treasure in map
    if(this->whetherShowPath && this->pathOption == 'M'){
        this->map[this->startPoint.first][this->startPoint.second].terrain = '@';
        this->map[this->treasure.first][this->treasure.second].terrain = 'X';
    }
    return ++pathLength;
}

void HuntClass::printStats(unsigned int pathLength){
    cout << "--- STATS ---\n";
    cout << "Starting location: "<< this->startPoint.first << ',' << this->startPoint.second<<'\n';
    cout << "Water locations investigated: " << this->waterLocationCount << '\n';
    cout << "Land locations investigated: " << this->landLocationCount << '\n';
    cout << "Went ashore: " << this->ashoreCount << '\n';
    if(this->success){
        cout << "Path length: " << pathLength << '\n';
        cout << "Treasure location: " << this->treasure.first << ',' << this->treasure.second << '\n';
    }
    cout << "--- STATS ---\n";
}

void HuntClass::printMap(){
    for(const vector<Square>& eachRow: this->map){
        for(const Square& eachSquare: eachRow){
            cout << eachSquare.terrain;
        }
        cout << '\n';
    }
}

void HuntClass::printListPath(){
    cout << "Sail:\n";
    cout << this->startPoint.first << ',' << this->startPoint.second << '\n';
    uint32_t i = 0;
    
    for(; i < this->path.size(); ++i){
        //loop until we find first land location
        if(this->map[this->path[i].first][this->path[i].second].terrain == 'o') break;
        cout << this->path[i].first << ',' << this->path[i].second << '\n';
    }
    cout << "Search:\n";
    for(; i < this->path.size(); ++i){
        cout << this->path[i].first << ',' << this->path[i].second << '\n';
    }
}
