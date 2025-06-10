#include <iostream>
#include <vector>
#include <string>

template <class T>
class BST {
public:
//составляем структуру класса
    struct Node {
        Node* Left;//левый потомок
        Node* Right;//правый потомок
        T Value;//значение
        Node(T val) : Left(nullptr), Right(nullptr), Value(val) {}
    };
    BST() : root(nullptr) {}//конструктор(пустое дерево)
    ~BST() { clear(root); }//деконструктор
    
//основные методы бст дерева
    //функция добавление эелемента
    void Add(const T& value) {
        root = vstavka(root, value);
    }
    //функция поисеа
    Node* Find(const T& value) {
        return poisk(root, value);
    }
    //функция удаления
    void Remove(Node* node) {
        root = udalenie(root, node->Value);
    }
//обход дерева
    void obhod(Node* node) {
        if (node == nullptr) return;
        obhod(node->Left);
        obhod(node->Right);
        std::cout << node->Value << " ";
    }
//форматируем деерво в строку
    std::string serialize() {
        if (root == nullptr) return "";
        std::vector<Node*> cur = { root };
        std::string result;
        bool hasNodes = true;

        while (hasNodes) {
            hasNodes = false;
            std::vector<Node*> next;
            std::string levelStr;

            for (Node* node : cur) {
                if (!levelStr.empty()) levelStr += ",";

                if (node == nullptr) {
                    levelStr += "N";
                    next.push_back(nullptr);
                    next.push_back(nullptr);
                }
                else {
                    levelStr += std::to_string(node->Value);
                    next.push_back(node->Left);
                    next.push_back(node->Right);
                    if (node->Left != nullptr || node->Right != nullptr) hasNodes = true;
                }
            }

            if (!result.empty()) result += ",";
            result += levelStr;
            cur = next;
        }

        size_t lastNonN = result.find_last_not_of(",N");
        if (lastNonN != std::string::npos) {
            result = result.substr(0, lastNonN + 1);
        }

        return result;
    }
    //форматируем строку в дерево
    void deserialize(const std::string& data) {
        clear(root);
        root = nullptr;
        if (data.empty()) return;

        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = data.find(',');

        while (end != std::string::npos) {
            tokens.push_back(data.substr(start, end - start));
            start = end + 1;
            end = data.find(',', start);
        }
        tokens.push_back(data.substr(start));

        if (tokens.empty() || tokens[0] == "N") return;

        root = new Node(ZZZ(tokens[0]));
        std::vector<Node*> cur = { root };
        size_t i = 1;

        while (!cur.empty() && i < tokens.size()) {
            std::vector<Node*> next;

            for (Node* node : cur) {
                if (node == nullptr) {
                    i += 2;
                    next.push_back(nullptr);
                    next.push_back(nullptr);
                    continue;
                }

                if (i < tokens.size() && tokens[i] != "N") {
                    node->Left = new Node(ZZZ(tokens[i]));
                    next.push_back(node->Left);
                }
                else {
                    next.push_back(nullptr);
                }
                i++;

                if (i < tokens.size() && tokens[i] != "N") {
                    node->Right = new Node(ZZZ(tokens[i]));
                    next.push_back(node->Right);
                }
                else {
                    next.push_back(nullptr);
                }
                i++;
            }

            cur = next;
        }
    }

    Node* GetRoot() { return root; }

private:
    Node* root;

    void clear(Node* node) {
        if (node == nullptr) return;
        clear(node->Left);
        clear(node->Right);
        delete node;
    }
    //вставка
    Node* vstavka(Node* node, const T& value) {
        if (node == nullptr) {
            return new Node(value);
        }

        if (value < node->Value) {
            node->Left = vstavka(node->Left, value);
        }
        else if (value > node->Value) {
            node->Right = vstavka(node->Right, value);
        }

        return node;
    }
    //поиск
    Node* poisk(Node* node, const T& value) {
        if (node == nullptr || node->Value == value) {
            return node;
        }

        if (value < node->Value) {
            return poisk(node->Left, value);
        }
        else {
            return poisk(node->Right, value);
        }
    }
    //удаление
    Node* udalenie(Node* node, const T& value) {
        if (node == nullptr) return nullptr;

        if (value < node->Value) {
            node->Left = udalenie(node->Left, value);
        }
        else if (value > node->Value) {
            node->Right = udalenie(node->Right, value);
        }
        else {
            if (node->Left == nullptr) {
                Node* temp = node->Right;
                delete node;
                return temp;
            }
            else if (node->Right == nullptr) {
                Node* temp = node->Left;
                delete node;
                return temp;
            }

            Node* temp = findMin(node->Right);
            node->Value = temp->Value;
            node->Right = udalenie(node->Right, temp->Value);
        }

        return node;
    }

    Node* findMin(Node* node) {
        while (node->Left != nullptr) {
            node = node->Left;
        }
        return node;
    }

    T ZZZ(const std::string& s) {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(s);
        }
        else if constexpr (std::is_same_v<T, float>) {
            return std::stof(s);
        }
        else if constexpr (std::is_same_v<T, double>) {
            return std::stod(s);
        }
        else {
            return T{};
        }
    }
};

int main() {
    BST<int> bst;

    bst.Add(5);
    bst.Add(3);
    bst.Add(7);
    bst.Add(6);
    //обратный обход
    bst.obhod(bst.GetRoot());
    std::cout << std::endl;

    std::string serialized = bst.serialize();
    std::cout << serialized << std::endl;

    BST<int> bst2;
    bst2.deserialize(serialized);
    bst2.obhod(bst2.GetRoot());
    std::cout << std::endl;

    return 0;
}
