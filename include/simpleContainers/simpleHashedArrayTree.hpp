/// @file simpleHashedArrayTree.hpp
/// @brief File containing API and implementaiton of HashedArrayTree class

#ifndef SIMPLE_HASHED_ARRAY_TREE_HPP
#define SIMPLE_HASHED_ARRAY_TREE_HPP

#include <vector>

// ============================================================================================================================================
// =================================================================== API ====================================================================
// ============================================================================================================================================

namespace simpleContainers {
    template <typename T, typename Allocator>
    class HashedArrayTree;

    /// @brief Class representing a hashed array tree structure
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
        
        private:
            using LeafVector = std::vector<value_type, allocator_type>;
            using OuterAllocator = typename allocator_type::template rebind<LeafVector>::other;

            std::vector<LeafVector, OuterAllocator> mInternalData;

            // an outer vector with this capacity stores at maximum that
            // many inner vectors with the same capacity
            size_type mCurrentCapacity;
    };
} // namespace simpleContainers

// ============================================================================================================================================
// ============================================================== IMPLEMENTATION ==============================================================
// ============================================================================================================================================

namespace simpleContainers {
    template <typename T, typename Allocator>
    inline HashedArrayTree<T, Allocator>::HashedArrayTree(const allocator_type& alloc)
        : mInternalData{alloc}, mCurrentCapacity{0}
    {}
} // namespace simpleContainers

#endif // SIMPLE_HASHED_ARRAY_TREE_HPP
