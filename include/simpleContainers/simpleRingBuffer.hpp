#ifndef __SIMPLE_RING_BUFFER__
#define __SIMPLE_RING_BUFFER__

#include <iostream>
#include <type_traits>
#include <vector>

// =============================== API ===============================

namespace simpleContainers {
    template <typename DataType>
    class RingBuffer {
        public:
            using value_type = DataType;
            using reference = DataType&;
            using const_reference = const DataType&;
            using pointer = DataType*;
            // using const_pointer = DataType* const;
            using size_type = typename std::vector<DataType>::size_type;
            using difference_type = typename std::vector<DataType>::difference_type;

            template <bool ConstTag = false>
            class RingBufferIterator {
                public:
                    friend class RingBufferIterator<false>;
                    friend class RingBufferIterator<true>;

                    using iterator_category = std::forward_iterator_tag;
                    using difference_type = typename RingBuffer<DataType>::difference_type;
                    using value_type = typename RingBuffer<DataType>::value_type;
                    using pointer = typename RingBuffer<DataType>::pointer;
                    using reference = typename std::conditional<ConstTag, typename RingBuffer<DataType>::const_reference, typename RingBuffer<DataType>::reference>::type;

                    RingBufferIterator(pointer ptr = nullptr, RingBuffer<DataType>* rb = nullptr);

                    RingBufferIterator(const RingBufferIterator& other) = default;

                    // coversion from non const to const iterator
                    template <bool C = ConstTag, typename = typename std::enable_if<C>::type>
                    RingBufferIterator(const RingBufferIterator<false>& other);

                    // = default for other ctors and assignment?

                    // SFINAE will enable this for const iterators
                    template <bool C = ConstTag>
                    typename std::enable_if<C, reference>::type operator*();

                    // SFINAE will enable this for non const iterator
                    template <bool C = ConstTag>
                    typename std::enable_if<!C, reference>::type operator*();

                    pointer operator->();

                    RingBufferIterator& operator++(); // prefix
                    RingBufferIterator operator++(int); // postfix

                    friend bool operator==(const RingBufferIterator& lhs, const RingBufferIterator& rhs) {
                        return (lhs.mPtr == rhs.mPtr) && (lhs.mRingBufPtr == rhs.mRingBufPtr);
                    }
                    friend bool operator!=(const RingBufferIterator& lhs, const RingBufferIterator& rhs) {
                        return (lhs.mPtr != rhs.mPtr) || (lhs.mRingBufPtr != rhs.mRingBufPtr);
                    }

                private:
                    pointer mPtr;
                    RingBuffer<DataType>* mRingBufPtr;
            };

            using iterator = RingBufferIterator<false>;
            using const_iterator = RingBufferIterator<true>;
            
        public:
            static constexpr std::size_t defaultInitialCapacity = 64;

            RingBuffer() noexcept;
            explicit RingBuffer(const std::size_t initialCapacity) noexcept;
            RingBuffer(const RingBuffer& other) noexcept;   // = default;
            RingBuffer(RingBuffer&& other) noexcept;   // = default;

            RingBuffer& operator=(const RingBuffer& rhs) noexcept;   // = default;
            RingBuffer& operator=(RingBuffer&& rhs) noexcept;   // = default;

            ~RingBuffer() noexcept;   // = default;

            std::size_t getCurrentCapacity() const noexcept;
            std::size_t getCurrentSize() const noexcept;
            bool isEmpty() const noexcept;
            bool isFull() const noexcept;

            std::vector<DataType> getElementsInInsertionOrder() const;

            void push(const DataType& elem);
            void push(DataType&& elem);

            template <typename ...Args>
            void emplace(Args&&... args);

            // resize
            // swap

            iterator begin();
            iterator end();
            const_iterator cbegin();
            const_iterator cend();

        private:
            std::vector<DataType> mBuffer;
            std::size_t mCurrentCapacity;
            std::size_t mNewestElementInsertionIndex;
    };
} // namespace simpleContainers

// =============================== IMPLEMENTATION ===============================

namespace simpleContainers {
    template <typename DataType>
    inline RingBuffer<DataType>::RingBuffer() noexcept
        : mBuffer{std::vector<DataType>{}}, mCurrentCapacity{defaultInitialCapacity}, mNewestElementInsertionIndex{0}
    {
        mBuffer.reserve(defaultInitialCapacity);
        std::cout << "BUFFER DEFAULT CTOR CAPACITY " << mCurrentCapacity << std::endl;
        return;
    }

