#include <memory>
#include <string>

using namespace std; 
//using namespace nlohmann;

class AVL;

class AVLNode {
 public:
 	AVLNode(int key);
 	AVLNode(int key, std::weak_ptr<AVLNode> parent);
 	bool IsLeaf() const;
 	bool IsMissingChild() const;
 	bool HasLeftChild() const;
 	bool HasRightChild() const;
 	void DeleteChild(std::shared_ptr<AVLNode> v);
 	void ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u);

 	//int Height(shared_ptr<AVLNode> t);

 	int max(int leftH, int rightH); 


 protected:
	 int bf_; //Balence factor
	 int key_;
	 int height_; 
	 std::weak_ptr<AVLNode> parent_;
	 std::shared_ptr<AVLNode> left_;
	 std::shared_ptr<AVLNode> right_;
 
 friend class AVL; 

}; // class AVLNode

class AVL {
 public:
 	AVL();

 	void Insert(int key);
 	bool Delete(int key);
 	bool Find(int key) const;
 	std::string JSON() const;
 	size_t size() const;
 	bool empty() const;
 	int DeleteMin();

    void InsertH(int key); //creates node and calls recursive insert()
    void DeleteMinH(); //Deletes Min element in tree, calls recursive delete min


 protected:
    void insert(int key, shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent); 
    void deleteMin(std::shared_ptr<AVLNode> currentNode); 

 	int Height(shared_ptr<AVLNode> t);
	void DeleteLeaf(std::shared_ptr<AVLNode> currentNode);
	int DeleteMin(std::shared_ptr<AVLNode> currentNode);

	// Balance Functions
	shared_ptr<AVLNode> rightRotation(shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent);
	shared_ptr<AVLNode> leftRotation(shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent); 
	void rebalence(int key, shared_ptr<AVLNode> node, shared_ptr<AVLNode> parent); 

	//Members
 	std::shared_ptr<AVLNode> root_;
 	size_t size_;
}; // class AVL

