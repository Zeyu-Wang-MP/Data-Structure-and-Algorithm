#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

class SortByIndex{
private:
    const vector<int>& _originalVec;
public:
    SortByIndex(const vector<int>& originalVec):_originalVec(originalVec){}
    //primitive type should pass by value
    bool operator()(unsigned int lhs, unsigned int rhs) const{
        return this->_originalVec[lhs] < this->_originalVec[rhs];
    }
};

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& rhs){
    for(const T& eachElement: rhs){
        os << eachElement << "  ";
    }
    return os;
}


int main(){
    vector<int> original(5);
    vector<unsigned int> index(5);
    iota(original.rbegin(), original.rend(), 2);
    iota(index.begin(), index.end(), 0);
    SortByIndex comp(original);
    sort(index.begin(), index.end(), comp);
    cout << original << '\n';
    cout << index << endl;
}