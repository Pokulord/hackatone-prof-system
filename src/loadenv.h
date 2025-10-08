#pragma once

#include <string>

class EnvLoader {
public:
    // Trim whitespace from both ends
    static std::string trim(const std::string &s);

    // Parse a possibly quoted value, handling \" and \\ inside quotes
    static std::string parseValue(const std::string &raw);

    // Set env var using setenv on POSIX or putenv on platforms without setenv.
    // Returns true on success.
    static bool set_env_var(const std::string &key, const std::string &value, bool overwrite = true);

    // Load .env file. Returns number of variables set.
    static int load_file(const std::string &path, bool overwrite = true);

    // Load from a string (multiple lines). Returns number of variables set.
    static int load_from_string(const std::string &data, bool overwrite = true);
};