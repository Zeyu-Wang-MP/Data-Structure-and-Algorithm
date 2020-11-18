#include "PairingPQ.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <queue>


using namespace std;



int main(){
    vector<int> a{15,65,852,412,647,9};
    PairingPQ<int> test(a.rbegin(), a.rend());
    assert(test.size() == 6 && test.top() == 852);
    test.pop();
    assert(test.top() == 647);
    test.pop();
    assert(test.top() == 412);
    test.push(85412);
    assert(test.top() == 85412);
    
    
}