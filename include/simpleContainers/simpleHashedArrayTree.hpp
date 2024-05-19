/// @file simpleHashedArrayTree.hpp
/// @brief File containing API and implementaiton of HashedArrayTree class

#ifndef SIMPLE_HASHED_ARRAY_TREE_HPP
#define SIMPLE_HASHED_ARRAY_TREE_HPP

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

            HashedArrayTree(const allocator_type& alloc = allocator_type{});

            HashedArrayTree(const HashedArrayTree& other) = default;
            HashedArrayTree(HashedArrayTree&& other) noexcept = default;

            HashedArrayTree& operator=(const HashedArrayTree& rhs) = default;
            HashedArrayTree& operator=(HashedArrayTree&& rhs) noexcept = default;

            ~HashedArrayTree() noexcept = default;
            
            /// @brief Return the total capacity of the hashed array tree
            /// @details This capacity is always a pwoer of 2 such that it is greater
            ///          or equal to the number of currently stored elements
            size_type capacity() const noexcept;
        
        private:
            using LeafVector = std::vector<value_type, allocator_type>;
            using OuterAllocator = typename allocator_type::template rebind<LeafVector>::other;

            std::vector<LeafVector, OuterAllocator> mInternalData;

            // an outer vector with this capacity stores at maximum that
            // many inner vectors with the same capacity
            size_type mCurrentCapacity;
    };

    namespace internal {
        template <typename T>
        inline T next_power_of_2(T capacity) noexcept;
    } // namespace internal
} // namespace simpleContainers

// ============================================================================================================================================
// ============================================================== IMPLEMENTATION ==============================================================
// ============================================================================================================================================

namespace simpleContainers {
    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const allocator_type& alloc)
        : mInternalData{alloc}, mCurrentCapacity{0}
    {}

    template <typename T, typename Allocator>
    inline typename HashedArrayTree<T, Allocator>::size_type HashedArrayTree<T, Allocator>::capacity() const noexcept {
        return mCurrentCapacity * mCurrentCapacity;
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
    } // namespace internal
} // namespace simpleContainers

#endif // SIMPLE_HASHED_ARRAY_TREE_HPP
