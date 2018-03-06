//Author: Mary Hamidi 
#ifndef _AVL_COMMANDS
#define _AVL_COMMANDS

//#include "json.hpp"
#include <iostream>
#include <string>

#include "BST.h"

using namespace std; 
//using namespace nlohmann;

class AVLNode; 

class AVL: public BST
{
	//BST tree;
public:
    AVL():BST(){}; //tree.root_(nullptr), size_(0) {}
//delete()
//deleteMin()
//void rightRotation(AVLNode )
	void InsertH(int key); //creates node and calls recursive insert()
	void insert(int key, shared_ptr<BSTNode> node, shared_ptr<BSTNode> parent);//shared_ptr<BSTNode> newNode, shared_ptr<BSTNode> node); 
	//void Insert(int key);
	int Height(shared_ptr<BSTNode> t)
    {
      if (t)
		return t->height_;
	  else
		return -1; 
    } 
//AVLNode node; 

};

class AVLNode: public BSTNode
{
public:
	AVLNode(int key): BSTNode(key){}
	AVLNode(int key, std::weak_ptr<BSTNode> parent):BSTNode(key, parent)
	{
		balencefactor = Height(right_) - Height(left_); 
	}
	//shared_ptr<BSTNode> node;
    int Height(shared_ptr<BSTNode> t)
    {
      if (t)
		return t->height_;
	  else
		return -1; 
    } 
	
	friend class AVL;

private:
	int balencefactor; //Balence Factor 
};


#endif 