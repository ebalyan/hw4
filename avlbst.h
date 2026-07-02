#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    int height(AVLNode<Key, Value>* node) const;
    void updateNodeBalance(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    void rebalance(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    while(curr != NULL) {
        parent = curr;

        if(new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        }
        else if(new_item.first > curr->getKey()) {
            curr = curr->getRight();
        }
        else {
            curr->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newNode =
        new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if(new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    rebalance(parent);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* target =
    static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if(target == NULL) {
        return;
    }

    if(target->getLeft() != NULL && target->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(target));

        nodeSwap(target, pred);
    }

    AVLNode<Key, Value>* child = NULL;

    if(target->getLeft() != NULL) {
        child = target->getLeft();
    }
    else {
        child = target->getRight();
    }

    AVLNode<Key, Value>* parent = target->getParent();

    if(child != NULL) {
        child->setParent(parent);
    }

    if(parent == NULL) {
        this->root_ = child;
    }
    else if(parent->getLeft() == target) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete target;

    rebalance(parent);
}

template<class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) const
{
    if(node == NULL) {
        return 0;
    }

    int leftHeight = height(node->getLeft());
    int rightHeight = height(node->getRight());

    if(leftHeight > rightHeight) {
        return leftHeight + 1;
    }

    return rightHeight + 1;
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateNodeBalance(AVLNode<Key, Value>* node)
{
    if(node == NULL) {
        return;
    }

    int leftHeight = height(node->getLeft());
    int rightHeight = height(node->getRight());

    node->setBalance(rightHeight - leftHeight);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* newRoot = node->getRight();
    AVLNode<Key, Value>* beta = newRoot->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    newRoot->setParent(parent);

    if(parent == NULL) {
        this->root_ = newRoot;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(newRoot);
    }
    else {
        parent->setRight(newRoot);
    }

    newRoot->setLeft(node);
    node->setParent(newRoot);

    node->setRight(beta);
    if(beta != NULL) {
        beta->setParent(node);
    }

    updateNodeBalance(node);
    updateNodeBalance(newRoot);

    return newRoot;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* newRoot = node->getLeft();
    AVLNode<Key, Value>* beta = newRoot->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    newRoot->setParent(parent);

    if(parent == NULL) {
        this->root_ = newRoot;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(newRoot);
    }
    else {
        parent->setRight(newRoot);
    }

    newRoot->setRight(node);
    node->setParent(newRoot);

    node->setLeft(beta);
    if(beta != NULL) {
        beta->setParent(node);
    }

    updateNodeBalance(node);
    updateNodeBalance(newRoot);

    return newRoot;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    while(node != NULL) {
        updateNodeBalance(node);

         if(node->getBalance() < -1) {
            updateNodeBalance(node->getLeft());

            if(node->getLeft() != NULL && node->getLeft()->getBalance() > 0) {
                rotateLeft(node->getLeft());
            }

            AVLNode<Key, Value>* newRoot = rotateRight(node);
            node = newRoot->getParent();
        }
         else if(node->getBalance() > 1) {
            updateNodeBalance(node->getRight());

            if(node->getRight() != NULL && node->getRight()->getBalance() < 0) {
                rotateRight(node->getRight());
            }
             
            AVLNode<Key, Value>* newRoot = rotateLeft(node);
            node = newRoot->getParent();
        }
        else {
            node = node->getParent();
        }
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
