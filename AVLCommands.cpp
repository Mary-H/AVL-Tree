#ifndef _AVL_COMMANDS
#define _AVL_COMMANDS

#include <cassert>
#include <iostream>
#include <string>
#include <queue>
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
	right_(nullptr) 
{
	bf_ = Height(right_) - Height(left_); 
}

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
	key_(key),
	parent_(parent),
	left_(nullptr),
	right_(nullptr) 
{
	bf_ = Height(right_) - Height(left_); 
}

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

int AVLNode::Height(shared_ptr<AVLNode> t)
{
	if (t)
		return t->height_;
	else
		return -1; 
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
    	/*if (node->bf_ <= -2) //LeftLeft or LeftRight
        {
          if (key < node->left_->key_)
          	{
               // rightRotation(node, parent); //LeftLeft, rotate rigt where node->left will be the new subtree roo
            //else
                //leftRotation(node->left); 
                //rightRotation(node); 
         	}
        }*/
    }
    else if (key > node->key_)//(newNode->key_ > node->key_)
    {
    	insert(key, node->right_, node);
    }

    node->height_ = 1 + max( Height(node->left_), Height(node->right_)); 
    
}

/*shared_ptr <AVLNode> rightRotation(shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{
    shared_ptr<BSTNode> nodeLC = node ->left_.lock(); //node's Left Child
    shared_ptr<BSTNode> nodeLCRC = nodeLC -> right_.lock(); //node's Left Child's right child, nodeLC's Right Child

    node->left = nodeLCRC;  

    node->height_= 1 + max(Height(node->left_), Height(node->right_)); 
    nodeLC->height_= 1 + max(Height(nodeLC->left_), Height(nodeLC->right_)); 

    return nodeLC; 

}*/









int main(int argc, char** argv)
{

	/*std::ifstream file;
	file.open(argv[1]);
	nlohmann::json jsonObject;
	// Store the contents filename into jsonObject
	if (file.is_open()) {
	  file >> jsonObject;
	}
	string fileName;
	fileName.append(argv[1]);*/
    
    AVL T;

    /*T.InsertH(22);
    T.InsertH(44); 
    T.InsertH(12);
    T.InsertH(56);
    T.InsertH(2);*/

    T.InsertH(1); 
    T.InsertH(2); 
    T.InsertH(3); 

    
    cout << T.JSON() << endl; 

}



#endif 


