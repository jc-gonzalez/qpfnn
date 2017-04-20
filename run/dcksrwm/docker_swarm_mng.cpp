// -*- C++ -*-

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>

#include "process.h"

typedef std::vector<std::string> stringlist;
typedef std::vector<stringlist> table;

std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool getInfo(std::string srvName, std::stringstream & info, table & t)
{
    procxx::process srvPs("docker", "service", "ps");
    srvPs.add_argument(srvName);
    srvPs.exec();

    std::string line;
    std::vector<std::string> items;

    std::getline(srvPs.output(), line);
    while (std::getline(srvPs.output(), line)) {
        stringlist lineItems;
        int k = 0;
        info << line << std::endl;
        items.clear();
        split(line, ' ', items);
        for (auto & s : items) {
            if (!s.empty() && s != "\\_" && k < 5) { lineItems.push_back(s); ++k; }
            std::cerr << s << '|';
        }
        t.push_back(lineItems);
        std::cerr << '\n';
    }

    srvPs.wait();
    return (srvPs.code() == 0);
}

int main(int argc, char * argv[])
{
    std::string srvName(argv[1]);

    std::stringstream info("");
    table tbl;

    getInfo(srvName, info, tbl);

    std::cout << info.str() << std::endl;
    for (auto & lst : tbl) {
        for (auto & s : lst) {
            std::cout << s << '\t';
        }
        std::cout << '\n';
    }

    return 0;
}
