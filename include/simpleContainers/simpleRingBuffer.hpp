#ifndef __SIMPLE_RING_BUFFER__
#define __SIMPLE_RING_BUFFER__

#include <initializer_list>
#include <iostream>
#include <vector>

// =============================== API ===============================

namespace simpleContainers {
    template <typename T>
    class RingBuffer;

    template <typename T>
    inline bool operator==(const RingBuffer<T>& lhs, const RingBuffer<T>& rhs) noexcept;

    template <typename T>
    inline bool operator!=(const RingBuffer<T>& lhs, const RingBuffer<T>& rhs) noexcept;

    template <typename T>
    class RingBuffer {
        public:
            using value_type = T;
            using reference = T&;
            using const_reference = const T&;
            using pointer = T*;
            using const_pointer = const T*;
            using size_type = typename std::vector<T>::size_type;
            using difference_type = typename std::vector<T>::difference_type;

            template <bool constTag = false>
            class RingBufferIterator {
                public:
                    friend class RingBufferIterator<false>;
                    friend class RingBufferIterator<true>;

                    using iterator_category = std::bidirectional_iterator_tag;
                    using size_type = typename RingBuffer<T>::size_type;
                    using difference_type = typename RingBuffer<T>::difference_type;
                    using value_type = typename RingBuffer<T>::value_type;
                    using pointer = typename std::conditional<constTag, typename RingBuffer<T>::const_pointer, typename RingBuffer<T>::pointer>::type;
                    using reference = typename std::conditional<constTag, typename RingBuffer<T>::const_reference, typename RingBuffer<T>::reference>::type;
                    using ring_buffer_ptr = typename std::conditional<constTag, const RingBuffer<T>*, RingBuffer<T>*>::type;

                    RingBufferIterator(pointer ptr = nullptr, ring_buffer_ptr rb = nullptr) noexcept;
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
                        return (lhs.mPtr == rhs.mPtr) && (lhs.mRingBufPtr == rhs.mRingBufPtr);
                    }

                    friend bool operator!=(const RingBufferIterator& lhs, const RingBufferIterator& rhs) noexcept {
                        return (lhs.mPtr != rhs.mPtr) || (lhs.mRingBufPtr != rhs.mRingBufPtr);
                    }

                private:
                    pointer mPtr;
                    ring_buffer_ptr mRingBufPtr;
            };

            using iterator = RingBufferIterator<false>;
            using const_iterator = RingBufferIterator<true>;

        public:
            static constexpr size_type defaultInitialCapacity = 64;

            RingBuffer(const size_type initialCapacity = defaultInitialCapacity) noexcept;
            RingBuffer(const size_type initialCapacity, const T& val) noexcept;
            RingBuffer(const std::vector<T>& initVec) noexcept;
            RingBuffer(std::initializer_list<T> initList) noexcept;
            template <typename It>
            RingBuffer(It itStart, It itEnd) noexcept;

            RingBuffer(const RingBuffer& other) noexcept;   // = default;
            RingBuffer(RingBuffer&& other) noexcept;   // = default;

            RingBuffer& operator=(const RingBuffer& rhs) noexcept;   // = default;
            RingBuffer& operator=(RingBuffer&& rhs) noexcept;   // = default;

            ~RingBuffer() noexcept;   // = default;

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

            void swap(RingBuffer<T>& other) noexcept;

            iterator begin() noexcept;
            iterator end() noexcept;
            const_iterator begin() const noexcept;
            const_iterator end() const noexcept;
            const_iterator cbegin() const noexcept;
            const_iterator cend() const noexcept;

            friend bool operator== <>(const RingBuffer<T>& lhs, const RingBuffer<T>& rhs) noexcept;
            friend bool operator!= <>(const RingBuffer<T>& lhs, const RingBuffer<T>& rhs) noexcept;
        
        private:
            std::vector<T> mBuffer;
            size_type mCurrentCapacity;
            size_type mNewestElementInsertionIndex;
    };
} // namespace simpleContainers

