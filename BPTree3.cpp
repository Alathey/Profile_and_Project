#include <iostream>
using namespace std;

// create Node class
class Node
{
public:
    bool isLeaf;
    int *key, size; //節點、節點數量
    Node **ptr; //存指標
    Node(int capacity)
    {
        key = new int[capacity]{0};
        ptr = new Node *[capacity + 1]{NULL};
    }
};

// Tree class
class BPTree
{
private:
    int capacity;
    Node *root;
    bool containsValue(const int&, Node*);
    void remove(Node *);

public:
    // constructor
    BPTree(int capacity)
    {
        this->capacity = capacity;
        root = NULL;
    }
    // destructor
    ~BPTree()
    {
        cout << "delete node: ";
        remove(root);
        cout << endl;
    }

    Node *getRoot() { return root; }
    void display(Node *, int);
    void search(int);
    Node *insertValue(int, Node *);
    bool deleteVaule(int, Node *);
    void displayDoubleLinkList()
    {
        Node *current = root;
        cout << "doubleLink: " << endl;
        if (current != NULL)
        {
            while (current->isLeaf == false)
                current = current->ptr[0];
            while (true)
            {
                for (int i = 0; i < current->size; i++)
                    cout << current->key[i] << ", ";

                if (current->ptr[current->size] == NULL)
                {
                    cout << endl;
                    break;
                }
                current = current->ptr[current->size];
            }
            current = root;
            while (current->isLeaf == false)
                current = current->ptr[current->size];
            while (true)
            {
                for (int i = current->size - 1; i >= 0; i--)
                    cout << current->key[i] << ", ";

                if (current->ptr[0] == NULL)
                {
                    cout << endl;
                    break;
                }
                current = current->ptr[0];
            }
        }
    }
};
bool BPTree::containsValue(const int &x, Node *current = NULL)
{
    if (current == NULL)
        current = root;
    if (current->isLeaf == true)
    {
        for (int i = 0; i < current->size; i++)
        {
            if (x == current->key[i])
                return true;
        }
        return false;
    }
    else
    {
        for (int i = 0; i < current->size; i++)
        {
            if (x < current->key[i])
                return containsValue(x, current->ptr[i]);
        }
        return containsValue(x, current->ptr[current->size]);
    }
}

