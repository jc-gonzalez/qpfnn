// -*- C++ -*-

#include <string>
#include <vector>
#include <map>

#include "str.h"
#include "datatypes.h"

class MetadataInfo : public JValue {};

class MetadataReader {
public:
    MetadataReader() {}
    MetadataReader(std::string & fName) : fileName(fName) {}

    virtual bool setFile(std::string & fName) { return false; }
    virtual bool getMetadataInfo(MetadataInfo & md) { return false; }

protected:
    std::string fileName;
};

class FitsMetadataReader : public MetadataReader {

public:
    MetadataReader() {}
    MetadataReader(std::string & fName) : fileName(fName) {}

    virtual bool setFile(std::string & fName) {
        return false;
    }
    virtual bool getMetadataInfo(MetadataInfo & md) {
        std::string content;
        readFitsMetadata(content);
        std::cerr << content << "\n";
        return false;
    }

protected:
    bool readFitsMetadata(std::string & str) {
        const int BUF_LEN = 80;
        const int TAG_LEN = 8;
        char buffer[BUF_LEN + 1];

        buffer[BUF_LEN] = 0;

        int bytes, tbytes = 0;
        str = "{ \"Header\": { ";
        bool inHdr = true;
        bool isStart = true;
        int numOfExtensions = 0;

        std::ifstream in(fileName.c_str());

        while (in.good()) {
            bytes = in.read(buffer, BUF_LEN);
            if (bytes < 0) { break; }
            tbytes += bytes;
            std::string tag = std::string(buffer, TAG_LEN);
            if (tag == "END     ") {
                str += "}";
                inHdr = false;
            } else {
                std::string content = std::string(buffer + TAG_LEN, BUF_LEN - TAG_LEN);
                if (content.at(0) == '=') {
                    content.erase(0, 1);
                } else {
                    content = "\"" + content + "\"";
                }
                if (tag == "XTENSION") {
                    inHdr = true;
                    clearComment(content)
                    str += std::string((numOfExtensions < 1) ? ", \"Extensions\": [ " : ", ") +
                        " { \"" + tag + "\": " + content;
                    numOfExtensions++;
                } else {
                    if (inHdr) {
                        str::replaceAll(content, " T ", " true ");
                        str::replaceAll(content, " F ", " false ");
                        clearComment(content);
                        if (!isStart) { str += ", "; }
                        str += "\"" + tag + "\": " + content;
                    }
                }
            }
            isStart = false;
        }
        in.close();

        str += (numOfExtensions > 0) ? "] }": "}";
        /*
        str.replace(QRegExp("[ ]+\""), "\"");
        str.replace(QRegExp("[ ]+'"), "'");

        QRegExp * rx = new QRegExp("(\"HISTORY\"):(\".*\"),(\"HISTORY\":)");
        rx->setMinimal(true);
        int pos = 0;
        while ((pos = rx->indexIn(str, pos)) != -1) {
            std::string srx(rx->cap(1) + ":" + rx->cap(2) + ",");
            str.replace(rx->cap(0), srx);
        }
        delete rx;
        rx = new QRegExp("(\"HISTORY\"):(\".*\")[}]");
        rx->setMinimal(true);
        pos = 0;
        while ((pos = rx->indexIn(str, pos)) != -1) {
            std::string srx(rx->cap(1) + ": [" + rx->cap(2) + "] }");
            str.replace(rx->cap(0), srx);
            pos += srx.length();
        }
        delete rx;
        */
        //str.replace(",\"\":\"\"", "");
        //str.replace("\":'", "\":\"");
        //str.replace(QRegExp("'[ ]*,"), "\",");
    }

    void clearComment(std::string & line) {
        size_t pos = line.rfind(" / ");
        if (pos != std::string::npos) {
            line.erase(pos,1000);
        }
        str::trim(line);
    }
};
