#include "os_core.h"

#ifdef GAME_WIN
#   include "os_core_win32.h"
#else
#   error "Unsupported OS!"
#endif

fn os_read_entire_file(std::string_view filename) -> std::string {
    std::ifstream file;
    file.open(filename);
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string buf = buffer.str();

    return buf;
}


fn os_trim(std::string text) -> std::string {
    std::string s = text;
    // left Global::trim
    s.erase(s.begin(),
        std::find_if(s.begin(), s.end(),
            [](unsigned char ch) { return !std::isspace(ch); }));

    // right Global::trim
    s.erase(
        std::find_if(s.rbegin(), s.rend(),
            [](unsigned char ch) { return !std::isspace(ch); }).base(),
        s.end());
    return s;
}

fn os_set_working_dir(const std::string& path) -> void {
#ifdef GAME_WIN
    os_set_working_dir_win32(path);
#else
#   error "Unsupported OS!"
#endif
}

fn os_get_time() -> f64 {
#ifdef GAME_WIN
    return os_get_time_win32();
#else
#   error "Unsupported OS!"
#endif
}