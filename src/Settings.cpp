#include "Settings.h"
#include <tinyxml2.h>
#include <iostream>

bool loadSettingsFromXML(const std::string& path, Settings& out) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Settings: could not load XML '" << path << "', using defaults." << std::endl;
        return false;
    }
    auto* root = doc.FirstChildElement("assets");
    if (!root) {
        std::cerr << "Settings: missing <assets> root, using defaults." << std::endl;
        return false;
    }
    auto* settingsElem = root->FirstChildElement("settings");
    if (!settingsElem) {
        std::cerr << "Settings: no <settings> section, using defaults." << std::endl;
        return false;
    }
    if (auto* display = settingsElem->FirstChildElement("display")) {
        display->QueryIntAttribute("width", &out.width);
        display->QueryIntAttribute("height", &out.height);
    }
    if (auto* timing = settingsElem->FirstChildElement("timing")) {
        timing->QueryIntAttribute("renderFPS", &out.renderFPS);
        timing->QueryIntAttribute("logicFPS", &out.logicFPS);
    }
    // Basic sanity clamps
    if (out.width < 320){ 
        out.width = 320;
        std::cerr << "Settings: width too small, clamped to 320." << std::endl;
    }
    if (out.height < 240){ 
        out.height = 240;
        std::cerr << "Settings: height too small, clamped to 240." << std::endl;
    }
    if (out.renderFPS < 10){ 
        out.renderFPS = 10;
        std::cerr << "Settings: renderFPS too low, clamped to 10." << std::endl;
    }
    // if (out.renderFPS > 240){ 
    //     out.renderFPS = 240;
    //     std::cerr << "Settings: renderFPS too high, clamped to 240." << std::endl;
    // }
    if (out.logicFPS < 15){ 
        out.logicFPS = 15;
        std::cerr << "Settings: logicFPS too low, clamped to 15." << std::endl;
    }
    // if (out.logicFPS > 240){ 
    //     out.logicFPS = 240;
    //     std::cerr << "Settings: logicFPS too high, clamped to 240." << std::endl;
    // }
    return true;
}
