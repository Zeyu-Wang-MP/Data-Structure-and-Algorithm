#include <cstdint>
#include <vector>
#include <limits>
#include <cmath>


enum class Mode: uint8_t{
    MST, FASTTSP, OPTTSP
};

class Drone{
private:
//private type
    enum class LocationType: uint8_t{medical, normal, border};
    struct Location{
        int x;
        int y;
        LocationType type;
        Location(int _x, int _y):x(_x), y(_y){
            if(x < 0 && y < 0) type = LocationType::medical;
            else if(x == 0 || y == 0) type = LocationType::border;
            else type = LocationType::normal;
        }
    };
    // used in the prim table
    struct Vertex{
        double d;
        int p;
        bool whetherVisited;
        Vertex():d(std::numeric_limits<double>::infinity()), p(-1), whetherVisited(false){}
    };
    
    // used in the Kruskal's Algorithm
    struct Edge{
        unsigned v1;
        unsigned v2;
        double distance;
        bool whetherUsed;
        Edge(unsigned _v1, unsigned _v2, double _d):v1(_v1), v2(_v2), distance(_d), whetherUsed(false){}
    };
    // functor used to compare two Edge
    class Comp{
    public:
        bool operator()(const Edge& lhs, const Edge& rhs){
            return lhs.distance < rhs.distance;
        }
    };

    // functor used in part1 MST
    class DWithBorder{
    private:
        const std::vector<Location>& locationTable;
    public:
        DWithBorder(const std::vector<Location>& _lt): locationTable(_lt){}
        double operator()(unsigned first, unsigned second){
            if((locationTable[first].type == LocationType::medical && locationTable[second].type == LocationType::normal) ||
                (locationTable[first].type == LocationType::normal && locationTable[second].type == LocationType::medical)){
                return std::numeric_limits<double>::infinity();
            }
            else{
                double x1{static_cast<double>(locationTable[first].x)}, x2{static_cast<double>(locationTable[second].x)};
                double y1{static_cast<double>(locationTable[first].y)}, y2{static_cast<double>(locationTable[second].y)};
                return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)); 
            }
        }
    };
    
    // functor used in FASTTSP 
    class DWithoutBorder{
    private:
        const std::vector<Location>& locationTable;
    public:
        DWithoutBorder(const std::vector<Location>& _lt): locationTable(_lt){}
        double operator()(unsigned first, unsigned second){
            if(first == second) return 0;
            double x1{static_cast<double>(locationTable[first].x)}, x2{static_cast<double>(locationTable[second].x)};
            double y1{static_cast<double>(locationTable[first].y)}, y2{static_cast<double>(locationTable[second].y)};
            return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)); 
        }  
    };
    
    // functor used in OPTTSP, need to pass it by reference
    class DWithMatrix{
    private:
        std::vector<std::vector<double>> adjMatrix;
    public:
        DWithMatrix(const std::vector<Location>& locationTable){
            DWithoutBorder distance(locationTable);
            adjMatrix.resize(locationTable.size());
            for(std::vector<double>& eachRow : adjMatrix){
                eachRow.resize(locationTable.size());
            }
            // for each row in adjMatrix
            for(unsigned i = 0; i < adjMatrix.size(); ++i){
                for(unsigned j = 0; j < adjMatrix.size(); ++j){
                    adjMatrix[i][j] = distance(i, j);
                }
            }
        }
        double operator()(unsigned first, unsigned second){
            if(first == second) return 0;
            return adjMatrix[first][second];
        }
    };

//private member
    Mode userMode;
    std::vector<Location> locationTable;

    //following members are only used in OPTTSP
    double currBound; // update when we find better path
    std::vector<unsigned> path;
    double currPathLength;
    // only update when find the better path
    std::vector<unsigned> finalPath;
    //used to compute distance in genPerms
    DWithMatrix* distancePtr;

//private method
    // REQUIRE: all indices in toRealIndex are valid vertex index in location table
    // EFFECT: find the MST on the graph defined by given indices, use given functor to
    //         calculate the distance between two vertices, return the total weight
    //         using Prim's Algorithm
    template<typename FUNCTOR>
    double runMST1(const std::vector<unsigned>& toRealIndex, FUNCTOR& distance, bool whetherPrint);

    // same to runMST1 but using Kruskal's Algorithm
    template<typename FUNCTOR>
    double runMST2(const std::vector<unsigned>& toRealIndex, FUNCTOR& distance, bool whetherPrint);
    
    // EFFECT: run TSP, store the path in the given vector
    // using nearest neighbor + 2-opt
    template<typename FUNCTOR>
    double runFASTTSP1(FUNCTOR& distance, std::vector<unsigned>& travelOrder, bool whetherPrint);
    
    void runOPTTSP(DWithMatrix& distance);
    
    // helper functions used in OPTTSP
    // MODIFY: these helper functions will change currPathLength and path vector
    void genPerms(size_t permLength);
    bool promising(size_t permLength);

public:
    // REQUIRE: cin starts at number of locations
    // MODIFY: create locationTable
    Drone(Mode _userMode);
    
    // EFFECT: run the specified mode
    int run();
};