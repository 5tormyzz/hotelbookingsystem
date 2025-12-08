#include "bookingData.h"
#include <random>
#include <iostream>
#include <fstream>
#include <json.hpp>

std::random_device rd;
std::mt19937 gen(rd());


//Tämä aliohjelma luo satunnaisen määrän yhden- ja kahdenhengen huoneita.
void bookingData::generateNumofRooms() {

    std::uniform_int_distribution<int> dist(20, 150); //Luodaan random numero 20 ja 150 väliltä
    numberOfOneRooms = dist(gen);
    numberOfTwoRooms = numberOfOneRooms;

}

//Tämä aliohjelma valitsee kuinka suuren alennuksen asiakas saa.
void bookingData::generateDiscounts() {
    priceOne = 100.00;
    priceTwo = 150.00;
    const std::vector discounts = {0.8, 0.9, 1.0}; //Generoidaan 20% 10% tai 0% alennus desimaalilukuna
    std::uniform_int_distribution<> dist(0, discounts.size() - 1);


    discount = discounts[dist(gen)];
    priceOne *= discount;
    priceTwo *= discount;

}

//Tämä aliohjelma luo vektorin, joka sisältää kaikki huonenumerot.
void bookingData::createRoomsVector() {


    for (int i = 1; i < numberOfOneRooms+1; i++) { //Luodaan yhden hengen huoneet sisältävä vektori
        roomNumberOne.push_back(i);
    }

    for (int i = roomNumberOne.back()+1; i < numberOfTwoRooms*2+1; i++) { //Luodaan kahden hengen huoneet sisältävä vektori
        roomNumberTwo.push_back(i);
    }

    std::ofstream roomsFile("../fileSystem/roomsfile.json"); //Luodaan tiedosto, jonka sisälle tallennamme vektorien sisällöt
    roomsFile << "[\n"; //lisätään json arrayn alkuun auki sulku "["

    bool first = true; //tarkistetaan onko json tiedostoon lisättävä arvo "jonon" ensimmäinen.

    for (int num : roomNumberOne) {

        if (!first)
            roomsFile << ",\n"; //Tätä hyödynnetään lisäämään pilkku loppuun aina, jos lisättävä huone ei ole ensimmäinen listassa

        first = false;

        //Tässä on vain perus logiikkaa sille, miten json tiedosto tulee järjestellä
        roomsFile << "  {\n";
        roomsFile << "    \"roomnumber\": " << num << "\n";
        roomsFile << "  }";
    }

    //Tämä looppi toimi aivan samalla tavalla, kuin ensimmäinenkin.
    for (int num2 : roomNumberTwo) {

        if (!first)
            roomsFile << ",\n";

        first = false;

        roomsFile << "  {\n";
        roomsFile << "    \"roomnumber\": " << num2 << "\n";
        roomsFile << "  }";
    }

    roomsFile << "\n]\n"; //loopin päätyttyä lisäämme arrayn loppuun kiinni sulun "]"
}


void bookingData::readRoomVector() { //Tilanteessa, jossa huonenumerot sisältävä tiedosto on jo olemassa luetaan sen sisältä huonenumerot vektoreihin

    roomNumberOne.clear(); //Tyhjennetään varmuuden vuoksi vektorit
    roomNumberTwo.clear();

    std::ifstream bookingFile("../fileSystem/roomsfile.json"); //Avataan tiedosto lukutilassa
    if (!bookingFile.is_open()) {
        std::cerr << "Tiedoston avaaminen epäonnistui" << std::endl;
    }

    nlohmann::json j; //Luodaan nlohmannin kirjastolla json muuttuja, joka auttaa json tiedostojen parseemista
    bookingFile >> j;

    size_t totalRooms = j.size(); //käytetään size_t integer tyyppiä, jotta voidaan varmistaa, että arvo ei koskaan ole negatiivinen.
    size_t half = totalRooms / 2;

    for (size_t i = 0; i < totalRooms; i++) {          //Tämä looppi menee json muuttujaan tallennettua tiedostoa läpi ja etsii kirjainsarjaa "roomnumber"
        int roomNum = j[i]["roomnumber"].get<int>();   //kun se löytää kirjainsarjan se ottaa sen perään merkityn int luvun, jonka se laittaa
        if (i < half) {
            roomNumberOne.push_back(roomNum);
        } else {
            roomNumberTwo.push_back(roomNum);
        }
    }
}

