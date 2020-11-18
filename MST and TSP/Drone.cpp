#include "Drone.h"
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm> // for reverse, sort
#include <numeric> // for iota

using namespace std;

Drone::Drone(Mode _userMode):userMode(_userMode),
    currBound(0),
    currPathLength(0), 
    distancePtr(nullptr){
    unsigned locationNum;
    cin >> locationNum;
    this->locationTable.reserve(locationNum);
    for(unsigned i = 0; i < locationNum; ++i){
        int x, y;
        cin >> x >> y;
        this->locationTable.push_back(Location(x, y));
    }
}

int Drone::run(){
    switch(this->userMode){
        case Mode::MST:{
            vector<unsigned> vertexIndexTable;
            vertexIndexTable.reserve(this->locationTable.size());
            for(unsigned i = 0; i < this->locationTable.size(); ++i){
                vertexIndexTable.push_back(i);
            }
            DWithBorder distance(this->locationTable);
            try{
                this->runMST1(vertexIndexTable, distance, true);
            }
            catch(const exception& e){
                cerr << e.what() << endl;
                return 1;
            }
            return 0;
        }
        break;
        case Mode::FASTTSP:{
            vector<unsigned> temp;
            DWithoutBorder distance(this->locationTable);
            this->runFASTTSP1(distance, temp, true);
            return 0;
        }
        break;
        case Mode::OPTTSP:{
            DWithMatrix distance(this->locationTable);
            this->runOPTTSP(distance);
            return 0;
        }
        break;
    }
    return 0;
}

template<typename FUNCTOR>
double Drone::runMST1(const vector<unsigned>& toRealIndex, FUNCTOR& distance, bool whetherPrint){
    double totalWeight{0};
    // vertex order is same to toRealIndex
    vector<Vertex> primTable;

    primTable.resize(toRealIndex.size());
    // setup the starting point
    // the parent of first vertex is itself
    primTable[0].d = 0;
    primTable[0].p = toRealIndex[0];
    for(unsigned i = 0; i < primTable.size(); ++i){
        // start the linear search for the smallest d in all unvisited vertices
        // minIndex is the index for minimum vertex in pritTable, in order to get the 
        // real index in location table, use toRealIndex[index]
        size_t minIndex{primTable.size()};
        double minDistance{numeric_limits<double>::infinity()};
        // can use a vector to record which index in primTable is unvisited, if do so we 
        // don't need to traverse the whole primTable every time
        // (but't I didn't do that in this implementation)
        for(size_t j = 0; j < primTable.size(); ++j){
            if(!primTable[j].whetherVisited && primTable[j].d < minDistance){
                minIndex = j;
                minDistance = primTable[j].d;
            }
        }
        
        // if the remaining unvisited vertices are all in another area
        if(minIndex == primTable.size()){
            throw runtime_error("Cannot construct MST");
        }

        // add this vertex to MST
        primTable[minIndex].whetherVisited = true;
        totalWeight += minDistance;
        
        // after adding this new vertex, check if any other outie can update parent
        // to this vertex to get the smaller path to current MST than before
        for(unsigned j = 0; j < primTable.size(); ++j){
            // if this vertex is unvisited
            if(!primTable[j].whetherVisited){
                double newDistance = distance(toRealIndex[minIndex], toRealIndex[j]);
                if(newDistance < primTable[j].d){
                    primTable[j].d = newDistance;
                    primTable[j].p = toRealIndex[minIndex];
                }
            }
        }
    }
    if(whetherPrint){
        cout << totalWeight << '\n';
        for(unsigned i = 0; i < primTable.size(); ++i){
            if(primTable[i].p != static_cast<int>(toRealIndex[i])){
                if(primTable[i].p < static_cast<int>(toRealIndex[i])){
                    cout << primTable[i].p << ' ' << toRealIndex[i] << '\n';
                }
                else{
                    cout << toRealIndex[i] << ' ' << primTable[i].p << '\n';
                }   
            }
        }
    }
    return totalWeight;
}

