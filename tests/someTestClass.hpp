#ifndef __SOME_TEST_CLASS__
#define __SOME_TEST_CLASS__

#define SOME_CLASS_DEBUG

#ifdef SOME_CLASS_DEBUG
    #include <iostream>
    #define LOG(msg) do { std::cout << msg << std::endl; } while(0);
#else
    #define LOG(msg) do {} while(0);
#endif // SOME_CLASS_DEBUG

class SomeClass {
    public:
        SomeClass(int val = 0) : mSomeValue{val} { 
            LOG("SomeClass default CTOR"); 
        }

        SomeClass(const SomeClass& other) : mSomeValue{other.mSomeValue} { 
            LOG("SomeClass copy CTOR");
        }

        SomeClass(SomeClass&& other) noexcept : mSomeValue{std::move(other.mSomeValue)} {
            LOG("SomeClass move CTOR");
        }

        SomeClass& operator=(const SomeClass& rhs) {
            LOG("SomeClass copy assignment operator");
            if (this != &rhs) {
                mSomeValue = rhs.mSomeValue;
            }
            return *this;
        }

        SomeClass& operator=(SomeClass&& rhs) noexcept {
            LOG("SomeClass move assignment operator");
            if (this != &rhs) {
                mSomeValue = std::move(rhs.mSomeValue);
                rhs.mSomeValue = 0;
            }
            return *this;
        }

        ~SomeClass() noexcept { 
            LOG("SomeClass DTOR"); 
        }

    private:
        int mSomeValue;
};

#endif // __SOME_TEST_CLASS__
