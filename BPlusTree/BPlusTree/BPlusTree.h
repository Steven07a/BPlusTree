#ifndef BPlusTree_H_
#define BPlusTree_H_

#include <iostream>
#include <iomanip>
#include <assert.h>
#include "C:\Users\Steven_dev\Desktop\CS_Stuff\BTree-Functions\BTree-Functions\BTree-Functions\bTreeFunctions.h"

using namespace std;

template <class T>
class BPlusTree
{
public:
    class Iterator {
    public:
        friend class BPlusTree;
        Iterator(BPlusTree<T>* _it = NULL, int _key_ptr = 0) :it(_it), key_ptr(_key_ptr) {}

        T operator *() {
            assert(key_ptr < it->data_count);
            return it->data[key_ptr];
        }

        Iterator operator++(int un_used) {
            key_ptr++;
            if (key_ptr == it->data_count) {
                it = it->next;
                key_ptr = 0;
            }
            return *this;
        }

        Iterator operator++() {
            key_ptr++;
            if (key_ptr == it->data_count) {
                it = it->next;
                key_ptr = 0;
            }
            return *this;
        }
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs) {
            if (&lhs.it == &rhs.it && lhs.key_ptr == rhs.key_ptr) {
                return true;
            } else {
                return false;
            }
        }

        friend bool operator !=(const Iterator& lhs, const Iterator& rhs) {
            if (lhs.it != rhs.it) {
                return true;
            } else {
                return false;
            }
        }
        void print_Iterator() {
            cout << "Iterator is pointing at" 
                << it->data[key_ptr] << "\t key_ptr " << key_ptr << endl;
        }
        bool is_null() { return !it; }
    private:
        BPlusTree<T>* it;
        int key_ptr;
    };

    
    BPlusTree(bool dups = false) {
        dups_ok = dups;
        child_count = 0;
        data_count = 0;
        numOfDataElements = 0;
        for (int i = 0; i < MAXIMUM + 1; i++) {
            data[i] = T();
        }

        for (int i = 0; i < MAXIMUM + 2; i++) {
            subset[i] = NULL;
        }
        next = NULL;
    }
    //big three:
    BPlusTree(const BPlusTree<T>& other);
    ~BPlusTree() {
        clear_tree();
    }
    BPlusTree<T>& operator =(const BPlusTree<T>& RHS) {
        this->copy_tree(RHS);
        return *this;
    }

    bool insert(const T& entry);                //insert entry into the tree
    bool remove(const T& entry);                //remove entry from the tree

    void clear_tree();                          //clear this object (delete all nodes etc.)
    void copy_tree(const BPlusTree<T>& other);  //copy other into this object

    bool contains(const T& entry) const;        //true if entry can be found in the array
    T& get(const T& entry);                     //return a reference to entry in the tree
    T* find(const T& entry) const;              //return a pointer to this key. NULL if not there.
    T& get_existing(T item);                    //asserts something exist
    
    Iterator find_Iterator(const T& entry) {
        Iterator temp = begin();
        while (temp != end()) {
            if (*temp == entry) {
                return temp;
            } else {
                temp++;
            }
        }
        return NULL;
    }

    int size() const;                           //count the number of elements in the tree
    bool empty() const;                         //true if the tree is empty

    void print_tree(int level = 0, ostream& outs = cout) const; //print a readable version of the tree
    friend ostream& operator<<(ostream& outs, const BPlusTree<T>& print_me) {
        print_me.print_tree(0, outs);
        return outs;
    }

    Iterator begin() {
        if (this == NULL) {
            return NULL;
        }
        if (is_leaf()) {
            Iterator e;
            e = this;
            for (int i = 0; i < this->data_count; i++) {
                e++;
            }
            return e;
        } else {
            return this->subset[0]->begin();
        }
    }
    
    Iterator end() {
        return Iterator(NULL);
        /*if (this == NULL) {
            return NULL;
        }
        if (is_leaf() && this->next == NULL) {
            Iterator e;
            e = this;
            
            return e;
        } else {
            return this->subset[child_count - 1]->end();
        }*/
    }


