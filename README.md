# unique_void
A smart pointer that stores any type as a void* and properly deletes it using type erasure.

This class does not require the user to manually pass in a template argument or a deleter, but it must be initialized with a pointer of the original type:

```c++
#include <string>
#include "unique_void.hpp"

int main() {

    // these are all valid because they are initialized with pointers of the original type
    unique_void p1(new int(959));
    unique_void p2(new double(595.5));
    unique_void p3(new std::string("Hello World"));
    int* a = new int(890);
    unique_void p4(a);

    // this is not valid because it is initialized with a void pointer
    int* b = new int(890);
    unique_void p5((void*)b);

    return 0;
}
```

Because this class supports move semantics and does not require a template argument, you can store a collection of unique_void objects that point to different types:

```c++
#include <string>
#include <vector>
#include "unique_void.hpp"

int main() {

    std::vector<unique_void> v;

    // these are all valid because they are initialized with pointers of the original type
    v.push_back(unique_void(new int(959)));
    v.push_back(unique_void(new double(595.5)));
    v.push_back(unique_void(new std::string("Hello World")));
    int* a = new int(890);
    v.push_back(unique_void(a));

    return 0;
}
```

This class supports most of the methods in std::unique_ptr, but it does not support the `*` or `->` operators because void pointers cannot be dereferenced. Instead, cast the result of `get()` to the proper type in order to dereference it, e.g. `*((int*)p.get()) += 2`

The easiest way to test this class is to edit the deleter like so:

```c++
template <typename T>
void createDeleter() {
    deleter = [](void const * ptr) -> void {
        T const * p = static_cast<T const*>(ptr);
        std::cout << "{" << *p << "} located at [" << p <<  "] is being deleted.\n";
        delete p;
    };
}
```

This will print out the types before they are deleted like so:

```c++
#include <string>
#include <vector>
#include <memory>
#include "unique_void.hpp"

int main() {

    // swap()
    unique_void p1(new int(90));
    unique_void p2(new std::string("foo"));
    p1.swap(p2);

    // release()
    unique_void p3(new int(100)); // 100 will not be printed out
    int* b = (int*)p3.release();
    unique_void p4(new int(*b + 1)); // should be 101
    delete b;

    // reset()
    unique_void p5(new int(94));
    p5.reset(new std::string("bar"));

    // move semantics
    unique_void p6(new int(200));
    unique_void p7(nullptr);
    p7 = std::move(p6);

    return 0;
}
```

which should output something like this:
```
{94} located at [0x100300030] is being deleted.
{200} located at [0x100400000] is being deleted.
{bar} located at [0x100300050] is being deleted.
{101} located at [0x100300040] is being deleted.
{90} located at [0x100300000] is being deleted.
{foo} located at [0x100300010] is being deleted.
```
