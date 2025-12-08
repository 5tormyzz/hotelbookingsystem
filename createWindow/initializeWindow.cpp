#include "initializeWindow.h"

#include <iostream>
#include <ostream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "bookingData.h"
#include "inputValidation.h"

#include <stdio.h>


//TÄMÄ KOODI ON MUOKATTU VALMIISTA POHJASTA, JOKA LÖYTYY OSOITTEESTA:
//https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp

bookingData bookingData;
inputValidation inputValidation;



static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW virhe %d: %s\n", error, description); //Palauttaa GLFW:n virhekoodin ja kuvauksen
}

WindowInitializer::WindowInitializer() {}

WindowInitializer::~WindowInitializer()
{
    cleanup(); //Hallitsee luonnin
}

bool WindowInitializer::init()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;

    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1280, 800, "Hotellin varausjärjestelmä", nullptr, nullptr);
    if (!window)
        return false;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::StyleColorsDark();

    return true;
}

void WindowInitializer::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        static int page = 0;
        static int selectedRoomType = 0;
        bool validFirst = bookingData.firstName[0] != '\0';
        bool validLast = bookingData.lastName[0] != '\0';
        bool validNights = bookingData.numNights != 0;

        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));


        //Tässä annamme ikkunalle eri flagejä, jotka mm. estävät menun
        //liikuttamisen ikkunassa ja muuttavat kokoa aina siten, että se täyttää koko ikkunan.
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoBringToFrontOnFocus |
                                ImGuiWindowFlags_NoNav |
                                    ImGuiWindowFlags_NoScrollbar;


        ImGui::Begin("Hotellin Varausjärjestelmä", nullptr, flags);

        if (ImGui::Button("Varaa huone")) page = 0;
        ImGui::SameLine();
        if (ImGui::Button("Etsi huonenumero")) page = 1;
        ImGui::SameLine();

        ImGui::Separator();

        if (page == 0) {

        ImGui::Text("Huoneen varaus");
        ImGui::PushItemWidth(150);
        ImGui::InputTextWithHint(
            "##Etunimi",
            "Etunimi",
            bookingData.firstName,
            IM_ARRAYSIZE(bookingData.firstName),
            ImGuiInputTextFlags_CallbackCharFilter,
            inputValidation::LettersOnly);

        ImGui::Dummy(ImVec2(100, 5));

        ImGui::PushItemWidth(150);
        ImGui::InputTextWithHint(
            "##Sukunimi",
            "Sukunimi",
            bookingData.lastName,
            IM_ARRAYSIZE(bookingData.lastName),
            ImGuiInputTextFlags_CallbackCharFilter,
            inputValidation::LettersOnly);

        ImGui::Dummy(ImVec2(100, 5));

        ImGui::Text("Huoneen koko");
        ImGui::RadioButton("Yhden hengen huone", &selectedRoomType, 0);
        ImGui::RadioButton("Kahden hengen huone", &selectedRoomType, 1);


        ImGui::Dummy(ImVec2(100, 5));

        ImGui::InputInt(
        "Öiden määrä",
        &bookingData.numNights);
        if (bookingData.numNights < 0) {
            bookingData.numNights = 0;
        }

        ImGui::Separator();

        static bool priceCalculated = false;
        static bool error = false;
        static float totalPrice;

        if (ImGui::Button("Tee varaus")) {
            if (validFirst && validLast && validNights) {
                bookingData.readRoomVector();
                bookingData.roomType = selectedRoomType;
                bookingData.generateDiscounts();
                bookingData.pickRoomNumber();
                bookingData.createBooking();
                bookingData.vacancy = false;
                if (bookingData.roomType == 0) {
                    totalPrice = bookingData.numNights*bookingData.priceOne;
                    priceCalculated = true;
                    error = false;
                }
                else
                    totalPrice = bookingData.numNights*bookingData.priceTwo;
                priceCalculated = true;
                error = false;
            }
            else {
                error = true;
            }

        }
        if (error) {
            ImGui::Dummy(ImVec2(100, 5));
            ImGui::Text("Virhe. Muista syöttää nimesi ja öiden määrä");
        }
        else if (bookingData.roomNumber == 1000) {
            ImGui::Dummy(ImVec2(100, 5));
            ImGui::Text("Valitsemaasi huonetyyppiä ei ole saatavilla");                     //Virhetilanne, jossa huoneita ei ole saatavilla.
        }
        else if (bookingData.bookingNumber == 1) {
            ImGui::Dummy(ImVec2(100, 5));
            ImGui::Text("Kriittinen sovellusvirhe. Varausnumeroita ei ole enää saatavilla"); //Virhetilanne, jossa varausnumerot ovat loppuneet,
        }                                                                                        //jonka ei pitäisi ikinä toteutua
        else if (priceCalculated) {
            ImGui::Dummy(ImVec2(100, 5));
            ImGui::Text("Kiitos varauksesta: %s %s", bookingData.firstName, bookingData.lastName);
            ImGui::Text("Varausnumero: %d", bookingData.bookingNumber);
            ImGui::Text("Huone numeronne on: %d", bookingData.roomNumber);
            ImGui::Text("Yöpymisen hinta: %.2f €", totalPrice);
            }

        }

        if (page == 1) {

        static bool searchMade = false;
        static char firstName2[64] = "";
        static char lastName2[64] = "";
        static int room = -1;
        static char buffer[16] = "";
        int bookingNum = 0;

        ImGui::Text("Huoneen etsintä");

        ImGui::Dummy(ImVec2(50, 5));
        ImGui::Text("Nimellä:");

        ImGui::PushItemWidth(150);
        ImGui::InputTextWithHint(
              "##Etunimi",
              "Etunimi",
              firstName2,
              IM_ARRAYSIZE(firstName2),
              ImGuiInputTextFlags_CallbackCharFilter,
              inputValidation::LettersOnly);

        ImGui::Dummy(ImVec2(25, 2.5));

        ImGui::PushItemWidth(150);
        ImGui::InputTextWithHint(
                "##Sukunimi",
                "Sukunimi",
                lastName2,
                IM_ARRAYSIZE(lastName2),
                ImGuiInputTextFlags_CallbackCharFilter,
                inputValidation::LettersOnly);

        ImGui::Dummy(ImVec2(50, 5));
        ImGui::Text("Tai varausnumerolla:");

        ImGui::InputTextWithHint(
        "##Varausnumero",
        "Varausnumero",
        buffer,
        IM_ARRAYSIZE(buffer),
        ImGuiInputTextFlags_CallbackCharFilter,
        inputValidation::NumbersOnly);

        if (buffer[0] != '\0') {
            try {
                bookingNum = std::stoi(buffer);
            }catch (...){
                std::cerr << "Virhe" << std::endl;
            }
        }



        ImGui::Separator();
        if (ImGui::Button("Etsi huone")) {
               searchMade = true;
               room = bookingData.searchRoom(firstName2, lastName2, bookingNum);
        }

        if (searchMade) {
           ImGui::Dummy(ImVec2(100, 5));
           if (room != -1)
               ImGui::Text("Huone numeronne on: %d" , room);
           else if (firstName2[0] != '\0' || lastName2[0] != '\0')
               ImGui::Text("Nimeä ei löytynyt.");
            }

        }

        ImGui::End();

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void WindowInitializer::cleanup()
{
    if (!window) return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}