template<typename FUNCTOR>
double Drone::runMST2(const vector<unsigned>& toRealIndex, FUNCTOR& distance, bool whetherPrint){
    size_t vertexNum = toRealIndex.size();
    // first initialize the representative table 
    vector<unsigned> representTable(vertexNum, 0);
    std::iota(representTable.begin(), representTable.end(), 0);
    
    vector<Edge> edgeTable;
    edgeTable.reserve(vertexNum * (vertexNum - 1) / 2);
    for(unsigned i = 0; i < toRealIndex.size(); ++i){
        for(unsigned j = i + 1; j < toRealIndex.size(); ++j){
            edgeTable.emplace_back(i, j, distance(toRealIndex[i], toRealIndex[j]));
        }
    }
    std::sort(edgeTable.begin(), edgeTable.end(), Comp());

    unsigned edgeUsedCount{0};
    double totalLength{0};
    for(unsigned i = 0; i < edgeTable.size(); ++i){
        if(edgeTable[i].distance == numeric_limits<double>::infinity()){
            throw runtime_error("Cannot construct MST");
        }

        unsigned v1 = edgeTable[i].v1;
        unsigned v2 = edgeTable[i].v2;
        // find the ultimate representative for these two vertex
        unsigned represent1 = representTable[v1];
        unsigned represent2 = representTable[v2];
        while(represent1 != v1){
            v1 = represent1;
            represent1 = representTable[v1];
        }
        while(represent2 != v2){
            v2 = represent2;
            represent2 = representTable[v2];
        }

        // update representative
        unsigned temp = edgeTable[i].v1;
        while(representTable[temp] != represent1){
            unsigned tempOld = temp;
            temp = representTable[tempOld];
            representTable[tempOld] = represent1;
        }
        temp = edgeTable[i].v2;
        while(representTable[temp] != represent2){
            unsigned tempOld = temp;
            temp = representTable[tempOld];
            representTable[tempOld] = represent1;
        }
        // if these two vertices have same ultimate representative
        if(represent1 == represent2) continue;
        else{
            totalLength += edgeTable[i].distance;
            edgeTable[i].whetherUsed = true;
            representTable[represent2] = represent1;
            ++edgeUsedCount;
            if(edgeUsedCount == toRealIndex.size() - 1) break;
        }
    }
    
    if(whetherPrint){
        // print result
        cout << totalLength << '\n';
        unsigned printCount{0};
        for(const Edge& eachEdge: edgeTable){
            if(eachEdge.whetherUsed){
                if(toRealIndex[eachEdge.v1] < toRealIndex[eachEdge.v2]){
                    cout << toRealIndex[eachEdge.v1] << ' ' << toRealIndex[eachEdge.v2] << '\n';
                }
                else{
                    cout << toRealIndex[eachEdge.v2] << ' ' << toRealIndex[eachEdge.v1] << '\n';
                }
                ++printCount;
                if(printCount == edgeUsedCount) break;
            }
        }
    }
    
    return totalLength;
}

