#pragma once

#include <cstddef>

#ifdef _WIN32
    #ifdef BITSET_SEGMENTED_EXPORTS
        #define BITSET_API __declspec(dllexport)
    #else
        #define BITSET_API __declspec(dllimport)
    #endif
#else
    #define BITSET_API
#endif

namespace bsa {

class bitset_segmented {
public:
    bitset_segmented();
    ~bitset_segmented();

    void set(std::size_t index);
    bool unset(std::size_t index);
    bool get(std::size_t index) const;
    void reserve_for_max_index(std::size_t max_index);

    bool empty() const;
    std::size_t head() const;
    std::size_t tail() const;

private:
    struct impl;
    impl* pimpl;
};

} // namespace bsa

// C# API
extern "C" {
    BITSET_API void* bitset_create();
    BITSET_API void bitset_destroy(void* bs);

    BITSET_API void bitset_set(void* bs, std::size_t index);
    BITSET_API bool bitset_unset(void* bs, std::size_t index);
    BITSET_API bool bitset_get(void* bs, std::size_t index);
    BITSET_API void bitset_reserve(void* bs, std::size_t max_index);

    BITSET_API bool bitset_empty(void* bs);
    BITSET_API std::size_t bitset_head(void* bs);
    BITSET_API std::size_t bitset_tail(void* bs);
}