// =============================== IMPLEMENTATION ===============================

namespace simpleContainers {
    template <typename T>
    template <bool constTag>
    inline RingBuffer<T>::RingBufferIterator<constTag>::RingBufferIterator(pointer ptr, ring_buffer_ptr rb) noexcept
        : mPtr{ptr}, mRingBufPtr{rb}
    {
        if (constTag == true) {
            std::cout << "CONSTRUCTING CONST ITERATOR" << std::endl;
        }
        else {
            std::cout << "CONSTRUCTING NON CONST ITERATOR" << std::endl;
        }
    }

    template <typename T>
    template <bool constTag> 
    template <bool C, typename> 
    inline RingBuffer<T>::RingBufferIterator<constTag>::RingBufferIterator(const RingBuffer<T>::RingBufferIterator<false> &other) noexcept
        : mPtr{other.mPtr}, mRingBufPtr{other.mRingBufPtr} 
    {
        std::cout << "converting non const iter to const iter" << std::endl;
    }

    template <typename T>
    template <bool constTag>
    inline typename RingBuffer<T>::template RingBufferIterator<constTag>::reference
    RingBuffer<T>::RingBufferIterator<constTag>::operator*() const noexcept {
        return *mPtr;
    }

    template <typename T>
    template <bool constTag>
    inline typename RingBuffer<T>::template RingBufferIterator<constTag>::pointer 
    RingBuffer<T>::RingBufferIterator<constTag>::operator->() const noexcept {
        return mPtr;
    }

    template <typename T>
    template <bool constTag>
    inline typename RingBuffer<T>::template RingBufferIterator<constTag>& 
    RingBuffer<T>::RingBufferIterator<constTag>::operator++() noexcept {
        auto mBufPtr = mRingBufPtr->mBuffer.data();
        const size_type mBufSize = mRingBufPtr->mBuffer.size();
        const size_type mBufCapacity = mRingBufPtr->mCurrentCapacity;

        ++mPtr;

        if (mBufSize == mBufCapacity) {  // most common case
            if (mPtr == mBufPtr + mBufSize) {
                mPtr = mBufPtr;
            }

            if (mPtr == mBufPtr + mRingBufPtr->mNewestElementInsertionIndex) {
                mPtr = nullptr;
            }
        }
        else if (mBufSize == 0) {
            mPtr = nullptr;
        }
        else if (mBufSize < mBufCapacity && mPtr == mBufPtr + mBufSize) {
            mPtr = nullptr;
        }

        return *this;
    }

