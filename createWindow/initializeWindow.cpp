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
    glfwSetErrorCallback(glfw_error_callback); //Luodaan funktio, jota kutsutaan, kun tulee virhetilanne
    if (!glfwInit())
        return false; //Alustetaan GLFW kirjasto, ja jos epäonnistuu returnataan false

    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); //Haetaan OpenGL 3.0 ja asetetaan glsl shar versioon 130

    window = glfwCreateWindow(1280, 800, "Hotellin varausjärjestelmä", nullptr, nullptr);
    if (!window)
        return false; // Luodaan 1280x800px ikkuna titlellä "Hotellin varausjärjestelmä" ja returnataan false, jos se epäonnistuu

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //Kytketään päälle V-Sync, joka siis lukitsee ruudunpäivitysnopeuden samaksi, kuin virkistystaajuus

    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); //Tarkastetaan IMGUIN versio ja luodaan uusi ImGui konteksti

    ImGui_ImplGlfw_InitForOpenGL(window, true); //Alustetaan GLFW backend syöttöjä varten
    ImGui_ImplOpenGL3_Init(glsl_version); //Alustetaan OpenGL3 backend renderöintiä varten

    ImGui::StyleColorsDark(); //Asetetaan ikkunan tausta mustaksi

    return true; //Palautetaan true, joka indikoi onnistunutta ikkunan luontia
}

