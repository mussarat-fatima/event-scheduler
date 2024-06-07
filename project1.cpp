#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

class Event {
private:
    string name;
    string organizer;
    string category;
    string date;
    string startTime;
    string endTime;
    int seats;
    string venue;

public:
    Event() = default;
    Event(string n, string o, string c, string d, string st, string et, int s, string v)
        : name(n), organizer(o), category(c), date(d), startTime(st), endTime(et), seats(s), venue(v) {}

    string getName() const { return name; }
    string getDate() const { return date; }
    string getStartTime() const { return startTime; }
    string getEndTime() const { return endTime; }
    string getVenue() const { return venue; }
    int getSeats() const { return seats; }

    void save() const {
        ofstream file("events.txt", ios::app);
        if (file.is_open()) {
            file << name << "," << organizer << "," << category << "," 
                 << date << "," << startTime << "," << endTime << "," 
                 << seats << "," << venue << endl;
            file.close();
        } else {
            cerr << "Unable to open file";
        }
    }

    static vector<Event> loadAll() {
        vector<Event> events;
        ifstream file("events.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos = 0;
                string n, o, c, d, st, et, v;
                int s;
                
                pos = line.find(",");
                n = line.substr(0, pos); line.erase(0, pos + 1);

                pos = line.find(",");
                o = line.substr(0, pos); line.erase(0, pos + 1);

                pos = line.find(",");
                c = line.substr(0, pos); line.erase(0, pos + 1);

                pos = line.find(",");
                d = line.substr(0, pos); line.erase(0, pos + 1);

                pos = line.find(",");
                st = line.substr(0, pos); line.erase(0, pos + 1);

                pos = line.find(",");
                et = line.substr(0, pos); line.erase(0, pos + 1);

                pos = line.find(",");
                s = stoi(line.substr(0, pos)); line.erase(0, pos + 1);

                v = line;
                
                events.emplace_back(n, o, c, d, st, et, s, v);
            }
            file.close();
        } else {
            cerr << "Unable to open file";
        }
        return events;
    }

    void display() const {
        cout << "Event Name: " << name << "\nOrganizer: " << organizer << "\nCategory: " << category 
             << "\nDate: " << date << "\nTime: " << startTime << " - " << endTime 
             << "\nVenue: " << venue << "\nSeats: " << seats << endl;
        cout << "---------------------------------------" << endl;
    }
};

class Participant {
private:
    string name;
    string rollNumber;
    string department;
    string phoneNumber;
    string eventName;

public:
    Participant() = default;
    Participant(string n, string r, string d, string p, string e)
        : name(n), rollNumber(r), department(d), phoneNumber(p), eventName(e) {}

    void save() const {
        ofstream file("participants.txt", ios::app);
        if (file.is_open()) {
            file << name << "," << rollNumber << "," 
                 << department << "," << phoneNumber << "," 
                 << eventName << endl;
            file.close();
        } else {
            cerr << "Unable to open file";
        }
    }

    static map<string, int> countByEvent() {
        map<string, int> participantCount;
        ifstream file("participants.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos = line.rfind(",");
                string eventName = line.substr(pos + 1);
                participantCount[eventName]++;
            }
            file.close();
        } else {
            cerr << "Unable to open file";
        }
        return participantCount;
    }
};

