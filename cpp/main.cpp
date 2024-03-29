/*!
    \file "main.cpp"

    Author: Matt Ervin <matt@impsoftware.org>
    Formatting: 4 spaces/tab (spaces only; no tabs), 120 columns.
    Doc-tool: Doxygen (http://www.doxygen.com/)

    https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/
*/

//!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/main.md
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <algorithm>
#include <cassert>
#include <chrono>
#include <doctest/doctest.h> //!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/tutorial.md
#include <iterator>
#include <memory>
#include <queue>
#include <vector>
#include <set>
#include <span>

using namespace std;

// [----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------]

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
    virtual ~TreeNode() {
        if (left) {
            delete left;
        }
        if (right) {
            delete right;
        }
    }
};

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    using InorderIdxType = vector<int>::size_type;
    enum : int { valHalfRange = 3001 };

    /*
        Time = O(n)
        Space = O(n)  [call stack]

        This function is so optimized that it may be difficult to understand.
        
        This function recursively visits each node in the tree in PRE-order:
        The node is created and then its left sub-tree is created and then 
        its right sub-tree is created.
        NOTE: Visiting the nodes in PRE-order follows the same pattern as a
              binary search.  

        The function arguments define the binary search range in the 
        'inorder' array, however the 'inorder' array itself is not actually 
        required by this function.  The binary search range is only tracked
        by this function to detect leaf nodes in the tree, or more accurately,
        when this function has traversed beyond a leaf node and should return.

        This function uses the 'nodeValToInorderIdx' array to translate each 
        node value to its corresponding index in the 'inorder' array.  This 
        index, the midpoint of the binary search range, is then used to 
        calculate the [begin, end) of the left and right sub-trees for the 
        next recursive call.

        Several function arguments have been converted to class members in an 
        attempt to improve performance and simplify the implementation.
    */
    TreeNode* buildTree(size_t inorderBegin, size_t inorderEnd) {
        auto const there_are_nodes_to_process = inorderBegin < inorderEnd;
        if (there_are_nodes_to_process) {
            auto const nodeVal = (*preorder_)[preorderIdx_++];
            auto const inorderMid = nodeValToInorderIdx_[valHalfRange + nodeVal] - 1;
            return new TreeNode{
                std::move(nodeVal)
                , buildTree(inorderBegin, inorderMid)
                , buildTree(inorderMid + 1, inorderEnd)
            };
        }

        return nullptr;
    }

    /*
        Time = O(n + n) => O(2n) => O(n)
               n for visiting each node value to build preorder value -> inorder [array] index map.
               n for recusrively visiting every tree node.
        Space = O(6002 + n) => O(n)
                6002 for preorder value -> inorder [array] index map.
                n for recusrively visiting every tree node.
    */
    TreeNode* buildTree(vector<int> const& preorder, vector<int> const& inorder) {
        assert(preorder.size() == inorder.size());

        // Populate xlat table: node value -> inorder array index
        for (InorderIdxType idx = 0; inorder.size() > idx; ++idx) {
            nodeValToInorderIdx_[valHalfRange + inorder[idx]] = idx + 1;
        }

        preorder_ = &preorder;
        preorderIdx_ = 0;
        return buildTree(0, inorder.size());
    }

private:
    vector<int> const* preorder_{};
    size_t preorderIdx_{};
    InorderIdxType nodeValToInorderIdx_[valHalfRange * 2];
};

// [----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------]

struct elapsed_time_t
{
    std::chrono::steady_clock::time_point start{};
    std::chrono::steady_clock::time_point end{};
    
    elapsed_time_t(
        std::chrono::steady_clock::time_point start
        , std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now()
    ) : start{std::move(start)}, end{std::move(end)} {}
};

std::ostream&
operator<<(std::ostream& os, elapsed_time_t const& et)
{
    auto const elapsed_time = et.end - et.start;
    os << std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count()
       << '.' << (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count() % 1000)
       << " ms";
    return os;
}

TreeNode* createBst(std::vector<int> values) {
    TreeNode* root = nullptr;

    for (auto value : values) {
        // Search for parent node, if any.
        TreeNode* parent{};
        for (TreeNode* iter = root; iter; ) {
            parent = iter;
            iter = value < iter->val ? iter->left : iter->right;
        }
        
        // Find parent pointer.
        TreeNode** parentPtr{};
        if (parent) {
            if (value < parent->val) {
                parentPtr = &parent->left;
            } else {
                parentPtr = &parent->right;
            }
        } else {
            parentPtr = &root;
        }
        
        // Allocate new node.
        if (parentPtr) {
            *parentPtr = new TreeNode{std::move(value)};

            if (!root) {
                root = *parentPtr;
            }
        }
    }

    return root;
}

std::vector<int> inorder(TreeNode const* root) {
    std::vector<int> result{};

    if (root) {
        auto const leftResult = inorder(root->left);
        std::copy(leftResult.begin(), leftResult.end(), std::back_inserter(result));

        result.push_back(root->val);

        auto const rightResult = inorder(root->right);
        std::copy(rightResult.begin(), rightResult.end(), std::back_inserter(result));
    }

    return result;
}

std::vector<int> preorder(TreeNode const* root) {
    std::vector<int> result{};

    if (root) {
        result.push_back(root->val);

        auto const leftResult = preorder(root->left);
        std::copy(leftResult.begin(), leftResult.end(), std::back_inserter(result));

        auto const rightResult = preorder(root->right);
        std::copy(rightResult.begin(), rightResult.end(), std::back_inserter(result));
    }

    return result;
}

TEST_CASE("Case 1")
{
    cerr << "Case 1" << '\n';
    std::vector<int> const preorderRep{3,9,20,15,7};
    std::vector<int> const inorderRep{9,3,15,20,7};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = Solution{}.buildTree(preorderRep, inorderRep);
        auto const resultPreorder = preorder(result);
        auto const resultInorder = inorder(result);
        CHECK(resultPreorder == preorderRep);
        CHECK(resultInorder == inorderRep);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

TEST_CASE("Case 2")
{
    cerr << "Case 2" << '\n';
    std::vector<int> const preorderRep{-1};
    std::vector<int> const inorderRep{-1};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = Solution{}.buildTree(preorderRep, inorderRep);
        auto const resultPreorder = preorder(result);
        auto const resultInorder = inorder(result);
        CHECK(resultPreorder == preorderRep);
        CHECK(resultInorder == inorderRep);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

TEST_CASE("Case 10")
{
    cerr << "Case 10" << '\n';
    std::vector<int> const preorderRep{};
    std::vector<int> const inorderRep{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = Solution{}.buildTree(preorderRep, inorderRep);
        auto const resultPreorder = preorder(result);
        auto const resultInorder = inorder(result);
        CHECK(resultPreorder == preorderRep);
        CHECK(resultInorder == inorderRep);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

/*
    End of "main.cpp"
*/
