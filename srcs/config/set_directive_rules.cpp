#include "../classes/server_info.hpp"

#include <iostream> //TODO
void    set_limit_except_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "limit_except>" << line;
}

void    set_return_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "return>" << line;
}

void    set_root_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "root>" << line;
}

void    set_try_files_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "try_files>" << line;
}

void    set_auto_index_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "auto_index>" << line;
}

void    set_index_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "index>" << line;
}

void    set_cgi_pass_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "cgi_pass>" << line;
}

void    set_upload_rule(std::string const& line, __attribute_maybe_unused__ ServerInfo::s_location & location) {
    std::cout << "upload>" << line;
}
