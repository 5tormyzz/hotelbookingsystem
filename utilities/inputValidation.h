//
// Created by susib on 22.11.2025.
//

#ifndef INPUTVALIDATION_H
#define INPUTVALIDATION_H


#include <imgui.h>
#include <string>

class inputValidation {
public:
    static int LettersOnly(ImGuiInputTextCallbackData* data);
    static int NumbersOnly(ImGuiInputTextCallbackData* data);
};



#endif //INPUTVALIDATION_H
