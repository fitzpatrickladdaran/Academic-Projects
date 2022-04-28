/*
Heap.h

Heap template class to store data of any comparable type,
implemented using an array

Authour: Fitz Laddaran
Contact: fitzladdaran@gmail.com
Last Updated: 27/08/2020
*/

#pragma once
#include <stdexcept>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::runtime_error;

template <class T>
class Heap
{

public:

	Heap(int arrSize); // constructor
	Heap(const Heap& myHeap); // copy constructor 
	~Heap(); // destructor 
	Heap& operator=(const Heap& myHeap); // overloaded assignment operator
	void insert(T item); // inserts item
	T remove(); // removes and returns the root
	T peek() const; // peeks at the root
	Heap merge(const Heap& myHeap); // merges the called and calling object
	int size() const; // returns the size of the heap
	void print(); // prints the contents of the heap
	
private: 

	// attributes
	int currSize; // heap's current size
	int maxSize; // heap's maximum size
	T* heapArray; // array to store items

	// helper functions
	void bubbleDown(int startIndex); // heapify 
	void swap(T* x, T* y); // swaps x and y in an array
	void copy(const Heap& myHeap); // copy helper function
	void printHeap(); // print helper function
};

// returns a vector containing the m smallest values in the parameter 
// runs in O(n log m)
template<class T>
vector<T> smallestm(vector<T> myVect, int m) {

	int size = 0; // size counter
	
	// determine size
	if (myVect.size() < m) {
		size = myVect.size();
	}

	else {
		size = m;
	}

	Heap<T> myHeap(size); // constructing a new heap

	// stores elements from myVect into heap
	for (int i = 0; i < myVect.size(); ++i) {
		
		T value = myVect[i];

		// current heap size is less than max heap size
		if (myHeap.size() < size) {
			myHeap.insert(value); 
		}
		
		// current heap size is equal to max heap size
		else if (value <= myHeap.peek()) {

			myHeap.remove(); 
			myHeap.insert(value); 

		}
	}

	vector<T> rVect(size); // vector to be returned

	// fill rVect
	for (int i = size - 1; i >= 0; --i) {
		rVect[i] = myHeap.remove();
	}
	
	return rVect; 

}

// constructor
template <class T>
Heap<T>::Heap(int arrSize) { 

	currSize = 0;
	maxSize = arrSize;
	heapArray = new T[arrSize];

}

// copy constructor
template <class T>
Heap<T>::Heap(const Heap& myHeap) {

	copy(myHeap);

}

// destructor
template <class T>
Heap<T>::~Heap() {

	delete[] heapArray;
	heapArray = nullptr;

}

// overloaded assignment operator
template <class T>
Heap<T>& Heap<T>::operator=(const Heap& myHeap) {

	if (this != &myHeap) {

		delete[] heapArray;
		copy(myHeap);

	}

	return *this;

}

// inserts item
template <class T>
void Heap<T>::insert(T item) {

	if (currSize == maxSize) {
		throw runtime_error("Array is full, can't insert more");
	}

	int index = currSize; // counter for current size
	heapArray[currSize++] = item; // item inserted at next available index
	int parent; // index for parent location

	// bubble-up (heapify) if necessary
	// enters only if there is more than one item in the heap
	while (index != 0) {

		// inserted item is a left-child
		if (index % 2 == 0) {
			parent = (index / 2) - 1;
		}

		// inserted item a right-child
		else {
			parent = index / 2;
		}

		// bubble-up if necessary
		if (heapArray[index] > heapArray[parent]) {
			swap(&heapArray[index], &heapArray[parent]);
			index = parent;
		}

		// else break out of loop
		else {
			index = 0;
		}

	}
}

// removes and returns the first element
template <class T>
T Heap<T>::remove() {

	if (currSize == 0) {
		throw runtime_error("Array is empty, can't delete anything");
	}

	T root = heapArray[0]; // root to be returned

	// if there's more than one element in the heap
	if (currSize > 1) {

		heapArray[0] = heapArray[currSize - 1]; // replace root with last item in array
		currSize--; // decrement current size
		bubbleDown(0); // heapify

	}

	// if there's only one element in the heap
	else {
		currSize = 0;
	}

	return root;

}

// peeks at the first element
template <class T>
T Heap<T>::peek() const {

	if (currSize == 0) {
		throw runtime_error("Array is empty, nothing to return");
	}

	return heapArray[0];

}

// merges the calling object and called object
template <class T>
Heap<T> Heap<T>::merge(const Heap& myHeap) {

	Heap mergedHeap(currSize + myHeap.currSize); // creating a new heap 
	
	for (int i = 0; i < currSize; ++i) {
		mergedHeap.insert(heapArray[i]);
	}

	for (int i = 0; i < myHeap.currSize; ++i) {
		mergedHeap.insert(myHeap.heapArray[i]);
	}
	
	return mergedHeap;

}

// returns the called object's size
template <class T>
int Heap<T>::size() const {
	return currSize;
}

// HELPER FUNCTION: swaps x and y
// USED BY: insert(), remove()
template <class T>
void Heap<T>::swap(T* x, T* y) { 

	T temp = *x;
	*x = *y;
	*y = temp;

}

// HELPER FUNCTION: heapify
// USED BY: remove()
template <class T>
void Heap<T>::bubbleDown(int startIndex) {

	int left = (startIndex * 2) + 1; // left-child index
	int right = (startIndex * 2) + 2; // right-child index
	int current = startIndex; // current index 

	// two children 
	if (left <= currSize && right <= currSize) {

		// both children have biggers values than heapArray[current]
		if (heapArray[current] < heapArray[left] && heapArray[current] < heapArray[right]) {

			// if the right child has a bigger value
			if (heapArray[left] < heapArray[right]) {
				swap(&heapArray[right], &heapArray[current]);
				current = right;
				bubbleDown(current);
			}

			// if the left child has a bigger or equal value 
			else {
				swap(&heapArray[left], &heapArray[current]);
				current = left;
				bubbleDown(current);
			}

		}

		// if left-child is the only bigger element 
		else if (heapArray[current] < heapArray[left]) {
			swap(&heapArray[left], &heapArray[current]);
			current = left;
			bubbleDown(current);
		}

		// if right-child is the only bigger element
		else if (heapArray[current] < heapArray[right]) {
			swap(&heapArray[right], &heapArray[current]);
			current = right;
			bubbleDown(current);
		}

	}

	// only one child (which must be left)
	else if (left <= currSize) {

		// if left-child has a bigger value than heapArray[current]
		if (heapArray[current] < heapArray[left]) {
			swap(&heapArray[left], &heapArray[current]);
			current = left;
			bubbleDown(current);
		}

	}
}

// HELPER FUNCTION: deep copies a heap
// USED BY: copy constructor, overloaded assignment operator
template <class T>
void Heap<T>::copy(const Heap& myHeap) {

	currSize = myHeap.currSize;
	maxSize = myHeap.maxSize;
	heapArray = new T[maxSize];

	for (int i = 0; i < currSize; ++i) {
		heapArray[i] = myHeap.heapArray[i];
	}

}

// prints the Heap's contents
template <class T>
void Heap<T>::print() {

	printHeap();

}

// HELPER FUNCTION: prints the heap's array
// USED BY: print()
template <class T>
void Heap<T>::printHeap() {

	for (int i = 0; i < currSize; ++i) {
		cout << heapArray[i] << endl;
	}

}