#ifndef _HUNT_
#define _HUNT_
#include "constant.h" // for constants
#include "getMode.h" // for struct Mode
#include <utility> // for pair
#include <vector>
#include <string>
#include <deque>

// first is row, second is column
typedef std::pair<unsigned int, unsigned int> Location;

class HuntClass{
private:
    // describe each square in map
    // default terrain is '.', discovered is false, comeFrom is UINT32_MAX pair
    struct Square{
        // type of square, can be . o #
        char terrain;
        // indicate whether this square is discovered
        bool discovered;
        // use to backtrack
        Location comeFrom;
        Square(): terrain('.'), discovered(false), comeFrom(Location(UINT32_MAX, UINT32_MAX)){}
    };

    // 1 indicate Stack, 2 indicate queue
    uint8_t captainContainer;
    // 1 indicate Stack, 2 indicate queue
    uint8_t firstMateContainer;
    std::string huntOrder;
    bool whetherVerbose;
    bool whetherStats;
    bool whetherShowPath;
    // M indicate to use map, L indicate to use List
    char pathOption;
    

    // indicate whether the hunt succeed
    bool success;
    unsigned int mapsize;
    Location startPoint;
    Location treasure;
    std::vector<std::vector<Square>> map;

    // used to do back track
    std::deque<Location> path;
    // count how many land location we have already investigated
    uint64_t landLocationCount;
    // count how many water location we have already investigated
    uint64_t waterLocationCount;
    // count how many times the first-mate go ashore
    uint32_t ashoreCount;
    // helper function to help ctor set map by map
    void setByMap();
    // helper function to help ctor set map by list
    void setByList();
    
    // MODIFY: it will update the explored land location and impassable location to discovered
    //         but the water location will not be changed
    // EFFECT: return search result
    bool searchLand(const Location& landLocation);
    
    /* REQUIRE: succeed in finding treasure
       MODIFY: for show path with M mode, it will update the map to reflect the path to treasure,
               If user choose showpath with L mode, it will update the 
               deque path in explore order but without start point
       EFFECT: return path length from start point to treasure 
    */
    unsigned int backTrack();
    // for stats option
    void printStats(unsigned int pathLength);
    // for show-path option "L" mode
    void printListPath();
    // for show-path option "M" mode
    void printMap();

public:
    // REQUIRE: a valid mode struct, also need a valid map file content in stdin
    // EFFECT: set up the class member through specified user mode
    HuntClass(const Mode& userMode);
    void startHunt();


};

#endif