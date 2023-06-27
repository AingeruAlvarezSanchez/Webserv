#include "../classes/server_conf.hpp"
#include <iostream>

int main(int argc, char **argv) {
    ServerConf::LocationBlock location1 = {};
    ServerConf serverConf;

    serverConf.setPort(80);
    serverConf.setHost("0.0.0.0");
    serverConf.addServName("example.com");
    serverConf.addServName("example2.com");
    serverConf.addServName("example3.com");
    serverConf.addServName("example.com");
    serverConf.addErrorPage(404, "/error");
    serverConf.addErrorPage(405, "/error/405");
    serverConf.addErrorPage(404, "/error");
    serverConf.setMaxBytes(1000);
    serverConf.addLocation(location1);
    std::cout << "Port>" << serverConf.serverBlock().port << "\n";
    std::cout << "Host>" << serverConf.serverBlock().host << "\n";
    std::cout << "Maxbytes>" << serverConf.serverBlock().maxBytes << "\n";
    for (auto it  : serverConf.serverBlock().servNames) {
        std::cout << "name: " << it << " | ";
    }
    std::cout << "\n";
    for (const auto& it : serverConf.serverBlock().defErrorPage) {
        std::cout << "error page: " << it.first << " -> ";
        for (const auto& error : it.second) {
            std::cout << error << " | ";
        }
    }
    return 0;
}