Node *BPTree::insertValue(int x, Node *current = NULL)
{
    if (current == NULL)
        current = root;

    if (root == NULL)
    {
        root = new Node(capacity);
        root->key[0] = x;
        root->isLeaf = true;
        root->size = 1;
        return NULL;
    }
    else
    {
        // leaf
        if (current->isLeaf == true)
        {
            // duplicate
            if (containsValue(x, root) == true)
            {
                cout << x << "duplicate, not insert..." << endl;
                return NULL;
            }
            // not duplicate
            else
            {
                // leaf not filled
                if (current->size < capacity)
                {
                    int i = 0;
                    // 找到要insert到哪個位置
                    while (x > current->key[i] && i < current->size)
                    {
                        i++;
                    }

                    // 將>x的數值全部往後一格(sort)
                    for (int j = current->size; j > i; j--)
                        current->key[j] = current->key[j - 1];

                    current->key[i] = x;
                    current->size++;
                    // 鍵結也要更改
                    cout << "leaf not fill " << endl;

                    current->ptr[current->size] = current->ptr[current->size - 1];
                    current->ptr[current->size - 1] = NULL;
                    return NULL;
                }
                // leaf is filled
                else
                {
                    Node *newLeaf = new Node(capacity);
                    // 創造一個多一個空間的靜態陣列存all current key
                    int tempNode[capacity + 1];
                    for (int i = 0; i < capacity; i++)
                        tempNode[i] = current->key[i];

                    // 找到要insert到哪個位置
                    int i = 0, j;
                    while (x > tempNode[i] && i < capacity)
                        i++;
                    for (int j = capacity + 1; j > i; j--)
                        tempNode[j] = tempNode[j - 1];
                    tempNode[i] = x;
                    // 定義新的樹葉，左邊存兩個右邊三個
                    newLeaf->isLeaf = true;
                    current->size = (capacity + 1) / 2;
                    newLeaf->size = (capacity + 1) - (capacity + 1) / 2;

                    // 左右鍵結
                    cout << "leaf fill, new Node" << endl;
                    if (current->ptr[capacity] != NULL)
                    {
                        // cout << current->ptr[capacity] << endl;
                        current->ptr[capacity]->ptr[0] = newLeaf;
                        newLeaf->ptr[newLeaf->size] = current->ptr[capacity];
                    }
                    newLeaf->ptr[0] = current;
                    current->ptr[current->size] = newLeaf;
                    current->ptr[capacity] = NULL;

                    // 把暫存陣列給Leaf
                    for (int i = 0; i < current->size; i++)
                        current->key[i] = tempNode[i];
                    for (int i = 0, j = current->size; i < newLeaf->size; i++, j++)
                        newLeaf->key[i] = tempNode[j];
                    // cout << x <<"new Leaf" << endl;
                    if (current == root)
                    {
                        Node *newRoot = new Node(capacity);
                        newRoot->key[0] = newLeaf->key[0];
                        newRoot->ptr[0] = current;
                        newRoot->ptr[1] = newLeaf;
                        newRoot->isLeaf = false;
                        newRoot->size = 1;
                        root = newRoot;
                        return NULL;
                    }
                    else
                        return newLeaf;
                }
            }
        }
        // not leaf
        else
        {
            // 向下找要插入x的位置
            Node *child;
            for (int i = 0; i < current->size; i++)
            {
                if (x < current->key[i])
                {
                    child = current->ptr[i];
                    break;
                }
            }

            if (x >= current->key[current->size - 1])
                child = current->ptr[current->size];

            // 找到leaf後回傳result子樹是否有分裂
            Node *result = insertValue(x, child);
            // 子樹無分裂
            if (result == NULL)
                return NULL;
            // 子樹有分裂
            else
            {
                // 分裂的子樹都是從右邊產生因此KEY[0]會是要新增node的路標
                x = result->key[0];
                if (current->size < capacity)
                {
                    // 找到x要存入的位置
                    int i = 0;
                    while (x > current->key[i] && i < current->size)
                        i++;
                    // x 是node節點，要改Key跟Ptr
                    for (int j = current->size; j > i; j--)
                        current->key[j] = current->key[j - 1];

                    for (int j = current->size + 1; j > i + 1; j--)
                        current->ptr[j] = current->ptr[j - 1];
                    // 給current插入key x
                    if (result->isLeaf == true)
                        current->key[i] = x;
                    else
                        current->key[i] = result->key[result->size];       // 307行暫存在child尾端的路標
                    current->size++;
                    // 將新的節點連到child
                    current->ptr[i + 1] = result;
                    cout << "node not filled" << endl;
                    return NULL;
                }
                else
                {
                    Node *newNode = new Node(capacity);
                    int tempKey[capacity + 1];
                    Node *tempPtr[capacity + 2];
                    for (int i = 0; i < capacity; i++)
                        tempKey[i] = current->key[i];
                    for (int i = 0; i < capacity + 1; i++)
                        tempPtr[i] = current->ptr[i];
                    // 暫存的key,ptr，x之後向後移，並插入x及newleaf

                    int CurrentPos = 0, j;
                    while (x > tempKey[CurrentPos] && CurrentPos < capacity)
                        CurrentPos++;

                    for (int j = capacity + 1; j > CurrentPos; j--)
                        tempKey[j] = tempKey[j - 1];

                    if (result->isLeaf == true)
                        tempKey[CurrentPos] = x;
                    else
                        tempKey[CurrentPos] = result->key[result->size];

                    for (int j = capacity + 1; j > CurrentPos + 1; j--)
                    {
                        tempPtr[j] = tempPtr[j - 1];
                    }
                    tempPtr[CurrentPos + 1] = result;

                    //  設定新的node並把temp切半輸入parent(current)的node與新的parent node
                    newNode->isLeaf = false;
                    current->size = (capacity + 1) / 2;
                    newNode->size = capacity - (capacity + 1) / 2;
                    for (int i = 0; i < current->size; i++)
                        current->key[i] = tempKey[i];
                    for (int i = 0; i <= current->size; i++)
                        current->ptr[i] = tempPtr[i];

                    for (int i = 0; i < newNode->size; i++)
                        newNode->key[i] = tempKey[i + current->size + 1];
                    for (int i = 0; i <= newNode->size; i++)
                        newNode->ptr[i] = tempPtr[i + current->size + 1];
                    // current及newNode分開時中間會多出一個key，先暫存在newNode尾端，因為newNode會往回傳往上
                    newNode->key[newNode->size] = tempKey[current->size];

                    // 因為左2右3，左邊有一個路標是不需要的，因此將其往上挪給newNode
                    // current->ptr[current->size] = NULL;
                    // newNode->size;
                    cout << "node fill, new Node " << endl;

                    if (current == root)
                    {
                        cout << "root fill, root point to new Node" << endl;
                        Node *newRoot = new Node(capacity);
                        newRoot->key[0] = newNode->key[newNode->size];
                        newRoot->ptr[0] = current;
                        newRoot->ptr[1] = newNode;
                        newRoot->isLeaf = false;
                        newRoot->size = 1;
                        root = newRoot;
                        return NULL;
                    }
                    else
                        return newNode;
                }
            }
        }
    }
}