template<typename FUNCTOR>
double Drone::runFASTTSP1(FUNCTOR& distance, vector<unsigned>& travelOrder, bool whetherPrint){
    double totalDistance{0};
    // indicate whether each vertex is visited or not, index is same to locationTable
    vector<bool> visitedTable(this->locationTable.size(), false);
    travelOrder.clear();
    
    // first, add the start point 
    visitedTable[0] = true;
    travelOrder.push_back(0);
    unsigned currVertexIndex{0};

    for(size_t i = 1; i < visitedTable.size(); ++i){

        unsigned minIndex{0};
        double minDistance{numeric_limits<double>::infinity()};
        // find the smallest distance from all unvisited vertices to current vertex
        for(unsigned j = 0; j < visitedTable.size(); ++j){
            if(!visitedTable[j]){
                if(distance(currVertexIndex, j) < minDistance){
                    minIndex = j;
                    minDistance = distance(currVertexIndex, j);
                }
            }
        }

        // add this vertex to current TSP
        visitedTable[minIndex] = true;
        travelOrder.push_back(minIndex);

        // update current index
        currVertexIndex = minIndex;
    }
    // start 2-opt
    for(unsigned i = 0; i < travelOrder.size() - 2; ++i){
        for(unsigned j = i + 2; j < travelOrder.size() - 1; ++j){
            double currLength = distance(travelOrder[i], travelOrder[i + 1]) + 
                distance(travelOrder[j], travelOrder[j + 1]);
            double lengthAfterSwap = distance(travelOrder[i], travelOrder[j]) + 
                distance(travelOrder[i + 1], travelOrder[j + 1]);
            if(lengthAfterSwap < currLength){
                std::reverse(travelOrder.begin() + i + 1, travelOrder.begin() + j + 1);
            }
        }
        
        if(i != 0){
            unsigned j = travelOrder.size() - 1;
            double currLength = distance(travelOrder[i], travelOrder[i + 1]) + 
                distance(travelOrder[j], travelOrder[0]);
            double lengthAfterSwap = distance(travelOrder[i], travelOrder[j]) + 
                distance(travelOrder[i + 1], travelOrder[0]);
            if(lengthAfterSwap < currLength){
                std::reverse(travelOrder.begin() + i + 1, travelOrder.end());
            }
        }
    }
    for(unsigned i = 0; i < travelOrder.size() - 1; ++i){
        totalDistance += distance(travelOrder[i], travelOrder[i + 1]);
    }
    totalDistance += distance(travelOrder[travelOrder.size() - 1], travelOrder[0]);
    if(whetherPrint){
        cout << totalDistance << '\n';
        for(unsigned i : travelOrder){
            cout << i << ' ';
        }
        cout << endl;
    }
    
    return totalDistance;
}

void Drone::runOPTTSP(DWithMatrix& distance){
    // run FASTTSP to get the first bound
    this->currBound = this->runFASTTSP1(distance, this->path, false);
    this->finalPath = this->path;

    this->distancePtr = &distance;
    this->genPerms(1);
    this->distancePtr = nullptr;
    // print the final path
    cout << this->currBound << '\n';
    for(auto it = this->finalPath.begin(); it != this->finalPath.end(); ++it){
        cout << *it << ' ';
    }
    cout << endl;
}

void Drone::genPerms(size_t permLength) {
    if (permLength == this->path.size()) {
        // add the distance from end to start
        this->currPathLength += this->distancePtr->operator()(this->path[this->path.size() - 1], 0);
        // if we find a better path 
        if(this->currPathLength < this->currBound){
            this->currBound = this->currPathLength;
            this->finalPath = this->path;
        }
        this->currPathLength -= this->distancePtr->operator()(this->path[this->path.size() - 1], 0);
        return;
    }
    if (!this->promising(permLength))
        return;
    for (size_t i = permLength; i < this->path.size(); ++i) {
        // add the new vertex to current TSP
        swap(this->path[permLength], this->path[i]);
        this->currPathLength += this->distancePtr->operator()(this->path[permLength - 1], this->path[permLength]);
        this->genPerms(permLength + 1);
        this->currPathLength -= this->distancePtr->operator()(this->path[permLength - 1], this->path[permLength]);
        swap(this->path[permLength], this->path[i]);
    }
}

bool Drone::promising(size_t permLength){
    // if the number of remaining vertices is not so much, use MST to estimate the 
    // the lower bound will be slower than just try continue
    if((this->path.size() - permLength) <= 5) return true;
    
    // calculate the MST of the remaining vertices
    vector<unsigned> remain(this->path.begin() + permLength, this->path.end());
    double MSTlength = this->runMST1(remain, *(this->distancePtr), false);

    // calculate the distance from remaining MST to current TSP
    double armToStart{numeric_limits<double>::infinity()}, armToEnd{numeric_limits<double>::infinity()};
    for(vector<unsigned>::iterator it = remain.begin(); it != remain.end(); ++it){
        if(this->distancePtr->operator()(*it, 0) < armToStart){
            armToStart = this->distancePtr->operator()(*it, 0);
        }
        if(this->distancePtr->operator()(*it, this->path[permLength - 1]) < armToEnd){
            armToEnd = this->distancePtr->operator()(*it, this->path[permLength - 1]);
        }
    }
    if(MSTlength + armToEnd + armToStart + this->currPathLength > this->currBound) return false;
    else return true;
}
