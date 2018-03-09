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


void AVL::DeleteMinH() 
{
	deleteMin(root_); 
}

void AVL::deleteMin(std::shared_ptr<AVLNode> currentNode) //Need to rebalence 
{
	//if (currentNode)
		//cout << "deleteMin's currentNode:" << currentNode->key_  << endl;

	shared_ptr<AVLNode> lastNode = currentNode->parent_.lock();

	//if (lastNode)
		//cout << "currentNode's parent: " << lastNode->key_ << endl; 

	//shared_ptr<AVLNode> lastNodeParent = lastNode->parent_.lock(); 

	if(currentNode->left_ == nullptr)
	{
        shared_ptr<AVLNode> tempChild = currentNode->right_;
      
		DeleteLeaf(currentNode);

		if (tempChild != nullptr)
		{
		  
		  lastNode->left_ = tempChild;
		  tempChild->parent_ = lastNode;
		}

		return; 
		//lastNode->height_ = 1 + max(Height(lastNode->left_), Height(lastNode->right_)); 
    	//lastNode->bf_ = Height(lastNode->right_) - Height(lastNode->left_);

	}
	else
	{
		deleteMin(currentNode->left_); 
       // if (currentNode->left_)
		  //cout << "parent Key: " << currentNode->key_ << "Child Key: " << currentNode->left_->key_ << endl;

		currentNode->height_ = 1 + max(Height(currentNode->left_), Height(currentNode->right_)); 
    	currentNode->bf_ = Height(currentNode->right_) - Height(currentNode->left_);
    	
    	//if (lastNode != nullptr)
    	//{
    	//rebalence(currentNode->key_, currentNode, lastNode);
    	
    	currentNode = leftRotation(currentNode->right_,currentNode); 
	    if (lastNode == nullptr)//its the root 
			root_ = currentNode; 
		else
		{
		    currentNode->parent_ = lastNode;
        	lastNode->right_ = currentNode;
		}

    	
    	/*else if (currentNode->bf_ < -1 ) // Left Heavy tree
    	{
    		cout << "I am here bithch " << endl;
    		rebalence(currentNode->left_->key_, currentNode->left_, currentNode);
    	}
    	else
    	{
    		cout << "I am in this other side" <<  endl;
    		cout << currentNode->right_->height_ << endl;
    		rebalence(currentNode->right_->key_, currentNode->right_, currentNode);
    	}*/
    }

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
	result["height"] = root_->height_; 
	result["size"] = size_;
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
    	  parent->left_ = std::make_shared<AVLNode>(key, parent); 
    	else
          parent->right_ = std::make_shared<AVLNode>(key, parent);

         size_++; 
    	return; 
    }
    else if (key < node->key_)//insert on Left side 
    {
    	insert(key, node->left_, node);
    	rebalence(key, node, parent); 
    }
    else if (key > node->key_)// insert on right side
    {	
    	insert(key, node->right_, node);
    	rebalence(key, node, parent);    	
    }

    node->height_ = 1 + max(Height(node->left_), Height(node->right_)); 
    node->bf_ = Height(node->right_) - Height(node->left_);
    
}


void AVL::rebalence(int key, shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{
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
				parent->left_ = node;
			}
		}
		else	//LeftRight case
		{		
			//cout << "LR: Calling leftRotation on this node:" << node->left_->key_ << endl; 
			node->left_ = leftRotation(node->left_->right_, node->left_);  
			//cout << "LR: Calling rightRotation on this node:" << node->key_  <<" Node's left child:" << node->left_->key_<< endl; 
			node = rightRotation(node->left_, node); 
			if (parent == nullptr)//its the root 
	    	    root_ = node;
		    else
		    {
		    	node->parent_ = parent;
                if (parent->HasRightChild() && parent->right_->key_ == node->key_)
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
			//cout << "Calling leftRotation on this node:" << node->key_ << endl; 
			node = leftRotation(node->right_,node); 
			if (parent == nullptr)//its the root 
				root_ = node; 
			else
			{
				node->parent_ = parent;
        		parent->right_ = node;
			}
		}
		else //RightLeft case
		{
			node->right_ = rightRotation(node->right_->left_, node->right_);
			//cout << "Key of subtree rotated: " << node->right_->key_ << " " << node->key_ << endl;
			node->right_->parent_ = node;
			node = leftRotation(node->right_, node);
			//cout << "Key of second subtree rotated: " << node->key_ << " " << node->right_->key_ << endl; 
			if (parent == nullptr)//its the root 
            	root_ = node;
            else
            {
            	node->parent_ = parent;
                if (parent->HasRightChild() && parent->right_->key_ == node->key_)
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
		node->left_->parent_ = parent; 
	//node->left_.reset(); 

    parent->parent_ = node;				//Update parent's weak ptr connection to node
    node->left_ = parent;				//Establish shared ptr connection 

    node->parent_.reset();				//Clearing node's original ptr to parent 

    parent->height_ = max(Height(parent->right_), Height(parent->left_) ) + 1;
    node->height_ = max(Height(node->right_), Height(parent->left_)) + 1;

    node->bf_ = Height(node->right_) - Height(node->left_);
    parent->bf_ = Height(parent->right_) - Height(parent->left_);

    return node;    
}


int main(int argc, char** argv)
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
      	//cout << "###"; 
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

      //Declaring AVL 
  	  AVL T;
     /* T.InsertH(20); 
      T.InsertH(22);
      T.InsertH(10);
      T.InsertH(5);
      T.InsertH(15);
      T.InsertH(13);
      T.InsertH(14);
      T.InsertH(25); 
      T.InsertH(4);
      T.InsertH(3);
      T.InsertH(2);

      T.DeleteMinH(); 
      T.DeleteMinH();
      T.DeleteMinH();*/

      // Right Left Test

     /* T.InsertH(32);
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
      T.DeleteMinH(); //remove 25 fuck */

  	  T.InsertH(50);
      T.InsertH(25);
      T.InsertH(80);
      T.InsertH(88);

      /*T.InsertH(4);
      T.InsertH(6);
      T.InsertH(9);*/

      T.DeleteMinH(); 

      cout << T.JSON() << endl; 

}



#endif 


