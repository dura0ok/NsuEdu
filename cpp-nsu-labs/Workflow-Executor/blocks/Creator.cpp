#include <map>
#include <memory>
#include "Creator.h"
#include "creators/ReadFileCreator.h"
#include "creators/ReplaceBlockCreator.h"
#include "creators/GrepBlockCreator.h"
#include "creators/WriteFileBlockCreator.h"


std::unique_ptr<Creator> Creator::mapper(const std::string& command_name) {
    std::map<std::string, std::unique_ptr<Creator>> creators;
    creators["readfile"] = std::make_unique<ReadFileCreator>();
    creators["replace"] = std::make_unique<ReplaceBlockCreator>();
    creators["grep"] = std::make_unique<GrepBlockCreator>();
    creators["writefile"] = std::make_unique<WriteFileBlockCreator>();

    return std::move(creators[command_name]);
}