    template <typename DataType>
    inline RingBuffer<DataType>::RingBuffer(const std::size_t initialCapacity) noexcept
        : mBuffer{std::vector<DataType>{}}, mCurrentCapacity{initialCapacity}, mNewestElementInsertionIndex{0}
    {
        mBuffer.reserve(initialCapacity);
        std::cout << "BUFFER CTOR WITH CAPACITY " << initialCapacity << " mBuffer capacity " << mBuffer.capacity() <<  std::endl;
        return;
    }

    template <typename DataType>
    inline RingBuffer<DataType>::RingBuffer(const RingBuffer& other) noexcept 
        : mBuffer{other.mBuffer}, mCurrentCapacity{other.mCurrentCapacity}, mNewestElementInsertionIndex{other.mNewestElementInsertionIndex}
    {
        std::cout << "BUFFER COPY CTOR" <<  std::endl;
        return;
    }

    template <typename DataType>
    inline RingBuffer<DataType>::RingBuffer(RingBuffer&& other) noexcept
        : mBuffer{std::move(other.mBuffer)}, mCurrentCapacity{other.mCurrentCapacity}, mNewestElementInsertionIndex{other.mNewestElementInsertionIndex}
    {
        std::cout << "BUFFER MOVE CTOR" <<  std::endl;
        other.mBuffer = {};
        other.mCurrentCapacity = 0;
        other.mNewestElementInsertionIndex = 0;
        return;
    }

    template <typename DataType>
    inline RingBuffer<DataType>& RingBuffer<DataType>::operator=(const RingBuffer& rhs) noexcept {
        std::cout << "COPY ASSIGNMENT OPERATOR" <<  std::endl;
        if (this != &rhs) {
            mBuffer = rhs.mBuffer;
            mCurrentCapacity = rhs.mCurrentCapacity;
            mNewestElementInsertionIndex = rhs.mNewestElementInsertionIndex;
        }

        return *this;
    }

