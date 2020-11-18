#ifndef AVL_H
#define AVL_H

#include <iostream>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm> // for max


class AVL {
private:
    struct Node {
        int datum;
        int height;
        Node* left;
        Node* right;
        int left_height() {
            return left ? left->height : -1;
        }
        int right_height() {
            return right ? right->height : -1;
        }
        int balance() {
            return left_height() - right_height();
        }
        // Whenever the height of its children change, call
        // this function to recalculate the height of this node,
        // the parent.
        void fix_height() {
            height = 1 + std::max(left_height(), right_height());
        }
    };

public:
    void insert(int datum){
        this->root = this->insert_node(root, datum);
    }

    const Node* search(int datum) const{
        // just a regular BST search
        return this->search_node(root, datum);
    }
    

    void erase(int datum){
        this->root = this->remove_node(this->root, datum);
    }

    ~AVL(){
        this->destroy_node(root);
    }

    void print_diagram() {
        struct pos {
            int depth;
            int parent_dir;
        };
        std::vector<std::pair<int, pos>> points;
        std::function<void(Node*, pos p)> traverse_depth = [&](Node* n, pos p) {
            if (n == nullptr) {
                return;
            }
            traverse_depth(n->left, { p.depth + 1, 1 });
            points.push_back(std::pair<int, pos>({ n->datum, p }));
            traverse_depth(n->right, { p.depth + 1, -1 });
        };
        traverse_depth(root, { 0, 0 });
        // points is now filled
        int width = 2 * (int)points.size();
        int height = 0;
        for (int i = 0; i < (int)points.size(); i++) {
            height = std::max(height, points[i].second.depth);
        }
        height *= 2;
        height++;
        // now, we can build the buffer:
        std::vector<std::vector<char>> buffer(width, std::vector<char>(height, ' '));
        // add the numbers
        for (int i = 0; i < (int)points.size(); i++) {
            int n = points[i].first;
            int d = points[i].second.depth;
            buffer[2 * i + 1][d * 2] = char((n % 10) + '0');
            if (n >= 10) {
                // note: will truncate 3+ digit numbers to their last 2 digits
                buffer[2 * i + 0][d * 2] = char((n / 10) % 10 + '0');
            }
        }
        // add the corner edges
        for (int i = 0; i < (int)points.size(); i++) {
            int d = points[i].second.depth;
            int dir = points[i].second.parent_dir;
            if (dir == 0) {
                continue; // root
            }
            if (points[i + dir].second.depth == d - 1) {
                // adjacent parent
                buffer[2 * i + (dir > 0 ? 2 : 0)][d * 2 - 1] = (dir > 0 ? '/' : '\\');
            }
            else {
                int c = 2 * i + (dir > 0 ? 2 : -1);
                buffer[c][d * 2 - 1] = (dir > 0 ? '/' : '\\');
                buffer[c + dir][d * 2 - 2] = '-';
                for (int j = i + 2 * dir; points[j].second.depth != d - 1; j += dir) {
                    buffer[2 * j][d * 2 - 2] = '-';
                    buffer[2 * j + 1][d * 2 - 2] = '-';
                }

            }
        }
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::cout << buffer[x][y];
            }
            std::cout << std::endl;
        }
    }

private:
    Node* root = nullptr;
    size_t size = 0;

    // insert_node returns the new root of this subtree after inserting datum.
    Node* insert_node(Node* node, int datum){
        if (node == nullptr) {
            ++this->size;
            // at a leaf position in the tree, so create a new node
            return new Node{ datum, 0, nullptr, nullptr }; // it has height 0
        }
        if (datum < node->datum) {
            node->left = insert_node(node->left, datum);
            node->fix_height(); // remember to fix the height of a node after modifying its children
            node = this->checkAndBalance(node);
        }
        else {
            node->right = insert_node(node->right, datum);
            node->fix_height(); // remember to fix the height of a node after modifying its children
            node = this->checkAndBalance(node);
        }
        return node;
    }
    
    // remove the node if exist and return the root of this subtree after removing
    Node* remove_node(Node* node, int datum){
        // if we can't find the specified node
        if(node == nullptr) return nullptr;
        else if(datum < node->datum){
            node->left = this->remove_node(node->left, datum);
            node->fix_height();
            return this->checkAndBalance(node);
        }
        else if(datum > node->datum){
            node->right = this->remove_node(node->right, datum);
            node->fix_height();
            return this->checkAndBalance(node);
        }
        // if we find the specified node
        else{
            --this->size;
            //if this node has no child
            if(node->height == 0){
                delete node;
                return nullptr;
            }  
            // if this node only has left child
            else if(node->height == 1 && node->right == nullptr){
                Node* newRoot = node->left;
                delete node;
                return newRoot;
            }
            // if this node only has right child
            else if(node->height == 1 && node->left == nullptr){
                Node* newRoot = node->right;
                delete node;
                return newRoot;
            }
            // if this node has two children
            else if(node->left != nullptr && node->right != nullptr){
                // first, we need to find the smallest one in right subtree
                Node* smallest = node->right;
                while(smallest->left != nullptr) smallest = smallest->left;
                // the smallest node must either have 1 child or no child
                
                // copy the data from smallest to the node we need to delete
                node->datum = smallest->datum;
                // now it's still balanced
                node->right = this->remove_node(node->right, smallest->datum);
                node->fix_height();
                node = this->checkAndBalance(node);
                return node;
            }
            return nullptr; // in order to compile, can't reach here
        }
    }

    // EFFECT: check balance of this node and balance if needed
    //         return the root of new balanced subtree, guarantee that
    //         the subtree's height is right
    Node* checkAndBalance(Node* node){
        // if the right subtree is too big
        if(node->balance() < -1){
            // if the node has zigzag
            if(node->right->balance() > 0){
                node->right = this->rotate_right(node->right);
            }
            return this->rotate_left(node);
        }
        // if the left subtree is too big
        else if(node->balance() > 1){
            //if the node has zigzag
            if(node->left->balance() < 0){
                node->left = this->rotate_left(node->left);
            }
            return this->rotate_right(node);
        }
        // if this node is balanced
        else return node;
    }

    // search_node searches for 'datum' in the subtree rooted at 'node'.
    // if the node cannot be found, it returns nullptr.
    const Node* search_node(const Node* node, int datum) const{
        if (node == nullptr) {
            return nullptr; // not found (no node here)
        }
        if (node->datum == datum) {
            return node;
        }
        if (datum < node->datum) {
            // left subtree, since smaller than current node
            return search_node(node->left, datum);
        }
        else {
            // right subtree, since larger than current node
            return search_node(node->right, datum);
        }
    }
    
    //REQUIRE: node must have right child
    //EFFECT: return the new 'root' of the rotated subtree, guarantee that
    //        the heights of new subtree is right
    Node* rotate_left(Node* node){
        Node* newRoot = node->right;
        node->right = newRoot->left;
        newRoot->left = node;
        // update height
        node->fix_height();
        newRoot->fix_height();
        return newRoot;
    }

    //REQUIRE: node must have left child
    //EFFECT: return the new 'root' of the rotated subtree, guarantee that
    //        the heights of new subtree is right
    Node* rotate_right(Node* node){
        Node* newRoot = node->left;
        node->left = newRoot->right;
        newRoot->right = node;
        //update height
        node->fix_height();
        newRoot->fix_height();
        return newRoot;
    }

    // used for dtor to destruct the tree
    void destroy_node(Node* node){
        if (node == nullptr) {
            return;
        }
        destroy_node(node->left);
        destroy_node(node->right);
        delete node;
    }

};

#endif /* AVL_LAB_H */