private:
    static const int MINIMUM = 1;
    static const int MAXIMUM = 2 * MINIMUM;
    BPlusTree<T>* next;

    bool dups_ok;                                   //true if duplicate keys may be inserted
    int data_count;                                 //number of data elements
    T data[MAXIMUM + 1];                            //holds the keys
    int child_count;                                //number of children
    BPlusTree* subset[MAXIMUM + 2];                     //subtrees
    int numOfDataElements;

    bool is_leaf() const { return child_count == 0; }   //true if this is a leaf node

    //insert element functions
    bool loose_insert(const T& entry);              //allows MAXIMUM+1 data elements in the root
    void fix_excess(int i);                         //fix excess of data elements in child i

    //remove element functions:
    bool loose_remove(const T& entry);              //allows MINIMUM-1 data elements in the root
    void fix_shortage(int i);                       //fix shortage of data elements in child i

    void remove_biggest(T& entry);                  //remove the biggest child of this tree->entry
    void transfer_left(int i);                        //transfer one element LEFT from child i
    void transfer_right(int i);                       //transfer one element RIGHT from child i
    void merge_with_next_subset(int i);             //merge subset i with subset i+1
    void get_smallest(T& entry);                    //will get the smallest item within that tree
    void rotate_left(int i);
    void rotate_right(int i);
    BPlusTree<T>& get_biggest_BPlusTree();
    BPlusTree<T>& get_smallest_BPlusTree();
    void fix_next_pointers();
};

template <typename T>
T* BPlusTree<T>::find(const T& entry) const {
    int i = first_ge(data, data_count, entry);
    bool found = false;

    if (data[i] == entry && is_leaf() && i < data_count) {
        found = true;
    }

    if (found) {
        return const_cast<T*>(&data[i]);
    } else if (data[i] == entry && i < data_count) {
        //this else if can only happen if the item is found and were not in a leaf 
        //at which point we need to go to subset[i+1] to find the item
        assert(subset[i + 1] != NULL);
        return subset[i+1]->find(entry);
    } else if (subset[i] != NULL) {
        return subset[i]->find(entry);
    } else {
        return NULL;
    }
}

template <typename T>
void BPlusTree<T>::fix_excess(int i) {
    BPlusTree<T>* temp = new BPlusTree<T>;
    insert_item(subset, i + 1, child_count, temp);
    T entry;
    detach_item(subset[i]->data, subset[i]->data_count, entry);
    subset[i + 1]->insert(entry);
    
    // checks if our child is null if it is then set it
    if (subset[i]->subset[i] != NULL) {
        if (subset[i + 1]->subset[i + 1] == NULL) {
            subset[i + 1]->subset[i + 1] = new BPlusTree<T>;
        }
        /* splits our subtree and gives its children to a new subtree
        the part in parenthesies is what gets split
        ex:                                           20
                    5   20  40            ->       5       40
                  4  10  30  50 60              4   10  (30   50 60)
        */
        split(
            subset[i]->subset,
            subset[i]->child_count,
            subset[i + 1]->subset,
            subset[i + 1]->child_count
        );

        if (subset[i]->subset[0]->is_leaf()) {
            subset[i]->subset[subset[i]->child_count - 1]->next =
                subset[i + 1]->subset[0];
            //subset[i+1]
        }

    }
    
    detach_item(subset[i]->data, subset[i]->data_count, entry);
    ordered_insert(data, data_count, entry);
    //checks if the thing under us is a child if it is then we need to set his next and 
    //previous pointers
    if (subset[i]->is_leaf()) {
        for (int i = 0; i < child_count; i++) {
            if (i == 0) {
                subset[i]->next = subset[i + 1];
            } else if (i < child_count) {
                subset[i]->next = subset[i + 1];
            } else {
                subset[i]->next = NULL;
            }
        }
    } else {
        //if he is not a child then we clear up his next and previous pointers as he 
        //may have been a child at some point
        for (int i = 0; i < child_count; i++) {
            subset[i]->next = NULL;
        }
    }

    if (subset[i + 1]->is_leaf()) {
        ordered_insert(subset[i+1]->data, subset[i+1]->data_count, entry);
    }
}

