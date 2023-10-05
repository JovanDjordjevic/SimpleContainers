#ifndef __SOME_TEMPLATE_TEST_CLASS__
#define __SOME_TEMPLATE_TEST_CLASS__

#define SOME_TEMPLATE_CLASS_DEBUG

#ifdef SOME_TEMPLATE_CLASS_DEBUG
    #include <iostream>
    #define LOG(msg) do { std::cout << msg << std::endl; } while(0);
#else
    #define LOG(msg) do {} while(0);
#endif // SOME_TEMPLATE_CLASS_DEBUG

template <typename T>
class SomeTemplateClass {
    public:
        SomeTemplateClass(T val = T{}) : mSomeValue{val} {
            LOG("SomeTemplateClass default CTOR");
        }

        SomeTemplateClass(const SomeTemplateClass& other) : mSomeValue{other.mSomeValue} { 
            LOG("SomeTemplateClass copy CTOR");
        }

        SomeTemplateClass(SomeTemplateClass&& other) noexcept : mSomeValue{std::move(other.mSomeValue)} {
            LOG("SomeTemplateClass move CTOR");
        }

        SomeTemplateClass& operator=(const SomeTemplateClass& rhs) {
            LOG("SomeTemplateClass copy assignment operator");
            if (this != &rhs) {
                mSomeValue = rhs.mSomeValue;
            }
            return *this;
        }

        SomeTemplateClass& operator=(SomeTemplateClass&& rhs) noexcept {
            LOG("SomeTemplateClass move assignment operator");
            if (this != &rhs) {
                mSomeValue = std::move(rhs.mSomeValue);
            }
            return *this;
        }

        ~SomeTemplateClass() noexcept {
            LOG("SomeTemplateClass DTOR");
        }

        friend bool operator==(const SomeTemplateClass& lhs, const SomeTemplateClass& rhs) {
            return lhs.mSomeValue == rhs.mSomeValue;
        }

        friend bool operator<(const SomeTemplateClass& lhs, const SomeTemplateClass& rhs) {
            return lhs.mSomeValue < rhs.mSomeValue;
        }

        friend std::ostream& operator<<(std::ostream& os, const SomeTemplateClass& sc) {
            os << sc.mSomeValue;
            return os;
        }

    private:
        T mSomeValue;
};

#endif // __SOME_TEMPLATE_TEST_CLASS__
