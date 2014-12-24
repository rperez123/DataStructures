#include <vector>
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

    SegTree(vector<T> &a, T (*func)(T, T), int left = -1, int right = -1) {
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

        SegTree<T> leftChild = SegTree(a, func, left, this->mid);
        SegTree<T> rightChild = SegTree(a, func, this->mid+1, right);
        this->children.push_back(leftChild);
        this->children.push_back(rightChild);
        this->value = (*func)(leftChild.value, rightChild.value);
        return;
    }

    T rangeQuery(int left, int right) {
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

template <typename T> T getMin(T a, T b) {
    return min(a, b); // Hope that min() knows how to do it!
}

/**
 * Would be nice to have a templated RMQ struct which inherits from SegTree.
 * It would be as simple as setting this->func = getMin()
 * But I was having problems with calling the SegTree ctor from RMQ.
 */
/*
template <typename T>
struct RMQ : public SegTree<T> {
    RMQ(vector<int> &a, int left = -1, int right = -1) {
        this = SegTree<T>(a, getMin, left, right);
    }
};
*/

/**
 * LCA on a static tree, implemented by transforming the problem into an RMQ instance.
 * Ctor args:
 *
 * Has operations:
 *
 */
struct LCA {
    /* We're going to make a vector of ints such that LCA on two nodes corresponds
       to RMQ between some two indices of the vector. We need to be able to translate
       nodes into indices and vice versa.
    */
    vector<int> nodeToRMQIndex;
    vector<int> RMQindexToNode;
    SegTree rmq;

    LCA(int root, vector<vector<int> > neighbors) {
        vector<int> rmqVec; // We'll construct this with a DFS

    }
};




int mx(int a, int b) {return min(a, b);}

bool testSegTrees = false;
bool testRMQ = true;
int main(void) {
    if (testSegTrees) {
        vector<int> a;
        for (int i = 0; i < 10; i++) {
            a.push_back((3*i + 2) % 10);
        }
        SegTree<int> st = SegTree<int>(a, mx);
        cout << "Vector a:\n";
        for (int i = 0; i < a.size(); i++) {
            cout << a[i] << " ";
        }
        cout << "\n" << st.rangeQuery(0, 5);
        st.update(3, 9);
        cout << "\n" << st.rangeQuery(0, 5);
        st.update(0, 9);
        cout << "\n" << st.rangeQuery(0, 5);
    }
    if (testRMQ) {
        vector<int> a;
        for (int i = 0; i < 10; i++) {
            a.push_back((3*i+2) % 10);
        }
        RMQ<int> aRMQ = RMQ<int>(a);
        cout << "Look at a:\n";
        for (int i = 0; i < a.size(); i++) {
            cout << a[i] << " ";
        }
        cout << "\n" << aRMQ.rangeQuery(3, 5);
    }
}
