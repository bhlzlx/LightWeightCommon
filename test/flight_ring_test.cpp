#include <cassert>
#include <memory/flight_ring.h>

int main() {
    comm::FlightRing<2> fr(128, 8);
    for(uint32_t i = 0; i<1000; ++i) {
        assert(fr.alloc(3) != fr.InvalidAlloc);
        assert(fr.alloc(7) != fr.InvalidAlloc);
        assert(fr.alloc(15) != fr.InvalidAlloc);
        assert(fr.alloc(16) != fr.InvalidAlloc);
        fr.prepareNextFlight();
    }
    return 0;
}