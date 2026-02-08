#include "bitset_segmented/bitset_segmented.hpp"

#include <map>
#include <memory>
#include <limits>
#include <cstdint>

namespace bsa {

constexpr std::size_t BITSET_SEGMENT_SIZE = 16384; // (2^10)
constexpr std::size_t WORD_BITS = 64;
constexpr std::size_t WORD_COUNT = BITSET_SEGMENT_SIZE / WORD_BITS;

struct alignas(64) bitset_array {
    uint64_t words[WORD_COUNT]{};
    std::size_t count = 0;

    std::size_t local_min = std::numeric_limits<std::size_t>::max();
    std::size_t local_max = 0;

    bool test(std::size_t bit) const {
        auto word_idx = bit / WORD_BITS;
        auto mask = 1ULL << (bit % WORD_BITS);
        return words[word_idx] & mask;
    }

    void set(std::size_t bit) {
        auto word_idx = bit / WORD_BITS;
        auto mask = 1ULL << (bit % WORD_BITS);
        uint64_t &w = words[word_idx];
        if (w & mask) return;

        w |= mask;
        ++count;

        local_min = std::min(local_min, bit);
        local_max = std::max(local_max, bit);
    }

    void unset(std::size_t bit) {
        auto word_idx = bit / WORD_BITS;
        auto mask = 1ULL << (bit % WORD_BITS);
        uint64_t &w = words[word_idx];
        if (!(w & mask)) return;

        w &= ~mask;
        --count;

        if (count == 0) {
            local_min = std::numeric_limits<std::size_t>::max();
            local_max = 0;
            return;
        }

        if (bit == local_min) update_local_min();
        if (bit == local_max) update_local_max();
    }

    void update_local_min() {
        for (std::size_t wi = 0; wi < WORD_COUNT; ++wi) {
            if (words[wi]) {
                local_min = wi * WORD_BITS + __builtin_ctzll(words[wi]);
                return;
            }
        }
    }

    void update_local_max() {
        for (std::size_t wi = WORD_COUNT; wi-- > 0;) {
            if (words[wi]) {
                local_max = wi * WORD_BITS + (WORD_BITS - 1 - __builtin_clzll(words[wi]));
                return;
            }
        }
    }
};

struct bitset_segmented::impl {
    std::map<std::size_t, std::unique_ptr<bitset_array>> segments;
    std::size_t head_index = std::numeric_limits<std::size_t>::max();
    std::size_t tail_index = 0;

    void update_head_from() {
        std::size_t new_head = std::numeric_limits<std::size_t>::max();
        for (auto& [seg_idx, arr] : segments) {
            if (arr && arr->count) {
                std::size_t candidate = seg_idx * BITSET_SEGMENT_SIZE + arr->local_min;
                if (candidate < new_head) new_head = candidate;
            }
        }
        head_index = new_head;
    }

    void update_tail_from() {
        std::size_t new_tail = 0;
        for (auto& [seg_idx, arr] : segments) {
            if (arr && arr->count) {
                std::size_t candidate = seg_idx * BITSET_SEGMENT_SIZE + arr->local_max + 1;
                if (candidate > new_tail) new_tail = candidate;
            }
        }
        tail_index = new_tail;
    }
};

bitset_segmented::bitset_segmented() : pimpl(new impl{}) {}

bitset_segmented::~bitset_segmented() {
    delete pimpl;
}

void bitset_segmented::set(std::size_t index) {
    auto seg = index / BITSET_SEGMENT_SIZE;
    auto bit = index % BITSET_SEGMENT_SIZE;

    auto &s = pimpl->segments[seg];
    if (!s) s = std::make_unique<bitset_array>();

    auto prev_count = s->count;
    s->set(bit);

    if (prev_count == 0) {
        if (pimpl->head_index > index) pimpl->update_head_from();
        if (pimpl->tail_index < index + 1) pimpl->update_tail_from();
    } else {
        pimpl->head_index = std::min(pimpl->head_index, index);
        pimpl->tail_index = std::max(pimpl->tail_index, index + 1);
    }
}

bool bitset_segmented::unset(std::size_t index) {
    auto seg = index / BITSET_SEGMENT_SIZE;
    auto bit = index % BITSET_SEGMENT_SIZE;

    auto it = pimpl->segments.find(seg);
    if (it == pimpl->segments.end()) return false;

    auto &s = it->second;
    if (!s->test(bit)) return false;

    s->unset(bit);

    if (index == pimpl->head_index) pimpl->update_head_from();
    if (index + 1 == pimpl->tail_index) pimpl->update_tail_from();

    return true;
}

bool bitset_segmented::get(std::size_t index) const {
    auto seg = index / BITSET_SEGMENT_SIZE;
    auto bit = index % BITSET_SEGMENT_SIZE;

    auto it = pimpl->segments.find(seg);
    if (it == pimpl->segments.end()) return false;

    return it->second->test(bit);
}

bool bitset_segmented::empty() const {
    return pimpl->head_index == std::numeric_limits<std::size_t>::max();
}

void bitset_segmented::reserve_for_max_index(std::size_t max_index) {
    std::size_t needed = (max_index + BITSET_SEGMENT_SIZE - 1) / BITSET_SEGMENT_SIZE;

    for (std::size_t i = 0; i < needed; ++i) {
        auto &s = pimpl->segments[i];
        if (!s) s = std::make_unique<bitset_array>();
    }
}

std::size_t bitset_segmented::head() const {
    return empty() ? 0 : pimpl->head_index;
}

std::size_t bitset_segmented::tail() const {
    return empty() ? 0 : pimpl->tail_index;
}

} // namespace bsa

// C# API
#ifdef _WIN32
    #define BS_API extern "C" __declspec(dllexport)
#else
    #define BS_API extern "C"
#endif

BS_API void* bitset_create() {
    return new bsa::bitset_segmented();
}

BS_API void bitset_destroy(void* bs) {
    delete static_cast<bsa::bitset_segmented*>(bs);
}

BS_API void bitset_set(void* bs, std::size_t index) {
    static_cast<bsa::bitset_segmented*>(bs)->set(index);
}

BS_API bool bitset_unset(void* bs, std::size_t index) {
    return static_cast<bsa::bitset_segmented*>(bs)->unset(index);
}

BS_API bool bitset_get(void* bs, std::size_t index) {
    return static_cast<bsa::bitset_segmented*>(bs)->get(index);
}

BS_API void bitset_reserve(void* bs, std::size_t max_index) {
    static_cast<bsa::bitset_segmented*>(bs)->reserve_for_max_index(max_index);
}

BS_API bool bitset_empty(void* bs) {
    return static_cast<bsa::bitset_segmented*>(bs)->empty();
}

BS_API std::size_t bitset_head(void* bs) {
    return static_cast<bsa::bitset_segmented*>(bs)->head();
}

BS_API std::size_t bitset_tail(void* bs) {
    return static_cast<bsa::bitset_segmented*>(bs)->tail();
}