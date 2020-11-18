#include <utility> // for swap
#include <vector>
#include <functional> // for std::less
#include <iostream>
#include <algorithm>

using namespace std;

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& rhs){
    for(const T& eachElement: rhs){
        os << eachElement << "  ";
    }
    return os;
}

template<typename T, typename COMP_FUNCTOR = std::less<T>>
void bubbleSort(vector<T>& target){
    COMP_FUNCTOR comp;
    for(size_t i = 0; i < target.size(); ++i){
        bool swapped = false;
        for(size_t j = target.size() - 1; j > i; --j){
            if(comp(target[j], target[j - 1])){
                swap(target[j], target[j - 1]);
                swapped = true;
            }
        }
        if(!swapped) break;
    }
}

template<typename T, typename COMP_FUNCTOR = std::less<T>>
void selectionSort(vector<T> & target){
    COMP_FUNCTOR comp;
    for(size_t i = 0; i < target.size(); ++i){
        size_t min = i;//minimum index
        for(size_t j = i + 1; j < target.size(); ++j){
            if(comp(target[j], target[min])) min = j;
        }
        if(min != i) swap(target[i], target[min]);
    }
}

template<typename InputIterator, typename COMP_FUNCTOR>
void insertionSort(InputIterator begin, InputIterator end, COMP_FUNCTOR comp){
    // first swap the smallest element to the first 
    
    for(InputIterator i = end - 1; i != begin; --i){
        if(comp(*i, *(i - 1))) swap(*i, *(i - 1));
    }
    // now the first two element are sorted
    for(InputIterator i = begin + 2; i != end; ++i){
        // this is the element needed to insert to the already sorted arr
        auto currElement = *i;
        InputIterator j = i;
        while(comp(currElement, *(j - 1))){
            *j = *(j - 1);
            --j;
        }
        // j will stop at the location where currElement should be
        *j = currElement;
    }
}

template<typename InputIterator, typename COMP_FUNCTOR>
void quickSort(InputIterator begin, InputIterator end, COMP_FUNCTOR comp){
    // base case
    if(begin + 1 >= end) return;

    // start partition
    InputIterator pivot = end - 1;
    InputIterator left = begin;
    InputIterator right = end - 2;
    
    while(true){
        // move left until left >= pivot, that is swap candidate
        while(comp(*left, *pivot)) ++left;

        // move right until left >= right or right < pivot
        while(left < right && comp(*pivot, *right)) --right;

        if(left >= right) break;

        swap(*left, *right);
    }
    // after that, left will stay at the pivot or element larger than pivot
    swap(*left, *pivot);
    quickSort(begin, left, comp);
    quickSort(left + 1, end, comp);
}

template<typename InputIterator, typename COMP_FUNCTOR>
void mergeSort(InputIterator begin, InputIterator end, COMP_FUNCTOR comp){
    // base case, one or zero element
    if(begin + 1 >= end) return;
    
    // partition, left is always smaller than or equal to right
    InputIterator mid = begin + (end - begin) / 2;
    InputIterator left = begin;
    InputIterator right = mid;
    mergeSort(begin, mid, comp);
    mergeSort(mid, end, comp);

    // merge
    vector<decltype(*begin + 0)> temp(end - begin);
    for(auto it = temp.begin(); it != temp.end();){
        // if left is done
        if(left == mid) *(it++) = *(right++);
        // if right is done
        else if(right == end) *(it++) = *(left++);
        else{
            *(it++) = (comp(*left, *right)) ? *(left++) : *(right++);
        }
    }
    std::copy(temp.begin(), temp.end(), begin);
}

int main(){
    vector<int> a{1,23,4};
    mergeSort(a.begin(), a.end(), std::less<int>());
    cout << a;
}
