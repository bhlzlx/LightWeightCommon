#pragma once

namespace comm {
    /**
     * @brief 
     *   具体实现以后可以换内存池
     */

    void* comm_alloc(size_t size);
    void comm_free(void*);

}