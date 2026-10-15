# libftpp — TODO

## Done ✅
- [x] `Pool` is a class template; `Object` is nested and uses Pool's `TType`
- [x] `acquire` has a variadic pack + `std::forward`
- [x] `Node` lives in Pool, with `alignas(TType)` and a real `sizeof(TType)` byte buffer
- [x] `isFree` has a default value
- [x] placement new builds the TType inside the slot
- [x] `~Object` calls `~TType()` explicitly (leak demo understood)
- [x] Object copy is `= delete`
- [x] `std::vector<std::unique_ptr<Node>>` → slots don't move when the vector grows
- [x] Pool move ops use `std::move` and `this != &other`
- [x] `#include "resize.cpp"` removed from pool.hpp

---

## Pool — compile blockers (do these first)
- [x] **Line 84:** `node` is now a reference (no more unique_ptr copy)
- [ ] **Release:** Object only stores `ptr` → can't mark its slot free. The old `unique_ptr<Node>&` gave heap-use-after-free (freed by `p.resize(10)`).
  - What moved during `resize(10)`: the Node, or the `unique_ptr`? Point at the one that doesn't move.
  - You once wrote `Node* no = node.get();` — what kind of pointer is that?
  - The unique_ptr lives in the vector's buffer → does it move when the vector grows? Does the Node?
  - Can a reference member be re-pointed? What does `objNode = std::move(...)` / `objNode = nullptr` do to the **pool's** entry?
  - Move ctor has no init list → a reference member must be initialized (error appears once it's used)
  - Read: *owning vs non-owning pointers*, `unique_ptr::get()`
  - Who owns a Node? What does Object actually need: to own it, or just to point at it?
- [ ] Object constructor: the parameter type and the `objNode(objNode)` init both follow from the ownership decision

## Pool — logic
- [x] `resize` uses `vector::resize` → `size()` grows, elements really exist
- [x] `resize` gives every empty element a Node (and keeps existing ones when growing)
- [x] 🐞 **Same slot handed out twice** — fixed: `acquire` checks `isFree` and marks it taken (your output `heeloos heeloos`; with long strings ASan → double-free)
  - `if (pool[i])` asks "does this slot have a Node?" — not "is it free?" Where did `isFree` go?
  - Walk the loop by hand for 2 acquires on a `Pool(5)`: which `i` do you get each time?
  - `push_back` inside a loop over the same vector: how many Nodes does each acquire add? Print `size()`
  - Nodes created in `acquire` → is the pool still *pre*-allocated?
  - Split jobs: `resize` = make slots real · `acquire` = find a free one, mark it taken (grow only if none)
- [x] Line 80: ternary fixed
- [ ] When growing: which index is the first *new* Node?
- [ ] No free slot → `exit(2)`: a library killing the program, no destructors run, caller can't react. Read: exceptions. Also the comment says "doubles its size" — which is true?
- [ ] `((*n)->data) == nullptr`: can an array's address ever be null?
- [ ] `resize` to a **smaller** number while slots are in use: what should happen? Decide + document
- [ ] `resize` error: `cerr` + `return` means the caller can't tell it failed. Read: exceptions (`std::length_error`)
- [ ] Pool destroyed while Objects are still alive: what do their Node pointers point to? Decide + document
- [ ] `friend class Object;` (line 31): at that point which `Object` does it name? Is it needed? Read: *nested classes* access rules

## Object
- [ ] Move ops still use `objNode`, which no longer exists → compiles only because they're never instantiated. Test with `std::move`
- [ ] `~Object` on a **moved-from** Object: `ptr` is `nullptr` → `ptr->~TType()` crashes. What should it do?
- [ ] `std::move(other.ptr)` on a raw pointer: what does it actually do vs copying?
- [ ] After ownership is fixed, check both move ops still make sense

## Tests to write in main.cpp (run with `make debug`)
- [ ] Two acquires → print `obj.operator->()` → different addresses
- [ ] Release one (inner `{ }` scope), acquire again → same slot reused?
- [ ] `std::move` an Object into another, use the new one, old one's destructor doesn't crash
- [ ] Acquire **more** than the resize count → growth → earlier Objects still valid
- [ ] A test class with no default ctor, several ctors, and prints in ctor/dtor (count them!)
- [x] `Pool<std::pair>`, `Pool<std::vector<int>>`, `Pool<std::string>` in separate functions
- [ ] Use **long** strings (> 15 chars) in tests — read: *small string optimization* (why short ones hid the bug)
- [x] `-D_GLIBCXX_ASSERTIONS` added (`make bug`)
- [ ] Zero leaks, zero ASan errors under `make bug`

## Cleanup
- [ ] ⚠️ `.gitignore` ignores `Makefile` → it's **not tracked** and won't be submitted
- [ ] Delete `data-structures/pool/resize.cpp` (no longer used)
- [ ] Top comment still says "Pool is not templated"
- [ ] Old comments in Object's ctor (about placement new / TArgs) are outdated
- [ ] Debug `std::cout` in destructors: should a library print?

---

## Project structure (later)
- [ ] Makefile: `pathsub` → ?, `.c` → ?, `$(NAME)` rule runs nothing (read: `ar rcs`), quotes in `SRC_FILES`, `.PHONY`
- [ ] Makefile: `STD = c++23` but `debug`/`run` use `c++20` → pick one
- [ ] `include/libftpp.hpp` must include `data_structures.hpp` (and later all others)
- [ ] Every header compiles **alone**: `c++ -std=c++20 -fsyntax-only -x c++ <file>.hpp`
- [ ] Rule: no function bodies in headers except templates → fine for Pool, matters for DataBuffer

## DataBuffer (not started)
- [ ] `data_buffer.hpp` includes `data_buffer.cpp`, which includes the `.hpp` → trace it
- [ ] `.cpp`: `operator=` is missing something before its name; `~DataBuffer::DataBuffer` is in the wrong order
- [ ] Templated `operator<<` in a `.cpp` → read: *why templates must be in headers*
- [ ] `operator<<` returns `void` → try `buf << a << b`
- [ ] `operator>>` doesn't exist yet
- [ ] Read: serialization, *trivially copyable*; why can't `std::string` be copied byte by byte?
- [ ] Reading more bytes than stored → what happens?
- [ ] Include guard named `DATA_BUFFER_CPP` in a `.hpp`

---

## Be ready to explain at evaluation
- Why `resize` must not construct TType
- Why placement new must be paired with an explicit destructor call
- Why Object's copy is deleted, and what a move does
- Why `vector<unique_ptr<Node>>` keeps Objects valid when the pool grows (and the cost: one allocation per slot)
- Your growth policy, and what happens on shrink / pool destruction
