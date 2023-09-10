#ifndef __SIMPLE_RING_BUFFER__
#define __SIMPLE_RING_BUFFER__

#include <vector>
#include <iostream>

// =============================== API ===============================

namespace simpleContainers {
    template <typename DataType>
    class RingBuffer {
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

            // emplace
            // pop
            // resize

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
            if (mNewestElementInsertionIndex == mCurrentCapacity) {
                mNewestElementInsertionIndex = 0;
            }

            mBuffer[mNewestElementInsertionIndex] = elem;
            ++mNewestElementInsertionIndex;
        }
        else if (mBuffer.size() < mCurrentCapacity) { // only happens during the initial filling
            mBuffer.push_back(elem);
            ++mNewestElementInsertionIndex;
        }
        else {
            // should never get here
        }

        return;
    }
} // namespace simpleContainers

#endif // __SIMPLE_RING_BUFFER__
