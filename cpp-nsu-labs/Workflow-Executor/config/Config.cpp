#include <iostream>
#include <cassert>
#include "Config.h"
#include "../exceptions/BorderException.h"
#include "../helpers/LineHelper.h"
#include "../helpers/StringHelper.h"
#include "../blocks/Creator.h"
#include "../helpers/FileHelper.h"


Config::Config(const std::string &name) {
    this->name = name;
    this->file.open(name);
    if (!this->file.is_open()) throw std::runtime_error("Could not open file");
    //this->file.exceptions(std::ifstream::failbit);
}

std::vector<std::unique_ptr<Block>> Config::ParseConfig() {
    auto startLine = FindLineWithContent("desc");
    auto endLine = FindLineWithContent("csed");

    if (!startLine.first || !endLine.first) {
        throw BorderException();
    }

    auto blocks = this->GetRawBlocksInfo(startLine.second + 1, endLine.second);
    std::vector<std::unique_ptr<Block>> blockObjects;
    std::regex re(R"(^(\d+)\s*([-+=*\/])\s*(\S+)\s+(.+)$)");
    for (auto &block: blocks) {
        auto res = LineHelper::SplitStringByTokens(trim(block), re);
        // if(res.size() >= 1) throw "asd";
        auto t = LineHelper::ParseLineToBlock(res);
        blockObjects.push_back(std::move(t));
    }

    FileHelper::ClearProgress(this->file);
    return blockObjects;
}

std::pair<bool, size_t> Config::FindLineWithContent(const std::string& search) {
    std::string line;
    unsigned int curLine = 0;
    while(getline(this->file, line)) {
        curLine++;
        if (line.find(search, 0) != std::string::npos) {
            FileHelper::ClearProgress(this->file);
            return std::make_pair(true, curLine);
        }
        if(this->file.eof()) break;
    }


    FileHelper::ClearProgress(this->file);
    return std::make_pair(false, curLine);
}


std::vector<std::string> Config::GetRawBlocksInfo(size_t from, size_t to) {
    std::vector<std::string> data;
    data.reserve(to-from);
    unsigned int curLine = 0;
    std::string line;
    while (getline(this->file, line)) {
        curLine++;
        if (curLine < from || curLine >= to) continue;
        data.push_back(line);
    }

    return data;
}

std::vector<std::string> Config::ParseConveyor() {
    auto conveyorLineNumber = FindLineWithContent("csed").second + 1;
    auto conveyorLine = FileHelper::ReadNthLine(this->file, conveyorLineNumber);
    std::regex re("(\\d)");
    return LineHelper::SplitStringBySubMatches(trim(conveyorLine), re);
}



