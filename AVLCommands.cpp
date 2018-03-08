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
/*void AVL::InsertH(int key)
{
	//newnode = make_shared<AVLNode>(key);
	shared_ptr<AVLNode> newNode = make_shared<AVLNode>(key);
	//return insert(root_, newNode);//shared_ptr<AVLNode> node);

}

void AVL::insert(shared_ptr<AVLNode> node, shared_ptr<AVLNode> newNode)
{
    if (node == nullptr)
    {
    	node = newNode; 
    	size_++; 
    	//return; 
    }
    else if ( newNode->key_ < node->key_)
    {
    	insert(newNode, node->left_);
    }
    else if (newNode->key_ > node->key_)
    {
    	insert(newNode, node->right_);
    }
    newNode->height_ = 1+ max(Height(newNode->left_), Height(newNode->right_)); 
    
}*/

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
			result[key]["height"] = v->height_;
			result[key]["b-factor"] = v->bf_;
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
	result["size"] = size_;
	return result.dump(2) + "\n";
}

//**********************************************************************************//



void AVL::InsertH(int key)
{
	//newnode = make_shared<BSTNode>(key);
	//shared_ptr<AVLNode> newNode = make_shared<AVLNode>(key);
	//cout << "Key: " << newNode->key_  << " Balance: " << newNode->bf_<< endl;
	if (root_ == nullptr)
    {
      root_ = std::make_shared<AVLNode>(key);
      root_->height_ = 0;
      root_->bf_ = 0;
      size_++;
    }
    else
      insert(key, root_, nullptr); 
    
    //cout << newNode->height_ << endl;
}


void AVL::insert(int key, shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent) //shared_ptr<BSTNode> newNode, shared_ptr<BSTNode> node)
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
    else if (key < node->key_)//( newNode->key_ < node->key_)
    {
        //node->left_ = std::make_shared<BSTNode>(key, lastNode);
    	insert(key, node->left_, node);
    	if (Height(node->right_) - Height(node->left_) == -2) //bf_ <= -2) //LeftLeft
        {
          if (key < node->left_->key_)
          {
           // if (node->parent_.lock() == nullptr)
            //	node->parent_.reset();

            //shared_ptr<AVLNode> temp = node->left_->parent_.lock();

            //
           /* cout << "Old Root Adress: " << node << endl;
            cout << "Root Parent Adress: " << node->parent_.lock() << endl;*/
            
           // cout << "Key: " << node->left_->key_ << endl;

            node = rightRotation(node->left_, node);

            /*shared_ptr<AVLNode> temp = node->parent_.lock();

            cout << "Key: " << temp->key_ << endl;*/


            if (parent == nullptr)//node->parent_.lock() == nullptr)
            {
            	cout << parent << endl;
            	cout << node << endl;
            	cout << "getting here: " << endl;// parent->left_ << endl;
            	//node->parent_ = parent->left_;
            	root_ = node;
            }
            else
            {
            	cout << "Key Parent " << parent->key_ << endl;
            	cout << "Child : " << node->key_ << endl;
            	node->parent_ = parent;
            	parent->left_ = node;
            	//parent = node;
            }
            


            /*cout << "Key new root: " << node->key_ << endl;
            cout << "New Root Adress: " << node << endl;
            cout << "Parent Adress: " << node->right_->parent_.lock() << endl;*/
            //root_ = node;


          }
        }  
    }
    else if (key > node->key_)//(newNode->key_ > node->key_)
    {
    	insert(key, node->right_, node);
    	cout << " *** getting here" << endl;
    }

    node->height_ = 1 + max(Height(node->left_), Height(node->right_)); 
    node->bf_ = Height(node->left_) - Height(node->right_);
    
}



shared_ptr<AVLNode> AVL::rightRotation(shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{ 
   // shared_ptr<AVLNode> temp = node;

    parent->left_ = node->right_;

    parent->parent_ = node;//->right_;

    node->right_ = parent;

    node->parent_.reset();

   // weak_ptr<AVLNode> child = node->right_; 
    parent->height_ = max(Height(parent->right_), Height(parent->left_) ) + 1;
    node->height_ = max(Height(node->left_), parent->height_) + 1;
   
    return node;    

	//return nullptr;
}

//parent = child;

    //node->right_ = child;
    
    //node->ReplaceChild(node->right_, temp);

/*
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
    
    //cout << fileName << endl; 

    string dump = jsonObject.dump(4); 
    //cout << dump << endl; 

    //int numoperations = jsonObject["metadata"]["numOps"];
    string opnum;  

    AVL T; //Declaring AVL 

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
        cout << "Inserting:" << key << endl;
        //T.InsertH(key);   
    }

    else if (jsonObject[opnum]["operation"] == "DeleteMin")
    {
      cout << "DeleteMin" << endl;
      //T.DeleteMin(); 
    }
  }
      T.InsertH(15); 
      T.InsertH(10);
      T.InsertH(9);
      cout << T.JSON() << endl; 

}*/




int main(int argc, char** argv)
{

    
    AVL T;

    /*T.InsertH(22);
    T.InsertH(44); 
    T.InsertH(12);
    T.InsertH(56);
    T.InsertH(2);*/
    
    T.InsertH(17);
    T.InsertH(15); 
    T.InsertH(10);
    T.InsertH(11);
    T.InsertH(8);
    T.InsertH(6); 
    T.InsertH(1);

    //T.Delete(12);

    //T.DeleteMin();

    //T.InsertH(22);
    //T.InsertH(12);
    //T.InsertH(55);
    //T.InsertH(54); 

    
    cout << T.JSON() << endl; 

}



#endif 


