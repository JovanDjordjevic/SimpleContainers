/// @file simpleRingBuffer.hpp
/// @brief File containing API and implementaiton of RingBuffer class

#ifndef SIMPLE_RING_BUFFER_HPP
#define SIMPLE_RING_BUFFER_HPP

#include <algorithm>
#include <initializer_list>
#include <vector>

#ifndef SIMPLE_RING_BUFFER_DEBUG
    /// @brief Macro for debug checks
    /// @details This definition controls if some debug checks are made at compile and runtime. If the standard NDEBUG
    ///          is defined or user defines SIMPLE_RING_BUFFER_NO_DEBUG during during compilation, no debug checks will be made.
    ///          It is recomended to leave debug checks active during development, and turn them off for release builds
    #define SIMPLE_RING_BUFFER_DEBUG
#endif // #ifndef SIMPLE_RING_BUFFER_DEBUG

#if defined NDEBUG || defined SIMPLE_RING_BUFFER_NO_DEBUG
    #undef SIMPLE_RING_BUFFER_DEBUG
#endif // #if defined NDEBUG || defined SIMPLE_RING_BUFFER_NO_DEBUG

#ifdef SIMPLE_RING_BUFFER_DEBUG
    #include <iostream>

    /// @brief Default stream where debug messages will be printed when they are enabled
    #define SIMPLE_RING_BUFFER_DEBUG_OUTPUT_STREAM std::cerr

    #define SIMPLE_RING_BUFFER_ASSERT(cond, msg) \
        do { \
            if (!(cond)) { \
                SIMPLE_RING_BUFFER_DEBUG_OUTPUT_STREAM << "\nRING BUFFER ASSERT FAILED WITH MESSAGE: " << msg \
                    << "\nEXPECTED: " << #cond \
                    << "\nFILE: " << __FILE__ << ", LINE " << __LINE__ << std::endl << std::endl; \
                std::abort(); \
            } \
        } while(0)

    #define SIMPLE_RING_BUFFER_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
    #define SIMPLE_RING_BUFFER_ASSERT(cond, msg) do {} while (false)
    #define SIMPLE_RING_BUFFER_STATIC_ASSERT(cond, msg) ;
#endif // #ifdef SIMPLE_RING_BUFFER_DEBUG

// ============================================================================================================================================
// =================================================================== API ====================================================================
// ============================================================================================================================================

/// @brief Namespace containing all relevant classes and functions
namespace simpleContainers {
    template <typename T, typename Allocator>
    class RingBuffer;