void BPTree::search(int x)
{
    if (containsValue(x, root) == true)
        cout << "found" << endl;
    else
        cout << "not found" << endl;
}

void BPTree::display(Node *current, int level = 0)
{
    if (current != NULL)
    {
        for (int i = 0; i < level; i++)
            cout << "   ";
        if (current->isLeaf == true)
            cout << "[";
        else
            cout << "(";

        for (int i = 0; i < capacity; i++)
        {
            if (i < current->size)
                cout << current->key[i] << " ";
            else
                cout << "_ ";
        }

        if (current->isLeaf == true)
            cout << "]" << endl;
        else
            cout << "):" << endl;
        if (current->isLeaf != true)
        {
            level++;
            for (int i = 0; i <= current->size; i++)
                display(current->ptr[i], level);
        }
    }
}

void BPTree::remove(Node *current)
{
    if (current != NULL)
    {
        if (current->isLeaf == false)
        {
            for (int i = 0; i <= current->size; i++)
                remove(current->ptr[i]);
            current->isLeaf = true;
        }
        if (current->isLeaf == true)
        {
            delete[] current->key;
            delete[] current->ptr;
            delete current;
            cout << current << ", ";
        }
    }
}

bool BPTree::deleteVaule(int x, Node *current = NULL)
{
    if (current == NULL)
        current = root;
    // leaf
    if (current->isLeaf == true)
    {
        if (containsValue(x) == false)
        {
            cout << x << "not found in tree" << endl;
            return true;
        }
        else
        {
            int i = 0;
            // 找到要delete到哪個位置
            while (x > current->key[i] && i < current->size)
            {
                i++;
            }
            for (int j = i; j < current->size; j++)
                current->key[j] = current->key[j + 1];

            //有可能覆蓋掉ptr[0]，所以往上傳給node時再做事，暫存在ptr尾端
            Node* temp = current->ptr[current->size- 1];
            current->ptr[current->size - 1] = current->ptr[current->size];
            current->ptr[current->size] = temp;
            current->size--;
            // cout << "leaf delete" << endl;
            // 考慮root情況，size==0即刪除root
            if (current == root && current->size == 0)
            {

                delete[] current->key;
                delete[] current->ptr;
                delete current;
                root = NULL;
                cout << "Tree = NULL" << endl;
                return true;
            }
            else
            {
                // under-full 不足一半 合併
                if (current->size < (capacity + 1) / 2)
                    return false;
                else
                    return true;
            }
        }
    }
    // branch
    else
    {
        Node *child;
        int childPos = 0;
        while (x >= current->key[childPos] && childPos < current->size)
            childPos++;
        child = current->ptr[childPos];
        bool result = deleteVaule(x, child);
        //該子樹位置有一半以上capacity
        if (result == true)
            return true;
        // 子樹的位置不到一半的CAPACITY要合併或是跟旁邊借一個
        else
        {

            int leftSibling = childPos - 1;     // 要處理不足一半的節點位置及鄰居
            int rightSibling = childPos + 1;
            Node *leftNode = current->ptr[leftSibling];
            Node *rightNode = current->ptr[rightSibling];

            // NODE剩一半不用合併
            if (child->isLeaf == false && child->size == (capacity + 1) / 2)
            {
                return true;
            }
            // left borrow(左鄰居數量大於一半)
            else if (leftSibling >= 0 && leftNode->size > (capacity + 1) / 2)
            {

                
                // child內部往後移一格
                for (int i = child->size; i > 0; i--)
                    child->key[i] = child->key[i - 1];
                // child first更改為parent左兄弟 key，current 左兄弟的key更改為左兄弟內最右的key
                child->key[0] = leftNode->key[leftNode->size - 1];
                current->key[leftSibling] = leftNode->key[leftNode->size - 1];
                

                // 子樹是LEAF要處理雙向鍵結：ptr也要全部往後一格 
                if (child->isLeaf == true)
                {
                    Node* temp = child->ptr[child->size+1];              // DELETE原本向上傳的PTR[0]存在SIZE+1 407行
                    cout << "temp" << temp << endl;
                    child->ptr[child->size+1] = child->ptr[child->size];
                    child->ptr[child->size] = temp;
                    leftNode->ptr[leftNode->size-1] = leftNode->ptr[leftNode->size];
                    leftNode->ptr[leftNode->size] =NULL;
                    // child->ptr[0] = child->ptr[1];
                }
                // 子樹是node要處理PTR往後一格
                else
                {
                    for (int i = child->size + 1; i > 0; i--)
                        child->ptr[i] = child->ptr[i - 1];
                    child->ptr[0] = leftNode->ptr[leftNode->size];
                }
                child->size++;
                leftNode->size--;
                cout << "borrow from leftNode" << endl;
                return true;
            }
            else if (rightSibling <= current->size && rightNode->size > (capacity + 1) / 2)
            {
                // right borrow
                child->key[child->size] = rightNode->key[0];
                
                cout << current->key[childPos];
                for (int i = 0; i < rightNode->size - 1; i++)
                    rightNode->key[i] = rightNode->key[i + 1];
                current->key[childPos] = rightNode->key[0];
                
                if (child->isLeaf == true)
                {
                    Node* temp = child->ptr[child->size+1];
                    cout << "temp" << temp << endl;
                    child->ptr[child->size+1] = child->ptr[child->size];
                    child->ptr[child->size] = temp;
                    rightNode->ptr[1] = rightNode->ptr[0];
                    rightNode->ptr[rightNode->size-1] = rightNode->ptr[rightNode->size];
                    rightNode->ptr[rightNode->size] = NULL;

                }
                else
                {

                    child->ptr[child->size+1] = rightNode->ptr[0];
                    child->key[child->size] = child->ptr[child->size+1]->key[0];
                    for (int i = 0; i < rightNode->size; ++i)
                    {
                        rightNode->ptr[i] = rightNode->ptr[i + 1];
                    }
                }
                
                
                // rightNode->ptr[0] = NULL;
                child->size++;
                rightNode->size--;
                cout << "borrow from rightNode" << endl;
                return true;
            }
            else if (leftSibling >= 0 && leftNode->size <= (capacity + 1) / 2)
            {

                // left merge
                if (child->isLeaf == false)
                {
                    leftNode->key[leftNode->size] = current->key[leftSibling];
                    leftNode->size++;
                }
                for (int j = 0; j < child->size; j++)
                    leftNode->key[j + leftNode->size] = child->key[j];

                
                for (int j = 0; j <= child->size; j++)
                {
                    leftNode->ptr[j + leftNode->size] = child->ptr[j];
                }
                if(child->isLeaf == true){
                    child->ptr[child->size]->ptr[0] = leftNode;
                }
                leftNode->size += child->size;
                child->size = 0;

                delete child->ptr;
                delete child->key;
                delete child;

                for (int i = leftSibling; i < current->size; i++)
                    current->key[i] = current->key[i + 1];

                for (int i = leftSibling + 1; i <= current->size; i++)
                    current->ptr[i] = current->ptr[i + 1];
                current->size--;

                cout << "merge from leftNode" << endl;
                if (current == root && current->size == 0)
                {
                    delete[] current->ptr;
                    delete[] current->key;
                    delete[] current;
                    root = leftNode;
                    cout << "no neighbor, root = child" << endl;
                    return true;
                }
                if (current->size > (capacity + 1) / 2)
                    return true;
                else
                    return false;
            }
            else if (rightSibling <= current->size && rightNode->size <= (capacity + 1) / 2)
            {
                // right merge
                if (child->isLeaf == false)
                {
                    child->key[child->size] = current->key[childPos];
                    child->size++;
                }
                Node* temp = child->ptr[child->size+1];
                // node的key全部合併(右給左)
                for (int j = 0; j < rightNode->size; j++)
                    child->key[j + child->size] = rightNode->key[j];

                for (int j = 0; j <= rightNode->size; j++)
                {
                    child->ptr[j + child->size] = rightNode->ptr[j];
                }

                if(child->isLeaf == true){
                    rightNode->ptr[rightNode->size]->ptr[0] = child;
                    child->ptr[child->size] = temp;
                }
                child->size += rightNode->size;
                rightNode->size = 0;

                delete rightNode->ptr;
                delete rightNode->key;
                delete rightNode;

                current->size--;
                for (int i = childPos; i < current->size; i++)
                    current->key[i] = current->key[i + 1];

                for (int i = childPos + 1; i <= current->size; i++)
                    current->ptr[i] = current->ptr[i + 1];

                cout << "merge from rightNode" << endl;

                // 如果是root的情況
                if (current == root && current->size == 0)
                {
                    delete[] current->ptr;
                    delete[] current->key;
                    delete[] current;
                    root = child;
                    cout << "no neighbor, root = child" << endl;
                    return true;
                }

                if (current->size > (capacity + 1) / 2)
                    return true;
                else
                    return false;
            }
            else
            {
                // 不須做任何動作
                return true;
            }
        }
    }
}

