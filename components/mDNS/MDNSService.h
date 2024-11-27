#include "esp_mdns.h" // The original ESP mDNS API
#include "mdns.h"
#include <string>    // For std::string
#include <vector>   // For std::vector
#include <functional> // For std::function (callbacks)
#include <memory>    // For std::unique_ptr (smart pointers)

class Mdns {
public:

    class SearchHandle {
    public:
        SearchHandle(mdns_search_once_t* handle) : _handle(handle) {}
        ~SearchHandle() {
            if (_handle) {
                mdns_query_async_delete(_handle);
            }
        }
    private:
        mdns_search_once_t* _handle;    

        SearchHandle(const SearchHandle&) = delete;
        SearchHandle& operator=(const SearchHandle&) = delete;
    };

};