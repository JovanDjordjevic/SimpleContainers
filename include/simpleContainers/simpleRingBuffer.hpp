#ifndef __SIMPLE_RING_BUFFER__
#define __SIMPLE_RING_BUFFER__

#include <initializer_list>
#include <iostream>
#include <vector>

// =============================== API ===============================

namespace simpleContainers {
    template <typename T, typename Allocator>
    class RingBuffer;

    template <typename T, typename Allocator>
    inline bool operator==(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator>
    inline bool operator!=(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;

    template <typename T, typename Allocator = std::allocator<T>>
    class RingBuffer {
        public:
            using value_type = T;
            using allocator_type = Allocator;
            using reference = T&;
            using const_reference = const T&;
            using pointer = T*;
            using const_pointer = const T*;
            using size_type = typename std::vector<T, Allocator>::size_type;
            using difference_type = typename std::vector<T, Allocator>::difference_type;

            template <bool constTag = false>
            class RingBufferIterator {
                public:
                    friend class RingBufferIterator<false>;
                    friend class RingBufferIterator<true>;

                    using iterator_category = std::bidirectional_iterator_tag;
                    using size_type = typename RingBuffer<T, Allocator>::size_type;
                    using difference_type = typename RingBuffer<T, Allocator>::difference_type;
                    using value_type = typename RingBuffer<T, Allocator>::value_type;
                    using pointer = typename std::conditional<constTag, typename RingBuffer<T, Allocator>::const_pointer, typename RingBuffer<T, Allocator>::pointer>::type;
                    using reference = typename std::conditional<constTag, typename RingBuffer<T, Allocator>::const_reference, typename RingBuffer<T, Allocator>::reference>::type;
                    using ring_buffer_ptr = typename std::conditional<constTag, const RingBuffer<T, Allocator>*, RingBuffer<T, Allocator>*>::type;

                    RingBufferIterator(size_type pos = 0, ring_buffer_ptr rb = nullptr) noexcept;
                    RingBufferIterator(const RingBufferIterator& other) noexcept = default;

                    // coversion from non const to const iterator
                    template <bool C = constTag, typename = typename std::enable_if<C>::type>
                    RingBufferIterator(const RingBufferIterator<false>& other) noexcept;

                    // = default for other ctors and assignment?

                    reference operator*() const noexcept;
                    pointer operator->() const noexcept;

                    RingBufferIterator& operator++() noexcept; // prefix
                    RingBufferIterator operator++(int) noexcept; // postfix

                    RingBufferIterator& operator--() noexcept; // prefix
                    RingBufferIterator operator--(int) noexcept; // postfix

                    void swap(RingBufferIterator& other) noexcept;

                    friend bool operator==(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        return (lhs.mPosition == rhs.mPosition) && (lhs.mRingBufPtr == rhs.mRingBufPtr);
                    }

                    friend bool operator!=(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        return (lhs.mPosition != rhs.mPosition) || (lhs.mRingBufPtr != rhs.mRingBufPtr);
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
            static constexpr size_type defaultInitialCapacity = 64;

            RingBuffer(const size_type initialCapacity = defaultInitialCapacity, const Allocator& alloc = Allocator{}) noexcept;
            RingBuffer(const size_type initialCapacity, const T& val, const Allocator& alloc = Allocator{}) noexcept;
            RingBuffer(const std::vector<T, Allocator>& initVec, const Allocator& alloc = Allocator{}) noexcept;
            RingBuffer(std::initializer_list<T> initList, const Allocator& alloc = Allocator{}) noexcept;
            template <typename It>
            RingBuffer(It itStart, It itEnd, const Allocator& alloc = Allocator{}) noexcept;

            RingBuffer(const RingBuffer& other) noexcept;   // = default;
            RingBuffer(RingBuffer&& other) noexcept;   // = default;

            RingBuffer& operator=(const RingBuffer& rhs) noexcept;   // = default;
            RingBuffer& operator=(RingBuffer&& rhs) noexcept;   // = default;

            ~RingBuffer() noexcept;   // = default;

            allocator_type get_allocator() const noexcept;
            size_type capacity() const noexcept;
            void changeCapacity(const size_type newCapacity) noexcept;
            size_type size() const noexcept;
            size_type max_size() const noexcept;
            bool empty() const noexcept;
            bool full() const noexcept;
            void clear() noexcept;

            std::vector<T> getElementsInInsertionOrder() const noexcept;

            void push(const T& elem);
            void push(T&& elem);

            template <typename ...Args>
            void emplace(Args&&... args);

            void swap(RingBuffer& other) noexcept;

            reference operator[](size_type pos) noexcept;
            const_reference operator[](size_type pos) const noexcept;
            reference at(size_type pos);
            const_reference at(size_type pos) const;

            iterator begin() noexcept;
            iterator end() noexcept;
            const_iterator begin() const noexcept;
            const_iterator end() const noexcept;
            const_iterator cbegin() const noexcept;
            const_iterator cend() const noexcept;

            friend bool operator== <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;
            friend bool operator!= <>(const RingBuffer<T, Allocator>& lhs, const RingBuffer<T, Allocator>& rhs) noexcept;
        
        private:
            std::vector<T, Allocator> mBuffer;
            size_type mCurrentCapacity;
            size_type mNewestElementInsertionIndex;
    };
} // namespace simpleContainers

// =============================== IMPLEMENTATION ===============================

namespace simpleContainers {
    template <typename T, typename Allocator>
    template <bool constTag>
    inline RingBuffer<T, Allocator>::RingBufferIterator<constTag>::RingBufferIterator(size_type pos, ring_buffer_ptr rb) noexcept
        : mPosition{pos}, mRingBufPtr{rb}
    {
        if (constTag == true) {
            std::cout << "CONSTRUCTING CONST ITERATOR" << std::endl;
        }
        else {
            std::cout << "CONSTRUCTING NON CONST ITERATOR" << std::endl;
        }
    }

    template <typename T, typename Allocator>
    template <bool constTag> 
    template <bool C, typename> 
    inline RingBuffer<T, Allocator>::RingBufferIterator<constTag>::RingBufferIterator(const RingBuffer<T, Allocator>::RingBufferIterator<false> &other) noexcept
        : mPosition{other.mPosition}, mRingBufPtr{other.mRingBufPtr} 
    {
        std::cout << "converting non const iter to const iter" << std::endl;
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>::reference
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator*() const noexcept {
        return (*mRingBufPtr)[mPosition];
    }

    template <typename T, typename Allocator>
    template <bool constTag>
    inline typename RingBuffer<T, Allocator>::template RingBufferIterator<constTag>::pointer 
    RingBuffer<T, Allocator>::RingBufferIterator<constTag>::operator->() const noexcept {
        return &((*mRingBufPtr)[mPosition]);
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
    inline void RingBuffer<T, Allocator>::RingBufferIterator<constTag>::swap(RingBufferIterator& other) noexcept {
        std::swap(mPosition, other.mPosition);
        std::swap(mRingBufPtr, other.mRingBufPtr);
        return;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(const size_type initialCapacity, const Allocator& alloc) noexcept
        : mBuffer{std::vector<T, Allocator>{alloc}}, mCurrentCapacity{initialCapacity == 0 ? defaultInitialCapacity : initialCapacity}, mNewestElementInsertionIndex{0}
    {
        mBuffer.reserve(mCurrentCapacity);
        std::cout << "BUFFER CTOR WITH CAPACITY " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
        return;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(const size_type initialCapacity, const T& val, const Allocator& alloc) noexcept
        : mBuffer{std::vector<T, Allocator>(initialCapacity == 0 ? defaultInitialCapacity : initialCapacity, val, alloc)}, mCurrentCapacity{initialCapacity == 0 ? defaultInitialCapacity : initialCapacity}, mNewestElementInsertionIndex{0}
    {
        mBuffer.reserve(mCurrentCapacity);
        std::cout << "BUFFER FILL CTOR WITH CAPACITY " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
        return;
    }
    
    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(const std::vector<T, Allocator>& initVec, const Allocator& alloc) noexcept 
        : mBuffer(initVec, alloc), mCurrentCapacity{initVec.size()}, mNewestElementInsertionIndex{0}
    {
        std::cout << "BUFFER CTOR FROM CONST REF VECTOR " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(std::initializer_list<T> initList, const Allocator& alloc) noexcept
        : mBuffer(initList, alloc), mCurrentCapacity{initList.size()}, mNewestElementInsertionIndex{0}
    {
        std::cout << "BUFFER CTOR FROM INITIALIZER LIST " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
    }

    template <typename T, typename Allocator>
    template <typename It>
    inline RingBuffer<T, Allocator>::RingBuffer(It itStart, It itEnd, const Allocator& alloc) noexcept 
        : mBuffer(itStart, itEnd, alloc), mCurrentCapacity{static_cast<size_type>(std::distance(itStart, itEnd))}, mNewestElementInsertionIndex{0}
    {
        std::cout << "BUFFER CTOR FROM ITERATOR PAIR capacity " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(const RingBuffer& other) noexcept 
        : mBuffer{other.mBuffer}, mCurrentCapacity{other.mCurrentCapacity}, mNewestElementInsertionIndex{other.mNewestElementInsertionIndex}
    {
        std::cout << "BUFFER COPY CTOR" <<  std::endl;
        return;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::RingBuffer(RingBuffer&& other) noexcept
        : mBuffer{std::move(other.mBuffer)}, mCurrentCapacity{other.mCurrentCapacity}, mNewestElementInsertionIndex{other.mNewestElementInsertionIndex}
    {
        std::cout << "BUFFER MOVE CTOR" <<  std::endl;
        other.mBuffer = {};
        other.mCurrentCapacity = 0;
        other.mNewestElementInsertionIndex = 0;
        return;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>& RingBuffer<T, Allocator>::operator=(const RingBuffer& rhs) noexcept {
        std::cout << "COPY ASSIGNMENT OPERATOR" <<  std::endl;
        if (this != &rhs) {
            mBuffer = rhs.mBuffer;
            mCurrentCapacity = rhs.mCurrentCapacity;
            mNewestElementInsertionIndex = rhs.mNewestElementInsertionIndex;
        }

        return *this;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>& RingBuffer<T, Allocator>::operator=(RingBuffer&& rhs) noexcept {
        std::cout << "MOVE ASSIGNMENT OPERATOR" <<  std::endl;
        if (this != &rhs) {
            mBuffer = std::move(rhs.mBuffer);
            rhs.mBuffer = {};

            mCurrentCapacity = rhs.mCurrentCapacity;
            rhs.mCurrentCapacity = 0;
            mNewestElementInsertionIndex = rhs.mNewestElementInsertionIndex;
            rhs.mNewestElementInsertionIndex = 0;
        }

        return *this;
    }

    template <typename T, typename Allocator>
    inline RingBuffer<T, Allocator>::~RingBuffer() noexcept {
        std::cout << "BUFFER DTOR" << std::endl;
        return;
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
    inline void RingBuffer<T, Allocator>::changeCapacity(const size_type newCapacity) noexcept {
        if (newCapacity == mCurrentCapacity || newCapacity == 0) {
            return;
        }

        std::vector<T, Allocator> newBuffer(get_allocator());
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

        return;
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
    inline std::vector<T> RingBuffer<T, Allocator>::getElementsInInsertionOrder() const noexcept {
        if (mBuffer.size() < mCurrentCapacity) {
            return mBuffer;
        }
        else {
            std::vector<T> result;
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
    inline void RingBuffer<T, Allocator>::push(const T& elem) {
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

        return;
    }

    template <typename T, typename Allocator>
    inline void RingBuffer<T, Allocator>::push(T&& elem) {
        if (mBuffer.size() == mCurrentCapacity) {   // most common case
            mBuffer[mNewestElementInsertionIndex] = std::forward<T>(elem);
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.push_back(std::forward<T>(elem));
        }

        ++mNewestElementInsertionIndex;

        if (mNewestElementInsertionIndex == mCurrentCapacity) {
            mNewestElementInsertionIndex = 0;
        }

        return;
    }

    template <typename T, typename Allocator>
    template <typename ...Args>
    inline void RingBuffer<T, Allocator>::emplace(Args&&... args) {
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

        return;
    }

    template <typename T, typename Allocator>
    inline void RingBuffer<T, Allocator>::swap(RingBuffer& other) noexcept {
        std::swap(mBuffer, other.mBuffer);
        std::swap(mCurrentCapacity, other.mCurrentCapacity);
        std::swap(mNewestElementInsertionIndex, other.mNewestElementInsertionIndex);
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::reference RingBuffer<T, Allocator>::operator[](size_type pos) noexcept {
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            pos += mNewestElementInsertionIndex;
            if (pos >= mCurrentCapacity) {
                pos -= mCurrentCapacity;
            }
        }
        
        return mBuffer[pos];
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_reference RingBuffer<T, Allocator>::operator[](size_type pos) const noexcept {
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            pos += mNewestElementInsertionIndex;
            if (pos >= mCurrentCapacity) {
                pos -= mCurrentCapacity;
            }
        }
        
        return mBuffer[pos];
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::reference RingBuffer<T, Allocator>::at(size_type pos) {
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            pos += mNewestElementInsertionIndex;
            if (pos >= mCurrentCapacity) {
                pos -= mCurrentCapacity;
            }
        }
        
        return mBuffer.at(pos);
    }

    template <typename T, typename Allocator>
    inline typename RingBuffer<T, Allocator>::const_reference RingBuffer<T, Allocator>::at(size_type pos) const {
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            pos += mNewestElementInsertionIndex;
            if (pos >= mCurrentCapacity) {
                pos -= mCurrentCapacity;
            }
        }
        
        return mBuffer.at(pos);
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

} // namespace simpleContainers

#endif // __SIMPLE_RING_BUFFER__
