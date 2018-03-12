//Author: Mary Hamidi 
//Sources: Derived from Rob Gysel's given BST code. Functions that have a "H" at the end of name are rewritten for AVL 
#ifndef _AVL_COMMANDS
#define _AVL_COMMANDS

#include <cassert>
#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include "json.hpp"

#include "AVLCommands.h"


using namespace std; 
using namespace nlohmann;

using std::shared_ptr;
using std::make_shared;
using std::weak_ptr;


/*
*  Next things to achieve:
*  1 - Clean up the code
*      - one way to make it more readable is to implement the predefined functions
*        e.i node->HasRightChild()... whenever its necessary
*  2 - Update heights whenever a child is swapped 
*      - DeleteH is a function that we have not cheked for height updating
*  3 - Create the if statements for balancing based on balance factor
*      - That's the reason why we have the bf_ field, use it
*  4 - Start working on BSTSanityCkeck.cxx
*/


AVLNode::AVLNode(int key) :
	key_(key),
	parent_(std::weak_ptr<AVLNode>()),
	left_(nullptr),
	right_(nullptr) {}

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
	key_(key),
	parent_(parent),
	left_(nullptr),
	right_(nullptr) {}

bool AVLNode::IsLeaf() const {
	return left_ == nullptr && right_ == nullptr;
}

bool AVLNode::HasLeftChild() const {
	return left_ != nullptr;
}

bool AVLNode::HasRightChild() const {
	return right_ != nullptr;
}



