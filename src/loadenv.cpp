#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <vector>

#include "loadenv.h"

// Trim whitespace from both ends
std::string EnvLoader::trim(const std::string &s) {
    size_t i = 0;
    size_t j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j - 1]))) --j;
    return s.substr(i, j - i);
}

// Parse a possibly quoted value, handling \" and \\ inside quotes
std::string EnvLoader::parseValue(const std::string &raw) {
    std::string v = trim(raw);
    if (v.size() >= 2 && ((v.front() == '"' && v.back() == '"') || (v.front() == '\'' && v.back() == '\''))) {
        char q = v.front();
        std::string out;
        for (size_t i = 1; i + 1 < v.size(); ++i) {
            char c = v[i];
            if (c == '\\' && i + 1 + 1 < v.size()) {
                char nxt = v[i + 1];
                // simple escape handling
                if (nxt == q) { out.push_back(q); i += 1; continue; }
                if (nxt == '\\') { out.push_back('\\'); i += 1; continue; }
                // keep backslash for unknown escapes
                out.push_back('\\');
                // fallthrough to push nxt normally on next iteration
            } else {
                out.push_back(c);
            }
        }
        return out;
    }
    // Unquoted: remove inline comments after an unescaped #
    std::string out;
    bool escaped = false;
    for (size_t i = 0; i < v.size(); ++i) {
        char c = v[i];
        if (!escaped && c == '#') break; // comment
        if (!escaped && c == '\\') { escaped = true; continue; }
        out.push_back(c);
        escaped = false;
    }
    return trim(out);
}

// Set env var using setenv on POSIX or putenv on platforms without setenv.
// Returns true on success.
bool EnvLoader::set_env_var(const std::string &key, const std::string &value, bool overwrite) {
#if defined(_WIN32) || defined(_WIN64)
    // Windows _putenv_s is available in C runtime
    int r = 0;
    r = _putenv_s(key.c_str(), value.c_str());
    return r == 0;
#else
    // POSIX setenv
    return setenv(key.c_str(), value.c_str(), overwrite ? 1 : 0) == 0;
#endif
}

// Load .env file. Returns number of variables set.
int EnvLoader::load_file(const std::string &path, bool overwrite) {
    std::ifstream ifs(path.c_str());
    if (!ifs.is_open()) return -1;
    std::string line;
    int count = 0;
    while (std::getline(ifs, line)) {
        // trim leading spaces for comment detection
        std::string tline = trim(line);
        if (tline.empty()) continue;
        if (tline.size() && tline[0] == '#') continue;

        // find first '=' not in quotes
        size_t eq = std::string::npos;
        bool in_single = false, in_double = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '\'' && !in_double) in_single = !in_single;
            else if (c == '"' && !in_single) in_double = !in_double;
            else if (c == '=' && !in_single && !in_double) { eq = i; break; }
        }
        if (eq == std::string::npos) continue; // no key=value

        std::string key = trim(line.substr(0, eq));
        std::string rawval = line.substr(eq + 1);
        if (key.empty()) continue;

        // Remove optional export prefix: export KEY=VALUE
        const std::string export_prefix = "export ";
        if (key.size() > export_prefix.size()) {
            std::string lowkey = key;
            // check prefix case-sensitively as in shell it's 'export'
            if (key.compare(0, export_prefix.size(), export_prefix) == 0) {
                key = trim(key.substr(export_prefix.size()));
            }
        }

        std::string value = parseValue(rawval);

        if (set_env_var(key, value, overwrite)) ++count;
    }
    return count;
}

// Load from a string (multiple lines). Returns number of variables set.
int EnvLoader::load_from_string(const std::string &data, bool overwrite) {
    std::istringstream iss(data);
    std::string line;
    int count = 0;
    while (std::getline(iss, line)) {
        std::string tline = trim(line);
        if (tline.empty()) continue;
        if (tline.size() && tline[0] == '#') continue;

        size_t eq = std::string::npos;
        bool in_single = false, in_double = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '\'' && !in_double) in_single = !in_single;
            else if (c == '"' && !in_single) in_double = !in_double;
            else if (c == '=' && !in_single && !in_double) { eq = i; break; }
        }
        if (eq == std::string::npos) continue;

        std::string key = trim(line.substr(0, eq));
        std::string rawval = line.substr(eq + 1);
        if (key.empty()) continue;

        const std::string export_prefix = "export ";
        if (key.size() > export_prefix.size()) {
            if (key.compare(0, export_prefix.size(), export_prefix) == 0) {
                key = trim(key.substr(export_prefix.size()));
            }
        }

        std::string value = parseValue(rawval);

        if (set_env_var(key, value, overwrite)) ++count;
    }
    return count;
}