template <typename T>
void BPlusTree<T>::print_tree(int level, ostream& outs) const {
    if (this != NULL) {
        for (int i = data_count; i > 0; i--) {
            subset[i]->print_tree(level + 1);
            outs << setw(4 * level) << "" << data[i - 1] << endl;
        }
        subset[0]->print_tree(level + 1);
    }
}

template <typename T>
bool BPlusTree<T>::loose_insert(const T& entry) {
    int i = first_ge(data, data_count, entry);
    bool inserted;
    //checks if its a leaf then checks if dups are ok if they are then insert otherwise 
    //override the item and return false
    if (is_leaf()) {
        if (dups_ok) {
            ordered_insert(data, data_count, entry);
            inserted = true;
        } else {
            if (data[i] == entry) {
                T temp;
                insert_item(data, i, data_count, entry);
                delete_item(data, i + 1, data_count, temp);
                return false;
            } else {
                if (!contains(entry)) {
                    ordered_insert(data, data_count, entry);
                    inserted = true;
                }
            }
        }
    } else {
        //check if the child is null if it is make a new child then insert
        if (data[i] == entry) {
            T temp;
            //delete_item(data, i, data_count, temp);
            insert_item(data, i, data_count, entry);
            delete_item(data, i + 1, data_count, temp);
            return false;
        }
        if (subset[i] == NULL) {
            child_count++;
            subset[i] = new BPlusTree<T>;
        }
        inserted = subset[i]->loose_insert(entry);
    }
    //this is the parent checking to see if its child is fat 
    if (!is_leaf() && subset[i] != NULL) {
        if (subset[i]->data_count > MAXIMUM) {
            fix_excess(i);
        }
    }
    return inserted;
}

template <typename T>
void BPlusTree<T>::clear_tree() {
    //clears the tree by clearing its children first then clearing up
    if (this != NULL) {
        for (int i = data_count - 1; i >= 0; i--) {
            data[i] = T();
        }
        for (int i = child_count - 1; i >= 0; i--) {
            subset[i]->clear_tree();
            delete subset[i];
            subset[i] = NULL;
        }
        data_count = 0;
        child_count = 0;
        subset[0]->clear_tree();

    }
}

template <typename T>
bool BPlusTree<T>::insert(const T& entry) {
    int index = first_ge(data, data_count, entry);
    bool inserted;
    if (dups_ok) {
        inserted = loose_insert(entry);
    } else {
        if (!contains(entry)) {
            inserted = loose_insert(entry);
        } else {
            loose_insert(entry);
            inserted = false;
        }
    }
    if(!this->is_leaf())
    this->fix_next_pointers();
    //this is where we check if the root of the tree is fat if it is then fix it
    if (data_count > MAXIMUM) {
        int tempNumofDataElements = numOfDataElements;
        BPlusTree<T>* temp = new BPlusTree<T>;
        temp->subset[0] = new BPlusTree<T>;
        temp->subset[0]->copy_tree(*this);
        temp->child_count++;
        temp->fix_excess(0);
        this->copy_tree(*temp);
        this->fix_next_pointers();
        numOfDataElements = tempNumofDataElements;
    }
    if (inserted) {
        numOfDataElements++;
    }
    return inserted;
}

template <typename T>
void BPlusTree<T>::copy_tree(const BPlusTree<T>& other) {
    this->clear_tree();
    copy_array(data, other.data, data_count, other.data_count);
    this->child_count = other.child_count;
    this->data_count = other.data_count;
    this->numOfDataElements = other.numOfDataElements;
    if (other.child_count > 0) {
        for (int i = 0; i < other.child_count; i++) {
            subset[i] = new BPlusTree<T>;
            subset[i]->copy_tree(*other.subset[i]);
        }
    }
    //this copies the pointers we put it here as this is when the children are 
    //already made 
    if (subset[0] != NULL && subset[0]->is_leaf()) {
        for (int i = 0; i < child_count; i++) {
            if (i == 0) {
                this->subset[i]->next = subset[i + 1];
            } else if (i < child_count) {
                this->subset[i]->next = subset[i + 1];
            } else {
                this->subset[i]->next = NULL;
            }
        }
    }
    if (!this->is_leaf()) {
        if (subset[0]->child_count > 1 && subset[0]->subset[0]->is_leaf() && subset[0] != NULL) {
            for (int i = 0; i < child_count-1; i++) {
                subset[i]->subset[subset[i]->child_count - 1]->next
                    = subset[i + 1]->subset[0];
            }
        }
    }
    
}

