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
#include <unordered_map>
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
    using inorderIdxType = vector<int>::size_type;
    inorderIdxType const invalidInorderIdx = (std::numeric_limits<inorderIdxType>::max)();
    using preorderValToInorderIdxType = std::span<inorderIdxType>;
    enum : int { valHalfRange = 3001 };

    /*
        Time = O(n)   [there is no longer a nested find()/loop]
        Space = O(n)  [for call stack]
    */
    TreeNode* buildTree(
        span<int const> preorder
        , span<int const> inorder
        , preorderValToInorderIdxType const& preorderValToInorderIdx
        , inorderIdxType inorderIdxOffset = 0
    ) {
        assert(preorder.size() == inorder.size());
        auto const no_more_nodes = preorder.empty() || inorder.empty();
        if (no_more_nodes) {
            return nullptr;
        } else {
            auto parent = std::make_unique<TreeNode>(preorder[0]);

            auto const preorderValToInorderIdxSpanSize = preorderValToInorderIdx.size();
            auto const nodeValueIsValid = 0 - valHalfRange < parent->val && valHalfRange > parent->val;
            assert(nodeValueIsValid);
            if (nodeValueIsValid) {
                auto const inorderParentIdx = preorderValToInorderIdx[valHalfRange + parent->val] - inorderIdxOffset;
                assert(inorder[inorderParentIdx] == parent->val);

                // Process left sub-tree (if any).
                auto const inorderLeft = inorder.first(inorderParentIdx);
                auto const preorderLeft = preorder.subspan(1, inorderLeft.size());
                parent->left = buildTree(preorderLeft, inorderLeft, preorderValToInorderIdx, inorderIdxOffset);
                
                // Process right sub-tree (if any).
                auto const inorderRight = inorder.last(inorder.size() - inorderParentIdx - 1);
                auto const preorderRight = preorder.last(inorderRight.size());
                parent->right = buildTree(preorderRight, inorderRight, preorderValToInorderIdx, inorderIdxOffset + inorderParentIdx + 1);
            }
            
            return parent.release();
        }
    }

    /*
        Time = O(n + n) => O(2n) => O(n)
               n for visiting each node value to build preorder value -> inorder [array] index map.
               n for recusrively visiting every tree node.
        Space = O(6002 + n + n) => O(2n) => O(n)
                6002 for preorder value -> inorder [array] index map.
                n for hash table used to build preorder value -> inorder [array] index map.
                n for recusrively visiting every tree node.
    */
    TreeNode* buildTree(vector<int> const& preorder, vector<int> const& inorder) {
        // Map inorder values to their corresponding array indexes.
        // This is only used to construct the preorder value -> inorder [array] index map.
        std::unordered_map<int, vector<int>::size_type> inorderValToIdx{};
        for (vector<int>::size_type idx = 0; inorder.size() > idx; ++idx) {
            inorderValToIdx[inorder[idx]] = idx;
        }
        
        // Populate map: preorder value -> inorder [array] index
        std::array<inorderIdxType, valHalfRange * 2> preorderValsToInorderIdxs{};
        preorderValsToInorderIdxs.fill(invalidInorderIdx);
        for (inorderIdxType idx = 0; preorder.size() > idx; ++idx) {
            auto const preorderVal = preorder[idx];
            auto const inorderMapIter = inorderValToIdx.find(preorderVal);
            assert(inorderValToIdx.end() != inorderMapIter);
            if (inorderValToIdx.end() != inorderMapIter) {
                preorderValsToInorderIdxs[valHalfRange + preorderVal] = inorderMapIter->second;
            }
        }

        return buildTree(span(preorder), span(inorder), preorderValsToInorderIdxs);
    }
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
