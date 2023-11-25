#ifndef SOME_ALLOCATOR_CLASS_HPP
#define SOME_ALLOCATOR_CLASS_HPP

#include <string>
#include <sstream>

#define SOME_ALLOCATOR_CLASS_DEBUG

#ifdef SOME_ALLOCATOR_CLASS_DEBUG
    #include <iostream>
    #define LOG(msg) do { std::cout << msg << std::endl; } while(0)
#else
    #define LOG(msg) do {} while(0);
#endif // SOME_ALLOCATOR_CLASS_DEBUG


// NOTE: this allocator does not fully satisfy the "Allocator" named requiement (https://en.cppreference.com/w/cpp/named_req/Allocator)
// TODO: implement a fully conforming allocator for testing

template <typename T>
class SomeAllocatorClass {
public:
    using value_type = T;

    SomeAllocatorClass() noexcept {
        LOG("SomeAllocatorClass CTOR");
    }

    template <typename U>
    SomeAllocatorClass(const SomeAllocatorClass<U>&) noexcept {
        LOG("SomeAllocatorClass COPY CTOR");
    }

    T* allocate(std::size_t n) {
        LOG(("SomeAllocatorClass allocate " + std::to_string(n)));

        if (n > max_size()) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) noexcept {
        std::ostringstream oss;
        oss << "SomeAllocatorClass deallocate adr " << p <<  " size " << n;
        LOG(oss.str());

        ::operator delete(p);
    }

    std::size_t max_size() const noexcept {
        LOG("SomeAllocatorClass max_size");
        return std::size_t(-1) / sizeof(T);
    }

    void construct(T* p, const T& value) {
        std::ostringstream oss;
        oss << "SomeAllocatorClass construct adr " << p <<  " value " << value;
        LOG(oss.str());

        new (p) T(value);
    }

    void destroy(T* p) noexcept {
        std::ostringstream oss;
        oss << "SomeAllocatorClass destroy adr " << p;
        LOG(oss.str());

        p->~T();
    }
};

#endif // #ifndef SOME_ALLOCATOR_CLASS_HPP