template <typename T>
void BPlusTree<T>::fix_next_pointers() {
    if (this->subset[0]->is_leaf()) {
        for (int i = 0; i < child_count; i++) {
            if (i == 0) {
                this->subset[i]->next = subset[i + 1];
            } else if (i < child_count) {
                this->subset[i]->next = subset[i + 1];
            } else {
                this->subset[i]->next = NULL;
            }
        }
    } else {
        for (int i = 0; i < child_count; i++) {
            this->subset[i]->fix_next_pointers();
            if (!this->subset[0]->is_leaf()) {
                if (i == 0) {
                    BPlusTree<T>* LHS;
                    LHS = &this->subset[i]->get_biggest_BPlusTree();
                    BPlusTree<T>* RHS;
                    RHS = &this->subset[i+1]->get_smallest_BPlusTree();
                    LHS->next = RHS;
                } else if (i > 0) {
                    BPlusTree<T>* LHS; 
                    LHS = &this->subset[i - 1]->get_biggest_BPlusTree();
                    BPlusTree<T>* RHS; 
                    RHS = &this->subset[i]->get_smallest_BPlusTree();
                    LHS->next = RHS;
                }
            }
        }
    }
}

template<typename T>
BPlusTree<T>& BPlusTree<T>::get_biggest_BPlusTree() {
    if (this->is_leaf()) {
        return *this;
    } else {
        return this->subset[child_count - 1]->get_biggest_BPlusTree();
    }
}

template <typename T>
BPlusTree<T>& BPlusTree<T>::get_smallest_BPlusTree() {
    if (this->is_leaf()) {
        return *this;
    } else {
        return this->subset[0]->get_smallest_BPlusTree();
    }
}

template<typename T>
BPlusTree<T>::BPlusTree(const BPlusTree<T>& other) {
    dups_ok = false;
    child_count = 0;
    data_count = 0;
    numOfDataElements = 0;
    for (int i = 0; i < MAXIMUM + 1; i++) {
        data[i] = T();
    }

    for (int i = 0; i < MAXIMUM + 2; i++) {
        subset[i] = NULL;
    }
    next = NULL;
    if (&other != NULL) {
        this->copy_tree(other);
    }
}

template<typename T>
bool BPlusTree<T>::contains(const T& entry) const {
    T* temp = NULL;
    temp = find(entry);
    return temp == NULL ? false : true;
}

template <typename T>
T& BPlusTree<T>::get(const T& entry) {
    T* temp = NULL;
    temp = find(entry);
    if (temp == NULL) {
        this->insert(entry);
        temp = this->find(entry);
    }
    return get_existing(entry);
}

template <typename T>
T& BPlusTree<T>::get_existing(T item) {
    T* temp = NULL;
    temp = find(item);
    assert(temp != NULL);
    return *temp;
}

template <typename T>
bool BPlusTree<T>::loose_remove(const T& entry) {
    int i = first_ge(data, data_count, entry);
    bool deleted = false;
    T temp = T();
    //if its a leaf we just delete the item
    if (is_leaf()) {
        if (data[i] == entry) {
            delete_item(data, i, data_count, temp);
            deleted = true;
        }
        //check if our item is in this current root if it is then delete it and replace it with
        //its right childs smallest item
    } else {
        if (data[i] == entry && i < data_count) {
            deleted = subset[i + 1]->loose_remove(entry);
            T temp;
            if (subset[i + 1] != NULL) {
                subset[i + 1]->get_smallest(temp);
                data[i] = temp;
            }
            if (!is_leaf() && subset[i + 1] != NULL) {
                if (subset[i + 1]->data_count < MINIMUM) {
                    fix_shortage(i + 1);
                }
            }
            
        } else {
            deleted = subset[i]->loose_remove(entry);
        }
    }
    //checks if we have a shortage and fixes it
    if (!is_leaf() && subset[i] != NULL) {
        if (subset[i]->data_count < MINIMUM) {
            fix_shortage(i);
        }
    }
    return deleted;
}