    template <typename DataType>
    inline RingBuffer<DataType>& RingBuffer<DataType>::operator=(RingBuffer&& rhs) noexcept {
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

    template <typename DataType>
    inline RingBuffer<DataType>::~RingBuffer() noexcept {
        std::cout << "BUFFER DTOR" << std::endl;
        return;
    }

    template <typename DataType>
    inline std::size_t RingBuffer<DataType>::getCurrentCapacity() const noexcept {
        return mCurrentCapacity;
    }

    template <typename DataType>
    inline std::size_t RingBuffer<DataType>::getCurrentSize() const noexcept {
        return mBuffer.size();
    }

    template <typename DataType>
    inline bool RingBuffer<DataType>::isEmpty() const noexcept {
        return mBuffer.size() == 0;
    }

    template <typename DataType>
    inline bool RingBuffer<DataType>::isFull() const noexcept {
        return mBuffer.size() == mCurrentCapacity;
    }

    template <typename DataType>
    std::vector<DataType> RingBuffer<DataType>::getElementsInInsertionOrder() const {
        // TODO replace with iterator implementation
        if (mBuffer.size() < mCurrentCapacity) {
            return mBuffer;
        }
        else {
            std::vector<DataType> result;
            result.reserve(mCurrentCapacity);

            for (std::size_t i = mNewestElementInsertionIndex; i < mCurrentCapacity; ++i) {
                result.emplace_back(mBuffer[i]);
            }
            
            for (std::size_t i = 0; i < mNewestElementInsertionIndex; ++i) {
                result.emplace_back(mBuffer[i]);
            }

            return result;
        }
    }

    template <typename DataType>
    inline void RingBuffer<DataType>::push(const DataType& elem) {
        if (mBuffer.size() == mCurrentCapacity) {   // more common case
            mBuffer[mNewestElementInsertionIndex] = elem;
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.push_back(elem);
        }
        else {
            // should not get here
        }

        ++mNewestElementInsertionIndex;

        if (mNewestElementInsertionIndex == mCurrentCapacity) {
            mNewestElementInsertionIndex = 0;
        }

        return;
    }

    template <typename DataType>
    inline void RingBuffer<DataType>::push(DataType&& elem) {
        if (mBuffer.size() == mCurrentCapacity) {   // more common case
            mBuffer[mNewestElementInsertionIndex] = std::forward<DataType>(elem);
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.push_back(std::forward<DataType>(elem));
        }
        else {
            // should not get here
        }

        ++mNewestElementInsertionIndex;

        if (mNewestElementInsertionIndex == mCurrentCapacity) {
            mNewestElementInsertionIndex = 0;
        }

        return;
    }

    template <typename DataType>
    template <typename ...Args>
    inline void RingBuffer<DataType>::emplace(Args&&... args) {
        if (mBuffer.size() == mCurrentCapacity) {   // more common case
            mBuffer[mNewestElementInsertionIndex] = DataType{std::forward<Args>(args)...};
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.emplace_back(std::forward<Args>(args)...);
        }
        else {
            // should not get here
        }

        ++mNewestElementInsertionIndex;

        if (mNewestElementInsertionIndex == mCurrentCapacity) {
            mNewestElementInsertionIndex = 0;
        }

        return;
    }

    template <typename DataType>
    template <bool ConstTag>
    inline RingBuffer<DataType>::RingBufferIterator<ConstTag>::RingBufferIterator(pointer ptr, RingBuffer<DataType>* rb) 
        : mPtr{ptr}, mRingBufPtr{rb}
    {
        if (ConstTag == true) {
            std::cout << "CONSTRUCTING CONST ITERATOR" << std::endl;
        }
        else {
            std::cout << "CONSTRUCTING NON CONST ITERATOR" << std::endl;
        }
    }


    template <typename DataType>
    template <bool ConstTag> 
    template <bool C, typename> 
    inline RingBuffer<DataType>::RingBufferIterator<ConstTag>::RingBufferIterator(const RingBuffer<DataType>::RingBufferIterator<false> &other) 
        : mPtr{other.mPtr}, mRingBufPtr{other.mRingBufPtr} 
    {
        std::cout << "converting non const iter to const iter" << std::endl;
    }

    template <typename DataType>
    template <bool ConstTag>
    template <bool C>
    inline typename std::enable_if<!C, typename RingBuffer<DataType>::template RingBufferIterator<ConstTag>::reference>::type
    RingBuffer<DataType>::RingBufferIterator<ConstTag>::operator*() {
        return *mPtr;
    }

    template <typename DataType>
    template <bool ConstTag>
    template <bool C>
    inline typename std::enable_if<C, typename RingBuffer<DataType>::template RingBufferIterator<ConstTag>::reference>::type
    RingBuffer<DataType>::RingBufferIterator<ConstTag>::operator*() {
        return *mPtr;
    }

    template <typename DataType>
    template <bool ConstTag>
    inline typename RingBuffer<DataType>::template RingBufferIterator<ConstTag>::pointer 
    RingBuffer<DataType>::RingBufferIterator<ConstTag>::operator->() {
        return mPtr;
    }

    template <typename DataType>
    template <bool ConstTag>
    inline typename RingBuffer<DataType>::template RingBufferIterator<ConstTag>& 
    RingBuffer<DataType>::RingBufferIterator<ConstTag>::operator++() {
        mPtr++;

        auto& mBuf = mRingBufPtr->mBuffer;
        auto mBufSize = mBuf.size();

        if (mBufSize == 0) {
            mPtr = nullptr;
        }
        else if (mBufSize < mRingBufPtr->mCurrentCapacity) {
            if (mPtr == &(mBuf[0]) + mBufSize) {
                mPtr = nullptr;
            }
        }
        else {  // size == capacity
            if (mPtr == &(mBuf[0]) + mBufSize) {
                mPtr = &(mBuf[0]);
            }

            if (mPtr == &(mBuf[mRingBufPtr->mNewestElementInsertionIndex])) {
                mPtr = nullptr;
            }
        }

        return *this;
    }

    template <typename DataType>
    template <bool ConstTag>
    inline typename RingBuffer<DataType>::template RingBufferIterator<ConstTag> 
    RingBuffer<DataType>::RingBufferIterator<ConstTag>::operator++(int) {
        RingBufferIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template <typename DataType>
    inline typename RingBuffer<DataType>::iterator RingBuffer<DataType>::begin() {
        if (mBuffer.empty()) {
            return end();
        }
        else if (mBuffer.size() < mCurrentCapacity) {
            return iterator{&mBuffer[0], this};
        }
        else { // size == capacity
            return iterator{&mBuffer[mNewestElementInsertionIndex], this};
        }
    }

    template <typename DataType>
    inline typename RingBuffer<DataType>::iterator RingBuffer<DataType>::end() {
        return iterator{nullptr, this};
    }

    template <typename DataType>
    inline typename RingBuffer<DataType>::const_iterator RingBuffer<DataType>::cbegin() {
        if (mBuffer.empty()) {
            return cend();
        }
        else if (mBuffer.size() < mCurrentCapacity) {
            return const_iterator{&mBuffer[0], this};
        }
        else { // size == capacity
            return const_iterator{&mBuffer[mNewestElementInsertionIndex], this};
        }
    }

    template <typename DataType>
    inline typename RingBuffer<DataType>::const_iterator RingBuffer<DataType>::cend() {
        return const_iterator{nullptr, this};
    }
} // namespace simpleContainers

#endif // __SIMPLE_RING_BUFFER__