void AVLNode::DeleteChild(std::shared_ptr<AVLNode> v) {
	if (left_ == v) {
		left_ = nullptr;
	} else if (right_ == v) {
		right_ = nullptr;
	} else {
		std::cerr << "AVLNode::DeleteChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}

void AVLNode::ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u) {
	if (left_ == u || right_ == u) {
		std::cerr << "AVLNode::ReplaceChild Error: child passed as replacement\n";
	}
	if (left_ == v) {
		left_ = u;
		u->parent_ = v->parent_;
	} else if (right_ == v) {
		right_ = u;
		u->parent_ = v->parent_;
	} else {
		std::cerr << "AVLNode::ReplaceChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}

AVL::AVL() : root_(nullptr), size_(0) {}

void AVL::Insert(int key) {
	if (root_ == nullptr) {
		root_ = std::make_shared<AVLNode>(key);
		size_++;
		return;
	}
	std::shared_ptr<AVLNode> currentNode = root_, lastNode = nullptr;
	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	if (key < lastNode->key_) {
		lastNode->left_ = std::make_shared<AVLNode>(key, lastNode);
	} else {
		lastNode->right_ = std::make_shared<AVLNode>(key, lastNode);
	}
	size_++;
}

int AVL::Height(shared_ptr<AVLNode> t)
{
	if (t)
		return t->height_;
	else
		return -1; 
}

int AVLNode::max(int leftH, int rightH)
{
	return (leftH > rightH) ? leftH: rightH;
}

bool AVL::Delete(int key) {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			if (currentNode->IsLeaf()) {
				DeleteLeaf(currentNode);
			} else if (currentNode->left_ == nullptr) {
				assert(currentNode->right_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->right_);
				size_--; assert(size_ >= 0);
			} else if (currentNode->right_ == nullptr) {
				assert(currentNode->left_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->left_);
				size_--; assert(size_ >= 0);
			} else {
				currentNode->key_ = DeleteMin(currentNode);
			}
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

int AVL::DeleteMin() {
	return DeleteMin(root_);
}


int AVL::DeleteMin(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> lastNode = nullptr;
	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = currentNode->left_;
	}
	int result = lastNode->key_;
	std::shared_ptr<AVLNode> parent = lastNode->parent_.lock();
	if (parent == nullptr) {
		// lastNode is root
		if (lastNode->right_ != nullptr) {
			root_ = lastNode->right_;
			lastNode->right_->parent_.reset();
		} else {
			root_ = nullptr;
		}
	} else {
		// lastNode under the root
		if (lastNode->right_ != nullptr) {
			parent->left_ = lastNode->right_;
			lastNode->right_->parent_ = parent;
		} else {
			parent->left_ = nullptr;
		}
  }
	size_--; assert(size_ >= 0);
	return result;
}

void AVL::DeleteLeaf(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
	if (parent == nullptr) {
		// Delete root
		root_ = nullptr;
		size_--; assert(size_ == 0);
	} else {
		if (parent->right_ == currentNode) {
			parent->right_ = nullptr;
		} else if (parent->left_ == currentNode) {
			parent->left_ = nullptr;
		} else {
			std::cerr << "AVL::DeleteLeaf Error: inconsistent state\n";
		}
		size_--; assert(size_ >= 0);
	}
}

size_t AVL::size() const {
	return size_;
}

bool AVL::empty() const {
	return size_ == 0;
}

bool AVL::Find(int key) const {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			return true;
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

std::string AVL::JSON() const {
	nlohmann::json result;
	std::queue< std::shared_ptr<AVLNode> > nodes;
	if (root_ != nullptr) {
		result["root"] = root_->key_;
		nodes.push(root_);
		while (!nodes.empty()) {
			auto v = nodes.front();
			nodes.pop();
			std::string key = std::to_string(v->key_);
			result[key]["balance factor"] = v->bf_;
			result[key]["height"] = v->height_;
			if (v->left_ != nullptr) {
				result[key]["left"] = v->left_->key_;
				nodes.push(v->left_);
			}
			if (v->right_ != nullptr) {
				result[key]["right"] = v->right_->key_;
				nodes.push(v->right_);
			}
			if (v->parent_.lock() != nullptr) {
				result[key]["parent"] = v->parent_.lock()->key_;
			} else {
				result[key]["root"] = true;
			}
		}
	}
	if (root_) 
	{
		result["height"] = root_->height_; 
		result["size"] = size_;
	}

	else // tree empty 
	{
		result["height"] = -1; 
		result["size"] = 0;
	}

	return result.dump(2) + "\n";
}

//**********************************************************************************//



void AVL::InsertH(int key)
{
	if (root_ == nullptr)
    {
      root_ = std::make_shared<AVLNode>(key);
      root_->height_ = 0;
      root_->bf_ = 0;
      size_++;
    }
    else
      insert(key, root_, nullptr); 
}

void AVL::insert(int key, shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{
    if (node == nullptr)
    {
       if (key < parent->key_)
       {
       	  cout << "parent key: "  << parent->key_ << "Inserting " << key << endl;
    	  parent->left_ = std::make_shared<AVLNode>(key, parent); 
    	  cout << "After segfaut" << endl;
       }
     	else
    	{
          parent->right_ = std::make_shared<AVLNode>(key, parent);
        }
        //cout << parent->left_->key_ << endl; //when this line is commented, the height checking does not show
        cout << "About to return " << endl;
        size_++;
    	return; 
    }
    else if (key < node->key_)//insert on Left side 
    {
    	cout << "key < node->key_: Node key: "  << node->key_ << "Inserting " << key << endl;
    	insert(key, node->left_, node);
    	rebalence(key, node, parent); 
    }
    else if (key > node->key_)// insert on right side
    {	
    	cout << "key > node->key_: Node key: "  << node->key_ << "Inserting " << key << endl;
    	insert(key, node->right_, node);
    	rebalence(key, node, parent);    	
    }
    
    cout << "now above height cheking: " << endl;
    node->height_ = 1 + max(Height(node->left_), Height(node->right_)); 
    node->bf_ = Height(node->right_) - Height(node->left_);
    
}


void AVL::rebalence(int key, shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{
	/*if (parent)
		cout << "Rebalence: " << parent->key_;
	cout << " Node key: " << node->key_ << endl; */
	if (Height(node->right_) - Height(node->left_) == -2) //LeftLeft or LeftRight case, tree left heavy
    {
		if (key < node->left_->key_)	//Single rotation, LeftLeft case
		{
			node = rightRotation(node->left_, node);
			if (parent == nullptr)//its the root 
				root_ = node;
			else
			{
				node->parent_ = parent;
			//	parent->left_ = node;
        		if (parent->HasRightChild() && parent->right_ == node->right_)  
        		  parent->right_ = node;
        		else
        		  parent->left_ = node;
			}
		}
		else	//LeftRight case
		{		
			node->left_ = leftRotation(node->left_->right_, node->left_);  
			node = rightRotation(node->left_, node); 
			if (parent == nullptr)//its the root 
	    	    root_ = node;
		    else
		    {
		    	node->parent_ = parent;
                /*if (parent->HasRightChild() && parent->right_->key_ == node->key_)
        		  parent->right_ = node;
        		else
        		  parent->left_ = node;*/
        		if (parent->HasRightChild() && parent->right_ == node->right_)  
        		  parent->right_ = node;
        		else
        		  parent->left_ = node;
		    }
		}
    }
	if (Height(node->right_) - Height(node->left_) == 2) //RightRight or RightLeft case
    {
		if (key > node->right_->key_) //RR case 
		{
			node = leftRotation(node->right_,node); 
			if (parent == nullptr)//its the root 
				root_ = node; 
			else
			{
				node->parent_ = parent;
        		//parent->right_ = node;
        		if (parent->HasRightChild() && parent->right_ == node->left_)  
        		  parent->right_ = node;
        		else
        		  parent->left_ = node;
			}
		}
		else //RightLeft case
		{
			//cout << "Parent: " << parent->key_ << endl;
			//cout << "Here Swapping: " << node->right_->key_ << endl; 
			node->right_ = rightRotation(node->right_->left_, node->right_);
		//	cout << "Now right: " << node->right_->key_ << endl;
	///		cout << "following: " << node->right_->right_->key_ << endl;
			//node->right_->parent_ = node;
			node = leftRotation(node->right_, node);
			if (parent == nullptr)//its the root 
            	root_ = node;
            else
            {
            	node->parent_ = parent;
                //if (parent->HasRightChild() && parent->left_->key_ == node->key_)
                //  parent->left_ = node;
        		//else
        		//  parent->right_ = node;
        		if (parent->HasRightChild() && parent->right_ == node->left_)  
        		  parent->right_ = node;
        		else
        		  parent->left_ = node;
            }
		}
    }

}

shared_ptr<AVLNode> AVL::rightRotation(shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{ 
    parent->left_ = node->right_;

    if (node->right_)
    	node->right_->parent_ = parent;

    parent->parent_ = node;
    node->right_ = parent;

    node->parent_.reset();

    parent->height_ = max(Height(parent->right_), Height(parent->left_) ) + 1;
    node->height_ = max(Height(node->left_), parent->height_) + 1;

    node->bf_ = Height(node->right_) - Height(node->left_);
    parent->bf_ = Height(parent->right_) - Height(parent->left_);

    return node;    
}

shared_ptr<AVLNode> AVL::leftRotation(shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{ 

	parent->right_ = node->left_;		//Assign node's left subtree to tree

	if (node->left_)
	{
		node->left_->parent_ = parent; 
	//	node->left_.reset();
	}

    parent->parent_ = node;				//Update parent's weak ptr connection to node
    node->left_ = parent;				//Establish shared ptr connection 

    node->parent_.reset();				//Clearing node's original ptr to parent 

    parent->height_ = max(Height(parent->right_), Height(parent->left_) ) + 1;
    node->height_ = max(Height(node->right_), Height(parent->left_)) + 1;

    node->bf_ = Height(node->right_) - Height(node->left_);
    parent->bf_ = Height(parent->right_) - Height(parent->left_);

    return node;    
}

void AVL::DeleteMinH() // For AVL 
{
	deleteMin(root_); 
}

void AVL::deleteMin(std::shared_ptr<AVLNode> currentNode) 
{
	shared_ptr<AVLNode> lastNode = currentNode->parent_.lock();

	if(currentNode->left_ == nullptr)
	{
        shared_ptr<AVLNode> tempChild = currentNode->right_;
  
  		if (lastNode != nullptr)
  		{
  			DeleteLeaf(currentNode);
  		}
		else// deleting root
		{
			if (currentNode->right_) //Root has subtree
			{
				root_ = currentNode->right_; 
				currentNode->parent_.reset();
				size_--; 
				return;
			}
			else //last node in tree
			{
				root_ = nullptr; 
				size_--; 
			}
		}
		if (tempChild != nullptr)
		{		  
		  lastNode->left_ = tempChild;
		  tempChild->parent_ = lastNode;
		}
		return; 
	}
	else
	{
		deleteMin(currentNode->left_); 
     
		currentNode->height_ = 1 + max(Height(currentNode->left_), Height(currentNode->right_)); 
    	currentNode->bf_ = Height(currentNode->right_) - Height(currentNode->left_);

	    if (currentNode->bf_ == - 2)  // left heavy 
		{
		   // cout << "Tree left heavy, rebalance" << endl; 
		    if (Height(currentNode->left_-> left_) > Height(currentNode->left_->right_)) //LeftLeft Case
		    {
		        currentNode = rightRotation(currentNode->left_, currentNode);
				if (lastNode == nullptr)//its the root 
					root_ = currentNode;
				else
				{
					currentNode->parent_ = lastNode;
				//	parent->left_ = node;
	        		if (lastNode->HasRightChild() && lastNode->right_ == currentNode->right_)  
	        		  lastNode->right_ = currentNode;
	        		else
	        		  lastNode->left_ = currentNode;
				}
		    }
		    else 
		    {
		        //cout << "Left Right case" << endl; 
		        currentNode->left_ = leftRotation(currentNode->left_->right_, currentNode->left_);
		        currentNode = rightRotation(currentNode->left_, currentNode); 
		        if (lastNode== nullptr)//its the root 
		              root_ = currentNode;
		        else
		        {
		          currentNode->parent_ = lastNode;
		          if (lastNode->HasRightChild() && lastNode->right_ == currentNode)
		            lastNode->right_ = currentNode;
		          else
		            lastNode->left_ = currentNode;
		        }
		    }////////
		}
		if (currentNode->bf_ == 2) //Right heavy
		{
			if (Height(currentNode->right_->right_) >= Height(currentNode->right_->left_)) // bug was < or <= 
			{
			   currentNode = leftRotation(currentNode->right_,currentNode); //RR case 
			  
			   if (lastNode == nullptr)//its the root 
					root_ = currentNode; 
			   else
			   {
					currentNode->parent_ = lastNode;
	        		//parent->right_ = node;
	        		if (lastNode->HasRightChild() && lastNode->right_ == currentNode->left_)  
	        		  lastNode->right_ = currentNode;
	        		else
	        		  lastNode->left_ = currentNode;
			    }
			   
        	}
        	else // RightLeft case
        	{
        	    currentNode->right_ = rightRotation(currentNode->right_->left_, currentNode->right_); //node and parent 
		//		currentNode->right_->parent_ = currentNode;

				currentNode = leftRotation(currentNode->right_, currentNode);

				if (currentNode->parent_.lock() == nullptr)//its the root 
            		root_ = currentNode;
            	else
            	{
            		currentNode->parent_ = lastNode;
                	if (lastNode->HasRightChild() && lastNode->right_ == currentNode->left_)
        		 		lastNode->right_ = currentNode;
        			else
        		  		lastNode->left_ = currentNode;
        		}
        	}
		}
		currentNode->height_ = 1 + max(Height(currentNode->left_), Height(currentNode->right_)); 
        currentNode->bf_ = Height(currentNode->right_) - Height(currentNode->left_);   
    }
}


void AVL::DeleteH(int key)
{
	deleteH(key, root_); 
}

void AVL::deleteH(int key, shared_ptr<AVLNode> currentNode )
{

	if (currentNode == nullptr)
		return; //key not found
	else
	{

		if (key < currentNode->key_)
			deleteH(key, currentNode->left_); 
		else
	    {
			if (key > currentNode->key_) 
				deleteH(key, currentNode->right_); 
			else    
			{// key == to currentNode key 
				//cout << "Key: " << currentNode->key_ << endl;
				if (currentNode->left_ == nullptr || currentNode ->right_ == nullptr ) // only one child, just swap
				{
				//	cout << "Key: " << currentNode->key_ << endl;
					if (currentNode->left_ != nullptr)
					{
				//		cout << "Here deleting: " << currentNode->left_->key_ << endl;
						std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				        parent->ReplaceChild(currentNode, currentNode->left_);
				        currentNode->parent_.reset();
						currentNode->left_.reset();
                        size_--;
					}
					else if (currentNode->right_ != nullptr)
					{
						std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				        parent->ReplaceChild(currentNode, currentNode->right_);//left_);
						currentNode->parent_.reset();
						currentNode->right_.reset();

					    size_--;
					}
					else
					{	
					    DeleteLeaf(currentNode);
					    return;
					}
					
				}
				else // there are two kids 
				{
					//cout << "currentNode: " << currentNode->key_ << endl;
					//cout << " left child " << currentNode->left_->key_ << endl;
		        	shared_ptr<AVLNode> getMin = findMin(currentNode->right_);
		        	//cout << "Min: " << getMin->key_<< endl;

					currentNode->key_ = getMin->key_; // findMin(currentNode->right_)->key_;
                    //cout << "New Key: " << currentNode->key_ << endl;

				    //cout << "Entering" << endl;
		        
		        	deleteH(currentNode->key_, currentNode->right_);

				}
		    }
		}
	}

    shared_ptr<AVLNode> lastNode = currentNode->parent_.lock();
    
    //cout << "Current Node height: " << currentNode->bf_  << " " << currentNode->key_ << endl;
   
	currentNode->height_ = 1 + max(Height(currentNode->left_), Height(currentNode->right_)); 
	currentNode->bf_ = Height(currentNode->right_) - Height(currentNode->left_);

	if (Height(currentNode->right_) - Height(currentNode->left_) == -2)//(currentNode->bf_ == - 2)  // left heavy 
	{
	    //cout << "Tree left heavy, rebalance" << endl; 
	    if (Height(currentNode->left_-> left_) > Height(currentNode->left_->right_)) //LeftLeft Case
	    {

      
            currentNode = rightRotation(currentNode->left_, currentNode);
			if (lastNode == nullptr)//its the root 
				root_ = currentNode;
			else
			{
				currentNode->parent_ = lastNode;
			//	parent->left_ = node;
        		if (lastNode->HasRightChild() && lastNode->right_ == currentNode->right_)  
        		  lastNode->right_ = currentNode;
        		else
        		  lastNode->left_ = currentNode;
			}

	    }
	    else 
	    {
	       // cout << "Left Right case" << endl; 
	        currentNode->left_ = leftRotation(currentNode->left_->right_, currentNode->left_);
	        currentNode = rightRotation(currentNode->left_, currentNode);
	        if (lastNode == nullptr)//its the root 
	              root_ = currentNode;
	        else
	        {
	          currentNode->parent_ = lastNode;

	          if (lastNode->HasRightChild() && lastNode->right_ == currentNode->right_)  
        		  lastNode->right_ = currentNode;
        	  else
        		  lastNode->left_ = currentNode;  
	        }
	    }////////
	}
	if (Height(currentNode->right_) - Height(currentNode->left_) >= 2)//(currentNode->bf_ == 2) //Right heavy
	{
		//cout << "rebalancing on " << currentNode->key_ << " " << currentNode->right_->height_ << endl;
		if (Height(currentNode->right_->right_) >= Height(currentNode->right_->left_)) // bug was < or <= 
		{
		   cout << currentNode->right_->key_ << endl;
		   currentNode = leftRotation(currentNode->right_,currentNode); //RR case 
		 //  cout << "New Height: " << currentNode->key_ << " " << currentNode->left_->key_ << endl; 
		   if (lastNode == nullptr)//its the root 
				root_ = currentNode; 
		   else
		   {
				
			    currentNode->parent_ = lastNode;
        		if (lastNode->HasRightChild() && lastNode->right_ == currentNode->left_)  
        		  lastNode->right_ = currentNode;
        		else
        		  lastNode->left_ = currentNode;
			}
    	}
    	else // RightLeft case
    	{
    	    currentNode->right_ = rightRotation(currentNode->right_->left_, currentNode->right_); //node and parent 
			currentNode->right_->parent_ = currentNode;

			currentNode = leftRotation(currentNode->right_, currentNode);

			if (currentNode->parent_.lock() == nullptr)//its the root 
        		root_ = currentNode;
        	else
        	{
        		currentNode->parent_ = lastNode;
        		if (lastNode->HasRightChild() && lastNode->right_ == currentNode->left_)  
        		  lastNode->right_ = currentNode;
        		else
        		  lastNode->left_ = currentNode;
    		}
    	}
	}
	currentNode->height_ = 1 + max(Height(currentNode->left_), Height(currentNode->right_)); 
    currentNode->bf_ = Height(currentNode->right_) - Height(currentNode->left_);   

}


shared_ptr<AVLNode> AVL::findMin(shared_ptr<AVLNode> node)
{
	 if( node == NULL)
     	return node;

     while( node->left_ != NULL )
     {

     	node = node->left_;
       // cout << "Inside while: " << node->key_ << endl;
     }
     return node;
}


int main(int argc, char** argv) //Takes a json file with AVL commands, Insert, Delete, or DeleteMin 
{
  ifstream file;
  file.open(argv[1]);
  nlohmann::json jsonObject;
  // Store the contents filename into jsonObject
  if (file.is_open()) {
    file >> jsonObject;
  }
  string fileName;
  fileName.append(argv[1]);
    
  string opnum;  

  for (auto itr = jsonObject.begin(); itr != (--jsonObject.end()); ++itr)
  { 
    opnum = itr.key();

    if(jsonObject[opnum]["operation"] != "DeleteMin")
    {
      int key = jsonObject[opnum]["key"];
      if (jsonObject[opnum]["operation"]== "Delete")  
         cout << "Deleteing:" << key << endl;
        //T.Delete(key); 
      else 
      {
        //cout << "Inserting:" << key << endl;
        //T.InsertH(key);   
      }
    }

    else if (jsonObject[opnum]["operation"] == "DeleteMin")
    {
      cout << "DeleteMin" << endl;
      //T.DeleteMin(); 
    }
  }

  	  AVL T;
  	  // test for deleteMin
      /*T.InsertH(20); 
      T.InsertH(22);
      T.InsertH(10);
      T.InsertH(5);
      T.InsertH(15);
      T.InsertH(13);
      T.InsertH(14);
      T.InsertH(25); 
      T.InsertH(4);
      T.InsertH(3);
      T.InsertH(2);*/

      /*T.DeleteMinH(); // 2
      T.DeleteMinH();//3
      T.DeleteMinH(); //4
      T.DeleteMinH(); // 5
      T.DeleteMinH(); //10
      T.DeleteMinH(); //13
      T.DeleteMinH(); //14
      T.DeleteMinH(); // 15
      T.DeleteMinH(); //20
      T.DeleteMinH(); // 22
      T.DeleteMinH();//25*/

      // test for delete(key)


      /// goood for testing *******
     /* T.InsertH(6);
      T.InsertH(4);
      T.InsertH(50);
      T.InsertH(7);
      T.InsertH(5);
      T.InsertH(71);
      T.InsertH(45);
      T.InsertH(34);

      T.DeleteH(50);*/


 //     T.InsertH(4);
 //     T.InsertH(50);
 //     T.InsertH(77);
 //     T.InsertH(88);
 //     T.InsertH(5);

 //     T.DeleteH(50);



     // T.DeleteH(6);
      //T.DeleteH(4);




      // Right Left Test

      /*T.InsertH(32);
      T.InsertH(45);
      T.InsertH(34);
      T.InsertH(67);
      T.InsertH(98);
      T.InsertH(124);
      T.InsertH(5);
      T.InsertH(25); 
      T.InsertH(29); 
      T.InsertH(234);
      T.InsertH(1);

      T.DeleteMinH(); //1
      T.DeleteMinH(); //5
      T.DeleteMinH(); //remove 25 
      T.DeleteMinH(); //29 , left rotate working 
      T.DeleteMinH(); //32
      T.DeleteMinH(); // 34
      T.DeleteMinH(); // 45
      T.DeleteMinH(); //67 
      T.DeleteMinH(); //98
      T.DeleteMinH(); //124
      T.DeleteMinH(); //234*/


   /* T.InsertH(50);
    T.InsertH(6);
    T.InsertH(10);  
    T.InsertH(4);  
    T.InsertH(55); 
    T.InsertH(54);
    T.InsertH(60);*/

     // SEGFAULT INSERTION

    
   /*   T.InsertH(15);
      T.InsertH(10);
     
      T.InsertH(25);
      T.InsertH(20);
      T.InsertH(9);    // WHITHOUT (9) RIGHT LEFT ROTATION
      T.InsertH(13);
      T.InsertH(30);
      T.InsertH(12);
      T.InsertH(50);

      
      T.DeleteH(15); 
      T.DeleteH(25); 
      T.InsertH(70); 
      
      T.DeleteH(13);
    //  T.DeleteH(30);
    //  T.InsertH(30);

      T.InsertH(11);  */
      
/*      T.InsertH(20);
      T.InsertH(50);
      T.InsertH(10);
      T.InsertH(13);
      T.InsertH(70);
      T.InsertH(40);
      T.InsertH(9);
      T.InsertH(12);
    //  T.InsertH(14);              

      T.DeleteH(13);
      T.InsertH(11);             */
      
     // T.DeleteH(12);
     // T.InsertH(14);


      //T.InsertH(13);

 //     T.InsertH(4);
 //     T.InsertH(50);



      // NOT WORKING RIGHT LEFT
    /*  T.InsertH(20);
      T.InsertH(10);
      T.InsertH(50);
      T.InsertH(30);
      T.InsertH(12);
      T.InsertH(70);
      T.InsertH(11);*/

      // WORKING RIGHT LEFT
     /* T.InsertH(20);
      T.InsertH(11);
      T.InsertH(50);
      T.InsertH(10);
      T.InsertH(70);
      T.InsertH(10);
      T.InsertH(60);*/



      //T.DeleteH(13);
     // T.InsertH(11);
      //T.InsertH(40);
      //T.InsertH(50);

  /*    T.InsertH(15);
      T.InsertH(10);
      T.InsertH(25);
      T.InsertH(20);
      T.InsertH(9);
      T.InsertH(13);
      T.InsertH(30);
      T.InsertH(12);
      T.InsertH(50);

      
      //T.DeleteH(15); 
     // T.DeleteH(25); 
      T.InsertH(70); 

     // T.DeleteH(13);
      T.InsertH(11);
      T.InsertH(14);

      T.DeleteMinH();
      T.DeleteMinH();
      T.DeleteMinH();
      T.DeleteMinH();
      T.DeleteMinH();*/


      T.InsertH(15);
      T.InsertH(30);
      T.InsertH(11);
      T.InsertH(12);
      T.DeleteH(15);
      T.InsertH(9);
      T.InsertH(33);
      T.InsertH(25);
      T.DeleteH(11);
      T.DeleteMinH();
      T.InsertH(23);
      T.DeleteH(23);
      T.InsertH(26);
      T.InsertH(40);
      T.InsertH(60);
      T.DeleteH(30);
      T.DeleteH(33);


      cout << T.JSON() << endl; 

}



#endif 


