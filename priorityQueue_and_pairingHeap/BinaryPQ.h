// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include "Eecs281PQ.h"

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
    } // BinaryPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, data(start, end) {
        this->updatePriorities();
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automaticslly.
    ~BinaryPQ() override {
    } // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    void updatePriorities() override{
        // start from the first internal node
        size_t i = this->data.size() / 2;
        while(i >= 1){
            this->fixDown(i);
            --i;
        }
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    void push(const TYPE & val) override{
        this->data.push_back(val);
        this->fixUp(this->data.size());
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    void pop() override {
        this->swapByIndex(1, this->data.size());
        this->data.pop_back();
        this->fixDown(1);
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    const TYPE & top() const override{
        return this->getByIndex(1);
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return this->data.size();
    } // size()


    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return this->data.empty();
    } // empty()


private:
    // Note: This vector *must* be used your heap implementation.
    std::vector<TYPE> data;
    // COMP_FUNCTOR comp; (base class)
    
    // REQUIRE: index >= 1 && index <= size of vector
    // EFFECT: get element reference by index
    TYPE& getByIndex(std::size_t index){
        return this->data[index - 1];
    }
    const TYPE& getByIndex(std::size_t index) const{
        return this->data[index - 1];
    }
    
    //REQUIRE:  1 <= index <= size of vector
    //MODIFY: swap the elements in specified index
    void swapByIndex(std::size_t index1, std::size_t index2){
        std::swap(this->data[index1 - 1], this->data[index2 - 1]);
    }
    
    // REQUIRE: index is location on where we change an element
    // MODIFY: fix up the data vector to build a valid heap
    void fixUp(std::size_t index){
        while(index != 1){
            // if its parent is smaller than it
            if(this->compare(this->getByIndex(index / 2), this->getByIndex(index))){
                this->swapByIndex(index, index/2);
            }
            else break;
            index = index / 2;
        }
    }
    
    void fixDown(std::size_t index){
        // until we reach the leaf node
        while(index <= this->data.size() / 2){
            size_t childIndex = index * 2;
            // if we have two child and the right child is larger than left child
            if(childIndex < this->data.size() && 
                this->compare(this->getByIndex(childIndex), this->getByIndex(childIndex + 1))){
                ++childIndex;
            }
            // if this element if smaller than his child
            if(this->compare(this->getByIndex(index), this->getByIndex(childIndex))){
                this->swapByIndex(index, childIndex);
            }
            else break;
            index = childIndex;
        }
    }
    
}; // BinaryPQ


#endif // BINARYPQ_H
