#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
using namespace std;
class TreeNode {
public:
    int id;
    vector<TreeNode*> children;
    int subtree_size;
    TreeNode(int id) : id(id), subtree_size(1) {}
};
class Tree {
private:
    TreeNode* root;
    unordered_map<int, TreeNode*> nodes;
    void SubtreeSizesHelper(TreeNode* node) {
        for (TreeNode* child : node->children) {
            SubtreeSizesHelper(child);
            node->subtree_size += child->subtree_size;
        }
    }
public:
    Tree(int root_id, const vector<pair<int, int>>& edges) {
        buildTree(root_id, edges);
    }
    void buildTree(int root_id, const vector<pair<int, int>>& edges) {
        root = new TreeNode(root_id);
        nodes[root_id] = root;
        unordered_map<int, vector<int>> adj;
        for (const auto& edge : edges) {
            int u = edge.first;
            int v = edge.second;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        queue<int> q;
        q.push(root_id);
        unordered_set<int> visited;
        visited.insert(root_id);
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int v : adj[u]) {
                if (visited.find(v) == visited.end()) {
                    TreeNode* child = new TreeNode(v);
                    nodes[u]->children.push_back(child);
                    nodes[v] = child;
                    visited.insert(v);
                    q.push(v);
                }
            }
        }
    }
    void SubtreeSizes() {
        SubtreeSizesHelper(root);
    }
    vector<int> getSubtreeSizes(int V) {
        vector<int> res(V + 1, 0);
        for (const auto& pair : nodes) {
            res[pair.first] = pair.second->subtree_size;
        }
        return res;
    }
};
int main() {
    int V;
    cin >> V;
    vector<pair<int, int>> edges(V - 1);
    for (int i = 0; i < V - 1; ++i) {
        cin >> edges[i].first >> edges[i].second;
    }
    Tree tree(1, edges);
    tree.SubtreeSizes();
    vector<int> subtree_sizes = tree.getSubtreeSizes(V);
    for (int i = 1; i <= V; ++i) {
        cout << subtree_sizes[i] << " ";
    }
    return 0;
}
