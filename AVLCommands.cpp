//Author: Mary Hamidi 
#include <iostream>
#include <fstream>
#include <string>


#include "AVLCommands.h"
#include "json.hpp"

// Made a little change 
// Mi amor es Mary Hamidi : Att Roman A S


using namespace std; 
using namespace nlohmann;


void AVL::InsertH(int key)
{
	//newnode = make_shared<BSTNode>(key);
	//shared_ptr<AVLNode> newNode = make_shared<AVLNode>(key);
	//cout << "Key: " << newNode->key_  << " Balance: " << newNode->balencefactor<< endl;
	if (root_ == nullptr)
    {
      root_ = std::make_shared<BSTNode>(key);
      size_++;
    }
    else
      insert(key, root_, nullptr); 
    
    //cout << newNode->height_ << endl;
}

void AVL::insert(int key, shared_ptr<BSTNode> node, shared_ptr<BSTNode> parent) //shared_ptr<BSTNode> newNode, shared_ptr<BSTNode> node)
{
    //std::shared_ptr<BSTNode> currentNode = root_;   //, lastNode = nullptr;
    if (node == nullptr)
    {
       if (key < parent->key_)
    	  parent->left_ = std::make_shared<BSTNode>(key, parent); 
    	else
          parent->right_ = std::make_shared<BSTNode>(key, parent);

         size_++; 
    	return; 
    }
    else if (key < node->key_)//( newNode->key_ < node->key_)
    {
        //node->left_ = std::make_shared<BSTNode>(key, lastNode);
    	insert(key, node->left_, node);
    	//insert(newNode, node->left_);
    }
    else if (key > node->key_)//(newNode->key_ > node->key_)
    {
    	insert(key, node->right_, node);
    	//insert(newNode, node->right_);
    }
    node->height_ = 1 + max(Height(node->left_), Height(node->right_)); 
    //newNode->height_ = 1 + max(Height(newNode->left_), Height(newNode->right_)); 
    
}

/*void AVL::Insert(int key) {
    if (root_ == nullptr) {
        root_ = std::make_shared<BSTNode>(key);
        size_++;
        return;
    }
    std::shared_ptr<BSTNode> currentNode = root_, lastNode = nullptr;
    while (currentNode != nullptr) {
        lastNode = currentNode;
        currentNode = (key < currentNode->key_) ?
            currentNode->left_ : currentNode->right_;
    }
    if (key < lastNode->key_) {
        lastNode->left_ = std::make_shared<BSTNode>(key, lastNode);
    } else {
        lastNode->right_ = std::make_shared<BSTNode>(key, lastNode);
    }
    size_++;
}*/

/*int AVLNode::Height(shared_ptr<BSTNode> t)
{
	if (t)
		return t->height_;
	else
		return -1; 
} */

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

    T.InsertH(22);
    T.InsertH(44); 
    T.InsertH(12);
    T.InsertH(56);
    T.InsertH(2);


    //T.Insert(2);
    //T.Insert(56);
    //T.Insert(2);
    //T.Insert(4);
    
    cout << T.JSON() << endl; 

	cout << "Hello world" << endl ; 

}



