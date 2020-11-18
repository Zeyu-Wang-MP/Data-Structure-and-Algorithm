#ifndef _HASH_TABLE_
#define _HASH_TABLE_

#include <functional> // for hash
#include <vector>
#include <utility> // for move
#include <cstdint>
#include <iostream>

// open address hashtable
// REQUIRE: key and value must have default ctor, copy ctor, operator =
template<typename KeyType, typename ValType, typename Hasher = std::hash<KeyType>, typename Pred = std::equal_to<KeyType>>
class HashTable{
private:
    enum class BucketStatus: uint8_t{Empty, Occupied, Deleted};
    class Bucket{
    private:
        BucketStatus status;
        KeyType key;
        ValType val;
    public:
        // use default ctors of key and val, can not create Bucket outside the hastTable class
        Bucket():status(BucketStatus::Empty){}
        const KeyType& getKey(){return this->key;}
        ValType& getVal(){return this->val;}

    friend HashTable;
    };

    size_t elementNum;
    size_t deletedNum;
    std::vector<Bucket> table;

    Hasher hasher;
    Pred comp;
    
    //MODIFY: update table, set deletedNum to 0
    void growAndRehash(){
        std::cout << "rehash\n";
        std::vector<Bucket> oldTable(this->table);
        this->table.clear();
        this->table.resize(2 * oldTable.size());
        // for each Bucket in old table
        for(size_t i = 0; i < oldTable.size(); ++i){
            // rehash only for occupied bucket
            if(oldTable[i].status == BucketStatus::Occupied){
                size_t newHash = this->hasher(oldTable[i].key);
                // when rehashing, there must isn't any duplicate key
                // for each linear probing
                for(size_t j = 0; j < this->table.size(); ++j){
                    size_t newLocation = (newHash + j) % this->table.size();
                    if(this->table[newLocation].status == BucketStatus::Empty){
                        this->table[newLocation].status = BucketStatus::Occupied;
                        this->table[newLocation].key = std::move(oldTable[i].key);
                        this->table[newLocation].val = std::move(oldTable[i].val);
                        break;
                    }
                    else continue;
                }
            }
        }
        this->deletedNum = 0;
    }

public:
    // default ctor
    HashTable(): elementNum{0}, deletedNum{0}{
        this->table.resize(10);
    }
    
    // EFFECT: return a pointer to bucket if found, return nullptr if not found
    Bucket* find(const KeyType& key){
        size_t originalHash = this->hasher(key);
        // if this location is empty
        if(this->table[originalHash % this->table.size()].status == BucketStatus::Empty){
            return nullptr;
        }
        // else, we need to check until we find the target or we find the next empty bucket
        else{
            for(unsigned int i = 0; i < this->table.size(); ++i){
                size_t newLocation = (originalHash + i) % this->table.size();
                BucketStatus currStatus = this->table[newLocation].status;
                // if this bucket is deleted
                if(currStatus == BucketStatus::Deleted){
                    continue;
                }
                else if(currStatus == BucketStatus::Occupied){
                    if(this->comp(key, this->table[newLocation].key)){
                        return &(this->table[newLocation]);
                    }
                    else continue;
                }
                // if this bucket if empty
                else return nullptr;
            }
            // if we loop through the table and no returns(indicates there's too much deleted buckets)
            // since the occupied bucket number can't be larger than half of the table size
            return nullptr;
        }
    }
 
    // MODIFY: update table and elementNum if key is not in the table, also update 
    //         deletedNum if the inserted pair occupy the deleted bucket
    // EFFECT: if the table has the given key, return a reference to the corresponding value
    //         else insert a new pair with this key using default ctor for value
    //         if current size is eqaul to half of the table size
    //         double the table and rehash each item
    ValType& operator[](const KeyType& key){
        if(this->elementNum >= this->table.size() / 2){
            this->growAndRehash();
        }

        Bucket* result = this->find(key);
        // if the table has the given key
        if(result != nullptr) return result->val;
        
        // if we need to insert a new pair
        else{
            size_t originalHash = this->hasher(key);
            for(size_t i = 0; i < this->table.size(); ++i){
                size_t newLocation = (originalHash + i) % this->table.size();
                // if this location is occupied
                if(this->table[newLocation].status == BucketStatus::Occupied) continue;
                else{
                    if(this->table[newLocation].status == BucketStatus::Deleted) --this->deletedNum;
                    ++this->elementNum;
                    this->table[newLocation].status = BucketStatus::Occupied;
                    this->table[newLocation].key = key;
                    return this->table[newLocation].val;
                }
            }
            return this->find(key)->val;
        } 
    }
    
    size_t size() const{return this->elementNum;}
    
    // MODIFY: if remove a pair, update the table, elementNum, deletedNum
    // EFFECT: if key exists, erase this pair, return 1, otherwise do nothing and return 0
    //         rehash if deletedNum is equal to or larger than half of the table size
    size_t erase(const KeyType& key){
        Bucket* target = this->find(key);
        if(target == nullptr) return 0;
        else{
            --this->elementNum;
            ++this->deletedNum;
            target->status = BucketStatus::Deleted;
            target->key = KeyType();
            target->val = ValType();
            if(this->deletedNum >= this->table.size() / 2){
                this->growAndRehash();
            }
            return 1;
        }
    }
};
   



#endif