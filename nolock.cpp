#include <iostream>
#include <atomic>
#include <memory>

//定义栈节点结构体
template <typename T>
struct Node
{
    T value;
    std::atomic<Node<T>*> next;

    Node(T val) : value(val),next(nullptr){} 
};

//无所栈实现
template <typename T>
class LockFreeStack
{
public:
    LockFreeStack() : head(nullptr){}

    //入栈操作
    void push(T value)
    {
        Node<T>* newNode = new Node<T>(value);
        Node<T>* oldHead;
        do {
            oldHead = head.load(std::memory_order_relaxed);  // 获取当前栈顶
            newNode->next.store(oldHead, std::memory_order_relaxed);  // 将新节点的next指向旧的栈顶
        } while (!head.compare_exchange_weak(oldHead, newNode, std::memory_order_release, std::memory_order_relaxed));  // 原子地将栈顶更新为新节点
    }

    // 出栈操作
    bool pop(T& result) {
        Node<T>* oldHead;
        do {
            oldHead = head.load(std::memory_order_relaxed);  // 获取当前栈顶
            if (!oldHead) {
                return false;  // 栈为空
            }
        } while (!head.compare_exchange_weak(oldHead, oldHead->next, std::memory_order_release, std::memory_order_relaxed));  // 原子地将栈顶更新为下一个节点

        result = oldHead->value;
        delete oldHead;  // 删除被弹出的节点
        return true;
    }

     // 判断栈是否为空
    bool isEmpty() const 
    {
        return head.load(std::memory_order_acquire) == nullptr;
    }
};

int main() 
{
    LockFreeStack<int> stack;

    // 入栈
    stack.push(10);
    stack.push(20);
    stack.push(30);

    // 出栈
    int result;
    while (stack.pop(result)) {
        std::cout << "Popped: " << result << std::endl;
    }

    return 0;
}