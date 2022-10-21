#include "memory.h"
#include <cstdlib>

namespace comm {

    void* comm_alloc(size_t size) {
        return malloc(size);
    }

    void comm_free(void* ptr) {
        return free(ptr);
    }

}