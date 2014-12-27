/**
  * This file includes an implementation of Segment Trees, as well as data structures for the
  * Range Minimum Query and Lowest Common Ancestor problems. These are all in the same file
  * because they are closely related - RMQ can be implemented as a special case of a segtree,
  * while LCA can be reduced to RMQ on a specially constructed vector.
  */

#include <vector>
#include <string>
#include <iostream>

using namespace std;

/**
 * Segment tree on a dynamic vector.
 * Ctor args: (a, func, left = -1, right = -1)
 *    a: a vector<T> of values
 *    func: Any associative function which describes how to take two subtree vals
 *          and combine them to get the val for this entire tree (e.g. max).
 *    left, right: Int indices indicating the slice of the vector that this SegTree represents.
 *                 Defaults to representing the entire vector.
 *
 * Has operations:
 *    T rangeQuery(int left, int right)
 *    void update(int idx, T newVal)
 */
template <typename T>
struct SegTree {
    int left, right, mid;
    T value;
    vector<SegTree<T> > children;
    T (*func)(T, T);

    SegTree() {}
    SegTree(const vector<T> &a, T (*func)(T, T), int left = -1, int right = -1) {
        if (right == -1) {
            left = 0;
            right = a.size() - 1;
        }
        this->left = left;
        this->right = right;
        this->mid = (left + right)/2;
        this->func = func;

        if (left == right) {
            this->value = a[left]; return;
        }

        SegTree<T> leftChild = SegTree<T>(a, func, left, this->mid);
        SegTree<T> rightChild = SegTree<T>(a, func, this->mid+1, right);
        this->children.push_back(leftChild);
        this->children.push_back(rightChild);
        this->value = (*func)(leftChild.value, rightChild.value);
        return;
    }

    T rangeQuery(int left, int right) {
        // Expects left < right
        if (left == this->left and right == this->right) {
            return this->value;
        }
        if (right <= this->mid) {
            return this->children[0].rangeQuery(left, right);
        }
        if (left > this->mid) {
            return this->children[1].rangeQuery(left, right);
        }

        T leftAns = this->children[0].rangeQuery(left, this->mid);
        T rightAns = this->children[1].rangeQuery(this->mid+1, right);
        return (*(this->func))(leftAns, rightAns);
    }

    void update(int idx, T newVal) {
        if (this->left == this->right) {
            this->value = newVal;
            return;
        }
        if (idx <= this->mid) {
            this->children[0].update(idx, newVal);
        }
        else if (idx > this->mid) {
            this->children[1].update(idx, newVal);
        }

        this->value = (*(this->func))(this->children[0].value, this->children[1].value);
        return;
    }
};


/**
 * Templated RMQ struct which inherits from SegTree.
 * It's just a SegTree with this->func = getMin()
 * Ctor args:
 *      RMQ(const vector<T> &a)
 *
 * Has the same operations as SegTree:
 *      T rangeQuery(int left, int right)
 *      void update(int idx, T newVal)
 */
template<typename T>
struct RMQ : SegTree<T> {
    static T getMin(T a, T b) {return min(a, b);} // Hope that min() knows how to do it!
    RMQ(const vector<T>& a) : SegTree<T>(a, RMQ<T>::getMin) {}
    RMQ() {}
};

/**
 * LCA on a static directed tree, implemented by transforming the problem into an RMQ instance.
 * Ctor args:
 *      LCA(int root, const vector<vector<int> > &neighbors)
 *
 * Has operation:
 *      int lcaQuery(int node1, int node2)
 */
struct LCA {
    /* We're going to make a vector of ints such that LCA on two nodes corresponds
       to RMQ between some two indices of the vector. We need to be able to translate
       nodes into indices and vice versa.
    */
    vector<int> nodeToRMQIndex;
    RMQ<pair<int, int> > rmq; // will hold <depth, node> pairs

    LCA(int root, const vector<vector<int> > &neighbors) {
        this->nodeToRMQIndex.resize(neighbors.size());

        // Need to construct vector such that LCA corresponds to RMQ.
        vector<pair<int, int> > rmqVec; // <depth, node>
        int currDepth = 0;
        this->dfs(root, currDepth, neighbors, rmqVec); // populates rmqVec and nodeToRMQIndex
        this->rmq = RMQ<pair<int, int> >(rmqVec);
    }

    void dfs(int node, int currDepth, const vector<vector<int> > &neighbors, vector<pair<int, int> > &rmqVec) {
        // The whole point is to add the dfs paths to rmqVec, so expect rmqVec to be modified.
        // Also changes nodeToRMQIndex
        pair<int, int> curr = pair<int, int>(currDepth, node);
        rmqVec.push_back(curr);
        this->nodeToRMQIndex[node] = rmqVec.size() - 1; // We're writing the depth of node in rmqVec multiple times, but
                                                        // it turns out that we can use any of those positions in rmqVec
                                                        // for doing the RMQ calculation which corresponds to LCA
        for (int i = 0; i < neighbors[node].size(); i++) {
            this->dfs(neighbors[node][i], currDepth + 1, neighbors, rmqVec);
            rmqVec.push_back(curr);
        }
    }

    int lcaQuery(int node1, int node2) {
        int rmqIndex1 = this->nodeToRMQIndex[node1];
        int rmqIndex2 = this->nodeToRMQIndex[node2];
        if (rmqIndex1 > rmqIndex2) {swap(rmqIndex1, rmqIndex2);}
        pair<int, int> result = this->rmq.rangeQuery(rmqIndex1, rmqIndex2); // <depth, node>
        return result.second;
    }
};


bool testSegTrees = false;
bool testRMQ = false;
bool testLCA = true;
int main(void) {
    if (testLCA) {
        /* Test it on the following tree:    1
                                           2   5
                                          3 4
        */
        vector<vector<int> > neighbors;
        neighbors.resize(6); // 1 indexed
        neighbors[1].push_back(2); neighbors[1].push_back(5);
        neighbors[2].push_back(3); neighbors[2].push_back(4);

        LCA lcaTest(1, neighbors);
        cout << lcaTest.lcaQuery(3, 4) << "\n";
        cout << lcaTest.lcaQuery(3, 5) << "\n";
        cout << lcaTest.lcaQuery(2, 3) << "\n";
        cout << lcaTest.lcaQuery(4, 4) << "\n";
        cout << lcaTest.lcaQuery(5, 3) << "\n";

        // Outputs 2, 1, 2, 4, 1 as it should.
    }

}
