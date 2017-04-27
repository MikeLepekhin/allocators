#include <cstdio>
#include <iostream>
#include <algoirthm>
#include <memory>
#include <cstdlib>
#include <utility>

template<class T, class Allocator = std::allocator>
class List {
private:
    Allocator<T> alloc_;

    struct Node {
        T key;
        Node* next;
        Node* prev;

        Node(): key(), next(nullptr), prev(nullptr) {}

        Node(const T& key): key(key), next(nullptr), prev(nullptr) {}

        Node(T&& key): key(key), next(nullptr), prev(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    size_t size_;
    
    void insertAfterPtr(Node* pos, Node* newVert) {
        ++size_;
        newVert->next = pos ? pos->next : head_;
        newVert->prev = pos;
        if (newVert->next == nullptr)
            tail_ = newVert;
        if (newVert->prev == nullptr)
            head_ = newVert;
    }

    void insertBeforePtr(Node* pos, Node* newVert) {
        ++size_;
        newVert->next = pos;
        newVert->prev = pos ? pos->prev : tail_;
        if (newVert->next == nullptr)
            tail_ = newVert;
        if (newVert->prev == nullptr)
            head_ = newVert;
    }

public:
    explicit List(const Allocator& alloc) alloc_(alloc) {}
    
    List(size_t count, const T& value = T(), const Allocator& alloc = Allocator()): alloc_(alloc), size_(0), head_(nullptr), tail_(nullptr) {
        head_;
    }

    List(const List& other) {
        Node* beg = other.head_;
        while (beg != nullptr) {
            push_back(beg->key);
            beg = beg->next;
        }
    }

    List(List&& other): head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }

    List& operator=(const List& other) {
        if (this != &other) {
            ~List();
            Node* beg = other.head_;
            while (beg != nullptr) {
                push_back(beg->key);
                beg = beg->next;
            }
        }
        return *this;
    }

    List& operator=(List&& other) {
        if (this != &other) {
            ~List();
            Node* beg = other.head_;
            while (beg != nullptr) {
                push_back(beg->key);
                beg = beg->next;
            }
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    size_t size() const {
        return size_;
    }


    template<class... Args>
    Node* constructNode(Args&&... args) {
        Node* ptr = alloc_.allocate(1);
        alloc_.construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }


    void push_back(const T& val) {
        ++size_;
        if (tail_ == nullptr) {
            tail_ = constructNode(val);
            head_ = tail_;
            return;
        }
        head_->next = constructNode(val);
        head_ = head_->next;
    }

    void push_back(T&& val) {
        insertAfterPtr(tail_, constructNode(val));
    }

    void push_back(const T& val) {
        insertAfterPtr(tail_, constructNode(val));
    }
    
    void push_front(T&& val) {
        insertBeforePtr(head_, constructNode(val));
    }

    void push_front(const T& val) {
        insertBeforePtr(head_, constructNode(val));
    }

    void insert_after(Node* pos, const T& val) {
        insertAfterPtr(pos, constructNode(val));
    }
    
    void insert_after(Node* pos, T&& val) {
        insertAfterPtr(pos, constructNode(val));
    }

    void insert_before(Node* pos, const T& val) {
        insertBeforePtr(pos, constructNode(val));
    }
    
    void insert_before(Node* pos, T&& val) {
        insertBeforePtr(pos, constructNode(val));
    }

    void erase(Node* pos) {
        pos->next->prev = pos->prev;
        pos->prev->next = pos->next;
        if (pos == tail_)
            tail_ = tail_->prev;
        if (pos == head_)
            head_ = head_->next;
        delete pos;
        --size_;
    }

    void pop_back() {
        erase(tail_);
    }

    void pop_front() {
        erase(head_);
    }

    ~List() {
        while (size_ > 0) 
            erase(tail_);
    }
};
