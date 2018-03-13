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
		root_->left_.reset();
		root_->right_.reset(); 
		root_ = nullptr;
		size_--; assert(size_ == 0);
	} else {
		if (parent->right_ == currentNode) {
			currentNode->parent_.reset();
			parent->right_ = nullptr;
			currentNode->left_.reset();
			currentNode->right_.reset();
		} else if (parent->left_ == currentNode) {
			currentNode->parent_.reset();
			parent->left_ = nullptr;
			currentNode->left_.reset();
			currentNode->right_.reset();
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
    	  parent->left_ = std::make_shared<AVLNode>(key, parent); 
       }
       else
    	{
          parent->right_ = std::make_shared<AVLNode>(key, parent);
        }
     
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
    else
    {
    	std::cerr << "AVLNode::Duplicate Error: duplicate node\n";
		exit(EXIT_FAILURE);
    }

    node->height_ = 1 + max(Height(node->left_), Height(node->right_)); 
    node->bf_ = Height(node->right_) - Height(node->left_);
    
}


void AVL::rebalence(int key, shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent)
{
	
	if (Height(node->right_) - Height(node->left_) == -2) //LeftLeft or LeftRight case, tree left heavy
    {
		if (node->left_ && key < node->left_->key_)	//Single rotation, LeftLeft case
		{
			node = rightRotation(node->left_, node);
			if (parent == nullptr)//its the root 
				root_ = node;
			else
			{
				node->parent_ = parent;
        		if (parent->HasRightChild() && parent->right_ == node->right_)  
        		  parent->right_ = node;
        		else
        		  parent->left_ = node;
			}
		}
		else	//LeftRight case
		{	
			if (node->left_->right_)
			{
			    assert(node->left_ != nullptr);	
				node->left_ = leftRotation(node->left_->right_, node->left_);  
				node = rightRotation(node->left_, node); 
				if (parent == nullptr)//its the root 
		    	    root_ = node;
			    else
			    {
			    	node->parent_ = parent;
	        		if (parent->HasRightChild() && parent->right_ == node->right_)  
	        		  parent->right_ = node;
	        		else
	        		  parent->left_ = node;
			    }
			}
		}
    }
	if (Height(node->right_) - Height(node->left_) == 2) //RightRight or RightLeft case
    {
		if (node->right_ && key > node->right_->key_) //RR case 
		{
			node = leftRotation(node->right_,node); 
			if (parent == nullptr)//its the root 
				root_ = node; 
			else
			{
				node->parent_ = parent;
        		if (parent->HasRightChild() && parent->right_ == node->left_)  
        		  parent->right_ = node;
        		else
        		  parent->left_ = node;
			}
		}
		else //RightLeft case
		{
			if (node->right_->left_)
			{
				assert(node->right_ != nullptr);
				node->right_ = rightRotation(node->right_->left_, node->right_);
				node = leftRotation(node->right_, node);
				if (parent == nullptr)//its the root 
	            	root_ = node;
	            else
	            {
	            	node->parent_ = parent;
	               
	        		if (parent->HasRightChild() && parent->right_ == node->left_)  
	        		  parent->right_ = node;
	        		else
	        		  parent->left_ = node;
	            }
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
	shared_ptr<AVLNode> node = findMin(root_); 

    deleteH(node->key_, root_);
}

void AVL::DeleteH(int key)
{
	deleteH(key, root_); 
}

void AVL::deleteH(int key, shared_ptr<AVLNode> currentNode )
{
    shared_ptr<AVLNode> lastNode; 

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
			else  // key == to currentNode key   
			{
				if (currentNode->left_ == nullptr || currentNode ->right_ == nullptr ) // only one child, just swap
				{
					if (currentNode->left_ != nullptr)
					{
						std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
						if (parent != nullptr)
						{
				        	lastNode = currentNode->parent_.lock();
				        	parent->ReplaceChild(currentNode, currentNode->left_);
				        }
				        else//Root case
						{
							root_= currentNode->left_; 
						}
				        currentNode->parent_.reset();
						currentNode->left_.reset();
                        size_--;
					}
					else if (currentNode->right_ != nullptr)
					{
						std::shared_ptr<AVLNode> parent = currentNode->parent_.lock(); 
						if (parent != nullptr)
						{
				        	lastNode = currentNode->parent_.lock(); 

				        	parent->ReplaceChild(currentNode, currentNode->right_);
				        }
				        else//Root cases
						{
							root_= currentNode->right_; 
						}
						currentNode->parent_.reset();
						currentNode->right_.reset();
					    size_--;
					}
					else
					{	// deleting a leaf 
						std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
						if (parent)
						{
							if (parent->right_ == currentNode)
								parent->right_.reset();
							else
								parent->left_.reset();

							lastNode = currentNode->parent_.lock();

							currentNode->parent_.reset();
							currentNode = nullptr;
						    size_--; 
						    return;
						}
						else
					    {
					    	root_->left_.reset();
		                    root_->right_.reset(); 
	                    	root_ = nullptr;
	                    	size_--; 
	                    	return;
					    }

					}
					
				}
				else // there are two kids 
				{
		        	shared_ptr<AVLNode> getMin = findMin(currentNode->right_);

					currentNode->key_ = getMin->key_; 
		        
		        	deleteH(currentNode->key_, currentNode->right_);

				}
		    }
		}
	}

    if (lastNode)
    {
		currentNode->height_ = 1 + max(Height(currentNode->left_), Height(currentNode->right_)); 
		currentNode->bf_ = Height(currentNode->right_) - Height(currentNode->left_);

		if (Height(currentNode->right_) - Height(currentNode->left_) <= -2)//(currentNode->bf_ == - 2)  // left heavy 
		{
		    if (Height(currentNode->left_-> left_) > Height(currentNode->left_->right_)) //LeftLeft Case
		    {
	            currentNode = rightRotation(currentNode->left_, currentNode);
				if (lastNode == nullptr)  // its the root 
					root_ = currentNode;
				else
				{
					currentNode->parent_ = lastNode;
	        		if (lastNode->HasRightChild() && lastNode->right_ == currentNode->right_)  
	        		  lastNode->right_ = currentNode;
	        		else
	        		  lastNode->left_ = currentNode;
				}

		    }
		    else 
		    {
		    	if (currentNode->left_->right_)
		    	{
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
			    }
		    }
		}
		if (Height(currentNode->right_) - Height(currentNode->left_) >= 2)//(currentNode->bf_ == 2) //Right heavy
		{
			if (Height(currentNode->right_->right_) >= Height(currentNode->right_->left_)) // bug was < or <= 
			{
			   currentNode = leftRotation(currentNode->right_,currentNode); //RR case 
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
	    		if (currentNode->right_->left_)
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
		}
		currentNode->height_ = 1 + max(Height(currentNode->left_), Height(currentNode->right_)); 
	    currentNode->bf_ = Height(currentNode->right_) - Height(currentNode->left_);   
    }
}


shared_ptr<AVLNode> AVL::findMin(shared_ptr<AVLNode> node)
{
	if( node == NULL)
     	return node;

     while( node->left_ != NULL )
     {
     	node = node->left_;
     }
     return node;
}



int main(int argc, char** argv) //Takes a json file with AVL commands, Insert, Delete, or DeleteMin 
{
	ifstream file;
	file.open(argv[1]);
	nlohmann::json jsonObject;
	// Store the contents filename into jsonObject
	if (file.is_open()){
		file >> jsonObject;
	}
	string fileName;
	fileName.append(argv[1]);
	    
	string opnum;
	int count = 0;  

	AVL T; //Declaring AVL object

	for (auto itr = jsonObject.begin(); itr != (--jsonObject.end()); ++itr)
	{ 
	    opnum = itr.key();

	    if(jsonObject[opnum]["operation"] != "DeleteMin")
	    {
		    int key = jsonObject[opnum]["key"];
		   	if (jsonObject[opnum]["operation"]== "Delete") 
		   	{ 
		   		    count--;
		        	T.DeleteH(key); 
		    }
		    else
		    {
		            count++; 	   
		        	T.InsertH(key);
		    }   
	    }
	    else if (jsonObject[opnum]["operation"] == "DeleteMin")
	    {
	    	count--;
			T.DeleteMinH(); 
	    }
	    if (count == -1)
	    {
	    	return 0;
	    }
	}

      cout << T.JSON() << endl;

}

#endif 


