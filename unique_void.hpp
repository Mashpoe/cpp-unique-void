class unique_void {

    void* m_ptr;

    void (*deleter) (void const*); // pointer to deleter

    // use type erasure so the user doesn't have to pass in a template argument or a deleter
    template <typename T>
    void createDeleter() {
        deleter = [](void const * ptr) -> void {
            delete static_cast<T const*>(ptr);
        };
    }

public:

    template <typename T>
    unique_void(T * ptr) 
    : m_ptr(ptr) {
        createDeleter<T>();
    }

    unique_void()
    : m_ptr(nullptr), deleter(nullptr) {}

    unique_void(decltype(nullptr))
    : m_ptr(nullptr), deleter(nullptr) {}

    ~unique_void() {
        if (m_ptr != nullptr) {
            deleter(m_ptr);
        }
    }

    // remove copy semantics
    unique_void(unique_void const&) = delete;
    unique_void& operator=(unique_void const&) = delete;

    // move constructor
    unique_void(unique_void&& other) noexcept {
        m_ptr = other.m_ptr;
        deleter = other.deleter;

        other.m_ptr = nullptr;
        other.deleter = nullptr;
    }

    // move assignment
    unique_void& operator=(unique_void&& other) noexcept {
        if (this != &other) {
            if (m_ptr != nullptr) {
                deleter(m_ptr); // free the current data
            }

            // take ownership
            m_ptr = other.m_ptr;
            deleter = other.deleter;

            // reset other
            other.m_ptr = nullptr;
            other.deleter = nullptr;
        }
        return *this;
    }

    void* get() const noexcept {
        return m_ptr;
    }

    explicit operator bool() const noexcept {
        return m_ptr != nullptr;
    }

    void* release() noexcept {
        void* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    void swap(unique_void& other) noexcept {
        // swap the pointer
        void* tempPtr = m_ptr;
        m_ptr = other.m_ptr;
        other.m_ptr = tempPtr;

        // swap the deleter
        void (*tempDeleter) (void const*) = deleter;
        deleter = other.deleter;
        other.deleter = tempDeleter;
    }

    template <typename T>
    void reset(T* ptr) {
        if (m_ptr != nullptr) {
            deleter(m_ptr); // call the deleter for the old pointer
        }

        m_ptr = ptr;
        if (ptr != nullptr) {
            createDeleter<T>(); // create a new deleter for the new type
        }
    }

    void reset() {
        if (m_ptr != nullptr) {
            deleter(m_ptr); // call the deleter for the old pointer

            m_ptr = nullptr;
            deleter = nullptr;
        }
    }

};
