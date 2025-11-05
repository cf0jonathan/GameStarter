#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

// Forward declarations
class Object;
namespace tinyxml2 {
    class XMLElement;
}

// Registry pattern: maps type strings to factory functions
struct Library {
    std::unordered_map<
        std::string,
        std::function<std::unique_ptr<Object>(const tinyxml2::XMLElement*)>
    > map;
    
    Library();
    
    std::unique_ptr<Object> create(const std::string& type, const tinyxml2::XMLElement* xmlElement);
    bool hasType(const std::string& type) const;
};