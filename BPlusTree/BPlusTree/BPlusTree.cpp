/*
 * Author: Steven Herrera
 * Project: BPlusTree Class
 * Purpose: This is a binary tree which will can have multiple children
 *              it is still kept balanced and kept inorder like BTree but with additional functionality
 *
 * Notes: need to work on fixing the pointers as sometimes they point to random things 
 * also need to see if we can break the delete function more
 */

#include <iostream>
#include <cstdlib>
#include "BPlusTree.h"
#include "C:\Users\Steven_dev\Desktop\CS_Stuff\BTree-Functions\BTree-Functions\BTree-Functions\bTreeFunctions.h"

using namespace std;

void test_bplustree_interactive();
void test_bplustree_auto(int tree_size = 5000, int how_many = 500, bool report = false);
bool test_bplustree_auto(int how_many, bool report = true);
int Random(int lo, int hi);
void copy_array(int* destPtr, int* srcPtr, int size);
void test_bplustree_insert_random(int howMany);
void shuffle(int a[], unsigned int  size);

int main()
{

    //cout << endl << endl << endl << "===============================" << endl << endl << endl << endl;

    //------------------------------------------
    srand(time(0));
    //------------------------------------------


    //test_bplustree_insert_random(10000);


//    test_bplustree_remove();
    test_bplustree_interactive();
//    test_bplustree_big_three();
//    test_bplustree_auto(1000, 100, false);

    //    test_map();
    //    test_map_interactive();

    //    test_multimap();
    //    test_multimap_interactive();
    //    test_multimap_auto();

    //    test_iterator();
    cout << endl << "===============================" << endl;
    return 0;
}
//void test_iterator() {
//    const bool debug = false;
//    BPlusTree<int> bpt;
//    for (int i = 0; i < 125; i++)
//        bpt.insert(Random(0, 100));
//    cout << bpt << endl;
//
//
//    cout << "------------------------------------------------------------" << endl;
//    for (BPlusTree<int>::Iterator it = bpt.begin();
//        it != bpt.end(); ++it) {
//        if (debug) it.print_Iterator();
//        cout << "[" << *it << "] ";
//        if (debug) cout << endl;
//    }
//    cout << endl;
//    cout << "------------------------------------------------------------" << endl;
//    cout << "test ++ operator: " << endl;
//    BPlusTree<int>::Iterator it = bpt.begin();
//    cout << "{" << *(it++) << "}" << endl;
//    cout << "{" << *it << "}" << endl;
//    for (int i = 60; i < 75; i++) {
//        it = bpt.find(i);
//        if (!it.is_null())
//            cout << *it << " was found." << endl;
//        else
//            cout << i << " was not found" << endl;
//    }
//    cout << "====================================================================" << endl;
//
//}

void test_bplustree_auto(int tree_size, int how_many, bool report) {
    bool verified = true;
    for (int i = 0; i < how_many; i++) {
        if (report) {
            cout << "*********************************************************" << endl;
            cout << " T E S T:    " << i << endl;
            cout << "*********************************************************" << endl;
        }
        if (!test_bplustree_auto(tree_size, report)) {
            cout << "T E S T :   [" << i << "]    F A I L E D ! ! !" << endl;
            verified = false;
            return;
        }

    }
    cout << "**************************************************************************" << endl;
    cout << "**************************************************************************" << endl;
    cout << "             E N D     T E S T: " << how_many << " tests of " << tree_size << " items: ";
    cout << (verified ? "VERIFIED" : "VERIFICATION FAILED") << endl;
    cout << "**************************************************************************" << endl;
    cout << "**************************************************************************" << endl;

}

bool test_bplustree_auto(int how_many, bool report) {
    const int MAX = 10000;
    assert(how_many < MAX);
    BPlusTree<int> bpt;
    int a[MAX];
    int original[MAX];
    int deleted_list[MAX];

    int original_size;
    int size;
    int deleted_size = 0;

    //fill a[ ]
    for (int i = 0; i < how_many; i++) {
        a[i] = i;
    }
    //shuffle a[ ]: Put this in a function!
    for (int i = 0; i < how_many; i++) {
        int from = Random(0, how_many - 1);
        int to = Random(0, how_many - 1);
        int temp = a[to];
        a[to] = a[from];
        a[from] = temp;
    }
    //copy  a[ ] -> original[ ]:
    copy_array(original, a, how_many);
    size = how_many;
    original_size = how_many;
    for (int i = 0; i < size; i++) {
        bpt.insert(a[i]);

    }
    if (report) {
        cout << "========================================================" << endl;
        cout << "  " << endl;
        cout << "========================================================" << endl;
        cout << bpt << endl << endl;
    }
    for (int i = 0; i < how_many; i++) {
        int r = Random(0, how_many - i - 1);
        if (report) {
            cout << "========================================================" << endl;
            cout << bpt << endl;
            cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . " << endl;
            cout << "deleted: "; print_array(deleted_list, deleted_size);
            cout << "   from: "; print_array(original, original_size);
            cout << endl;
            cout << "  REMOVING [" << a[r] << "]" << endl;
            cout << "========================================================" << endl;
        }
        bpt.remove(a[r]);


        delete_item(a, r, size, deleted_list[deleted_size++]);
        //i do not have an is_valid function yet
        /*if (!bpt.is_valid()) {
            cout << setw(6) << i << " I N V A L I D   T R E E" << endl;
            cout << "Original Array: "; print_array(original, original_size);
            cout << "Deleted Items : "; print_array(deleted_list, deleted_size);
            cout << endl << endl << bpt << endl << endl;
            return false;
        }*/

    }
    if (report) cout << " V A L I D    T R E E" << endl;

    return true;
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

void shuffle(int a[], unsigned int  size) {
    for (int i = size - 1; i > 0; --i) {
        int j = rand() % i;
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }
}