template <typename T>
void BPlusTree<T>::remove_biggest(T& entry) {
    if (is_leaf()) {
        detach_item(data, data_count, entry);
    } else {
        subset[child_count - 1]->remove_biggest(entry);
    }
    if (!is_leaf() && subset[child_count - 1] != NULL) {
        if (subset[child_count - 1]->data_count < MINIMUM) {
            fix_shortage(child_count - 1);
        }
    }
}

template <typename T>
void BPlusTree<T>::merge_with_next_subset(int i) {
    // merges the data 
    merge(
        subset[i]->data,
        subset[i]->data_count,
        subset[i + 1]->data,
        subset[i + 1]->data_count
    );
    if (child_count > 1) {
        child_count--;
    }
    T item = T();
    if (subset[i]->subset[i] != NULL) {
        detach_item(subset[i]->subset[i]->data, subset[i]->subset[i]->data_count, item);
        //subset[i]->subset[i]->child_count--;
    }
    
    //merges the subsets of that data
    merge(
        subset[i]->subset,
        subset[i]->child_count,
        subset[i + 1]->subset,
        subset[i + 1]->child_count
    );
    if (subset[i]->subset[i] != NULL && item != T()) {
        ordered_insert(subset[i]->subset[i]->data, subset[i]->subset[i]->data_count, item);
    }
    //moves down the parent to the children
    if (!subset[i]->is_leaf()) {
        T temp = T();
        delete_item(data, i, data_count, temp);
        data_count++;
        ordered_insert(subset[i]->data, subset[i]->data_count, temp);
    }
    //this can only happen in a case where we merge and subset i+2 has an
    //item and subset i+1 does not
    if (subset[i + 1] != NULL && subset[i + 2] != NULL) {
        if (subset[i + 1]->data_count == 0 && subset[i + 2]->data_count > 0) {
            swaps(subset[i + 1], subset[i + 2]);
            subset[i]->next = subset[i + 1];
            subset[i + 1]->next = NULL;
        }
    }
}

template <typename T>
void BPlusTree<T>::transfer_right(int i) {
    T item;
    //takes the largest data out of subset[i] then inserts it into roots data
    //we then take the next largest item out of data and move that to subset[i+1]
    detach_item(subset[i]->data, subset[i]->data_count, item);
    ordered_insert(subset[i + 1]->data, subset[i + 1]->data_count, item);
    data[i] = item;
}

template <typename T>
void BPlusTree<T>::fix_shortage(int i) {
    //fixes shortage for begining
    if (i == 0) {
        if (subset[i + 1]->data_count > MINIMUM && subset[i+1]->is_leaf()) {
            transfer_left(i + 1);
        } else if (subset[i+1]->data_count > MINIMUM) {
            rotate_left(i + 1);
        } else {
            T item;
            merge_with_next_subset(i);
            delete_item(data, i, data_count, item);
        }
        //takes care of shortage for the middle
    } else if (i < child_count - 1) {
        if (subset[i + 1]->data_count > MINIMUM && subset[i+1]->is_leaf()) {
            transfer_left(i + 1);
        } else if (subset[i + 1]->data_count > MINIMUM) {
            rotate_left(i + 1);
        } else if (subset[i - 1]->data_count > MINIMUM && subset[i - 1]->is_leaf()) {
            transfer_right(i - 1);
        } else if (subset[i - 1]->data_count > MINIMUM) {
            rotate_right(i - 1);
        } else {
            T item;
            merge_with_next_subset(i-1);
            delete_item(data, i - 1, data_count, item);
        }
        //takes care of shortage for the end
    } else {
        if (subset[i - 1]->data_count > MINIMUM && subset[i - 1]->is_leaf()) {
            transfer_right(i - 1);
        } else if (subset[i - 1]->data_count > MINIMUM) {
            rotate_right(i - 1);
        } else {
            T item;
            merge_with_next_subset(i - 1);
            delete_item(data, i - 1, data_count, item);
        }
    }

}

