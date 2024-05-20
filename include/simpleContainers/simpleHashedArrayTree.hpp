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
    template <typename T, typename Allocator>
    class HashedArrayTree;

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

            void debugPrint() const noexcept;

            HashedArrayTree(const allocator_type& alloc = allocator_type{});

            HashedArrayTree(const HashedArrayTree& other) = default;
            HashedArrayTree(HashedArrayTree&& other) noexcept = default;

            HashedArrayTree& operator=(const HashedArrayTree& rhs) = default;
            HashedArrayTree& operator=(HashedArrayTree&& rhs) noexcept = default;

            ~HashedArrayTree() noexcept = default;

            /// @brief Get number of elements in O(sqrt(n)) time
            size_type size() const noexcept;
            size_type max_size() const noexcept;
            /// @brief Return the total capacity of the hashed array tree
            /// @details This capacity is always a power of 2
            size_type capacity() const noexcept;

            bool full() const noexcept;
            bool empty() const noexcept;

            /// @brief Reserve enough memory to store at least capacity elements
            void reserve(const size_type newCapacity) noexcept;
            
            void clear() noexcept;

            void push_back(const value_type& elem);
            void push_back(value_type&& elem);
            template <typename ...Args>
            void emplace_back(Args&&... args);

        private:
            using LeafVector = std::vector<value_type, allocator_type>;
            using OuterAllocator = typename allocator_type::template rebind<LeafVector>::other;

            std::vector<LeafVector, OuterAllocator> mInternalData;

            // an outer vector with this capacity stores at maximum that
            // many inner vectors with the same capacity
            size_type mInternalVectorCapacity;

            // total number of elements currently stored in the HAT
            size_type mSize;

            // current power of 2 such that 2 ^ mCurrentPow = mInternalVectorCapacity
            // stored here to avoid recalculating on every element access
            size_type mCurrentPow;
    };

    namespace internal {
        template <typename T>
        inline T next_power_of_2(T capacity) noexcept;

        template <typename T>
        inline T what_power_of_2(T capacity) noexcept;
    } // namespace internal
} // namespace simpleContainers

// ============================================================================================================================================
// ============================================================== IMPLEMENTATION ==============================================================
// ============================================================================================================================================

namespace simpleContainers {
    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::debugPrint() const noexcept {
        std::cout << std::endl;
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
    }

    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const allocator_type& alloc)
        : mInternalData{alloc}, mInternalVectorCapacity{0}, mSize{0}, mCurrentPow{0}
    {
        // mCurrentPow = 0 in the beginning even though 2 ^ 0 != mInternalVectorCapacity !!
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::size() const noexcept {
        return mSize;
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::max_size() const noexcept {
        // Even though the internal imeplementation of HAT here is a vector of vectors, giving a max_size of 
        // mInternalData.max_size() * mInternalData.max_size() is not feasible since it is pretty much impossible
        // for any structure to be able to allocate that much memory (also that calculation will most certainly cause a 
        // size_t overflow). Therefore we can safely limit ourselves to the max_size of a single vector (and therefore 
        // limit the mInternalVectorCapacity to sqrt(mInternalData.max_size()))
        return mInternalData.max_size();
    }

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::capacity() const noexcept {
        return mInternalVectorCapacity * mInternalVectorCapacity;
    }

    template <typename T, typename Allocator>
    inline bool HashedArrayTree<T, Allocator>::full() const noexcept {
        return mSize == capacity();
    }

    template <typename T, typename Allocator>
    inline bool HashedArrayTree<T, Allocator>::empty() const noexcept {
        return mSize == 0;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::reserve(const size_type newCapacity) noexcept {
        if (newCapacity <= capacity()) {
            return;
        }

        const size_type ceilOfRoot = static_cast<size_type>(std::ceil(std::sqrt(newCapacity)));
        const size_type newInternalVectorCapacity = internal::next_power_of_2<size_type>(ceilOfRoot);

        mInternalData.reserve(newInternalVectorCapacity);

        for (size_type i = mInternalVectorCapacity; i < newInternalVectorCapacity; ++i) {
            mInternalData.emplace_back(std::vector<value_type, allocator_type>{});
        }

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

        mInternalVectorCapacity = newInternalVectorCapacity;
        mCurrentPow = internal::what_power_of_2<size_type>(mInternalVectorCapacity);
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::clear() noexcept {
        for (auto& leafVector : mInternalData) {
            leafVector.clear();
        }
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::push_back(const value_type& elem) {
        if (full()) {
            reserve(capacity() + 1);
        }

        for (auto& leafVector : mInternalData) {
            // if we get to an empty leaf vector it is guaranteed to be the first one that is empty
            if (leafVector.capacity() < mInternalVectorCapacity) {
                leafVector.reserve(mInternalVectorCapacity);
                leafVector.push_back(elem);
                break;
            }

            if (leafVector.size() == leafVector.capacity()) {
                continue;
            }

            leafVector.push_back(elem);
            break;
        }
        
        ++mSize;
    }

    template <typename T, typename Allocator>
    inline void HashedArrayTree<T, Allocator>::push_back(value_type&& elem) {
        if (full()) {
            reserve(capacity() + 1);
        }

        for (auto& leafVector : mInternalData) {
            // if we get to an empty leaf vector it is guaranteed to be the first one that is empty
            if (leafVector.capacity() < mInternalVectorCapacity) {
                leafVector.reserve(mInternalVectorCapacity);
                leafVector.push_back(std::forward<value_type>(elem));
                break;
            }

            if (leafVector.size() == leafVector.capacity()) {
                continue;
            }

            leafVector.push_back(std::forward<value_type>(elem));
            break;
        }
        
        ++mSize;
    }

    template <typename T, typename Allocator>
    template <typename ...Args>
    inline void HashedArrayTree<T, Allocator>::emplace_back(Args&&... args) {
        if (full()) {
            reserve(capacity() + 1);
        }

        for (auto& leafVector : mInternalData) {
            // if we get to an empty leaf vector it is guaranteed to be the first one that is empty
            if (leafVector.capacity() < mInternalVectorCapacity) {
                leafVector.reserve(mInternalVectorCapacity);
                leafVector.emplace_back(std::forward<Args>(args)...);
                break;
            }

            if (leafVector.size() == leafVector.capacity()) {
                continue;
            }

            leafVector.emplace_back(std::forward<Args>(args)...);
            break;
        }
        
        ++mSize;
    }

    namespace internal {
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
    } // namespace internal
} // namespace simpleContainers

#endif // SIMPLE_HASHED_ARRAY_TREE_HPP