void WindowInitializer::mainLoop()
{
    while (!glfwWindowShouldClose(window)) //Ikkunaloop
    {
        int width, height; //Int arvo ikkunan leveydelle ja korkeudelle
        static int page = 0; //Int arvo, joka määrittää millä sivulla ollaan
        static int selectedRoomType = 0; //Apumuuttuja huoneen valinnalle
        bool validFirst = bookingData.firstName[0] != '\0'; //Tarkistus sille, että käyttäjä muistaa täyttää etunimen
        bool validLast = bookingData.lastName[0] != '\0'; //Tarkistus sille, että käyttäjä muistaa täyttää sukunimen
        bool validNights = bookingData.numNights != 0; //Tarkistus sille, että käyttäjä muistaa anaa öiden määrän

        glfwPollEvents(); //Alustetaan glfw polleventit, jotka hallitsevat kaikkia hiiren liikkeestä, ikkunan koon muuttamiseen asti
        glfwGetFramebufferSize(window, &width, &height); //Kutsutaan, jotta IMGUI ikkuna täyttää koko OpenGL:n luoman ikkunan

        ImGui_ImplOpenGL3_NewFrame(); //Luodaan uusi frame OpenGL:lle, eli valmiina renderöimään
        ImGui_ImplGlfw_NewFrame(); //Luodaan uusi frame GLFW:lle, eli vastaanottamaan syöttöjä
        ImGui::NewFrame(); //Kertoo imguille, että luodaan uusi frame
        ImGui::SetNextWindowPos(ImVec2(0, 0)); //Asetetaan imgui luomamme ikkunan vasempaan yläreunaan
        ImGui::SetNextWindowSize(ImVec2((float)width, (float)height)); //Määritetään ikkunan koko width ja height muuttujilla


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


        ImGui::Begin("Hotellin Varausjärjestelmä", nullptr, flags); //Aloitetaan ikkunan renderöinti

        if (ImGui::Button("Varaa huone")) page = 0; //Painike, joka asettaa välilehden arvoon 0
        ImGui::SameLine();
        if (ImGui::Button("Etsi huonenumero")) page = 1; //Painike, joka asettaa välilehden arvoon 1

        ImGui::Separator(); //Tämä luo viivan, joka erottelee eri elementtejä UI:ssa

        if (page == 0) {

        ImGui::Text("Huoneen varaus");
        ImGui::PushItemWidth(150); //Asetetaan input laatikon leveys 150px
        ImGui::InputTextWithHint( //Luodaan syöttölaatikko, jonka sisällä on "hint" eli vihje mitä laatikkoon tulee kirjoittaa
            "##Etunimi",
            "Etunimi",
            bookingData.firstName,
            IM_ARRAYSIZE(bookingData.firstName),
            ImGuiInputTextFlags_CallbackCharFilter,
            inputValidation::LettersOnly); //Kutsutaan luomaamme filtteriä, joka hyväksyy vain latinalaisia aakkosia

        ImGui::Dummy(ImVec2(100, 5)); //Dummy, joka luo käytännössä pienen välin luomiemme elementtien väliin.

        ImGui::PushItemWidth(150);
        ImGui::InputTextWithHint( //Täysin sama, kuin aikaisempi, mutta sukunimelle
            "##Sukunimi",
            "Sukunimi",
            bookingData.lastName,
            IM_ARRAYSIZE(bookingData.lastName),
            ImGuiInputTextFlags_CallbackCharFilter,
            inputValidation::LettersOnly);

        ImGui::Dummy(ImVec2(100, 5));

        ImGui::Text("Huoneen koko"); //Renderöidään ikkunaan teksti "Huoneen koko"
        ImGui::RadioButton("Yhden hengen huone", &selectedRoomType, 0); //Luodaan kaksi kappaletta monivalinta nappeja, jotka asettavat huonekoon
        ImGui::RadioButton("Kahden hengen huone", &selectedRoomType, 1);


        ImGui::Dummy(ImVec2(100, 5));

        ImGui::InputInt( //Luodaan syöttölaatikko int luvulle, jolla saadaa käyttäjältä yöpymisen pituus
        "Öiden määrä",
        &bookingData.numNights);
        if (bookingData.numNights < 0) { //Tarkistus sille, että käyttäjä ei anna negatiivista yömäärää
            bookingData.numNights = 0;
        }

        ImGui::Separator();

        static bool priceCalculated = false; //Apu muuttuja, jolla saadaan renderöityä
        static bool error = false; //Apu muuttuja, jolla tunnistetaan virhetilanne
        static float totalPrice;

        if (ImGui::Button("Tee varaus")) { //Nappi, joka kutsuu kaikki bookingDatan aliohjelmat
            if (validFirst && validLast && validNights) {
                bookingData.readRoomVector(); // Hakee tiedostosta huonedatan ja tallentaa vektoriin varausta varten
                bookingData.roomType = selectedRoomType; // Asettaa valitun huoneen
                bookingData.generateDiscounts(); // Generoi asiakkaalle annettavat alennukset
                bookingData.pickRoomNumber(); // Generoi asiakkaalle huonenumeron
                bookingData.createBooking(); // Luo varauksen ja tallentaa tiedostoon
                if (bookingData.roomType == 0) { // Laskee lopullisen hinnan huoneelle
                    totalPrice = bookingData.numNights*bookingData.priceOne;
                    priceCalculated = true;
                    error = false;
                }
                else
                    totalPrice = bookingData.numNights*bookingData.priceTwo;
                priceCalculated = true;
                error = false;
            }
            else { //Jos unohdit laittaa etu- tai sukunimen ja/tai yöpymisen pituuden tulee virhetilanne
                error = true;
            }

        }
        if (error) {
            ImGui::Dummy(ImVec2(100, 5));
            ImGui::Text("Virhe. Muista syöttää nimesi ja öiden määrä");                             //Virhetilanne, jossa käyttäjä unohtaa syöttää jotain
        }
        else if (bookingData.roomNumber == 1000) {
            ImGui::Dummy(ImVec2(100, 5));
            ImGui::Text("Valitsemaasi huonetyyppiä ei ole saatavilla");                             //Virhetilanne, jossa huoneita ei ole saatavilla.
        }
        else if (bookingData.bookingNumber == 1) {
            ImGui::Dummy(ImVec2(100, 5));
            ImGui::Text("Kriittinen sovellusvirhe. Varausnumeroita ei ole enää saatavilla");        //Virhetilanne, jossa varausnumerot ovat loppuneet,
        }                                                                                               //jonka ei pitäisi ikinä toteutua
        else if (priceCalculated) {
            ImGui::Dummy(ImVec2(100, 5));                                                       //Onnistuneesti luotu varaus,
            ImGui::Text("Kiitos varauksesta: %s %s", bookingData.firstName, bookingData.lastName);  //jonka tiedot renderöidään asiakkaalle
            ImGui::Text("Varausnumero: %d", bookingData.bookingNumber);
            ImGui::Text("Huone numeronne on: %d", bookingData.roomNumber);
            ImGui::Text("Yöpymisen hinta: %.2f €", totalPrice);
            }

        }

        if (page == 1) {

        static bool searchMade = false;     //Apumuuttuja, jolla tarkistetaan onko käyttäjä painanut haku nappia
        static char firstName2[64] = "";    //Char muuttuja, johon tallennetaan hakuehto etunimi
        static char lastName2[64] = "";     //Char muuttuja, johon tallennetaan hakuehto sukunimi
        static int room = -1;
        static char buffer[16] = "";        //Muuttuja johon tallennetaan huonenumero
        int bookingNum = 0;                 //Muuttuja, jota käytetään varausnumeron syöteen saamiseen

        ImGui::Text("Huoneen etsintä"); //Otsikko välilehdelle

        ImGui::Dummy(ImVec2(50, 5));
        ImGui::Text("Nimellä:");

        ImGui::PushItemWidth(150);
        ImGui::InputTextWithHint( //Tekstikenttä etuminelle hakua varten
              "##Etunimi",
              "Etunimi",
              firstName2,
              IM_ARRAYSIZE(firstName2), //Laskee char muuttujan koon, eli nimen pituuden
              ImGuiInputTextFlags_CallbackCharFilter, //Filtteri syötetylle tekstille
              inputValidation::LettersOnly); //Meidän custom filtteri tekstille

        ImGui::Dummy(ImVec2(25, 2.5));

        ImGui::PushItemWidth(150);
        ImGui::InputTextWithHint( //Samanlainen, mutta sukunimeä varten
                "##Sukunimi",
                "Sukunimi",
                lastName2,
                IM_ARRAYSIZE(lastName2),
                ImGuiInputTextFlags_CallbackCharFilter,
                inputValidation::LettersOnly);

        ImGui::Dummy(ImVec2(50, 5));
        ImGui::Text("Tai varausnumerolla:");

        ImGui::InputTextWithHint( //Kokeilin tehdä vielä int arvolle teksti boksin muutelemalla sitä charrista jne. InputInt funktion sijaan
        "##Varausnumero",
        "Varausnumero",
        buffer,
        IM_ARRAYSIZE(buffer),
        ImGuiInputTextFlags_CallbackCharFilter,
        inputValidation::NumbersOnly);

        if (buffer[0] != '\0') { //Asetetaan bookingnum muuttujan arvoksi bufferin arvo, jonka muokkaamme std::stoi funktiolla
            try {
                bookingNum = std::stoi(buffer);
            }catch (...){
                std::cerr << "Virhe" << std::endl;
            }
        }



        ImGui::Separator();
        if (ImGui::Button("Etsi huone")) {
               searchMade = true;
               room = bookingData.searchRoom(firstName2, lastName2, bookingNum); //Kutsutaan hakufunktiota toisesta luokasta
        }

        if (searchMade) { //Renderöidään ruudulle informaatiot, jotka saamme.
           ImGui::Dummy(ImVec2(100, 5));
           if (room != -1)
               ImGui::Text("Huone numeronne on: %d" , room);
           else if (firstName2[0] != '\0' || lastName2[0] != '\0') //Tarkistetaan onko tulos tyhjä
               ImGui::Text("Nimeä ei löytynyt.");
            }

        }

        ImGui::End(); //Lopettaa imgui ikkuna instanssin

        ImGui::Render(); //Valmistelee imgui ikkunan ja antaa datat openGL:lle
        int w, h;
        glfwGetFramebufferSize(window, &w, &h); //Noutaa ikkunan leveyden ja korkeuden
        glViewport(0, 0, w, h); //Kertoo openGL:lle, että piirrosalue kattaa koko ikkunan
        glClear(GL_COLOR_BUFFER_BIT); //Tyhjää ruudun uutta framea varten.

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); //Hakee kaikki komennot mitä käytettiin kuluneen framen aikana
        glfwSwapBuffers(window);                                //ja lähettää ne OpenGL:lle
    }
}

void WindowInitializer::cleanup() //Tämä sulkee ikkunan turvallisesti
{
    if (!window) return;

    ImGui_ImplOpenGL3_Shutdown(); //Siivoaa OpenGl Imgui backendin
    ImGui_ImplGlfw_Shutdown(); //Siivoaa GLFW Imgui backendin
    ImGui::DestroyContext(); //Poistaa Imgui kontekstin

    glfwDestroyWindow(window); //Poistaa GLFW ikkunan
    glfwTerminate(); //Siivoa GLFW:n täysin
}