void bookingData::pickRoomNumber() {

    std::vector<int> bookedRooms;
    std::vector<int> bookingNums;

    std::ifstream bookingFile("../fileSystem/bookingfile.json"); //Luetaan varatut huoneet vektorista

    nlohmann::json j;

    if (bookingFile.is_open())
    {
        bookingFile >> j;
        bookingFile.close();

        for (const auto& entry : j)
        {
            if (entry.contains("roomNumber"))
                bookedRooms.push_back(entry["roomNumber"].get<int>());

            if (entry.contains("bookingNumber"))
                bookingNums.push_back(entry["bookingNumber"].get<int>());
        }
    }


    std::vector<int>& availableRooms = (roomType == 0) ? roomNumberOne : roomNumberTwo; // if-else tyylinen ratkaisu, joka tarkistaa, onko huoneita jäljellä.
    if (availableRooms.empty()) {
        roomNumber = 0; //jos huoneita ei löydy asetamme huonenuomeron oletuksena nollaan (jota ei ole olemassa)
        return;
    }


    std::uniform_int_distribution<> dist(0, availableRooms.size() - 1);
    std::uniform_int_distribution dist2(10000, 99999);
    int candidateNumber = dist2(gen);

    while (true) {
        int index = dist(gen); //tallennetaan indeksiin randomisti valittu huone
        int candidateRoom = availableRooms[index]; //haetaan huonenumero kyseisestä indeksistä

        if (std::find(bookedRooms.begin(), bookedRooms.end(), candidateRoom) == bookedRooms.end()) {
            roomNumber = candidateRoom; //Etsitään onko huone varattu, ja jos ei, niin annetaan se asiakkaalle

            if (std::find(bookingNums.begin(), bookingNums.end(), candidateNumber) == bookingNums.end()) {
                bookingNumber = candidateNumber; //Tehdään varausnumeron tarkistus myös.
            }
            break;
        }

        if (bookedRooms.size() >= availableRooms.size()) {
            roomNumber = 1000; //Jos huoneita ei ole saatavilla asetetaan huonenumero nollaan ja katkaistaan looppi
            break;
        }
        if (std::find(bookingNums.begin(), bookingNums.end(), candidateNumber) != bookingNums.end()) {
            bookingNumber = 1;
        }                       //Jos varausnumerot ovat jostain syystä loppuneet asetetaan se yhteen ja katkaistaan looppi
        break;
    }
}


void bookingData::createBooking() {
    nlohmann::json j;

    std::ifstream inputFile("../fileSystem/bookingfile.json");
    if (inputFile.is_open()) { //Pieni testauslogiikka, joka testaa nlohmannin parselogiikkaa
        try {
            inputFile >> j;
        } catch (nlohmann::json::parse_error&) {
            j = nlohmann::json::array();
        }
        inputFile.close();
    } else {
        j = nlohmann::json::array(); // luodaan array json filuun
    }

    //Tallennetaan bookingEntry muuttujaan kaikki tiedot
    nlohmann::json bookingEntry;
    bookingEntry["firstName"] = firstName;
    bookingEntry["lastName"] = lastName;
    bookingEntry["bookingNumber"] = bookingNumber;
    bookingEntry["roomNumber"] = roomNumber;
    bookingEntry["numNights"] = numNights;


    j.push_back(bookingEntry); //Lisätään kaikki yläpuolella olevat bookingEntryt luodun arrayn sisään


    std::ofstream outputFile("../fileSystem/bookingfile.json"); //Avataan bookingfile
    if (!outputFile.is_open()) {
        std::cerr << "bookingfile.json tiedoston avaaminen epäonnistui\n";
        return;
    }

    outputFile << j.dump(4); //dumpataan kaikki luotu data json tiedoston sisään.
    outputFile.close();
}


int bookingData::searchRoom(char* firstName, char* lastName, int bookingNum) {

    std::ifstream inputFile("../fileSystem/bookingfile.json"); //Avataan lukutilassa varaukset
    if (!inputFile.is_open()) {
        std::cerr << "Tiedoston avaaminen epäonnistui" << std::endl;
        return -1;
    }

    nlohmann::json j;
    inputFile >> j;

    for (const auto& entry : j) {

        const std::string& entryFirst = entry.value("firstName", ""); //Etsitään ja tallennetaan firstName merkkijonon jälkeen oleva stringi
        const std::string& entryLast = entry.value("lastName", ""); //Etsitään ja tallennetaan lastName merkkijonon jälkeen oleva stringi
        const int entryBookingNumber = entry.value("bookingNumber", -1); ////Etsitään ja tallennetaan bookingNumber merkkijonon jälkeen oleva int

        bool matchNum = (bookingNum != -1 && bookingNum == entryBookingNumber); //Tarkastetaan, että varausnumero ei ole negatiivinen ja, että se löytyy tiedostosta
        bool matchName = firstName && lastName && std::strlen(firstName) > 0 && std::strlen(lastName) > 0 && std::strcmp(entryFirst.c_str(), firstName) == 0 && std::strcmp(entryLast.c_str(), lastName) == 0;
        //Tarkastetaan, että etu- ja sukunimi löytyvät ja eivät ole 0 kirjainta pitkiä ja että ne löytyvät

        if (matchNum || matchName) { //Jos käyttäjä antaa jommankumman palautetaan huonenumero, muutoin palautetaan -1.
            return entry.value("roomNumber", -1);
        }

    }

    return -1;
}
