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
- [ ] **Ownership:** Object now holds `std::unique_ptr<Node>&` — a reference to the pool's *unique_ptr*, not to the Node.
  - The unique_ptr lives in the vector's buffer → does it move when the vector grows? Does the Node?
  - Can a reference member be re-pointed? What does `objNode = std::move(...)` / `objNode = nullptr` do to the **pool's** entry?
  - Move ctor has no init list → a reference member must be initialized (error appears once it's used)
  - Read: *owning vs non-owning pointers*, `unique_ptr::get()`
  - Who owns a Node? What does Object actually need: to own it, or just to point at it?
- [ ] Object constructor: the parameter type and the `objNode(objNode)` init both follow from the ownership decision

## Pool — logic
- [ ] `resize` only calls `reserve` → no Node is ever created, `size()` stays 0, the loop never runs
  - Which call actually creates a Node in a `unique_ptr`?
- [x] Line 80: ternary fixed
- [ ] `poolArena[size()]` is one past the end. After growing, which index is the first *new* Node?
- [ ] `resize` to a **smaller** number while slots are in use: what should happen? Decide + document
- [ ] `resize` error: `cerr` + `return` means the caller can't tell it failed. Read: exceptions (`std::length_error`)
- [ ] Pool destroyed while Objects are still alive: what do their Node pointers point to? Decide + document
- [ ] `friend class Object;` (line 31): at that point which `Object` does it name? Is it needed? Read: *nested classes* access rules

## Object
- [ ] `~Object` on a **moved-from** Object: `ptr` is `nullptr` → `ptr->~TType()` crashes. What should it do?
- [ ] `std::move(other.ptr)` on a raw pointer: what does it actually do vs copying?
- [ ] After ownership is fixed, check both move ops still make sense

## Tests to write in main.cpp (run with `make debug`)
- [ ] Two acquires → print `obj.operator->()` → different addresses
- [ ] Release one (inner `{ }` scope), acquire again → same slot reused?
- [ ] `std::move` an Object into another, use the new one, old one's destructor doesn't crash
- [ ] Acquire **more** than the resize count → growth → earlier Objects still valid
- [ ] A test class with no default ctor, several ctors, and prints in ctor/dtor (count them!)
- [ ] `Pool<int>`, `Pool<std::string>`
- [ ] Zero leaks, zero ASan errors, also with `-D_GLIBCXX_ASSERTIONS` (add it to `make debug`)

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