int main()
{
    int order, a, capacity;
    // 輸入方式：第一個是order，其他是想要insert的數字
    // ex: 1 2 1 3 9 11 17 19 25 27 33 35 41 43 49 5 7 13 15 21 23 29 31 37 39 45 47
    cout << "Input insert order(ex:3):";
    cin >> order;
    capacity = 2 * order;
    BPTree node(capacity);
    while (true)
    {
        cout << "Insert value(ex: 2 3 5 7 8):";
        while (cin >> a)
        {
            if (a == -1)
                break;
            cout << "-------------------" << endl;
            cout << "Insert key: " << a << endl;
            node.insertValue(a);

            node.display(node.getRoot());

            if (getchar() == '\n')
                break;
        }

        node.displayDoubleLinkList();
        cout << "-------------------" << endl;
        cout << "search value(ex: 1 2 4 3 2 ,if no search value input -1): ";
        //
        while (cin >> a)
        {
            if (a == -1)
                break;
            cout << "-------------------" << endl;
            cout << "search key: " << a << endl;
            node.search(a);
            if (getchar() == '\n')
                break;
        };
        cout << "-------------------" << endl;
        cout << "delete value(ex: 1 2 3 ,if no delete value input -1): ";
        // 想要刪除的數字
        // ex: 43 39 41 37 49 47
        while (cin >> a)
        {
            if (a == -1)
                break;
            cout << "-------------------" << endl;
            cout << "Delete key: " << a << endl;
            node.deleteVaule(a);
            node.display(node.getRoot());
            if (getchar() == '\n')
                break;
        };
        cout << "-------------------" << endl;
        node.displayDoubleLinkList();
        cout << "-------------------" << endl;
        cout << "Do you want to exit?(input -1 to exit, 1 to continue)";
        cin >> a;
        if(a == -1)
            break;
        
        
    } ;

    return 0;
}