    template <typename T, typename Allocator>
    inline bool operator==(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator!=(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator<(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator<=(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator>=(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    /// @brief Class representing a ring buffer structure
    /// @details This is the main class the user should interact with. RingBuffer of capacity N will
    ///          hold at most the last N inserted elements. Every insertion after the N-th will cause the oldest element to
    ///          be dropped. Internally, it is implemented as an adaptor of std::vector. Most of the member functions
    ///          are similar to a std::vector and have the same behavior and time complexity unless otherwise specified
    ///          Since the size of the ring buffer is the same as it's capacity in the expected use cases,
    ///          most of the operations are optimized for this case when possible.
    ///          A strict weak ordering can be established between instances of RingBuffer, behavior is the same as std::vector
    /// @tparam T Type of object contained inside RingBuffer. T must satisfy the same requirements as
    ///         if it was inserted into a std::vector
    /// @tparam Allocator Allocator for said type. RingBuffer itself does not do any of the allocator calls.
    ///         Those are done in the underlying std::vector which will use this allocator
    template <typename T, typename Allocator = std::allocator<T>>
    class RingBuffer {
        public:
            using value_type = T;
            using allocator_type = Allocator;
            using reference = T&;
            using const_reference = const T&;
            using pointer = typename std::allocator_traits<allocator_type>::pointer;
            using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
            using size_type = typename std::allocator_traits<allocator_type>::size_type;
            using difference_type = typename std::allocator_traits<allocator_type>::difference_type;

            SIMPLE_RING_BUFFER_STATIC_ASSERT((!std::is_same<value_type, bool>::value), "RingBuffer<bool> currently not supported.");
            SIMPLE_RING_BUFFER_STATIC_ASSERT((std::is_same<value_type, typename allocator_type::value_type>::value), "RingBuffer::value_type and RingBuffer::Allocator::value_type must be the same.");

            /// @brief Class representing iterators over RingBuffer
            /// @details RingBugffer iterators are compliant with the LegacyRandomAccessIterator named requirement.
            ///          All methods are O(1) time complexity
            /// @tparam constTag Compile time indicator if iterator is a const iterator or not
            template <bool constTag = false>
            class RingBufferIterator {
                public:
                    friend class RingBufferIterator<false>;
                    friend class RingBufferIterator<true>;

                    using iterator_category = std::random_access_iterator_tag;
                    using size_type = typename RingBuffer<T, Allocator>::size_type;
                    using difference_type = typename RingBuffer<T, Allocator>::difference_type;
                    using value_type = typename RingBuffer<T, Allocator>::value_type;
                    using pointer = typename std::conditional<constTag, typename RingBuffer<T, Allocator>::const_pointer, typename RingBuffer<T, Allocator>::pointer>::type;
                    using reference = typename std::conditional<constTag, typename RingBuffer<T, Allocator>::const_reference, typename RingBuffer<T, Allocator>::reference>::type;
                    using ring_buffer_ptr = typename std::conditional<constTag, const RingBuffer<T, Allocator>*, RingBuffer<T, Allocator>*>::type;

                    RingBufferIterator(size_type pos = 0, ring_buffer_ptr rb = nullptr) noexcept;
                    RingBufferIterator(const RingBufferIterator& other) noexcept = default;
                    /// @brief Converting constructor to create a const iterator from a non-const iterator
                    /// @details By using SFINAE, this constructor is only available for const iterators since they must 
                    ///          be implicitly constructible from a non-const iterator
                    template <bool C = constTag, typename = typename std::enable_if<C>::type>
                    RingBufferIterator(const RingBufferIterator<false>& other) noexcept;
                    RingBufferIterator(RingBufferIterator&& other) noexcept = default;
                    RingBufferIterator& operator=(const RingBufferIterator& rhs) noexcept = default;
                    RingBufferIterator& operator=(RingBufferIterator&& rhs) noexcept = default;
                    ~RingBufferIterator() noexcept = default;

                    void swap(RingBufferIterator& other) noexcept;

                    reference operator*() const noexcept;
                    pointer operator->() const noexcept;

                    reference operator[](const difference_type n) const noexcept;

                    RingBufferIterator& operator++() noexcept; // prefix
                    RingBufferIterator operator++(int) noexcept; // postfix
                    RingBufferIterator& operator+=(const difference_type n) noexcept;
                    RingBufferIterator operator+(const difference_type n) const noexcept;
                    friend RingBufferIterator operator+(const difference_type n, RingBufferIterator rhs) noexcept {
                        rhs += n;
                        return rhs;
                    }

                    RingBufferIterator& operator--() noexcept; // prefix
                    RingBufferIterator operator--(int) noexcept; // postfix  
                    RingBufferIterator& operator-=(const difference_type n) noexcept;
                    RingBufferIterator operator-(const difference_type n) const noexcept;
                    difference_type operator-(const RingBufferIterator& other) const noexcept; // Subtraction between two iterators

                    friend bool operator==(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        SIMPLE_RING_BUFFER_ASSERT(lhs.mRingBufPtr == rhs.mRingBufPtr, "RingBufferIterator == comparison must be done on iterators of the same RingBuffer");
                        return lhs.mPosition == rhs.mPosition;
                    }

                    friend bool operator!=(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        SIMPLE_RING_BUFFER_ASSERT(lhs.mRingBufPtr == rhs.mRingBufPtr, "RingBufferIterator != comparison must be done on iterators of the same RingBuffer");
                        return lhs.mPosition != rhs.mPosition;
                    }

                    friend bool operator<(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        SIMPLE_RING_BUFFER_ASSERT(lhs.mRingBufPtr == rhs.mRingBufPtr, "RingBufferIterator < comparison must be done on iterators of the same RingBuffer");
                        return lhs.mPosition < rhs.mPosition;
                    }

                    friend bool operator<=(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        SIMPLE_RING_BUFFER_ASSERT(lhs.mRingBufPtr == rhs.mRingBufPtr, "RingBufferIterator <= comparison must be done on iterators of the same RingBuffer");
                        return lhs.mPosition <= rhs.mPosition;
                    }

                    friend bool operator>(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        SIMPLE_RING_BUFFER_ASSERT(lhs.mRingBufPtr == rhs.mRingBufPtr, "RingBufferIterator > comparison must be done on iterators of the same RingBuffer");
                        return lhs.mPosition > rhs.mPosition;
                    }

                    friend bool operator>=(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        SIMPLE_RING_BUFFER_ASSERT(lhs.mRingBufPtr == rhs.mRingBufPtr, "RingBufferIterator >= comparison must be done on iterators of the same RingBuffer");
                        return lhs.mPosition >= rhs.mPosition;
                    }

                private:
                    // mPosition represents the element in order from oldest to newest inserted, it is not the actual
                    // pointer offset position from beginning of vector
                    size_type mPosition;
                    ring_buffer_ptr mRingBufPtr;
            };

            using iterator = RingBufferIterator<false>;
            using const_iterator = RingBufferIterator<true>;

        public:
            /// @brief RingBuffer cannot be constructed with 0 capacity so this arbitrary value was chosen as a default
            static constexpr size_type defaultInitialCapacity = 64;

            RingBuffer(const size_type initialCapacity = defaultInitialCapacity, const allocator_type& alloc = allocator_type{});
            RingBuffer(const size_type initialCapacity, const value_type& val, const allocator_type& alloc = allocator_type{});
            RingBuffer(const std::vector<value_type, allocator_type>& initVec, const allocator_type& alloc = allocator_type{});
            RingBuffer(std::initializer_list<value_type> initList, const allocator_type& alloc = allocator_type{});
            template <typename Iterator>
            RingBuffer(Iterator itStart, Iterator itEnd, const allocator_type& alloc = allocator_type{});

            RingBuffer(const RingBuffer& other) = default;
            RingBuffer(RingBuffer&& other) noexcept = default;

            RingBuffer& operator=(const RingBuffer& rhs) = default;
            RingBuffer& operator=(RingBuffer&& rhs) noexcept = default;

            ~RingBuffer() noexcept = default;

            allocator_type get_allocator() const noexcept;
            size_type capacity() const noexcept;
            /// @brief Change capacity of the current RingBuffer
            /// @details If the new capacity is lower than the current one, then only the newest
            ///          currentCapacity - newCapacity elements will be kept. If new capacity is greater, all elements
            ///          are kept and no elements are dropped for the next newCapacity - currentCapacity insertions.
            ///          In both cases this is a O(n) operation since internally a new vector containing the appropriate elements
            ///          is created and swapped with the current internal vector.
            void change_capacity(const size_type newCapacity) noexcept;
            size_type size() const noexcept;
            size_type max_size() const noexcept;
            bool empty() const noexcept;
            bool full() const noexcept;
            void clear() noexcept;

            /// @brief Get elements in RingBuffer in order they were inserted (oldest first)
            std::vector<value_type> get_elements() const noexcept;

            void push_back(const value_type& elem);
            void push_back(value_type&& elem);
            template <typename ...Args>
            void emplace_back(Args&&... args);

            void swap(RingBuffer& other) noexcept;

            /// @brief Erase element at given iterator
            /// @return Iterator to element that comes aftr the erased element (or end iterator if erased element was the last one)
            iterator erase(const_iterator it) noexcept;
            /// @brief Erase elements in iterator range [first, last)
            /// @return Iterator to element that comes aftr the last erased element (or end iterator if no elements exist after last)
            iterator erase(const_iterator first, const_iterator last) noexcept;

            /// @brief Subscript operator
            /// @details Indexing is done in insertion order, so the oldest element will be at position 0, the second oldest at position 1 etc.
            ///          This operator performs out of range checks for pos only when SIMPLE_RING_BUFFER_DEBUG is defined
            /// @return Reference to element at index pos.
            reference operator[](const size_type& pos) noexcept;
            /// @brief Subscript operator
            /// @details Indexing is done in insertion order, so the oldest element will be at position 0, the second oldest at position 1 etc.
            ///          This operator performs out of range checks for pos only when SIMPLE_RING_BUFFER_DEBUG is defined
            const_reference operator[](const size_type& pos) const noexcept;
            /// @brief Access element at specified position
            /// @details Indexing is done in insertion order, so the oldest element will be at position 0, the second oldest at position 1 etc.
            ///          Validity of pos is always checked
            reference at(const size_type& pos);
            /// @brief Access element at specified position
            /// @details Indexing is done in insertion order, so the oldest element will be at position 0, the second oldest at position 1 etc.
            ///          Validity of pos is always checked
            const_reference at(const size_type& pos) const;

            iterator begin() noexcept;
            iterator end() noexcept;
            const_iterator begin() const noexcept;
            const_iterator end() const noexcept;
            const_iterator cbegin() const noexcept;
            const_iterator cend() const noexcept;

            friend bool operator== <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;
            friend bool operator!= <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;
            friend bool operator< <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;
            friend bool operator<= <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;
            friend bool operator> <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;
            friend bool operator>= <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

        private:
            std::vector<value_type, allocator_type> mBuffer;
            size_type mCurrentCapacity;
            size_type mNewestElementInsertionIndex;
    };
} // namespace simpleContainers

// ============================================================================================================================================
// ============================================================== IMPLEMENTATION ==============================================================
// ============================================================================================================================================

namespace simpleContainers {
    template <typename T, typename Allocator>
    template <bool constTag>
    inline RingBuffer<T, Allocator>::RingBufferIterator<constTag>::RingBufferIterator(size_type pos, ring_buffer_ptr rb) noexcept
        : mPosition{pos}, mRingBufPtr{rb}
    {}

    template <typename T, typename Allocator>
    template <bool constTag> 
    template <bool C, typename> 
    inline RingBuffer<T, Allocator>::RingBufferIterator<constTag>::RingBufferIterator(const RingBuffer<T, Allocator>::RingBufferIterator<false> &other) noexcept
        : mPosition{other.mPosition}, mRingBufPtr{other.mRingBufPtr} 
    {}

    template <typename T, typename Allocator>
    template <bool constTag>
    inline void RingBuffer<T, Allocator>::RingBufferIterator<constTag>::swap(RingBufferIterator& other) noexcept {
        std::swap(mPosition, other.mPosition);
        std::swap(mRingBufPtr, other.mRingBufPtr);
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>::reference
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator*() const noexcept {
        SIMPLE_RING_BUFFER_ASSERT(mRingBufPtr != nullptr, "RingBufferIterator::operator* trying to dereference mRingBufPtr which is a nullptr");
        return (*mRingBufPtr)[mPosition];
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>::pointer 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator->() const noexcept {
        SIMPLE_RING_BUFFER_ASSERT(mRingBufPtr != nullptr, "RingBufferIterator::operator-> trying to dereference mRingBufPtr which is a nullptr");
        return &((*mRingBufPtr)[mPosition]);
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>::reference
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator[](const difference_type n) const noexcept {
        SIMPLE_RING_BUFFER_ASSERT(mRingBufPtr != nullptr, "RingBufferIterator::operator[] trying to dereference mRingBufPtr which is a nullptr");
        return (*mRingBufPtr)[mPosition + n];
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>& 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator++() noexcept {
        ++mPosition;
        return *this;
    }
    
    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag> 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator++(int) noexcept {
        RingBufferIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>& 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator+=(const difference_type n) noexcept {
        mPosition += n;
        return *this;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag> 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator+(const difference_type n) const noexcept {
        return RingBufferIterator<constTag>(mPosition + n, mRingBufPtr);
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>& 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator--() noexcept {
        --mPosition;
        return *this;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag> 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator--(int) noexcept {
        RingBufferIterator tmp = *this;
        --(*this);
        return tmp;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>& 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator-=(const difference_type n) noexcept {
        mPosition -= n;
        return *this;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag> 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator-(const difference_type n) const noexcept {
        return RingBufferIterator<constTag>(mPosition - n, mRingBufPtr);
    }
              
    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>::difference_type 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator-(const RingBufferIterator& other) const noexcept {
        return mPosition - other.mPosition;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(const size_type initialCapacity, const allocator_type& alloc)
        : mBuffer{std::vector<value_type, allocator_type>{alloc}}, mCurrentCapacity{initialCapacity}, mNewestElementInsertionIndex{0}
    {
        SIMPLE_RING_BUFFER_ASSERT(initialCapacity != 0, "RingBuffer must not be constructed with initial capacity of 0");
        mBuffer.reserve(mCurrentCapacity);
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(const size_type initialCapacity, const value_type& val, const allocator_type& alloc)
        : mBuffer{std::vector<value_type, allocator_type>(initialCapacity, val, alloc)}, mCurrentCapacity{initialCapacity}, mNewestElementInsertionIndex{0}
    {
        SIMPLE_RING_BUFFER_ASSERT(initialCapacity != 0, "RingBuffer must not be constructed with initial capacity of 0");
        mBuffer.reserve(mCurrentCapacity);
    }
    
    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(const std::vector<value_type, allocator_type>& initVec, const allocator_type& alloc)
        : mBuffer(initVec, alloc), mCurrentCapacity{initVec.size()}, mNewestElementInsertionIndex{0}
    {
        SIMPLE_RING_BUFFER_ASSERT(initVec.size() != 0, "RingBuffer must not be constructed from an empty std::vector");
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(std::initializer_list<value_type> initList, const allocator_type& alloc)
        : mBuffer(initList, alloc), mCurrentCapacity{initList.size()}, mNewestElementInsertionIndex{0}
    {
        SIMPLE_RING_BUFFER_ASSERT(initList.size() != 0, "RingBuffer must not be constructed from an empty std::initializer_list");
    }

    template <typename T, typename Allocator>
    template <typename Iterator>
    inline RingBuffer<T, Allocator>::RingBuffer(Iterator itStart, Iterator itEnd, const allocator_type& alloc)
        : mBuffer(itStart, itEnd, alloc), mCurrentCapacity{static_cast<size_type>(std::distance(itStart, itEnd))}, mNewestElementInsertionIndex{0}
    {
        SIMPLE_RING_BUFFER_ASSERT(std::distance(itStart, itEnd) >= 0, "Distance between iterators cannot be negative");
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::allocator_type RingBuffer<T, Allocator>::get_allocator() const noexcept {
        return mBuffer.get_allocator();
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::size_type RingBuffer<T, Allocator>::capacity() const noexcept {
        return mCurrentCapacity;
    }

    template <typename T, typename Allocator>
    inline void RingBuffer<T, Allocator>::change_capacity(const size_type newCapacity) noexcept {
        SIMPLE_RING_BUFFER_ASSERT(newCapacity != 0, "RingBuffer::change_capacity new capacity must not be 0");

        if (newCapacity == mCurrentCapacity) {
            return;
        }

        std::vector<value_type, allocator_type> newBuffer(get_allocator());
        newBuffer.reserve(newCapacity);

        auto it = begin();
        if (newCapacity < mCurrentCapacity) { // only keep the last newCapacity elements
            size_type tmp = mCurrentCapacity - newCapacity;
            while (tmp > 0) {
                ++it;
                --tmp;
            }
        }

        auto itEnd = end();
        while (it != itEnd) {
            newBuffer.emplace_back(*it);
            ++it;
        }

        mBuffer.swap(newBuffer);
        mNewestElementInsertionIndex = newBuffer.size() % newCapacity;
        mCurrentCapacity = newCapacity;
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::size_type RingBuffer<T, Allocator>::size() const noexcept {
        return mBuffer.size();
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::size_type RingBuffer<T, Allocator>::max_size() const noexcept {
        return mBuffer.max_size();
    }

    template <typename T, typename Allocator>
    inline bool RingBuffer<T, Allocator>::empty() const noexcept {
        return mBuffer.empty();
    }

    template <typename T, typename Allocator>
    inline bool RingBuffer<T, Allocator>::full() const noexcept {
        return mBuffer.size() == mCurrentCapacity;
    }

    template <typename T, typename Allocator>
    inline void RingBuffer<T, Allocator>::clear() noexcept {
        mNewestElementInsertionIndex = 0;
        mBuffer.clear();
    }

    template <typename T, typename Allocator>
    inline std::vector<typename RingBuffer<T, Allocator>::value_type> RingBuffer<T, Allocator>::get_elements() const noexcept {
        if (mBuffer.size() < mCurrentCapacity) {
            return mBuffer;
        }
        else {
            std::vector<value_type> result;
            result.reserve(mCurrentCapacity);

            for (size_type i = mNewestElementInsertionIndex; i < mCurrentCapacity; ++i) {
                result.emplace_back(mBuffer[i]);
            }
            
            for (size_type i = 0; i < mNewestElementInsertionIndex; ++i) {
                result.emplace_back(mBuffer[i]);
            }

            return result;
        }
    }

    template <typename T, typename Allocator>
    inline void RingBuffer<T, Allocator>::push_back(const value_type& elem) {
        if (mBuffer.size() == mCurrentCapacity) {   // most common case
            mBuffer[mNewestElementInsertionIndex] = elem;
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.push_back(elem);
        }

        ++mNewestElementInsertionIndex;

        if (mNewestElementInsertionIndex == mCurrentCapacity) {
            mNewestElementInsertionIndex = 0;
        }
    }

    template <typename T, typename Allocator>
    inline void RingBuffer<T, Allocator>::push_back(value_type&& elem) {
        if (mBuffer.size() == mCurrentCapacity) {   // most common case
            mBuffer[mNewestElementInsertionIndex] = std::forward<value_type>(elem);
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.push_back(std::forward<value_type>(elem));
        }

        ++mNewestElementInsertionIndex;

        if (mNewestElementInsertionIndex == mCurrentCapacity) {
            mNewestElementInsertionIndex = 0;
        }
    }

    template <typename T, typename Allocator>
    template <typename ...Args>
    inline void RingBuffer<T, Allocator>::emplace_back(Args&&... args) {
        if (mBuffer.size() == mCurrentCapacity) {   // most common case
            mBuffer[mNewestElementInsertionIndex] = T{std::forward<Args>(args)...};
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.emplace_back(std::forward<Args>(args)...);
        }

        ++mNewestElementInsertionIndex;

        if (mNewestElementInsertionIndex == mCurrentCapacity) {
            mNewestElementInsertionIndex = 0;
        }
    }

    template <typename T, typename Allocator>
    inline void RingBuffer<T, Allocator>::swap(RingBuffer& other) noexcept {
        std::swap(mBuffer, other.mBuffer);
        std::swap(mCurrentCapacity, other.mCurrentCapacity);
        std::swap(mNewestElementInsertionIndex, other.mNewestElementInsertionIndex);
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::iterator RingBuffer<T, Allocator>::erase(const_iterator it) noexcept {
        // reorder the internal vector so that it's erase method may be used
        auto mBugBegin = mBuffer.begin();
        std::rotate(mBugBegin, mBugBegin + mNewestElementInsertionIndex, mBuffer.end());
        auto dist = it - cbegin();
        mBuffer.erase(mBuffer.begin() + dist);
        mNewestElementInsertionIndex = mBuffer.size();
        return iterator{static_cast<typename iterator::size_type>(dist), this};
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::iterator RingBuffer<T, Allocator>::erase(const_iterator first, const_iterator last) noexcept {
        SIMPLE_RING_BUFFER_ASSERT((last - first) >= 0, "Iterator to last element cannot be before iterator to first element");
        // reorder the internal vector so that it's erase method may be used
        auto mBufBegin = mBuffer.begin();
        std::rotate(mBufBegin, mBufBegin + mNewestElementInsertionIndex, mBuffer.end());
        auto distFirst = first - cbegin();
        auto distLast = last - cbegin();
        mBufBegin = mBuffer.begin();
        mBuffer.erase(mBufBegin + distFirst, mBufBegin + distLast);
        mNewestElementInsertionIndex = mBuffer.size();
        return iterator{static_cast<typename iterator::size_type>(distFirst), this};
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::reference RingBuffer<T, Allocator>::operator[](const size_type& pos) noexcept {
        auto index = pos;
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            index += mNewestElementInsertionIndex;
            if (index >= mCurrentCapacity) {
                index -= mCurrentCapacity;
            }
        }

        SIMPLE_RING_BUFFER_ASSERT(0 <= index && index < mBuffer.size(), "RingBuffer subscript operator out of range");
        return mBuffer[index];
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_reference RingBuffer<T, Allocator>::operator[](const size_type& pos) const noexcept {
        auto index = pos;
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            index += mNewestElementInsertionIndex;
            if (index >= mCurrentCapacity) {
                index -= mCurrentCapacity;
            }
        }

        SIMPLE_RING_BUFFER_ASSERT(0 <= index && index < mBuffer.size(), "RingBuffer subscript operator out of range");
        return mBuffer[index];
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::reference RingBuffer<T, Allocator>::at(const size_type& pos) {
        auto index = pos;
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            index += mNewestElementInsertionIndex;
            if (index >= mCurrentCapacity) {
                index -= mCurrentCapacity;
            }
        }

        return mBuffer.at(index);
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_reference RingBuffer<T, Allocator>::at(const size_type& pos) const {
        auto index = pos;
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            index += mNewestElementInsertionIndex;
            if (index >= mCurrentCapacity) {
                index -= mCurrentCapacity;
            }
        }

        return mBuffer.at(index);
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::iterator RingBuffer<T, Allocator>::begin() noexcept {
        if (mBuffer.empty()) {
            return end();
        }

        return iterator{0, this};
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::iterator RingBuffer<T, Allocator>::end() noexcept {
        return iterator{mBuffer.size(), this};
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_iterator RingBuffer<T, Allocator>::begin() const noexcept {
        if (mBuffer.empty()) {
            return end();
        }

        return const_iterator{0, this};
    } 

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_iterator RingBuffer<T, Allocator>::end() const noexcept {
        return const_iterator{mBuffer.size(), this};
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_iterator RingBuffer<T, Allocator>::cbegin() const noexcept {
        return begin();
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_iterator RingBuffer<T, Allocator>::cend() const noexcept {
        return end();
    }

    template <typename T, typename Allocator>
    inline bool operator==(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T, typename Allocator>
    inline bool operator!=(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept {
        return lhs.size() != rhs.size() || !std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T, typename Allocator>
    inline bool operator<(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename T, typename Allocator>
    inline bool operator<=(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept {
        return !(rhs < lhs);
    }

    template <typename T, typename Allocator>
    inline bool operator>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept {
        return rhs < lhs;
    }

    template <typename T, typename Allocator>
    inline bool operator>=(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept {
        return !(lhs < rhs);
    }

} // namespace simpleContainers

#endif // SIMPLE_RING_BUFFER_HPP
