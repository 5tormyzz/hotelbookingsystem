#include "inputValidation.h"
#include <cctype>

int inputValidation::LettersOnly(ImGuiInputTextCallbackData* data)
{
    char c = data->EventChar;
    if (!std::isalpha(c) && c != ' ' && c != '-')
        return 1; // Hylkää kirjain
    return 0; // Hyväksy kirjain
}

int inputValidation::NumbersOnly(ImGuiInputTextCallbackData* data) {
    char c = data->EventChar;
    if (!std::isdigit(c) && c != ' ' && c != '-')
        return 1;
    return 0;
}