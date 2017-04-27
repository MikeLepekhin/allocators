#include <cstdio>
#include <iostream>
#include <algorithm>
#include <memory>

template<class T, class Allocator = std::allocator>
class List {
private:
    Allocator alloc_;
    size_t size_;

    struct Node {
        Node* next;
        Node* prev;
        T key;

        Node() { }
    };

    Node* left;
    Node* right;

public:
    List(): size_t(0), left(nullptr), right(nullptr) {}

    List(size_t count, const T& value = T(), const Allocator& alloc): size_(count) {
        
    }

    void insertBefore(Node* nd, const T& val) {

    }

    void insertBefore(Node* nd, T&& val) {

    }

    void insertAfter(Node* nd, const T& val) {

    }

    void insertAfter(Node* nd, T&& val) {

    }

    void push_back(const T& val) {

    }

    void push_back(T&& val) {

    }

    void pop_back() {

    }

    void pop_front() {

    }
};
    
