class unique_void {

    void* _ptr;

    void (*deleter) (void const*); // pointer to deleter

public:
    
    // use type erasure so the user doesn't have to pass in a template argument or a deleter
    template <typename T>
    unique_void(T * ptr) {
        _ptr = ptr;
        deleter = [](void const * ptr) -> void {
            delete static_cast<T const*>(ptr);
        };
    }

    ~unique_void() {
        deleter(_ptr);
    }

    void* get() {
        return _ptr;
    }

};
