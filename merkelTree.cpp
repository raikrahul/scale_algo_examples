#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <openssl/sha.h>

// Node structure for the Merkle tree
struct MerkleNode
{
    std::string data;
    std::string hash;
    MerkleNode *left;
    MerkleNode *right;

    // Constructor
    MerkleNode(const std::string &data) : data(data), left(nullptr), right(nullptr) {}

    // Compute the hash value for the node
    void computeHash()
    {
        if (left && right)
        {
            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, left->hash.c_str(), left->hash.size());
            SHA256_Update(&sha256, right->hash.c_str(), right->hash.size());
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256_Final(hash, &sha256);
            char buf[SHA256_DIGEST_LENGTH * 2 + 1];
            for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
            {
                sprintf(buf + (i * 2), "%02x", hash[i]);
            }
            buf[SHA256_DIGEST_LENGTH * 2] = '\0';
            this->hash = buf;
        }
    }
};

// Merkle tree class
class MerkleTree
{
private:
    MerkleNode *root;

    // Create a Merkle tree recursively
    MerkleNode *createTree(const std::vector<std::string> &data, int start, int end)
    {
        if (start == end)
        {
            return new MerkleNode(data[start]);
        }
        int mid = (start + end) / 2;
        MerkleNode *left = createTree(data, start, mid);
        MerkleNode *right = createTree(data, mid + 1, end);
        MerkleNode *node = new MerkleNode("");
        node->left = left;
        node->right = right;
        node->computeHash();
        return node;
    }

    // Print the Merkle tree recursively
    void printTree(MerkleNode *node, int level)
    {
        if (node)
        {
            std::cout << std::setw(level * 4) << ""
                      << "|_" << node->data << " : " << node->hash << std::endl;
            printTree(node->left, level + 1);
            printTree(node->right, level + 1);
        }
    }

public:
    // Constructor
    MerkleTree(const std::vector<std::string> &data)
    {
        int n = data.size();
        root = createTree(data, 0, n - 1);
    }

    // Print the Merkle tree
    void print()
    {
        printTree(root, 0);
    }

    MerkleNode *getRoot() const
    {
        return root;
    }
};

// Function to compare two Merkle trees
bool compareMerkleTrees(MerkleNode* root1, MerkleNode* root2) {
    if (!root1 && !root2) {
        // Both trees are empty, so they are consistent
        return true;
    }
    else if (!root1 || !root2) {
        // One tree is empty while the other is not, so they are inconsistent
        return false;
    }
    else if (root1->hash == root2->hash) {
        // Root node hashes are equal, so the trees are consistent
        return true;
    }
    else {
        // Root node hashes are different, continue traversal
        bool leftConsistent = compareMerkleTrees(root1->left, root2->left);
        bool rightConsistent = compareMerkleTrees(root1->right, root2->right);
        return leftConsistent && rightConsistent;
    }
}


int main()
{
    std::vector<std::string> data = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    MerkleTree merkleTree(data);
    merkleTree.print();

    std::vector<std::string> data1 = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    std::vector<std::string> data2 = {"A", "B", "C", "eeeee", "E", "F", "G", "H", "I", "J"};

    MerkleTree tree1(data1); // Construct the first Merkle tree
    MerkleTree tree2(data2); // Construct the second Merkle tree

    // Compare the two Merkle trees
    bool isConsistent = compareMerkleTrees(tree1.getRoot(), tree2.getRoot());
    if (isConsistent)
    {
        std::cout << "The Merkle trees are consistent." << std::endl;
    }
    else
    {
        std::cout << "The Merkle trees are inconsistent." << std::endl;
    }

    return 0;
}
