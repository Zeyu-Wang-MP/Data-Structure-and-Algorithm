#include"N_Queens.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;


int main(){
    cout << "input N: ";
    unsigned n{0};
    cin >> n;
    NQueen test(n);
    test.printSolution();
}