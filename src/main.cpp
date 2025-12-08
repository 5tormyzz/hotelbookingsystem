#include "initializeWindow.h"
#include "bookingData.h"
#include <iostream>
#include <filesystem>




int main() {

    bookingData bookingData;
    WindowInitializer window;
    std::string path = "../fileSystem/roomsfile.json";

    if (std::filesystem::exists(path)) {
        std::cout << "CONSOLE: Huone tiedosto on jo olemassa, skipataan luonti." << std::endl;
    } else {
        bookingData.generateNumofRooms();
        bookingData.createRoomsVector();
        std::cout << "CONSOLE: Huone tiedostoa ei löytynyt, luodaan uusi." << std::endl;
    }

    if (!window.init())
        return 1;

    window.mainLoop();
    return 0;


}