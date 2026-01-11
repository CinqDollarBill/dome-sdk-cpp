#ifndef DOME_UTILS_HPP
#define DOME_UTILS_HPP

#include <string>
#include <fstream>
#include <cstdlib>

namespace dome {

inline std::string get_env_var(const std::string& key) {
    const char* val = std::getenv(key.c_str());
    return val ? std::string(val) : "";
}

inline std::string parse_env_file(const std::string& path, const std::string& key) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, key.length()) == key && line[key.length()] == '=') {
            return line.substr(key.length() + 1);
        }
    }
    return "";
}

inline std::string load_config_value(const std::string& key) {
    std::string val = get_env_var(key);
    if (!val.empty()) return val;
    
    val = parse_env_file(".env", key);
    if (!val.empty()) return val;
    
    val = parse_env_file("../.env", key);
    if (!val.empty()) return val;
    
    return "";
}

}  // namespace dome

#endif  // DOME_UTILS_HPP
