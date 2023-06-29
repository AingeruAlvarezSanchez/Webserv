#include "webserv.h"
#include "../../classes/server_conf.hpp" //TODO just for clion

struct Directives {
public:
    typedef std::map<std::string, int(*)(ServerConf &serverConf, const std::string &value, const std::string &code)>::iterator   Iterator;
public:
    std::map<std::string, int(*)(ServerConf &serverConf, const std::string &value, const std::string &code)> directiveMap;
    Directives() {
        directiveMap["port:"] = &port_set;
        directiveMap["host:"] = &host_set;
        directiveMap["server_name:"] = &server_name_set;
        directiveMap["error_page:"] = &error_page_set;
        directiveMap["client_max_body_size:"] = &max_body_set;
        directiveMap["limit_except:"] = &limit_except_set;
        directiveMap["return:"] = &return_set;
        directiveMap["root:"] = &root_set;
        directiveMap["try_files:"] = &try_files_set;
        directiveMap["auto_index:"] = &auto_index_set;
        directiveMap["index:"] = &index_set;
        directiveMap["cgi_pass:"] = &cgi_pass_set;
        directiveMap["upload:"] = &upload_set;
    }
};

std::string fetch_directive(const std::string &line) {
    size_t start = line.find_first_not_of(" \t\n");
    size_t end = line.find(';') + 1;
    std::string content = line.substr(start, end - start);
    return content;
}

int parse_directive(const std::string &content, ServerConf &serverConf) {
    static Directives  directives;

    size_t directiveEnd = content.find(':') + 1;
    size_t valueEnd = content.find(';');
    std::string name = content.substr(0, directiveEnd);
    std::string value = content.substr(directiveEnd, valueEnd - directiveEnd);

    Directives::Iterator it = directives.directiveMap.find(name);
    if (it != directives.directiveMap.end()) {
        if (it->second(serverConf, value, "") == -1) {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}
