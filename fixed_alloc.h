#include <cstdio>
#include <memory>
#include <iostream>
#include <limits>

namespace helper
{
    template<class T>
    class Stack {
    private:
        static const int EXTENSION_COEFF = 2;
        static const int COMPRESS_COEFF = 2;
        static const int MIN_USE_COEFF = 25; // in percents
        static const int MIN_CAPACITY = 1;

        size_t size_;
        size_t capacity_;

        T* buf_;

    public:
        Stack(): size_(0), capacity_(MIN_CAPACITY), buf_(new T[MIN_CAPACITY]) {}

        Stack(const Stack& other) = delete;

        Stack(Stack&& other) {
            size_ = other.size_;
            capacity_ = other.capacity_;
            buf_ = other.buf_;
            other.size_ = 0;
            other.capacity_ = 0;
            other.buf_ = nullptr;
        }

        Stack& operator=(const Stack& other) = delete;

        Stack& operator=(Stack&& other) {
            if (this != &other) {
                size_ = other.size_;
                capacity_ = other.capacity_;
                buf_ = other.buf_;
                other.size_ = 0;
                other.capacity_ = 0;
                other.buf_ = nullptr;
            }
            return *this;
        }

        void recopy(size_t newSize) {
            T* newBuf = new T[newSize];
            for (size_t i = 0; i < size_; ++i) 
                newBuf[i] = buf_[i];
            delete[] buf_;
            buf_ = newBuf;
            capacity_ = newSize;
        }

        void push(const T& val) {
            if (size_ == capacity_) 
                recopy(capacity_ * EXTENSION_COEFF);
            buf_[size_++] = val;
        }

        T top() const {
            return buf_[size_ - 1];
        }
        
        void pop() {
            --size_;
            if (size_ * 100 / capacity_ < MIN_USE_COEFF && capacity_ > MIN_CAPACITY)
                recopy(capacity_ / COMPRESS_COEFF);
        }

        bool empty() const {
            return size_ == 0;
        }

        ~Stack() {
            delete[] buf_;
        }
    };
};

template<size_t chunkSize>
class FixedAllocator {
private:
    static const int EXTENSION_COEFF = 2;

    char* cur_;
    char* end_;
    helper::Stack<void*> freed_;
    helper::Stack<void*> alloced_;
    size_t allocSize_;

public:
    FixedAllocator(): cur_(nullptr), end_(nullptr), allocSize_(1) {}

    FixedAllocator(const FixedAllocator& other) = delete;

    FixedAllocator(FixedAllocator&& other) {
        cur_ = other.cur_;
        end_ = other.end_;
        allocSize_ = other.allocSize_;
        freed_ = std::move(other.freed_);
        other.cur_ = other.end_ = nullptr;
        other.allocSize_ = 0;
    }

    FixedAllocator& operator=(const FixedAllocator& other) = delete;
    

    FixedAllocator& operator=(FixedAllocator&& other) {
        if (this != &other) {
            cur_ = other.cur_;
            end_ = other.end_;
            allocSize_ = other.allocSize_;
            freed_ = std::move(other.freed_);
        }
        return *this;
    }

    void* allocate() {
        if (!freed_.empty()) {
            void* res = freed_.top();
            freed_.pop(); 
            return res;
        }
        if (cur_ == end_) {
            allocSize_ *= EXTENSION_COEFF;
            cur_ = operator new(allocSize_);
            end_ = cur_ + allocSize_;
        }
        cur_ += allocSize_;
        return (void*)(cur_ - allocSize_);
    }

    void deallocate(void* ptr) {
        freed_.push(ptr);
    }

    ~FixedAllocator() {
        while (!alloced_.empty()) {
            operator delete(alloced_.top());
            alloced_.pop();
        }
    }
};

template<class T>
class FastAllocator {
private:
    FixedAllocator<sizeof(T) * 4> fa4_;
    FixedAllocator<sizeof(T) * 8> fa8_;

public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;


    FastAllocator() = default;

    template<class U>
    FastAllocator(const FastAllocator<U>& other) {}

    template<class U>
    FastAllocator& operator=(const FastAllocator& other) {
        return *this;
    }

    T* allocate(size_t count) {
        switch (count) {
            case 4:
                return reinterpret_cast<T*>(fa4_.allocate());
                break;
            case 8:
                return reinterpret_cast<T*>(fa8_.allocate());
                break;
            default:
                return new T[count];
        }
    }

    void deallocate(T* ptr, size_t count) {
        switch (count) {
            case 4:
                fa4_.deallocate(ptr);
                break;
            case 8:
                fa8_.deallocate(ptr);
                break;
            default:
                delete[] ptr;
        }
    }

    pointer address(reference x) const {
        return &x;
    }

    const_pointer address(const_reference x) const {
        return &x;
    }

    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    void construct(pointer p, const_reference val) {
       new((void*)p) value_type(val);
    }


    template<class U, class... Args>
    void construct(U* p, Args&&... args) {
        new((void *)p) U(std::forward<Args>(args)...);
    }
    
    void destroy(pointer p) {
        deallocate(p);
    }

    template<class U>
    void destroy(U* p) {
        deallocate(p);
    }

    ~FastAllocator() = default;
};

namespace std {
    template<class T>
    struct allocator_traits<FastAllocator<T>> {
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef void* void_pointer;
        typedef const void* const_void_pointer;
    };
}
template<class T1, class T2>
bool operator==(const FastAllocator<T1>& fs, const FastAllocator<T2>& sc) {
    return &fs == &sc;
}

template<class T1, class T2>
bool operator!=(const FastAllocator<T1>& fs, const FastAllocator<T2>& sc) {
    return !(fs == sc);
}
