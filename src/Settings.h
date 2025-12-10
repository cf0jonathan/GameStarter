#pragma once
#include <string>

struct Settings {
    int width = 1280;
    int height = 720;
    int renderFPS = 60;
    int logicFPS = 60;
    int explosionVolume = 80;   // 0-128 (62% of max)
    int rocketVolume = 40;      // 0-128 (31% of max)
    // Future fields:
    // int physicsSubsteps = 1;
    // float timeScale = 1.0f;
};

// Loads settings from the given XML path. Returns true if loaded, false if fallback used.
bool loadSettingsFromXML(const std::string& path, Settings& out);
