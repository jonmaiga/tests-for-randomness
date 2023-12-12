#include "config.h"

#include <optional>

namespace tfr {

std::optional<config> g_config;

const config& get_config(){
    return *g_config;
}  
void set_config(const config& config){
    g_config = config;
}

bool is_debug() {
#ifdef TFR_DEVELOPMENT
    return true;
#else
    return false;
#endif
}
}