    template <typename T>
    template <bool constTag>
    inline typename RingBuffer<T>::template RingBufferIterator<constTag> 
    RingBuffer<T>::RingBufferIterator<constTag>::operator++(int) noexcept {
        RingBufferIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template <typename T>
    template <bool constTag>
    inline typename RingBuffer<T>::template RingBufferIterator<constTag>& 
    RingBuffer<T>::RingBufferIterator<constTag>::operator--() noexcept {
        auto mBufPtr = mRingBufPtr->mBuffer.data();
        const size_type mBufSize = mRingBufPtr->mBuffer.size();

        if (mBufSize == 0) {
            mPtr = nullptr;
        }
        else if (mPtr == nullptr) {
            size_type offset = mRingBufPtr->mNewestElementInsertionIndex;

            if (mBufSize == mRingBufPtr->mCurrentCapacity && offset == 0) {
                offset = mBufSize - 1;
            }
            else {
                --offset;
            }

            mPtr = mBufPtr + offset;
        }
        else {
            if (mBufSize == mRingBufPtr->mCurrentCapacity && mPtr == mBufPtr) {
                mPtr = mBufPtr + mBufSize;
            }

            --mPtr;
        }

        return *this;
    }

    template <typename T>
    template <bool constTag>
    inline typename RingBuffer<T>::template RingBufferIterator<constTag> 
    RingBuffer<T>::RingBufferIterator<constTag>::operator--(int) noexcept {
        RingBufferIterator tmp = *this;
        --(*this);
        return tmp;
    }

    template <typename T>
    template <bool constTag>
    inline void RingBuffer<T>::RingBufferIterator<constTag>::swap(RingBufferIterator& other) noexcept {
        std::swap(mPtr, other.mPtr);
        std::swap(mRingBufPtr, other.mRingBufPtr);
        return;
    }

    template <typename T>
    inline RingBuffer<T>::RingBuffer(const size_type initialCapacity) noexcept
        : mBuffer{std::vector<T>{}}, mCurrentCapacity{initialCapacity == 0 ? defaultInitialCapacity : initialCapacity}, mNewestElementInsertionIndex{0}
    {
        mBuffer.reserve(mCurrentCapacity);
        std::cout << "BUFFER CTOR WITH CAPACITY " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
        return;
    }

    template <typename T>
    inline RingBuffer<T>::RingBuffer(const size_type initialCapacity, const T& val) noexcept
        : mBuffer{std::vector<T>(initialCapacity == 0 ? defaultInitialCapacity : initialCapacity, val)}, mCurrentCapacity{initialCapacity == 0 ? defaultInitialCapacity : initialCapacity}, mNewestElementInsertionIndex{0}
    {
        mBuffer.reserve(mCurrentCapacity);
        std::cout << "BUFFER FILL CTOR WITH CAPACITY " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
        return;
    }
    
    template <typename T>
    inline RingBuffer<T>::RingBuffer(const std::vector<T>& initVec) noexcept 
        : mBuffer{initVec}, mCurrentCapacity{initVec.size()}, mNewestElementInsertionIndex{0}
    {
        std::cout << "BUFFER CTOR FROM CONST REF VECTOR " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
    }

    template <typename T>
    inline RingBuffer<T>::RingBuffer(std::initializer_list<T> initList) noexcept
        : mBuffer{initList}, mCurrentCapacity{initList.size()}, mNewestElementInsertionIndex{0}
    {
        std::cout << "BUFFER CTOR FROM INITIALIZER LIST " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
    }

    template <typename T>
    template <typename It>
    inline RingBuffer<T>::RingBuffer(It itStart, It itEnd) noexcept 
        : mBuffer(itStart, itEnd), mCurrentCapacity{static_cast<size_type>(std::distance(itStart, itEnd))}, mNewestElementInsertionIndex{0}
    {
        std::cout << "BUFFER CTOR FROM ITERATOR PAIR capacity " << mCurrentCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
    }

    template <typename T>
    inline RingBuffer<T>::RingBuffer(const RingBuffer& other) noexcept 
        : mBuffer{other.mBuffer}, mCurrentCapacity{other.mCurrentCapacity}, mNewestElementInsertionIndex{other.mNewestElementInsertionIndex}
    {
        std::cout << "BUFFER COPY CTOR" <<  std::endl;
        return;
    }

    template <typename T>
    inline RingBuffer<T>::RingBuffer(RingBuffer&& other) noexcept
        : mBuffer{std::move(other.mBuffer)}, mCurrentCapacity{other.mCurrentCapacity}, mNewestElementInsertionIndex{other.mNewestElementInsertionIndex}
    {
        std::cout << "BUFFER MOVE CTOR" <<  std::endl;
        other.mBuffer = {};
        other.mCurrentCapacity = 0;
        other.mNewestElementInsertionIndex = 0;
        return;
    }

    template <typename T>
    inline RingBuffer<T>& RingBuffer<T>::operator=(const RingBuffer& rhs) noexcept {
        std::cout << "COPY ASSIGNMENT OPERATOR" <<  std::endl;
        if (this != &rhs) {
            mBuffer = rhs.mBuffer;
            mCurrentCapacity = rhs.mCurrentCapacity;
            mNewestElementInsertionIndex = rhs.mNewestElementInsertionIndex;
        }

        return *this;
    }

    template <typename T>
    inline RingBuffer<T>& RingBuffer<T>::operator=(RingBuffer&& rhs) noexcept {
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

    template <typename T>
    inline RingBuffer<T>::~RingBuffer() noexcept {
        std::cout << "BUFFER DTOR" << std::endl;
        return;
    }

    template <typename T>
    inline typename RingBuffer<T>::size_type RingBuffer<T>::capacity() const noexcept {
        return mCurrentCapacity;
    }

    template <typename T>
    inline void RingBuffer<T>::changeCapacity(const size_type newCapacity) noexcept {
        if (newCapacity == mCurrentCapacity || newCapacity == 0) {
            return;
        }

        std::vector<T> newBuffer;
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

    template <typename T>
    inline typename RingBuffer<T>::size_type RingBuffer<T>::size() const noexcept {
        return mBuffer.size();
    }

    template <typename T>
    inline typename RingBuffer<T>::size_type RingBuffer<T>::max_size() const noexcept {
        return mBuffer.max_size();
    }

    template <typename T>
    inline bool RingBuffer<T>::empty() const noexcept {
        return mBuffer.empty();
    }

    template <typename T>
    inline bool RingBuffer<T>::full() const noexcept {
        return mBuffer.size() == mCurrentCapacity;
    }

    template <typename T>
    inline void RingBuffer<T>::clear() noexcept {
        mNewestElementInsertionIndex = 0;
        mBuffer.clear();
    }

    template <typename T>
    inline std::vector<T> RingBuffer<T>::getElementsInInsertionOrder() const noexcept {
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

    template <typename T>
    inline void RingBuffer<T>::push(const T& elem) {
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

    template <typename T>
    inline void RingBuffer<T>::push(T&& elem) {
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

    template <typename T>
    template <typename ...Args>
    inline void RingBuffer<T>::emplace(Args&&... args) {
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

    template <typename T>
    inline void RingBuffer<T>::swap(RingBuffer<T>& other) noexcept {
        std::swap(mBuffer, other.mBuffer);
        std::swap(mCurrentCapacity, other.mCurrentCapacity);
        std::swap(mNewestElementInsertionIndex, other.mNewestElementInsertionIndex);
    }

    template <typename T>
    inline typename RingBuffer<T>::iterator RingBuffer<T>::begin() noexcept {
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            return iterator{&mBuffer[mNewestElementInsertionIndex], this};
        }
        
        if (mBuffer.empty()) {
            return end();
        }
        
        // mBuffer is not empty but has less elements than mCurrentCapacity
        return iterator{&mBuffer[0], this};
    }

    template <typename T>
    inline typename RingBuffer<T>::iterator RingBuffer<T>::end() noexcept {
        return iterator{nullptr, this};
    }

    template <typename T>
    inline typename RingBuffer<T>::const_iterator RingBuffer<T>::begin() const noexcept {
        if (mBuffer.size() == mCurrentCapacity) { // most common case
            return const_iterator{&mBuffer[mNewestElementInsertionIndex], this};
        }
        
        if (mBuffer.empty()) {
            return end();
        }

        // mBuffer is not empty but has less elements than mCurrentCapacity
        return const_iterator{&mBuffer[0], this};
    } 

    template <typename T>
    inline typename RingBuffer<T>::const_iterator RingBuffer<T>::end() const noexcept {
        return const_iterator{nullptr, this};
    }

    template <typename T>
    inline typename RingBuffer<T>::const_iterator RingBuffer<T>::cbegin() const noexcept {
        return begin();
    }

    template <typename T>
    inline typename RingBuffer<T>::const_iterator RingBuffer<T>::cend() const noexcept {
        return end();
    }

    template <typename T>
    inline bool operator==(const RingBuffer<T>& lhs, const RingBuffer<T>& rhs) noexcept {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T>
    inline bool operator!=(const RingBuffer<T>& lhs, const RingBuffer<T>& rhs) noexcept {
        return lhs.size() != rhs.size() || !std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

} // namespace simpleContainers

#endif // __SIMPLE_RING_BUFFER__
