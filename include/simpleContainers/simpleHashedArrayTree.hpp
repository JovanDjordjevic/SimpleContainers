/// @file simpleHashedArrayTree.hpp
/// @brief File containing API and implementaiton of HashedArrayTree class

#ifndef SIMPLE_HASHED_ARRAY_TREE_HPP
#define SIMPLE_HASHED_ARRAY_TREE_HPP

#include <cmath>
#include <limits>
#include <type_traits>
#include <vector>

#ifndef SIMPLE_HASHED_ARRAY_TREE_DEBUG
    /// @brief Macro for debug checks
    /// @details This definition controls if some debug checks are made at compile and runtime. If the standard NDEBUG
    ///          is defined or user defines SIMPLE_HASHED_ARRAY_TREE_NO_DEBUG during during compilation, no debug checks will be made.
    ///          It is recomended to leave debug checks active during development, and turn them off for release builds
    #define SIMPLE_HASHED_ARRAY_TREE_DEBUG
#endif // #ifndef SIMPLE_HASHED_ARRAY_TREE_DEBUG

#if defined NDEBUG || defined SIMPLE_HASHED_ARRAY_TREE_NO_DEBUG
    #undef SIMPLE_HASHED_ARRAY_TREE_DEBUG
#endif // #if defined NDEBUG || defined SIMPLE_HASHED_ARRAY_TREE_NO_DEBUG

#ifdef SIMPLE_HASHED_ARRAY_TREE_DEBUG
    #include <iostream>

    /// @brief Default stream where debug messages will be printed when they are enabled
    #define SIMPLE_HASHED_ARRAY_TREE_DEBUG_OUTPUT_STREAM std::cerr

    #define SIMPLE_HASHED_ARRAY_TREE_ASSERT(cond, msg) \
        do { \
            if (!(cond)) { \
                SIMPLE_HASHED_ARRAY_TREE_DEBUG_OUTPUT_STREAM << "\nHASHED ARRAY TREE ASSERT FAILED WITH MESSAGE: " << msg \
                    << "\nEXPECTED: " << #cond \
                    << "\nFILE: " << __FILE__ << ", LINE " << __LINE__ << std::endl << std::endl; \
                std::abort(); \
            } \
        } while(0)

    #define SIMPLE_HASHED_ARRAY_TREE_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
    #define SIMPLE_HASHED_ARRAY_TREE_ASSERT(cond, msg) do {} while (false)
    #define SIMPLE_HASHED_ARRAY_TREE_STATIC_ASSERT(cond, msg) ;
#endif // #ifdef SIMPLE_HASHED_ARRAY_TREE_DEBUG

// ============================================================================================================================================
// =================================================================== API ====================================================================
// ============================================================================================================================================

namespace simpleContainers {
    namespace hat_internal {
        template <typename T, typename = void>
        struct is_iterator;
    } // namespace hat_internal

    template <typename T, typename Allocator>
    class HashedArrayTree;

