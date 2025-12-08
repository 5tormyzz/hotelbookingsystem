//
// Created by susib on 22.11.2025.
//

#ifndef INITIALIZEWINDOW_H
#define INITIALIZEWINDOW_H



#pragma once
#include <glfw3.h>
#include "imgui.h"

class WindowInitializer {
public:
    WindowInitializer();
    ~WindowInitializer();

    bool init();              // Initializes GLFW + OpenGL + ImGui
    void mainLoop();          // Runs the ImGui render loop
    void cleanup();           // Cleans everything up

private:
    GLFWwindow* window = nullptr;
    const char* glsl_version = nullptr;
};



#endif //INITIALIZEWINDOW_H
