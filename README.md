# Bitset Segmented

High-performance segmented bitset for financial markets, optimized for fast head/tail queries, ideal for price ladders. Also provides a **C# API** for integration.

---

## Features

* **Segmented Bitset**: stores bits in fixed-size blocks (`segments`) for efficient handling of large index ranges.
* **Fast head/tail queries**: maintains the smallest and largest set bit (`head` and `tail`) for quick top of book update.
* **Dynamic memory allocation**: segments are created on-demand, minimizing memory usage.
* **Reserve memory**: pre-allocate segments for a maximum index (`reserve_for_max_index`).
* **C# API**: `extern "C"` functions for cross-language integration.

---

## Class Overview

### `bsa::bitset_segmented`

**Constructor / Destructor:**

```cpp
bitset_segmented();
~bitset_segmented();
```

**Main Methods:**

| Method                                              | Description                                           |
| --------------------------------------------------- | ----------------------------------------------------- |
| `void set(std::size_t index)`                       | Sets the bit at the given index.                      |
| `bool unset(std::size_t index)`                     | Clears the bit and returns whether it was set before. |
| `bool get(std::size_t index) const`                 | Returns `true` if the bit is set.                     |
| `void reserve_for_max_index(std::size_t max_index)` | Pre-allocates segments up to the given index.         |
| `bool empty() const`                                | Returns `true` if no bits are set.                    |
| `std::size_t head() const`                          | Returns the smallest set index.                       |
| `std::size_t tail() const`                          | Returns the largest set index + 1.                    |

---

## C# API

External functions for C# integration:

```c
void* bitset_create();
void bitset_destroy(void* bs);
void bitset_set(void* bs, std::size_t index);
bool bitset_unset(void* bs, std::size_t index);
bool bitset_get(void* bs, std::size_t index);
void bitset_reserve(void* bs, std::size_t max_index);
bool bitset_empty(void* bs);
std::size_t bitset_head(void* bs);
std::size_t bitset_tail(void* bs);
```

---

## Installation on Linux

1. Clone the repository:

```bash
git clone https://github.com/lbricio/bitset_segmented.git
cd bitset_segmented
```

2. Build and install:

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

3. Include the library and headers in your project:

```bash
-I/usr/local/include
-L/usr/local/lib -lbitset_segmented
```

---

## Example Usage (C++)

```cpp
#include <iostream>
#include "bitset_segmented/bitset_segmented.hpp"

int main() {
    bsa::bitset_segmented bs;

    bs.set(10);
    bs.set(5000);
    bs.set(20000);

    std::cout << "Bit 10: " << bs.get(10) << "\n";
    std::cout << "Bit 5000: " << bs.get(5000) << "\n";
    std::cout << "Bit 20000: " << bs.get(20000) << "\n";
    std::cout << "Bit 15: " << bs.get(15) << "\n";

    std::cout << "Head: " << bs.head() << "\n";
    std::cout << "Tail: " << bs.tail() << "\n";

    return 0;
}
```

---

## Example Usage (C#)

```csharp
using System;
using System.Runtime.InteropServices;

class Program
{
    [DllImport("bitset_segmented.so")]
    public static extern IntPtr bitset_create();

    [DllImport("bitset_segmented.so")]
    public static extern void bitset_destroy(IntPtr bs);

    [DllImport("bitset_segmented.so")]
    public static extern void bitset_set(IntPtr bs, ulong index);

    [DllImport("bitset_segmented.so")]
    public static extern bool bitset_get(IntPtr bs, ulong index);

    static void Main()
    {
        IntPtr bs = bitset_create();
        bitset_set(bs, 10);
        bitset_set(bs, 5000);

        Console.WriteLine(bitset_get(bs, 10));    // true
        Console.WriteLine(bitset_get(bs, 5000));  // true
        Console.WriteLine(bitset_get(bs, 15));    // false

        bitset_destroy(bs);
    }
}
```
