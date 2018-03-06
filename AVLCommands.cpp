//Author: Mary Hamidi 
#include <iostream>
#include <fstream>
#include <string>


#include "AVLCommands.h"
#include "json.hpp"

// Made a little change 

using namespace std; 
//using namespace nlohmann;


void AVL::InsertH(int key)
{
	//newnode = make_shared<BSTNode>(key);
	//shared_ptr<AVLNode> newNode = make_shared<AVLNode>(key);
	//cout << "Key: " << newNode->key_  << " Balance: " << newNode->balencefactor<< endl;
	insert(key, tree.root_); //tree.root_, newNode);//shared_ptr<BSTNode> node);
    //cout << newNode->height_ << endl;
}

void AVL::insert(int key, shared_ptr<BSTNode> node)//shared_ptr<BSTNode> newNode, shared_ptr<BSTNode> node)
{
    if (node == nullptr)
    {
    	node = std::make_shared<BSTNode>(key);; 
    	size_++; 
    	return; 
    }
    else if (key < node->key_)//( newNode->key_ < node->key_)
    {
    	insert(key, node->left_);
    	//insert(newNode, node->left_);
    }
    else if (key < node->key_)//(newNode->key_ > node->key_)
    {
    	insert(key, node->right_);
    	//insert(newNode, node->right_);
    }
    node->height_ = 1 + max(Height(node->left_), Height(node->right_)); 
    //newNode->height_ = 1 + max(Height(newNode->left_), Height(newNode->right_)); 
    
}

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

    T.InsertH(33);
    T.InsertH(2); 
    T.InsertH(5);
    T.InsertH(6);
    cout << T.JSON() << endl; 

	cout << "Hello world" << endl ; 

}



