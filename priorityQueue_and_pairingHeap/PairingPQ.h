// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, previous{nullptr}
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE  elt;
            Node* child;
            Node* sibling;
            Node* previous;
        
    }; // Node

    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, 
        root{nullptr}, 
        pqSize{0} {}


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()):
        BaseClass{ comp }, root{nullptr}, pqSize{0} {
        while(start != end){
            this->addNode(*start);
            ++start;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other): BaseClass{ other.compare }, root{nullptr}, pqSize{0}{
        if(other.size() == 0) return;
        else{
            std::deque<Node*> table;
            table.push_back(other.root);
            while(!table.empty()){
                Node* currNode = table.front();
                table.pop_front();
                if(currNode->child != nullptr) table.push_back(currNode->child);
                if(currNode->sibling != nullptr) table.push_back(currNode->sibling);
                this->addNode(currNode->elt);
            } 
        }
    } // PairingPQ()

    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ& operator=(const PairingPQ& rhs) {
        PairingPQ<TYPE, COMP_FUNCTOR> temp(rhs);
        std::swap(this->root, temp.root);
        std::swap(this->pqSize, temp.pqSize);
        std::swap(this->compare, temp.compare);
        return *this;
    } // operator=()

    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() override{
        if(this->pqSize == 0) return;
        else{
            std::deque<Node*> table;
            table.push_back(this->root);
            while(!table.empty()){
                Node* currNode = table.front();
                table.pop_front();
                if(currNode->child != nullptr) table.push_back(currNode->child);
                if(currNode->sibling != nullptr) table.push_back(currNode->sibling);
                delete currNode;
            }
        }
    } // ~PairingPQ()

    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    void updatePriorities() override {
        if(this->pqSize == 1 || this->pqSize == 0) return;
        std::deque<Node*> table;
        table.push_back(this->root);
        this->root = nullptr;
        this->pqSize = 0;
        while(!table.empty()){
            Node* currNode = table.front();
            table.pop_front();
            if(currNode->sibling != nullptr) table.push_back(currNode->sibling);
            if(currNode->child != nullptr) table.push_back(currNode->child);
            
            this->addNode(currNode->elt);
            delete currNode; currNode = nullptr;
        }
    } // updatePriorities()

    // MODIFY: insert the new element and update the size
    void push(const TYPE& val) override{
        this->addNode(val);
    } // push()

    // MODIFY: pop the root and meld all children, update pqsize also
    // Runtime: Amortized O(log(n))
    void pop() override {
        if(this->pqSize == 1){
            delete this->root;
            this->root = nullptr;
            this->pqSize--;
        }
        // if the pq have at least two nodes
        else{
            std::deque<Node*> table;
            Node* currChild = this->root->child;
            delete this->root; this->root = nullptr;
            this->pqSize--;
            while(currChild->sibling != nullptr){
                currChild = currChild->sibling;
                currChild->previous->sibling = nullptr;
                currChild->previous->previous = nullptr;
                table.push_back(currChild->previous);
            }
            currChild->previous = nullptr;
            table.push_back(currChild); currChild = nullptr;
            while(table.size() > 1){
                Node* subRoot1 = table.front();
                table.pop_front();
                Node* subRoot2 = table.front();
                table.pop_front();
                table.push_back(this->meld(subRoot1, subRoot2));
            }
            this->root = table.front();
            table.pop_front();
        }
    } // pop()

    const TYPE& top() const override {
        return this->root->elt;
    } // top()

    // Runtime: O(1)
    std::size_t size() const override{
        return this->pqSize;
    } // size()

    // Runtime: O(1)
    bool empty() const override {
        if(this->pqSize == 0) return true;
        else return false;
    } // empty()

    // REQUIRE: new value is larger than old value, node must points to any node in this pq
    // MODIFY: make this priority queue valid again
    // EFFECT: replace the old value referenced by node with new_value, then extract this node
    //         and meld this node with the root
    void updateElt(Node* node, const TYPE & new_value) {
        if(node == this->root) return;
        else{
            node->elt = new_value;
            // if we deal with the leftmost node
            if(node->previous->child == node){
                node->previous->child = node->sibling;
                // if the sibling of this node is not nullptr
                if(node->sibling != nullptr){
                    node->sibling->previous = node->previous;
                }
                node->previous = nullptr;
                node->sibling = nullptr;
                this->root = this->meld(node, this->root); node = nullptr;
            }
            // if we deal with the sibling node
            else{
                node->previous->sibling = node->sibling;
                // if the sibling of this node is not nullptr
                if(node->sibling != nullptr){
                    node->sibling->previous = node->previous;
                }
                node->previous = nullptr;
                node->sibling = nullptr;
                this->root = this->meld(node, this->root); node = nullptr;
            }
        }
    } // updateElt()


    // MODIFY: update the root and pqSize
    // EFFECT: return the pointer that points to new added node
    Node* addNode(const TYPE& val) {
        if(this->pqSize == 0){
            this->root = new Node(val);
            this->pqSize++;
            return this->root;
        }
        else{
            Node* temp = new Node(val);
            this->root = this->meld(temp, this->root); 
            this->pqSize++;
            return temp;
        }
    } // addNode()

private:
    Node* root;
    std::size_t pqSize;
    // COMP_FUNCTOR compare


    // REQUIRE: parameters's sibling and previos must be nullptr, root can't be nullptr
    //          also, must point the original two roots to nullptr after call this function
    // EFFECT: meld two pairing heap, the smaller root will be the leftmost child of the larger one
    //         return a new root pointer
    Node* meld(Node* root1, Node* root2){
        // if the root2 is smaller than root1
        if(this->compare(root2->elt, root1->elt)){
            return this->meld(root2, root1);
        }
        // if the root2 is larger than or equal to root1
        else{
            // if root2 have no children
            if(root2->child == nullptr){
                root2->child = root1;
                root1->previous = root2;
                return root2;
            }
            // if root2 have children
            else{
                root1->previous = root2;
                root1->sibling = root2->child;
                root2->child->previous = root1;
                root2->child = root1;
                return root2;
            }
        }
    }

};


#endif // PAIRINGPQ_H
