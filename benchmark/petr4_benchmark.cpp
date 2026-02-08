#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include "bitset_segmented/bitset_segmented.hpp"

int main() {
    constexpr std::size_t N_INSERT = 100'000'000; // total de inserções
    constexpr int PRICE_CURRENT = 3665;           // preço atual em centavos (36.65 BRL)
    constexpr int PRICE_MIN = 1832;              // -50%
    constexpr int PRICE_MAX = 5500;              // +50%
    constexpr int HEAD_RANGE = 500;              // range +/- em torno do head

    bsa::bitset_segmented bs;

    std::mt19937_64 rng(std::random_device{}());

    std::vector<int> inserted_prices;
    inserted_prices.reserve(N_INSERT);

    auto start_total = std::chrono::high_resolution_clock::now();

    // ---------- INSERTION NEAR HEAD ----------
    auto start_insert = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < N_INSERT; ++i) {
        int price;
        if (bs.empty()) {
            std::uniform_int_distribution<int> dist_global(PRICE_MIN, PRICE_MAX);
            price = dist_global(rng);
        } else {
            int h = bs.head();
            std::uniform_int_distribution<int> dist_head(
                std::max(PRICE_MIN, h - HEAD_RANGE),
                std::min(PRICE_MAX, h + HEAD_RANGE)
            );
            price = dist_head(rng);
        }

        bs.set(price);
        inserted_prices.push_back(price);

        if (i % 100'000'000 == 0) {
            std::cout << "[Insert] Iteration " << i
                      << " | Head: " << bs.head()
                      << " | Tail: " << bs.tail() << "\n";
        }
    }
    auto end_insert = std::chrono::high_resolution_clock::now();

    std::cout << "\nInsertion of " << N_INSERT << " orders took: "
              << std::chrono::duration<double, std::milli>(end_insert - start_insert).count()
              << " ms\n\n";

    // ---------- REMOVALS ----------
    auto start_remove = std::chrono::high_resolution_clock::now();

    if (!bs.empty()) {
        // remover ordens do head, tail e aleatórias
        bs.unset(bs.head());
        bs.unset(bs.tail() - 1);

        std::uniform_int_distribution<int> dist_random(PRICE_MIN, PRICE_MAX);
        for (int i = 0; i < 10; ++i) { // remove 10 ordens aleatórias
            bs.unset(dist_random(rng));
        }
    }

    auto end_remove = std::chrono::high_resolution_clock::now();

    std::cout << "After removals:\n";
    std::cout << "Head now: " << bs.head() << "\n";
    std::cout << "Tail now: " << bs.tail() << "\n";
    std::cout << "Removals took: "
              << std::chrono::duration<double, std::micro>(end_remove - start_remove).count()
              << " µs\n\n";

    // ---------- RANDOM ACCESS CHECK ----------
    auto start_access = std::chrono::high_resolution_clock::now();
    std::uniform_int_distribution<int> dist_access(PRICE_MIN, PRICE_MAX);

    int unset_count = 0;
    for (int i = 0; i < 100; ++i) {
        int price = dist_access(rng);
        if (!bs.get(price)) {
            ++unset_count;
        }
    }
    auto end_access = std::chrono::high_resolution_clock::now();

    std::cout << "Random access checks took: "
              << std::chrono::duration<double, std::micro>(end_access - start_access).count()
              << " µs\n";
    std::cout << "Unset bits found: " << unset_count << "\n\n";

    // ---------- STATISTICS ----------
    std::cout << "Total active orders: " << bs.tail() - bs.head() << "\n";

    auto end_total = std::chrono::high_resolution_clock::now();
    std::cout << "Total simulation time: "
              << std::chrono::duration<double, std::milli>(end_total - start_total).count()
              << " ms\n";

    return 0;
}
