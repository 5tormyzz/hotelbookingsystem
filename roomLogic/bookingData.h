//
// Created by susib on 22.11.2025.
//

#ifndef BOOKINGDATA_H
#define BOOKINGDATA_H
#include <string>
#include <vector>
#include <json.hpp>


//Tämä luokka hallitsee kaikkia huoneisiin liittyviä muuttujia.
class bookingData {
public:
    std::vector<int> roomNumberOne;
    std::vector<int> roomNumberTwo;
    int roomType;
    int numNights;
    int numberOfOneRooms;
    int numberOfTwoRooms;
    float priceOne;
    float priceTwo;
    float discount;
    char firstName [128];
    char lastName [128];
    int bookingNumber;
    int roomNumber;

    void generateNumofRooms();
    void generateDiscounts();
    void createRoomsVector();
    void generateBookingNumber();
    void pickRoomNumber();
    void createBooking();
    void readRoomVector();
    int searchRoom(char* firstName, char* lastName, int bookingNum);
};


#endif //BOOKINGDATA_H
