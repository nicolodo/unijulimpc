/* This is a library module supporting lists. A list stores any number of items
and has a current item, which can be any item from the first item to the
last item or a unique 'none' position indicating
that no item is selected. Every operation is constant time.

The items stored in a list are of type 'item'. The typedef for this can be
changed for any particular application so that 'item' stands for any desired
type, e.g. a raw type such as int, double, ...

A typical forward traversal has the form:

  first(l);
  while (after(l)) {
    item x = get(l); ...
  }

Note that all key functions are symmetrical with respect
to traversal direction or have matching counterparts.

The lists provided by this module are not thread safe. */

#include <stdbool.h>

// The type of items stored in the list. Change this for different applications.
typedef int item;

// The list type is opaque (declared here, and defined in lists.c).
struct list;
typedef struct list list;

// Create a new empty list, with the default item to return in case no item is selected.
// No item is selected in an empty list.
list *newList(item e);

// Free up the list and all the data in it.
void freeList(list *xs);

// Set the current item to the first item or to the last item
// of the list. If the list has no items the functions do nothing
// and no item is selected.
void first(list *xs);
void last(list *xs);

// Returns true if no item is selected, i.e. the 'none' position. 
// Otherwise it returns false.
bool none(list *xs);

// Move the current item one place forwards in the list and return true.
// If after is called while the last item is the current item, then no item is
// selected and true is returned. If the function is called while no item
// is selected then the function does nothing and returns false.
bool after(list *xs);

// Move the current item one place backwards in the list and return true.
// If before is called while the first item is the current item, then no item is
// selected and true is returned. If the function is called while no item
// is selected then the function does nothing and returns false.
bool before(list *xs);

// Get the current item. If get is called and no item is selected
// then the default item is returned.
item get(list *xs);

// Set the current item and return true. If set is called while no
// item is selected then the function does nothing and returns false.
bool set(list *xs, item x);

// Inserts an item after the current item and makes it the current item.
// If insertAfter is called while no item is selected then
// the function inserts the item at the beginning of the list
// before the first item.
void insertAfter(list *xs, item x);

// Inserts an item before the current item and makes it the current item.
// If insertAfter is called while no item is selected then the function inserts 
// the item at the end of the list after the last item.
void insertBefore(list *xs, item x);

// Delete the current item and make its successor the current item, then return
// true. If deleteToAfter is called while the last item is the current item then 
// the last item is deleted, no item is selected, and true is returned. 
// If deleteToAfter is called while no item is selected then the
// function does nothing and returns false.
bool deleteToAfter(list *xs);

// Delete the current item and make its predecessor the current item, then return
// true. If deleteToBefore is called while the first item is the current item then
// the first item is deleted, no item is selected, and true is returned. 
// If deleteToBefore is called while no item is selected then the
// function does nothing and returns false.
bool deleteToBefore(list *xs);
