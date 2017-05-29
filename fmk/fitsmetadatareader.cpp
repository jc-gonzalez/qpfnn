/******************************************************************************
 * File:    fitsmetadatareader.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.FitsMetadataReader
 *
 * Version:  1.2
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implements FitsMetadataReader class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#include "fitsmetadatareader.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Constructor
//==========================================================================
FitsMetadataReader::FitsMetadataReader()
{
}

//==========================================================================
// Constructor
//==========================================================================
FitsMetadataReader::FitsMetadataReader(std::string & fName)
    : MetadataReader(fName)
{
}

//==========================================================================
// Destructor
//==========================================================================
FitsMetadataReader::~FitsMetadataReader()
{
}

//==========================================================================
// METHOD: setFile
// Define the file name of the data product to read
//==========================================================================
bool FitsMetadataReader::setFile(std::string & fName)
{
    return false;
}

//==========================================================================
// METHOD: getMetadataInfo
// Obtains and provides the metadata info taken from the product file
//==========================================================================
bool FitsMetadataReader::getMetadataInfo(MetadataInfo & md)
{
    std::string content;
    bool retVal = readFitsMetadata(content);
    if (retVal) { md.fromStr(content); }
    return retVal;
}

//==========================================================================
// METHOD: readFitsMetadata
// Performs the actual metadata retrieval from the file
//==========================================================================
bool FitsMetadataReader::readFitsMetadata(std::string & str)
{
    const int BLOCK_LEN = 36 * 80; // 2880 bytes
    const int BUF_LEN = 80;
    const int TAG_LEN = 8;

    char block[BLOCK_LEN + 1];
    char * buffer;
    int blockBytesLeft = 0;

    block[BUF_LEN] = 0;

    int bytes, tbytes = 0;

    bool inHdr = true;
    bool isStart = true;
    bool inHistory = false;
    int numOfExtensions = 0;

    str = "{ \"Header\": { ";

    std::ifstream in(fileName.c_str(), std::ifstream::binary);

    while (in) {

        // Get data from file or from internal block
        if (blockBytesLeft < BUF_LEN) {
            in.read(block, BLOCK_LEN);
            // Check everything is OK
            if (!in) { break; }
            buffer = block;
            blockBytesLeft = BLOCK_LEN;
        } else {
            buffer += BUF_LEN;
        }

        // Check if in data block
        if ((buffer[0] < 0x20) || (buffer[0] > 0x7f)) { break; }

        blockBytesLeft -= BUF_LEN;
        tbytes         += BUF_LEN;

        //std::cerr << "{{" << std::string(buffer, BUF_LEN) << "}}";

        // Get and cheeck tag
        std::string tag = std::string(buffer, TAG_LEN);
        if (tag == "END     ") {
            if (inHistory) { str += std::string(" ] ");  }
            str += "}";
            inHdr = false;
            inHistory = false;
        } else {
            // Get content
            std::string content = std::string(buffer + TAG_LEN, BUF_LEN - TAG_LEN);
            clearComment(content);
            if (content.at(0) == '=') {
                content.erase(0, 1);
            }
            // Clean-up content and tag
            clearQuotes(content);
            str::trim(tag, " \t\n\r\f\v");
            str::trim(content, " \t\n\r\f\v");
            if (tag == "XTENSION") {
                // Open section for extensions
                inHdr = true;
                str += std::string((numOfExtensions < 1) ? ", \"Extensions\": [ " : ", ") +
                    " { \"" + tag + "\": \"" + content + "\"";
                numOfExtensions++;
            } else {
                if (inHdr) {
                    if (tag == "HISTORY") {
                        str += std::string(!inHistory ? ", \"HISTORY\": [ " : std::string(", ")) +
                            "\"" + content + "\"";
                        inHistory = true;
                    } else {
                        if (inHistory) { str += std::string(" ], ");  }
                        inHistory = false;
                        str::replaceAll(content, " T ", " true ");
                        str::replaceAll(content, " F ", " false ");
                        if (!isStart) { str += std::string(", "); }
                        str += "\"" + tag + "\": \"" + content + "\"";
                    }
                }
            }
            //std::cerr << "  => '" << tag << "': '" << content << "'\n";
        }

        isStart = false;
    }
    in.close();

    str += (numOfExtensions > 0) ? "] }": "}";
}

//==========================================================================
// METHOD: clearComment
// Removes the comment section from the content string
//==========================================================================
void FitsMetadataReader::clearComment(std::string & line)
{
    size_t pos = line.rfind(" / ");
    if (pos != std::string::npos) {
        line.erase(pos,1000);
        str::trim(line, " \t\n\r\f\v");
    }
}

//==========================================================================
// METHOD: setFile
// Removes quotes from start and end of a string
//==========================================================================
void FitsMetadataReader::clearQuotes(std::string & line)
{
    size_t fpos = line.find_first_of("\"'");
    if (fpos != std::string::npos) line.erase(0, fpos + 1);
    size_t tpos = line.find_last_of("\"'");
    if (tpos != std::string::npos) line.erase(tpos);
    str::replaceAll(line, "\\", "/");
}
