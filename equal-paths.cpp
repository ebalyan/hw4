#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool equalPathsHelper(Node* root, int depth, int& leafDepth);

bool equalPaths(Node * root)
{
    // Add your code below
    int leafDepth = -1;
    return equalPathsHelper(root, 0, leafDepth);
}

bool equalPathsHelper(Node* root, int depth, int& leafDepth)
{
    if(root == nullptr) {
        return true;
    }

    if(root->left == nullptr && root->right == nullptr) {
        if(leafDepth == -1) {
            leafDepth = depth;
            return true;
        }

        return depth == leafDepth;
    }

    return equalPathsHelper(root->left, depth + 1, leafDepth) &&
           equalPathsHelper(root->right, depth + 1, leafDepth);
}

