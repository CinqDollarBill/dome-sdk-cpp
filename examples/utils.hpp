#ifndef DOME_EXAMPLES_UTILS_HPP
#define DOME_EXAMPLES_UTILS_HPP

#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>

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
    // 1. Check environment variable
    std::string val = get_env_var(key);
    if (!val.empty()) return val;
    
    // 2. Check .env in current directory
    val = parse_env_file(".env", key);
    if (!val.empty()) return val;
    
    // 3. Check .env in parent directory
    val = parse_env_file("../.env", key);
    if (!val.empty()) return val;
    
    return "";
}

inline void print_separator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

#endif // DOME_EXAMPLES_UTILS_HPP