    template <typename T, typename Allocator>
    inline bool operator==(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator!=(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator<(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator<=(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator>(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator>=(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept;

    /// @brief Class representing a hashed array tree structure (HAT)
    /// @details This is the main class the user should interact with. HashedArrayTree is a way to implement
    ///          dynamic arrays that reduces the amount of copying when resizing the structure
    ///          see https://en.wikipedia.org/wiki/Hashed_array_tree
    ///          Memory reserved by this container will always be a power of 2
    /// @tparam T Type of object contained inside HashedArrayTree
    /// @tparam Allocator Allocator for said type
    template <typename T, typename Allocator = std::allocator<T>>
    class HashedArrayTree {
        public:
            using value_type = T;
            using allocator_type = Allocator;
            using reference = T&;
            using const_reference = const T&;
            using pointer = typename std::allocator_traits<allocator_type>::pointer;
            using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
            using size_type = typename std::allocator_traits<allocator_type>::size_type;
            using difference_type = typename std::allocator_traits<allocator_type>::difference_type;

            /// @brief Class representing iterators over HashedArrayTree
            /// @details HashedArrayTree iterators are compliant with the LegacyRandomAccessIterator named requirement.
            ///          All methods are O(1) time complexity
            /// @tparam constTag Compile time indicator if iterator is a const iterator or not
            template <bool constTag = false>
            class HashedArrayTreeIterator {
                public:
                    friend class HashedArrayTreeIterator<false>;
                    friend class HashedArrayTreeIterator<true>;

                    using iterator_category = std::random_access_iterator_tag;
                    using size_type = typename HashedArrayTree<T, Allocator>::size_type;
                    using difference_type = typename HashedArrayTree<T, Allocator>::difference_type;
                    using value_type = typename HashedArrayTree<T, Allocator>::value_type;
                    using pointer = typename std::conditional<constTag, typename HashedArrayTree<T, Allocator>::const_pointer, typename HashedArrayTree<T, Allocator>::pointer>::type;
                    using reference = typename std::conditional<constTag, typename HashedArrayTree<T, Allocator>::const_reference, typename HashedArrayTree<T, Allocator>::reference>::type;
                    using hat_ptr_t = typename std::conditional<constTag, const HashedArrayTree<T, Allocator>*, HashedArrayTree<T, Allocator>*>::type;

                    HashedArrayTreeIterator(size_type pos = 0, hat_ptr_t hatPtr = nullptr) noexcept;
                    HashedArrayTreeIterator(const HashedArrayTreeIterator& other) noexcept = default;
                    /// @brief Converting constructor to create a const iterator from a non-const iterator
                    /// @details By using SFINAE, this constructor is only available for const iterators since they must 
                    ///          be implicitly constructible from a non-const iterator
                    template <bool C = constTag, typename = typename std::enable_if<C>::type>
                    HashedArrayTreeIterator(const HashedArrayTreeIterator<false>& other) noexcept;
                    HashedArrayTreeIterator(HashedArrayTreeIterator&& other) noexcept = default;
                    HashedArrayTreeIterator& operator=(const HashedArrayTreeIterator& rhs) noexcept = default;
                    HashedArrayTreeIterator& operator=(HashedArrayTreeIterator&& rhs) noexcept = default;
                    ~HashedArrayTreeIterator() noexcept = default;

                    void swap(HashedArrayTreeIterator& other) noexcept;

                    reference operator*() const noexcept;
                    pointer operator->() const noexcept;

                    reference operator[](const difference_type n) const noexcept;

                    HashedArrayTreeIterator& operator++() noexcept; // prefix
                    HashedArrayTreeIterator operator++(int) noexcept; // postfix
                    HashedArrayTreeIterator& operator+=(const difference_type n) noexcept;
                    HashedArrayTreeIterator operator+(const difference_type n) const noexcept;
                    friend HashedArrayTreeIterator operator+(const difference_type n, HashedArrayTreeIterator rhs) noexcept {
                        rhs += n;
                        return rhs;
                    }

                    HashedArrayTreeIterator& operator--() noexcept; // prefix
                    HashedArrayTreeIterator operator--(int) noexcept; // postfix  
                    HashedArrayTreeIterator& operator-=(const difference_type n) noexcept;
                    HashedArrayTreeIterator operator-(const difference_type n) const noexcept;
                    difference_type operator-(const HashedArrayTreeIterator& other) const noexcept; // Subtraction between two iterators

                    friend bool operator==(const HashedArrayTreeIterator& lhs, const HashedArrayTreeIterator& rhs) noexcept {
                        SIMPLE_HASHED_ARRAY_TREE_ASSERT(lhs.mHatPtr == rhs.mHatPtr, "HashedArrayTreeIterator == comparison must be done on iterators of the same HashedArrayTree");
                        return lhs.mPosition == rhs.mPosition;
                    }

                    friend bool operator!=(const HashedArrayTreeIterator& lhs, const HashedArrayTreeIterator& rhs) noexcept {
                        SIMPLE_HASHED_ARRAY_TREE_ASSERT(lhs.mHatPtr == rhs.mHatPtr, "HashedArrayTreeIterator != comparison must be done on iterators of the same HashedArrayTree");
                        return lhs.mPosition != rhs.mPosition;
                    }

                    friend bool operator<(const HashedArrayTreeIterator& lhs, const HashedArrayTreeIterator& rhs) noexcept {
                        SIMPLE_HASHED_ARRAY_TREE_ASSERT(lhs.mHatPtr == rhs.mHatPtr, "HashedArrayTreeIterator < comparison must be done on iterators of the same HashedArrayTree");
                        return lhs.mPosition < rhs.mPosition;
                    }

                    friend bool operator<=(const HashedArrayTreeIterator& lhs, const HashedArrayTreeIterator& rhs) noexcept {
                        SIMPLE_HASHED_ARRAY_TREE_ASSERT(lhs.mHatPtr == rhs.mHatPtr, "HashedArrayTreeIterator <= comparison must be done on iterators of the same HashedArrayTree");
                        return lhs.mPosition <= rhs.mPosition;
                    }

                    friend bool operator>(const HashedArrayTreeIterator& lhs, const HashedArrayTreeIterator& rhs) noexcept {
                        SIMPLE_HASHED_ARRAY_TREE_ASSERT(lhs.mHatPtr == rhs.mHatPtr, "HashedArrayTreeIterator > comparison must be done on iterators of the same HashedArrayTree");
                        return lhs.mPosition > rhs.mPosition;
                    }

                    friend bool operator>=(const HashedArrayTreeIterator& lhs, const HashedArrayTreeIterator& rhs) noexcept {
                        SIMPLE_HASHED_ARRAY_TREE_ASSERT(lhs.mHatPtr == rhs.mHatPtr, "HashedArrayTreeIterator >= comparison must be done on iterators of the same HashedArrayTree");
                        return lhs.mPosition >= rhs.mPosition;
                    }

                private:
                    // mPosition is the overall index of an element in the HAT this iterator is for
                    size_type mPosition;
                    hat_ptr_t mHatPtr;
            };

            using iterator = HashedArrayTreeIterator<false>;
            using const_iterator = HashedArrayTreeIterator<true>;

        public:
            void debugPrint() const noexcept;

            HashedArrayTree(const allocator_type& alloc = allocator_type{});
            HashedArrayTree(const size_type initialCapacity, const allocator_type& alloc = allocator_type{});
            HashedArrayTree(const size_type initialSize, const value_type& val, const allocator_type& alloc = allocator_type{});
            HashedArrayTree(const std::vector<value_type, allocator_type>& initVec, const allocator_type& alloc = allocator_type{});
            HashedArrayTree(std::initializer_list<value_type> initList, const allocator_type& alloc = allocator_type{});
            template <typename Iterator, typename std::enable_if<hat_internal::is_iterator<Iterator>::value, bool>::type = true>
            HashedArrayTree(Iterator itStart, Iterator itEnd, const allocator_type& alloc = allocator_type{});

            HashedArrayTree(const HashedArrayTree& other);
            HashedArrayTree(HashedArrayTree&& other) noexcept = default;

            HashedArrayTree& operator=(const HashedArrayTree& rhs);
            HashedArrayTree& operator=(HashedArrayTree&& rhs) noexcept = default;

            ~HashedArrayTree() noexcept = default;

            allocator_type get_allocator() const noexcept;

            /// @brief Get number of elements in O(sqrt(n)) time
            size_type size() const noexcept;
            size_type max_size() const noexcept;
            /// @brief Return the current capacity of the HAT
            /// @details Note that this is not the maximum capacity of the HAT
            ///          Filling out this current capacity does not necessarily
            ///          cause the internal data to be reorganized. Internal
            ///          Restructure only happens when the maximum allowed capacity is reached
            size_type capacity() const noexcept;
            /// @brief Return the maximum capacity the HAT can hold before the internal leaf vectors are restructured
            size_type max_capacity() const noexcept;
            /// @brief check if the HAT is full (must have non-zero size)
            bool full() const noexcept;
            bool empty() const noexcept;
            /// @brief Reserve enough memory to store at least capacity elements
            void reserve(const size_type newCapacity) noexcept;
            void clear() noexcept;
            /// @brief Deallocates as much unused memory as possible
            /// @details This will only deallocate memory of leaf vectors that are currently empty
            void shrink_to_fit() noexcept;

            /// @brief A copy of all the elements in the HAT as a vector
            std::vector<value_type> get_as_vector() const noexcept;

            void push_back(const value_type& elem);
            void push_back(value_type&& elem);
            template <typename ...Args>
            void emplace_back(Args&&... args);
            void pop_back();

            void swap(HashedArrayTree& other) noexcept;

            reference operator[](const size_type pos) noexcept;
            const_reference operator[](const size_type pos) const noexcept;
            reference at(const size_type pos);
            const_reference at(const size_type pos) const;

            reference front() noexcept;
            const_reference front() const noexcept;
            reference back() noexcept;
            const_reference back() const noexcept;
            iterator begin() noexcept;
            iterator end() noexcept;
            const_iterator begin() const noexcept;
            const_iterator end() const noexcept;
            const_iterator cbegin() const noexcept;
            const_iterator cend() const noexcept;

        private:
            using LeafVector = std::vector<value_type, allocator_type>;
            using OuterAllocator = typename allocator_type::template rebind<LeafVector>::other;

            std::vector<LeafVector, OuterAllocator> mInternalData;

            // an outer vector with this capacity stores at maximum that
            // many inner vectors with the same capacity
            size_type mInternalVectorCapacity;

            // total number of elements currently stored in the HAT
            size_type mSize;

            // Currently allocated memory for data in the HAT
            size_type mCurrentCapacity;

            // current power of 2 such that 2 ^ mCurrentPow = mInternalVectorCapacity
            // stored here to avoid recalculating on every element access
            size_type mCurrentPow;

            // index of the first leaf in which a new element can be inserted

            // NOTE FOR ME: in the current implementation, mFirstNonFullLeafIndex may be invalid after an insertion
            // that causes the HAT to reach full capacity, untill after the reserve() call
            size_type mFirstNonFullLeafIndex;
    };

    namespace hat_internal {
        template <typename T>
        inline T next_power_of_2(T capacity) noexcept;

        template <typename T>
        inline T what_power_of_2(T capacity) noexcept;
    } // namespace hat_internal
} // namespace simpleContainers

// ============================================================================================================================================
// ============================================================== IMPLEMENTATION ==============================================================
// ============================================================================================================================================

namespace simpleContainers {
    template <typename T, typename Allocator>
    template <bool constTag>
    inline HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::HashedArrayTreeIterator(size_type pos, hat_ptr_t hatPtr) noexcept
        : mPosition{pos}, mHatPtr{hatPtr}
    {}

    template <typename T, typename Allocator>
    template <bool constTag> 
    template <bool C, typename> 
    inline HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::HashedArrayTreeIterator(const HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<false>& other) noexcept
        : mPosition{other.mPosition}, mHatPtr{other.mHatPtr} 
    {}

    template <typename T, typename Allocator>
    template <bool constTag>
    inline void HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::swap(HashedArrayTreeIterator& other) noexcept {
        std::swap(mPosition, other.mPosition);
        std::swap(mHatPtr, other.mHatPtr);
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>::reference
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator*() const noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mHatPtr != nullptr, "HashedArrayTreeIterator::operator* trying to dereference mHatPtr which is a nullptr");
        return (*mHatPtr)[mPosition];
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>::pointer 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator->() const noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mHatPtr != nullptr, "HashedArrayTreeIterator::operator-> trying to dereference mHatPtr which is a nullptr");
        return &((*mHatPtr)[mPosition]);
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>::reference
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator[](const difference_type n) const noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mHatPtr != nullptr, "HashedArrayTreeIterator::operator[] trying to dereference mHatPtr which is a nullptr");
        return (*mHatPtr)[mPosition + static_cast<size_type>(n)];
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>& 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator++() noexcept {
        ++mPosition;
        return *this;
    }
    
    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag> 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator++(int) noexcept {
        HashedArrayTreeIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>& 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator+=(const difference_type n) noexcept {
        mPosition += static_cast<size_type>(n);
        return *this;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag> 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator+(const difference_type n) const noexcept {
        return HashedArrayTreeIterator<constTag>(mPosition + static_cast<size_type>(n), mHatPtr);
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>& 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator--() noexcept {
        --mPosition;
        return *this;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag> 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator--(int) noexcept {
        HashedArrayTreeIterator tmp = *this;
        --(*this);
        return tmp;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>& 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator-=(const difference_type n) noexcept {
        mPosition -= static_cast<size_type>(n);
        return *this;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag> 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator-(const difference_type n) const noexcept {
        return HashedArrayTreeIterator<constTag>(mPosition - static_cast<size_type>(n), mHatPtr);
    }
              
    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename HashedArrayTree<T, Allocator>::template HashedArrayTreeIterator<constTag>::difference_type 
    HashedArrayTree<T, Allocator>::HashedArrayTreeIterator<constTag>::operator-(const HashedArrayTreeIterator& other) const noexcept {
        return static_cast<difference_type>(mPosition - other.mPosition);
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::debugPrint() const noexcept {
        std::cout << std::endl << std::endl << "=========================================" << std::endl;
        std::cout << "Total size: " << mSize << " Total capacity: " << capacity() << " Size of internal vec: " << mInternalVectorCapacity << std::endl;
        for (size_type i = 0; i < mInternalVectorCapacity; ++i) {
            const auto& leaf = mInternalData[i];
            
            if (leaf.empty()) {
                std::cout << "Leaf size/cap: 0/" << leaf.capacity() << " | ...empty..." << std::endl;
                continue;
            }

            const size_type leafSize = leaf.size();

            std::cout << "Leaf size/cap: " << leafSize << "/" << leaf.capacity() << " | ";
            for (size_type j = 0; j < mInternalVectorCapacity; ++j) {
                if (j < leafSize) {
                    std::cout << leaf[j] << " | ";
                }
                else {
                    std::cout << "  |";
                }
            }
            std::cout << std::endl;
        }
        std::cout << "=========================================" << std::endl << std::endl;
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const allocator_type& alloc)
        : mInternalData{alloc}, mInternalVectorCapacity{0}, mSize{0}, mCurrentCapacity{0}, mCurrentPow{0}, mFirstNonFullLeafIndex{0}
    {
        // mCurrentPow = 0 in the beginning even though 2 ^ 0 != mInternalVectorCapacity !!
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const size_type initialCapacity, const allocator_type& alloc)
        : mInternalData{alloc}, mInternalVectorCapacity{0}, mSize{0}, mCurrentCapacity{0}, mCurrentPow{0}, mFirstNonFullLeafIndex{0}
    {
        reserve(initialCapacity);
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const size_type initialSize, const value_type& val, const allocator_type& alloc)
        : mInternalData{alloc}, mInternalVectorCapacity{0}, mSize{0}, mCurrentCapacity{0}, mCurrentPow{0}, mFirstNonFullLeafIndex{0}
    {
        reserve(initialSize);

        // TODO: find a smarter way
        for (size_type i = 0; i < initialSize; ++i) {
            emplace_back(val);
        }
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const std::vector<value_type, allocator_type>& initVec, const allocator_type& alloc)
        : mInternalData{alloc}, mInternalVectorCapacity{0}, mSize{0}, mCurrentCapacity{0}, mCurrentPow{0}, mFirstNonFullLeafIndex{0}
    {
        reserve(initVec.size());

        // TODO: find a smarter way
        for (const auto& elem : initVec) {
            emplace_back(elem);
        }
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(std::initializer_list<value_type> initList, const allocator_type& alloc)
        : mInternalData{alloc}, mInternalVectorCapacity{0}, mSize{0}, mCurrentCapacity{0}, mCurrentPow{0}, mFirstNonFullLeafIndex{0}
    {
        reserve(initList.size());

        // TODO: find a smarter way
        for (const auto& elem : initList) {
            emplace_back(elem);
        }
    }

    template <typename T, typename Allocator>
    template <typename Iterator, typename std::enable_if<hat_internal::is_iterator<Iterator>::value, bool>::type>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(Iterator itStart, Iterator itEnd, const allocator_type& alloc)
        : mInternalData{alloc}, mInternalVectorCapacity{0}, mSize{0}, mCurrentCapacity{0}, mCurrentPow{0}, mFirstNonFullLeafIndex{0}
    {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(std::distance(itStart, itEnd) >= 0, "Distance between iterators cannot be negative");
        while (itStart != itEnd) {
            emplace_back(*itStart);
            ++itStart;
        }
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const HashedArrayTree& other) 
        : mInternalData{other.get_allocator()},
          mInternalVectorCapacity{0},
          mSize{0},
          mCurrentCapacity{0},
          mCurrentPow{0},
          mFirstNonFullLeafIndex{0}
    {
        reserve(other.mCurrentCapacity);
        for (const auto& elem : other) {
            emplace_back(elem);
        }
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>& HashedArrayTree<T, Allocator>::operator=(const HashedArrayTree& rhs) {
        if (this != &rhs) {
            mInternalData = {};
            mInternalVectorCapacity = 0;
            mSize = 0;
            mCurrentCapacity = 0;
            mCurrentPow = 0;
            mFirstNonFullLeafIndex = 0;

            reserve(rhs.mCurrentCapacity);
            for (const auto& elem : rhs) {
                emplace_back(elem);
            }
        }

        return *this;
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::allocator_type HashedArrayTree<T, Allocator>::get_allocator() const noexcept {
        return mInternalData.get_allocator();
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::size() const noexcept {
        return mSize;
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::max_size() const noexcept {
        return mInternalData.max_size();
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::capacity() const noexcept {
        return mCurrentCapacity;
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::max_capacity() const noexcept {
        return mInternalVectorCapacity * mInternalVectorCapacity;
    }

    template <typename T, typename Allocator>
    inline bool HashedArrayTree<T, Allocator>::full() const noexcept {
        return mSize > 0 && mSize == mInternalVectorCapacity * mInternalVectorCapacity;
    }

    template <typename T, typename Allocator>
    inline bool HashedArrayTree<T, Allocator>::empty() const noexcept {
        return mSize == 0;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::reserve(const size_type newCapacity) noexcept {
        if (newCapacity <= mCurrentCapacity) {
            return;
        }

        if (newCapacity < max_capacity()) {
            size_type numLeafsToAllocateFor = newCapacity / mInternalVectorCapacity;
            if ((newCapacity - numLeafsToAllocateFor * mInternalVectorCapacity) > 0) {
                ++numLeafsToAllocateFor;
            }

            for (size_type i = 0; i < numLeafsToAllocateFor; ++i) {
                if (mInternalData[i].capacity() == 0) {
                    mInternalData[i].reserve(mInternalVectorCapacity);
                }
            }

            // TODO: Benchmark if this is noticeably slowe than doing it in the above loop with some bool flag `found`
            for (size_type i = 0; i < numLeafsToAllocateFor; ++i) {
                if (mInternalData[i].size() < mInternalData[i].capacity()) {
                    mFirstNonFullLeafIndex = i;
                    break;
                }
            }

            mCurrentCapacity = numLeafsToAllocateFor * mInternalVectorCapacity;

            return;
        }

        const size_type ceilOfRoot = static_cast<size_type>(std::ceil(std::sqrt(newCapacity)));
        const size_type newInternalVectorCapacity = hat_internal::next_power_of_2<size_type>(ceilOfRoot);

        mInternalData.reserve(newInternalVectorCapacity);

        for (size_type i = mInternalVectorCapacity; i < newInternalVectorCapacity; ++i) {
            mInternalData.emplace_back(std::vector<value_type, allocator_type>{});
        }

        size_type numLeafsToAllocateFor = newCapacity / newInternalVectorCapacity;
        if ((newCapacity - numLeafsToAllocateFor * newInternalVectorCapacity) > 0) {
            ++numLeafsToAllocateFor;
        }

        for (size_type i = 0; i < numLeafsToAllocateFor; ++i) {
            mInternalData[i].reserve(newInternalVectorCapacity);
        }

        // reorder
        for (size_type i = 0; i < mInternalVectorCapacity / 2; ++i) {
            auto& currentRow = mInternalData[i];

            // TODO: find a way to rewrite this alg to avoid this check on every loop iteration
            if (i == 0) {
                if (currentRow.empty()) {
                    break;
                }

                currentRow.reserve(newInternalVectorCapacity);
            }
            else {
                auto& v1 = mInternalData[i * 2];

                if (v1.empty()) {
                    break;
                }
                
                currentRow.reserve(newInternalVectorCapacity);
                currentRow.insert(currentRow.end(), std::make_move_iterator(v1.begin()), std::make_move_iterator(v1.end()));
                v1.clear();
            }

            auto& v2 = mInternalData[i * 2 + 1];

            if (v2.empty()) {
                continue;
            }

            currentRow.insert(currentRow.end(), std::make_move_iterator(v2.begin()), std::make_move_iterator(v2.end()));
            v2.clear();
        }

        // shrink the ones that are left
        for (size_type i = numLeafsToAllocateFor; i < mInternalVectorCapacity; ++i) {
            mInternalData[i] = std::vector<value_type, allocator_type>{};
        }

        // find index for first non-full leaf to avoid calculating that index on every insertion
        // TODO: See if there is a smarter way
        for (size_type i = 0; i < numLeafsToAllocateFor; ++i) {
            if (mInternalData[i].size() < mInternalData[i].capacity()) {
                mFirstNonFullLeafIndex = i;
                break;
            }
        }
    
        mCurrentCapacity = numLeafsToAllocateFor * newInternalVectorCapacity;
        mInternalVectorCapacity = newInternalVectorCapacity;
        mCurrentPow = hat_internal::what_power_of_2<size_type>(mInternalVectorCapacity);
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::clear() noexcept {
        for (auto& leafVector : mInternalData) {
            leafVector.clear();
        }

        mSize = 0;
        mFirstNonFullLeafIndex = 0;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::shrink_to_fit() noexcept {
        for (auto& leafVector : mInternalData) {
            if (leafVector.empty()) {
                mCurrentCapacity -= leafVector.capacity();
                leafVector = LeafVector{};
            }
        }
    }

    template <typename T, typename Allocator>
    inline std::vector<typename HashedArrayTree<T, Allocator>::value_type> HashedArrayTree<T, Allocator>::get_as_vector() const noexcept {
        std::vector<value_type> res;
        res.reserve(mSize);

        for (const auto& leaf : mInternalData) {
            res.insert(res.end(), leaf.begin(), leaf.end());
        }

        return res;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::push_back(const value_type& elem) {
        if (mSize == mCurrentCapacity) {
            reserve(mSize + 1);
        }

        auto& leaf = mInternalData[mFirstNonFullLeafIndex];

        leaf.push_back(elem);

        if (leaf.size() == mInternalVectorCapacity) {
            ++mFirstNonFullLeafIndex;
        }

        ++mSize;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::push_back(value_type&& elem) {
        if (mSize == mCurrentCapacity) {
            reserve(mSize + 1);
        }

        auto& leaf = mInternalData[mFirstNonFullLeafIndex];

        mInternalData[mFirstNonFullLeafIndex].push_back(std::forward<value_type>(elem));

        if (leaf.size() == mInternalVectorCapacity) {
            ++mFirstNonFullLeafIndex;
        }
        
        ++mSize;
    }

    template <typename T, typename Allocator>
    template <typename ...Args>
    inline void HashedArrayTree<T, Allocator>::emplace_back(Args&&... args) {
        if (mSize == mCurrentCapacity) {
            reserve(mSize + 1);
        }

        auto& leaf = mInternalData[mFirstNonFullLeafIndex];

        mInternalData[mFirstNonFullLeafIndex].emplace_back(std::forward<Args>(args)...);

        if (leaf.size() == mInternalVectorCapacity) {
            ++mFirstNonFullLeafIndex;
        }
        
        ++mSize;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::pop_back() {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mSize > 0, "Calling pop_back() on empty HashedArrayTree");
        if (mInternalData[mFirstNonFullLeafIndex].empty()) {
            --mFirstNonFullLeafIndex;
        }

        mInternalData[mFirstNonFullLeafIndex].pop_back();
        --mSize;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::swap(HashedArrayTree& other) noexcept {
        std::swap(mInternalData, other.mInternalData);
        std::swap(mInternalVectorCapacity, other.mInternalVectorCapacity);
        std::swap(mSize, other.mSize);
        std::swap(mCurrentCapacity, other.mCurrentCapacity);
        std::swap(mCurrentPow, other.mCurrentPow);
        std::swap(mFirstNonFullLeafIndex, other.mFirstNonFullLeafIndex);
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::reference HashedArrayTree<T, Allocator>::operator[](const size_type pos) noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(0 <= pos && pos < mSize, "HashedArrayTree subscript operator out of range");
        return mInternalData[pos >> mCurrentPow][pos & (mInternalVectorCapacity - 1)];
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_reference HashedArrayTree<T, Allocator>::operator[](const size_type pos) const noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(0 <= pos && pos < mSize, "HashedArrayTree subscript operator out of range");
        return mInternalData[pos >> mCurrentPow][pos & (mInternalVectorCapacity - 1)];
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::reference HashedArrayTree<T, Allocator>::at(const size_type pos) {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(0 <= pos && pos < mSize, "HashedArrayTree subscript operator out of range");
        return mInternalData.at(pos >> mCurrentPow).at(pos & (mInternalVectorCapacity - 1));
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_reference HashedArrayTree<T, Allocator>::at(const size_type pos) const {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(0 <= pos && pos < mSize, "HashedArrayTree subscript operator out of range");
        return mInternalData.at(pos >> mCurrentPow).at(pos & (mInternalVectorCapacity - 1));
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::reference HashedArrayTree<T, Allocator>::front() noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mSize > 0, "Calling front() on empty HashedArrayTree");
        return *begin();
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_reference HashedArrayTree<T, Allocator>::front() const noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mSize > 0, "Calling front() on empty HashedArrayTree");
        return *begin();
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::reference HashedArrayTree<T, Allocator>::back() noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mSize > 0, "Calling back() on empty HashedArrayTree");
        return *(end() - 1);
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_reference HashedArrayTree<T, Allocator>::back() const noexcept {
        SIMPLE_HASHED_ARRAY_TREE_ASSERT(mSize > 0, "Calling back() on empty HashedArrayTree");
        return *(end() - 1);
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::iterator HashedArrayTree<T, Allocator>::begin() noexcept {
        if (empty()) {
            return end();
        }

        return iterator{0, this};
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::iterator HashedArrayTree<T, Allocator>::end() noexcept {
        return iterator{mSize, this};
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_iterator HashedArrayTree<T, Allocator>::begin() const noexcept {
        if (empty()) {
            return end();
        }

        return const_iterator{0, this};
    } 

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_iterator HashedArrayTree<T, Allocator>::end() const noexcept {
        return const_iterator{mSize, this};
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_iterator HashedArrayTree<T, Allocator>::cbegin() const noexcept {
        return begin();
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::const_iterator HashedArrayTree<T, Allocator>::cend() const noexcept {
        return end();
    }

    template <typename T, typename Allocator>
    inline bool operator==(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T, typename Allocator>
    inline bool operator!=(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept {
        return lhs.size() != rhs.size() || !std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T, typename Allocator>
    inline bool operator<(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename T, typename Allocator>
    inline bool operator<=(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept {
        return !(rhs < lhs);
    }

    template <typename T, typename Allocator>
    inline bool operator>(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept {
        return rhs < lhs;
    }

    template <typename T, typename Allocator>
    inline bool operator>=(const HashedArrayTree<T, Allocator>& lhs, const HashedArrayTree<T, Allocator>& rhs) noexcept {
        return !(lhs < rhs);
    }

    namespace hat_internal {
        template<typename... Ts>
        struct make_void {
            using type = void;
        };
        
        template<typename... Ts>
        using void_t = typename make_void<Ts...>::type;

        template <typename T, typename>
        struct is_iterator : std::false_type 
        {};

        template <typename T>
        struct is_iterator<T, void_t<
            typename std::iterator_traits<T>::iterator_category
        >> : std::true_type 
        {};

        template <typename T>
        inline T next_power_of_2(T capacity) noexcept {
            SIMPLE_HASHED_ARRAY_TREE_STATIC_ASSERT(std::is_integral<T>::value, "Capacity must of integral type");
            SIMPLE_HASHED_ARRAY_TREE_ASSERT(capacity >= 0, "Capacity must be >= 0");

            if (capacity == 0) {
                return 1;
            }

            --capacity;

            constexpr auto num_digits_for_type = std::numeric_limits<T>::digits;

            for (size_t shift = 1; shift < num_digits_for_type; shift *= 2) {
                capacity |= capacity >> shift;
            }
            return capacity + 1;
        }

        template <typename T>
        inline T what_power_of_2(T capacity) noexcept {
            SIMPLE_HASHED_ARRAY_TREE_STATIC_ASSERT(std::is_integral<T>::value, "Capacity must of integral type");
            SIMPLE_HASHED_ARRAY_TREE_ASSERT(capacity > 0, "Capacity must be > 0");
            return static_cast<T>(std::log2(capacity));
        }
    } // namespace hat_internal
} // namespace simpleContainers

#endif // SIMPLE_HASHED_ARRAY_TREE_HPP