template <typename T>
void BPlusTree<T>::transfer_left(int i) {
    int num = 0;
    T item;
    ////takes the smallest data out of subset[i] then inserts it into roots data
    ////we then take the next smalled item out of data and move that to subset[i-1]
    //delete_item(subset[i]->data, num, subset[i]->data_count, item);
    //ordered_insert(data, data_count, item);
    //delete_item(data, i - 1, data_count, item);
    //ordered_insert(subset[i - 1]->data, subset[i - 1]->data_count, item);


    delete_item(subset[i]->data, num, subset[i]->data_count, item);
    ordered_insert(subset[i-1]->data, subset[i-1]->data_count, item);

    //BPlusTree<T>* btp;
    ////moves over any of the subsets that subset[i] may have had
    //if (subset[i]->child_count > 0 && !subset[i]->is_leaf()) {
    //    delete_item(subset[i]->subset, num, subset[i]->child_count, btp);
    //    if (subset[i - 1] == NULL) {
    //        subset[i - 1] = new BPlusTree<T>;
    //    }
    //    insert_item(
    //        subset[i - 1]->subset,
    //        subset[i - 1]->child_count,
    //        subset[i - 1]->child_count,
    //        btp
    //    );
    //}

    if (subset[1] != NULL) {
        data[0] = subset[1]->data[0];
    }
    if (subset[2] != NULL) {
        data[1] = subset[2]->data[0];
    }
}

template <typename T>
bool BPlusTree<T>::remove(const T& entry) {
    bool deleted = loose_remove(entry);
    //this checks if we the root are empty if we are then we need to make subset[0]
    //the new root
    if (data_count == 0 && child_count == 1) {
        BPlusTree<T>* temp = subset[0];
        copy_array(data, temp->data, data_count, temp->data_count);
        cleanArr(subset, child_count);
        copy_array(subset, temp->subset, child_count, temp->child_count);
        cleanArr(temp->subset, temp->child_count);
        delete temp;
    }
    if (deleted) {
        numOfDataElements--;
    }
    return deleted;
}

template<typename T>
int BPlusTree<T>::size() const {
    return numOfDataElements;
}

template <typename T>
bool BPlusTree<T>::empty() const {
    return numOfDataElements;
}
template <typename T>
void BPlusTree<T>::get_smallest(T& entry) {
    if (!is_leaf()) {
        subset[0]->get_smallest(entry);
    } else {
        entry = data[0];
    }
}

template <typename T>
void BPlusTree<T>::rotate_right(int i) {
    T item;
    //takes the largest data out of subset[i] then inserts it into roots data
    //we then take the next largest item out of data and move that to subset[i+1]
    detach_item(subset[i]->data, subset[i]->data_count, item);
    ordered_insert(data, data_count, item);
    delete_item(data, i + 1, data_count, item);
    ordered_insert(subset[i + 1]->data, subset[i + 1]->data_count, item);

    BPlusTree<T>* btp;
    //moves over any of the subsets that subset[i] may have had
    if (subset[i]->child_count > 0) {
        detach_item(subset[i]->subset, subset[i]->child_count, btp);
        insert_item(subset[i + 1]->subset, 0, subset[i + 1]->child_count, btp);
    }
}

template <typename T>
void BPlusTree<T>::rotate_left(int i) {
    int num = 0;
    T item;
    //takes the smallest data out of subset[i] then inserts it into roots data
    //we then take the next smalled item out of data and move that to subset[i-1]
    delete_item(subset[i]->data, num, subset[i]->data_count, item);
    ordered_insert(data, data_count, item);
    delete_item(data, i - 1, data_count, item);
    ordered_insert(subset[i - 1]->data, subset[i - 1]->data_count, item);

    BPlusTree<T>* btp;
    //moves over any of the subsets that subset[i] may have had
    if (subset[i]->child_count > 0) {
        delete_item(subset[i]->subset, num, subset[i]->child_count, btp);
        if (subset[i - 1] == NULL) {
            subset[i - 1] = new BPlusTree<T>;
        }
        insert_item(
            subset[i - 1]->subset,
            subset[i - 1]->child_count,
            subset[i - 1]->child_count,
            btp
        );
    }
}

#endif // !BPlusTree_H_