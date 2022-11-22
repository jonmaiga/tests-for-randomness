#include "config.h"

#include <optional>

namespace mixer {

std::optional<config> g_config;

const config& get_config(){
    return *g_config;
}  
void set_config(const config& config){
    g_config = config;
} 

}
