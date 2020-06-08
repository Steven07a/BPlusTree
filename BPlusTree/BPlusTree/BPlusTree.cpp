/*
 * Author: Steven Herrera
 * Project: BPlusTree Class
 * Purpose: This is a binary tree which will can have multiple children
 *              it is still kept balanced and kept inorder like BTree but with additional functionality
 *
 * Notes: ran out of time had to implement a cheap way of fixing the next pointers inorder to work on SQL class
 */

#include <iostream>
#include <cstdlib>
#include "BPlusTree.h"
#include "C:\Users\Steven_dev\Desktop\CS_Stuff\BTree-Functions\BTree-Functions\BTree-Functions\bTreeFunctions.h"

using namespace std;

void test_bplustree_interactive();
int Random(int lo, int hi);
void copy_array(int* destPtr, int* srcPtr, int size);
void test_bplustree_insert_random(int howMany);
void test_insert_and_delete_random(int howMany, bool print_debug = false);
void test_iterator();

void shuffle(int a[], unsigned int  size);
int Random(int lo, int hi);

int main()
{

    //cout << endl << endl << endl << "===============================" << endl << endl << endl << endl;

    //------------------------------------------
    srand(time(0));
    //cout << time(0) << endl << endl;
    //------------------------------------------


    //test_bplustree_insert_random(10000);


//    test_bplustree_remove();
    //test_bplustree_interactive();
    test_insert_and_delete_random(10000);

    test_iterator();
    cout << endl << "===============================" << endl;
    return 0;
}

void test_iterator() {
    const bool debug = false;
    BPlusTree<int> bpt;
    for (int i = 0; i < 125; i++)
        bpt.insert(Random(0, 100));
    cout << bpt << endl;


    cout << "------------------------------------------------------------" << endl;
    for (BPlusTree<int>::Iterator it = bpt.begin();
        it != bpt.end(); ++it) {
        if (debug) it.print_Iterator();
        int num = *it;
        cout << "[" << *it << "] ";
        if (debug) cout << endl;
    }
    cout << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "test ++ operator: " << endl;
    BPlusTree<int>::Iterator it = bpt.begin();
    cout << "{" << *(it++) << "}" << endl;
    cout << "{" << *it << "}" << endl;
    for (int i = 60; i < 75; i++) {
        it = bpt.find_Iterator(i);
        if (!it.is_null()) {
            int num = *it;
            cout << *it << " was found." << endl;
        } else
            cout << i << " was not found" << endl;
    }
    cout << "====================================================================" << endl;

}


void test_bplustree_interactive() {
    BPlusTree<int> bt;
    string input = "";
    (srand(time(NULL)));
    while (true) {
        cout << "[R]andom  [I]nsert  [C]lear si[Z]e  [S]earch [E]rase  e[X]it: ";
        cin >> input;
        if (input == "r" || input == "R") {
            int num = (rand() % 100) + 1;
            cout << "-- Inserting " << num << "\nBTree : item: " << num << endl;
            bt.insert(num);
            bt.print_tree();
            cout << "\n\n=========================\n";
        } else if (input == "i" || input == "I") {
            int num = 0;
            cout << "? ";
            cin >> num;
            if (bt.insert(num)) {
                cout << "-- Inserting " << num << "\nBTree : item: " << num << endl;
            } else {
                cout << "could not insert " << num << endl;
            }
            bt.print_tree();
            cout << "\n\n=========================\n";
        } else if (input == "s" || input == "S") {
            int num = 0;
            cout << "? ";
            cin >> num;
            if (bt.contains(num)) {
                cout << "item is found. |" << bt.get(num) << "|\n";
            } else {
                cout << "Item not found.\n";
            }
            bt.print_tree();
            cout << "\n\n=========================\n";
        } else if (input == "c" || input == "C") {
            bt.clear_tree();
            bt.print_tree();
            cout << "\n\n=========================\n";
        } else if (input == "x" || input == "X") {
            break;
        } else if (input == "z" || input == "Z") {
            cout << "Size of BTree: " << bt.size() << endl;
            bt.print_tree();
            cout << "\n\n=========================\n";
        } else if (input == "e" || input == "E") {
            int num = 0;
            cout << "? ";
            cin >> num;
            if (bt.remove(num)) {
                cout << "-- Erasing " << num << "\nBTree : item: " << num << endl;
            } else {
                cout << "Cannot remove " << num << " as it's not in the tree" << endl;
            }

            bt.print_tree();
            cout << "\n\n=========================\n";
        }
    }
    cout << "\n\n=========================\n";
}

int Random(int lo, int hi)
{
    int r = rand() % (hi - lo + 1) + lo;

    return r;
}

void copy_array(int* destPtr, int* srcPtr, int size) {
    for (int i = 0; i < size; i++) {
        destPtr[i] = srcPtr[i];
    }
}

void test_bplustree_insert_random(int howMany) {
    cout << "*********************************************************" << endl;
    cout << " INSERT RANDOM TEST: " << howMany << " ITEMS" << endl;
    cout << "*********************************************************" << endl;
    int *arr = new int[howMany];
    int found = 0;
    for (int i = 0; i < howMany; i++) {
        arr[i] = i+1;
    }
    shuffle(arr, howMany);

    BPlusTree<int> bpt;

    for (int i = 0; i < howMany; i++) {
        bpt.insert(arr[i]);
    }

    for (int i = 0; i < howMany; i++) {
        if (bpt.get(arr[i]) == arr[i]) {
            found++;
        }
    }


    if (found == howMany) {
        cout << " INSERTION TEST PASSED " << "FOUND " << found << " ITEMS" << endl << endl;
    } else {
        cout << " INSERTION TEST FAILED " << endl << endl;
    }
}

void test_insert_and_delete_random(int howMany, bool print_debug) {
    cout << "*********************************************************" << endl;
    cout << " INSERT AND DELETEING RANDOM TEST: " << howMany << " ITEMS" << endl;
    cout << "*********************************************************" << endl;
    int* arr = new int[howMany];
    int notFound = 0;
    string temp = "";
    for (int i = 0; i < howMany; i++) {
        arr[i] = i + 1;
    }

    shuffle(arr, howMany);

    BPlusTree<int> bt;

    for (int i = 0; i < howMany; i++) {
        bt.insert(arr[i]);
    }
    if (print_debug) {
        bt.print_tree();
        cout << "\n\n\n";
    }
    for (int i = 0; i < howMany; i++) {
        bt.remove(arr[i]);
        if (print_debug) {
            cout << "removing: " << arr[i] << "\n\n\n";
            bt.print_tree();
        }
        if (!bt.contains(arr[i])) {
            notFound++;
        } else {
            cout << arr[i] << " " << i << " BROKE HERE!!!!!!\n\n";
        }
    }
    delete[]arr;
    arr = NULL;

    if (notFound == howMany) {
        cout << " INSERTION AND DELETING TEST PASSED " << "DELETED " << notFound << " ITEMS" << endl << endl;
    } else {
        cout << " INSERTION AND DELETING TEST FAILED " << endl << endl;
    }
}

void shuffle(int a[], unsigned int  size) {
    for (int i = size - 1; i > 0; --i) {
        int j = rand() % i;
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }
}