class EventManagementSystem {
public:
    void run() {
        while (true) {
            int role;
            cout << "Are you an organizer or a participant?\n1. Organizer\n2. Participant\nEnter your choice: ";
            cin >> role;

            if (role == 1) {
                organizerMenu();
            } else if (role == 2) {
                participantMenu();
            } else {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }

private:
    void validateDate(const string& date) {
        // Regular expression to validate date in YYYY-MM-DD format
        regex datePattern(R"(\d{4}-\d{2}-\d{2})");

        if (!regex_match(date, datePattern)) {
            throw invalid_argument("Invalid date format. Please enter the date in YYYY-MM-DD format.");
        }

        // Extracting year, month, and day from the date string
        int year = stoi(date.substr(0, 4));
        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));

        // Checking for valid year range
        if (year < 1900 || year > 2100) {
            throw invalid_argument("Invalid year. Year must be between 1900 and 2100.");
        }

        // Checking for valid month range
        if (month < 1 || month > 12) {
            throw invalid_argument("Invalid month. Month must be between 01 and 12.");
        }

        // Checking for valid day range depending on the month
        switch (month) {
            case 1: // January
            case 3: // March
            case 5: // May
            case 7: // July
            case 8: // August
            case 10: // October
            case 12: // December
                if (day < 1 || day > 31) {
                    throw invalid_argument("Invalid day. Day must be between 01 and 31.");
                }
                break;
            case 4: // April
            case 6: // June
            case 9: // September
            case 11: // November
                if (day < 1 || day > 30) {
                    throw invalid_argument("Invalid day. Day must be between 01 and 30.");
                }
                break;
            case 2: // February
                // Leap year check for February
                if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                    if (day < 1 || day > 29) {
                        throw invalid_argument("Invalid day. Day must be between 01 and 29 in a leap year.");
                    }
                } else {
                    if (day < 1 || day > 28) {
                        throw invalid_argument("Invalid day. Day must be between 01 and 28.");
                    }
                }
                break;
            default:
                throw invalid_argument("Invalid month.");
        }
    }

    void organizerMenu() {
        string name, organizer, category, date, startTime, endTime, venue;
        int seats;

        cout << "Enter event name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter event organizer: ";
        getline(cin, organizer);
        cout << "Enter event category: ";
        getline(cin, category);
        
        cout << "Enter event date (YYYY-MM-DD): ";
        while (true) {
            try {
                getline(cin, date);
                validateDate(date);
                break;
            } catch (const invalid_argument& e) {
                cout << e.what() << endl;
                cout << "Enter event date (YYYY-MM-DD): ";
            }
        }
        
        while (true) {
            try {
                cout << "Enter event start time (HH:MM AM/PM or HH:MM): ";
                getline(cin, startTime);
                validateTime(startTime);
                cout << "Enter event end time (HH:MM AM/PM or HH:MM): ";
                getline(cin, endTime);
                validateTime(endTime);
                break;
            } catch (const invalid_argument& e) {
                cout << e.what() << endl;
            }
        }

        cout << "Enter number of seats: ";
        cin >> seats;

        try {
            venue = allocateVenue(date, startTime, endTime, seats);
            cout << "Venue allocated: " << venue << endl;
        } catch (const invalid_argument& e) {
            cout << e.what() << endl;
            return;
        }

        Event event(name, organizer, category, date, startTime, endTime, seats, venue);
        event.save();
        cout << "Event successfully registered with the following details:\n";
        cout << "Venue: " << venue << "\nDate: " << date << "\nTime: " << startTime << " - " << endTime << endl;
    }

    void participantMenu() {
        int choice;
        cout << "1. View Scheduled Events\n2. Register for an Event\nEnter your choice: ";
        cin >> choice;
        if (choice == 1) {
            displayEventsWithParticipantCount();
        } else if (choice == 2) {
            displayEventsWithParticipantCount();
            Participant participant = getParticipantDetails();
            participant.save();
            cout << "Successfully registered for the event.\n";
        } else {
            cout << "Invalid choice. Returning to main menu.\n";
        }
    }

    void displayEventsWithParticipantCount() {
        vector<Event> events = Event::loadAll();
        map<string, int> participantCount = Participant::countByEvent();
        for (const auto& event : events) {
            event.display();
            cout << "Registered Participants: " << participantCount[event.getName()] << endl;
            cout << "---------------------------------------" << endl;
        }
    }

    Participant getParticipantDetails() {
        string name, rollNumber, department, phoneNumber, eventName;

        cout << "Enter the event name you want to register for: ";
        cin.ignore();
        getline(cin, eventName);
        cout << "Enter your name: ";
        getline(cin, name);
        cout << "Enter your roll number: ";
        getline(cin, rollNumber);
        cout << "Enter your department: ";
        getline(cin, department);
        cout << "Enter your phone number: ";
        getline(cin, phoneNumber);

        return Participant(name, rollNumber, department, phoneNumber, eventName);
    }

    void validateTime(string& time) {
        // Regular expression to match both 12-hour (AM/PM) and 24-hour formats
        regex time12HourPattern(R"((1[0-2]|0?[1-9]):([0-5][0-9])\s?(AM|PM|am|pm))");
        regex time24HourPattern(R"((1[0-9]|2[0-3]|0?[0-9]):([0-5][0-9]))");

        smatch match;
        if (regex_match(time, match, time12HourPattern)) {
            string hourStr = match[1].str();
            int hour = stoi(hourStr);
            if (match[3] == "PM" || match[3] == "pm") {
                if (hour != 12) hour += 12;
            } else if (hour == 12) {
                hour = 0;
            }
            if (hour < 8 || hour >= 16) {
                throw invalid_argument("Please select a time slot between 8:00 AM and 4:00 PM.");
            }
        } else if (regex_match(time, match, time24HourPattern)) {
            int hour = stoi(match[1].str());
            if (hour < 8 || hour >= 16) {
                throw invalid_argument("Please select a time slot between 08:00 and 16:00.");
            }
        } else {
            throw invalid_argument("Invalid time format. Please enter time in HH:MM AM/PM or HH:MM format.");
        }
    }

    string allocateVenue(const string& date, const string& startTime, const string& endTime, int seats) {
        map<string, int> roomCapacities = {
            {"Room 1", 35},
            {"Room 2", 35},
            {"ITB lab 1", 20},
            {"ITB lab 2", 20},
            {"Project lab", 40},
            {"Programming lab", 40},
            {"IT room", 65}
        };

        vector<Event> events = Event::loadAll();
        string allocatedRoom;
        int minCapacityDifference = INT_MAX;

        for (const auto& room : roomCapacities) {
            if (room.second >= seats && isRoomAvailable(events, date, startTime, endTime, room.first)) {
                int capacityDifference = room.second - seats;
                if (capacityDifference < minCapacityDifference) {
                    minCapacityDifference = capacityDifference;
                    allocatedRoom = room.first;
                }
            }
        }

        if (allocatedRoom.empty()) {
            throw invalid_argument("No suitable room available for the given date and time.");
        }

        return allocatedRoom;
    }

    bool isRoomAvailable(const vector<Event>& events, const string& date, const string& startTime, const string& endTime, const string& room) {
        for (const auto& event : events) {
            if (event.getDate() == date && event.getVenue() == room) {
                if (!(endTime <= event.getStartTime() || startTime >= event.getEndTime())) {
                    throw invalid_argument("The room is already booked for another event at the given date and time.");
                }
            }
        }
        return true;
    }
};

int main() {
    EventManagementSystem system;
    system.run();
    return 0;
}
