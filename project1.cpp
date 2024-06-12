#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <stdexcept>
#include <limits>
#include <iomanip>

using namespace std;

class Event {
private:
    string name, organizer, category, date, startTime, endTime, venue;
    int seats;

public:
    Event();
    Event(string n, string o, string c, string d, string st, string et, int s, string v);
    string getName() const;
    string getOrganizer() const;
    string getCategory() const;
    string getDate() const;
    string getStartTime() const;
    string getEndTime() const;
    string getVenue() const;
    int getSeats() const;
    void save() const;
    static vector<Event> loadAll();
    void display() const;
};

class Participant {
private:
    string name, rollNumber, department, phoneNumber, eventName;

public:
    Participant();
    Participant(string n, string r, string d, string p, string e);
    void save() const;
    static map<string, int> countByEvent();
};

class EventManagementSystem {
public:
    void run();
    static void printHeader(const string& title);

private:
    void validateDate(const string& date);
    void validateTime(const string& startTime, const string& endTime);
    string roundTimeToNextInterval(const string& time);
    void organizerMenu();
    void displayScheduledEvents();
    void addNewEvent();
    void displayCreatedEvents();
    void participantMenu();
    void displayEventsForParticipants();
    void registerForEvent();
    string allocateVenue(const string& date, const string& startTime, const string& endTime, int seats);
    bool isRoomAvailable(const vector<Event>& events, const string& date, const string& startTime, const string& endTime, const string& room);
    void deleteEvent();
    void modifyEvent();
    void displayEventsTable(const vector<Event>& events, const map<string, int>& participantCount);
    bool isValidDateFormat(const string &date);
    bool isEventNameUnique(const string& eventName);
};

int main() {
    cout << endl;
    EventManagementSystem::printHeader("Welcome to OPTIMAL EVENT SCHEDULER");

    EventManagementSystem system;
    system.run();
    return 0;
}

// Event class definitions
Event::Event() = default;
Event::Event(string n, string o, string c, string d, string st, string et, int s, string v)
    : name(n), organizer(o), category(c), date(d), startTime(st), endTime(et), seats(s), venue(v) {}

string Event::getName() const { return name; }
string Event::getOrganizer() const { return organizer; }
string Event::getCategory() const { return category; }
string Event::getDate() const { return date; }
string Event::getStartTime() const { return startTime; }
string Event::getEndTime() const { return endTime; }
string Event::getVenue() const { return venue; }
int Event::getSeats() const { return seats; }

void Event::save() const {
    ofstream file("events.txt", ios::app);
    if (file.is_open()) {
        file << name << "," << organizer << "," << category << "," 
             << date << "," << startTime << "," << endTime << "," 
             << seats << "," << venue << endl;
        file.close();
    } else {
       // cerr << "Unable to open file to save event." << endl;
    }
}

vector<Event> Event::loadAll() {
    vector<Event> events;
    ifstream file("events.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            size_t pos = 0;
            string n, o, c, d, st, et, v;
            int s;

            try {
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
            } catch (const exception& e) {
                cerr << "Error parsing event data: " << e.what() << endl;
            }
        }
        file.close();
    } else {
       // cerr << "Unable to open file to load events." << endl;
    }
    return events;
}

void Event::display() const {
    cout << "+---------------------------------------+\n";
    cout << "| Event Name: " << name << "\n| Organizer: " << organizer << "\n| Category: " << category 
         << "\n| Date: " << date << "\n| Time: " << startTime << " - " << endTime 
         << "\n| Venue: " << venue << "\n| Seats: " << seats << endl;
    cout << "+---------------------------------------+" << endl;
}

// Participant class definitions
Participant::Participant() = default;
Participant::Participant(string n, string r, string d, string p, string e)
    : name(n), rollNumber(r), department(d), phoneNumber(p), eventName(e) {}

void Participant::save() const {
    ofstream file("participants.txt", ios::app);
    if (file.is_open()) {
        file << name << "," << rollNumber << "," 
             << department << "," << phoneNumber << "," 
             << eventName << endl;
        file.close();
    } else {
       // cerr << "Unable to open file to save participant." << endl;
    }
}

map<string, int> Participant::countByEvent() {
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
     //   cerr << "Unable to open file to load participants." << endl;
    }
    return participantCount;
}

// EventManagementSystem class definitions
void EventManagementSystem::run() {
    while (true) {
        try {
            int role;
            cout << "\n1. Are you an Organizer?\n2. Are you a Participant?\n0. Exit\nEnter your choice: ";
            cin >> role;
            cout << endl;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw invalid_argument("Invalid input. Please enter a number.");
            }

            switch (role) {
                case 1:
                    organizerMenu();
                    break;
                case 2:
                    participantMenu();
                    break;
                case 0:
                    return;
                default:
                    throw invalid_argument("Invalid choice. Please enter 0, 1, or 2.");
            }
        } catch (const invalid_argument& e) {
            cerr << e.what() << endl;
        }
    }
}

void EventManagementSystem::validateDate(const string& date) {
    regex datePattern(R"(\d{4}-\d{2}-\d{2})");
    if (!regex_match(date, datePattern)) {
        throw invalid_argument("Invalid date format. Please enter the date in YYYY-MM-DD format.");
    }

    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    if (year < 1900 || year > 2100) {
        throw invalid_argument("Invalid year. Year must be between 1900 and 2100.");
    }
    if (month < 1 || month > 12) {
        throw invalid_argument("Invalid month. Month must be between 01 and 12.");
    }

    if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) && (day < 1 || day > 31)) {
        throw invalid_argument("Invalid day. Day must be between 01 and 31.");
    } else if ((month == 4 || month == 6 || month == 9 || month == 11) && (day < 1 || day > 30)) {
        throw invalid_argument("Invalid day. Day must be between 01 and 30.");
    } else if (month == 2) {
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if ((isLeapYear && (day < 1 || day > 29)) || (!isLeapYear && (day < 1 || day > 28))) {
            throw invalid_argument("Invalid day. Day must be between 01 and 29 in a leap year and between 01 and 28 otherwise.");
        }
    }
}

void EventManagementSystem::validateTime(const string& startTime, const string& endTime) {
    regex timePattern(R"((1[0-9]|2[0-3]|0?[0-9]):([0-5][0-9]))");
    smatch matchStart, matchEnd;
    if (regex_match(startTime, matchStart, timePattern) && regex_match(endTime, matchEnd, timePattern)) {
        int startHour = stoi(matchStart[1].str());
        int endHour = stoi(matchEnd[1].str());
        int startMinute = stoi(matchStart[2].str());
        int endMinute = stoi(matchEnd[2].str());

        if (startHour < 8 || startHour >= 16) {
            throw invalid_argument("Please select a start time between 08:00 and 16:00.");
        }
        if (endHour < 8 || endHour >= 16 || (endHour == 16 && endMinute > 0)) {
            throw invalid_argument("Please select an end time between 08:00 and 16:00.");
        }
        if (endHour < startHour || (endHour == startHour && endMinute <= startMinute)) {
            throw invalid_argument("End time must be after start time.");
        }
    } else {
        throw invalid_argument("Invalid time format. Please enter time in HH:MM format.");
    }
}

string EventManagementSystem::roundTimeToNextInterval(const string& time) {
    int hour = stoi(time.substr(0, 2));
    int minute = stoi(time.substr(3, 2));

    minute = ((minute + 14) / 15) * 15;  // Round up to the nearest 15 minutes
    if (minute == 60) {
        minute = 0;
        hour += 1;
    }

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
    return string(buffer);
}

void EventManagementSystem::organizerMenu() {
    while (true) {
        try {
            int choice;
            cout << "\n1. Scheduled Events\n2. Add New Event\n3. Your Created Events\n4. Modify Event\n5. Delete Event\n0. Exit\nEnter your choice: ";
            cin >> choice;
            cout << endl;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw invalid_argument("Invalid input. Please enter a number.");
            }
            if (choice == 0) break;

            switch (choice) {
                case 1:
                    displayScheduledEvents();
                    break;
                case 2:
                    addNewEvent();
                    break;
                case 3:
                    displayCreatedEvents();
                    break;
                case 4:
                    modifyEvent();
                    break;
                case 5:
                    deleteEvent();
                    break;
                default:
                    throw invalid_argument("Invalid choice. Please enter 0, 1, 2, 3, 4, or 5.");
            }
        } catch (const invalid_argument& e) {
            cerr << e.what() << endl;
        }
    }
}

void EventManagementSystem::displayScheduledEvents() {
    vector<Event> events = Event::loadAll();
    if (events.empty()) {
        cout << "No events scheduled yet.\n";
        return;
    }

    map<string, int> participantCount = Participant::countByEvent();

    try {
        int categoryChoice;
        cout << "\nChoose a category:\n1. Workshop\n2. Seminar\n3. Lecture\n4. Exam\n5. Formal Event\n6. Miscellaneous\nEnter your choice: ";
        cin >> categoryChoice;
        cout << endl;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw invalid_argument("Invalid input. Please enter a number.");
        }

        string category;
        switch (categoryChoice) {
            case 1: category = "Workshop"; break;
            case 2: category = "Seminar"; break;
            case 3: category = "Lecture"; break;
            case 4: category = "Exam"; break;
            case 5: category = "Formal Event"; break;
            case 6: category = "Miscellaneous"; break;
            default: throw invalid_argument("Invalid choice. Please enter a number between 1 and 6.");
        }

        vector<Event> filteredEvents;
        for (const auto& event : events) {
            if (event.getCategory() == category) {
                filteredEvents.push_back(event);
            }
        }

        if (filteredEvents.empty()) {
            cout << "No event scheduled yet for this category." << endl;
        } else {
            displayEventsTable(filteredEvents, participantCount);
        }
    } catch (const invalid_argument& e) {
        cerr << e.what() << endl;
    }
}

void EventManagementSystem::addNewEvent() {
    string name, organizer, category, date, startTime, endTime, venue;
    int seats;

    try {
        cout << "\nEnter event name: ";
        cin.ignore();
        getline(cin, name);

        if (!isEventNameUnique(name)) {
            throw invalid_argument("An event with this name already exists.");
        }

        cout << "Enter Organizer's Name: ";
        getline(cin, organizer);

        int categoryChoice;
        cout << "Choose a category:\n1. Workshop\n2. Seminar\n3. Lecture\n4. Exam\n5. Formal Event\n6. Miscellaneous\nEnter your choice: ";
        cin >> categoryChoice;
        cout << endl;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw invalid_argument("Invalid input. Please enter a number.");
        }

        switch (categoryChoice) {
            case 1: category = "Workshop"; break;
            case 2: category = "Seminar"; break;
            case 3: category = "Lecture"; break;
            case 4: category = "Exam"; break;
            case 5: category = "Formal Event"; break;
            case 6: category = "Miscellaneous"; break;
            default: throw invalid_argument("Invalid choice. Returning to previous menu.");
        }

        cout << "Enter event date (YYYY-MM-DD): ";
        cin.ignore();
        while (true) {
            getline(cin, date);
            try {
                validateDate(date);
                break;
            } catch (const invalid_argument& e) {
                cerr << e.what() << endl;
                cout << "Enter event date (YYYY-MM-DD): ";
            }
        }

        while (true) {
            try {
                cout << "Enter event start time (HH:MM): ";
                getline(cin, startTime);
                cout << "Enter event end time (HH:MM): ";
                getline(cin, endTime);
                validateTime(startTime, endTime);
                startTime = roundTimeToNextInterval(startTime);
                endTime = roundTimeToNextInterval(endTime);
                break;
            } catch (const invalid_argument& e) {
                cerr << e.what() << endl;
            }
        }

        cout << "Enter number of seats: ";
        cin >> seats;
        cout << endl;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw invalid_argument("Invalid input. Please enter a number.");
        }

        while (true) {
            try {
                venue = allocateVenue(date, startTime, endTime, seats);
                cout << "Venue allocated: " << venue << endl;
                break;
            } catch (const invalid_argument& e) {
                cerr << e.what() << endl;
                cout << "Please choose a different time frame." << endl;

                while (true) {
                    try {
                        cout << "Enter event start time (HH:MM): ";
                        cin.ignore();
                        getline(cin, startTime);
                        cout << "Enter event end time (HH:MM): ";
                        getline(cin, endTime);
                        validateTime(startTime, endTime);
                        startTime = roundTimeToNextInterval(startTime);
                        endTime = roundTimeToNextInterval(endTime);
                        break;
                    } catch (const invalid_argument& e) {
                        cerr << e.what() << endl;
                    }
                }
            }
        }

        Event event(name, organizer, category, date, startTime, endTime, seats, venue);
        event.save();
        cout << "\nEvent successfully registered with the following details:\n";
        event.display();
    } catch (const invalid_argument& e) {
        cerr << e.what() << endl;
    }
}

void EventManagementSystem::displayCreatedEvents() {
    vector<Event> events = Event::loadAll();
    if (events.empty()) {
        cout << "No events scheduled yet.\n";
        return;
    }

    map<string, int> participantCount = Participant::countByEvent();
    string currentOrganizer;

    cout << "\nEnter Organizer Name: ";
    cin.ignore();
    getline(cin, currentOrganizer);

    bool foundEvent = false;
    for (const auto& event : events) {
        if (event.getOrganizer() == currentOrganizer) {
            foundEvent = true;
            event.display();
            cout << "Registered Participants: " << participantCount[event.getName()] << endl;
        }
    }

    if (!foundEvent) {
        cout << "No events found for this organizer.\n";
    }
}

void EventManagementSystem::participantMenu() {
    try {
        int choice;
        cout << "\n1. Scheduled Events\n2. Register for an Event\n0. Exit\nEnter your choice: ";
        cin >> choice;
        cout << endl;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw invalid_argument("Invalid input. Please enter a number.");
        }
        if (choice == 0) return;

        switch (choice) {
            case 1:
                displayEventsForParticipants();
                break;
            case 2:
                registerForEvent();
                break;
            default:
                throw invalid_argument("Invalid choice. Returning to previous menu.");
        }
    } catch (const invalid_argument& e) {
        cerr << e.what() << endl;
    }
}

void EventManagementSystem::displayEventsForParticipants() {
    vector<Event> events = Event::loadAll();
    if (events.empty()) {
        cout << "No events found.\n";
        return;
    }

    map<string, int> participantCount = Participant::countByEvent();
    displayEventsTable(events, participantCount);
}

void EventManagementSystem::registerForEvent() {
    string name, rollNumber, department, phoneNumber, eventName;

    try {
        cout << "\nEnter the event name you want to register for: ";
        cin.ignore();
        getline(cin, eventName);

        vector<Event> events = Event::loadAll();
        bool eventExists = false;
        for (const auto& event : events) {
            if (event.getName() == eventName) {
                eventExists = true;
                break;
            }
        }

        if (!eventExists) {
            throw invalid_argument("Error: The event \"" + eventName + "\" does not exist.");
        }

        cout << "Enter your name: ";
        getline(cin, name);
        cout << "Enter your roll number: ";
        getline(cin, rollNumber);
        cout << "Enter your department: ";
        getline(cin, department);
        cout << "Enter your phone number: ";
        getline(cin, phoneNumber);

        Participant participant(name, rollNumber, department, phoneNumber, eventName);
        participant.save();
        cout << "Successfully registered for the event.\n";
    } catch (const invalid_argument& e) {
        cerr << e.what() << endl;
    }
}

string EventManagementSystem::allocateVenue(const string& date, const string& startTime, const string& endTime, int seats) {
    map<string, int> roomCapacities = {
        {"Room 1", 35},
       {"Room 2", 35},
       {"ITB lab 1", 20},
        {"ITB lab 2", 20},
        {"Project lab", 40},
        {"Programming lab", 40},
        {"IT Conference room", 65},
        {"CS Lab 3", 130},
        {"Smart Conference Room", 210},
        {"Auditorium", 500}
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

void EventManagementSystem::deleteEvent() {
    try {
        string eventName;
        cout << "\nEnter the name of the event you want to delete: ";
        cin.ignore();
        getline(cin, eventName);

        vector<Event> events = Event::loadAll();
        bool eventFound = false;

        for (auto it = events.begin(); it != events.end(); ++it) {
            if (it->getName() == eventName) {
                eventFound = true;
                events.erase(it);
                break;
            }
        }

        ofstream file("events.txt");
        if (file.is_open()) {
            for (const auto& event : events) {
                file << event.getName() << "," << event.getOrganizer() << "," << event.getCategory() << ","
                     << event.getDate() << "," << event.getStartTime() << "," << event.getEndTime() << ","
                     << event.getSeats() << "," << event.getVenue() << endl;
            }
            file.close();
        } else {
          //  cerr << "Unable to open file to save events." << endl;
        }

        if (eventFound) {
            cout << "Event \"" << eventName << "\" deleted successfully." << endl;
        } else {
            cout << "Event \"" << eventName << "\" not found." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error deleting event: " << e.what() << endl;
    }
}

void EventManagementSystem::modifyEvent() {
    try {
        string eventName;
        cout << "\nEnter the name of the event you want to modify: ";
        cin.ignore();
        getline(cin, eventName);

        vector<Event> events = Event::loadAll();
        bool eventFound = false;
        Event modifiedEvent;

        for (auto& event : events) {
            if (event.getName() == eventName) {
                eventFound = true;
                modifiedEvent = event;
                break;
            }
        }

        if (!eventFound) {
            throw invalid_argument("Event \"" + eventName + "\" not found.");
        }

        cout << "Enter new details (leave empty to keep current value):\n";
        string input;

        cout << "Event Name [" << modifiedEvent.getName() << "]: ";
        getline(cin, input);
        if (!input.empty()) modifiedEvent = Event(input, modifiedEvent.getOrganizer(), modifiedEvent.getCategory(), modifiedEvent.getDate(), modifiedEvent.getStartTime(), modifiedEvent.getEndTime(), modifiedEvent.getSeats(), modifiedEvent.getVenue());

        cout << "Organizer [" << modifiedEvent.getOrganizer() << "]: ";
        getline(cin, input);
        if (!input.empty()) modifiedEvent = Event(modifiedEvent.getName(), input, modifiedEvent.getCategory(), modifiedEvent.getDate(), modifiedEvent.getStartTime(), modifiedEvent.getEndTime(), modifiedEvent.getSeats(), modifiedEvent.getVenue());

        cout << "Category [" << modifiedEvent.getCategory() << "]: ";
        getline(cin, input);
        if (!input.empty()) modifiedEvent = Event(modifiedEvent.getName(), modifiedEvent.getOrganizer(), input, modifiedEvent.getDate(), modifiedEvent.getStartTime(), modifiedEvent.getEndTime(), modifiedEvent.getSeats(), modifiedEvent.getVenue());

        cout << "Date [" << modifiedEvent.getDate() << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            validateDate(input);
            modifiedEvent = Event(modifiedEvent.getName(), modifiedEvent.getOrganizer(), modifiedEvent.getCategory(), input, modifiedEvent.getStartTime(), modifiedEvent.getEndTime(), modifiedEvent.getSeats(), modifiedEvent.getVenue());
        }

        cout << "Start Time [" << modifiedEvent.getStartTime() << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            validateTime(input, modifiedEvent.getEndTime());
            input = roundTimeToNextInterval(input);
            modifiedEvent = Event(modifiedEvent.getName(), modifiedEvent.getOrganizer(), modifiedEvent.getCategory(), modifiedEvent.getDate(), input, modifiedEvent.getEndTime(), modifiedEvent.getSeats(), modifiedEvent.getVenue());
        }

        cout << "End Time [" << modifiedEvent.getEndTime() << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            validateTime(modifiedEvent.getStartTime(), input);
            input = roundTimeToNextInterval(input);
            modifiedEvent = Event(modifiedEvent.getName(), modifiedEvent.getOrganizer(), modifiedEvent.getCategory(), modifiedEvent.getDate(), modifiedEvent.getStartTime(), input, modifiedEvent.getSeats(), modifiedEvent.getVenue());
        }

        cout << "Seats [" << modifiedEvent.getSeats() << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            modifiedEvent = Event(modifiedEvent.getName(), modifiedEvent.getOrganizer(), modifiedEvent.getCategory(), modifiedEvent.getDate(), modifiedEvent.getStartTime(), modifiedEvent.getEndTime(), stoi(input), "");
            try {
                string allocatedVenue = allocateVenue(modifiedEvent.getDate(), modifiedEvent.getStartTime(), modifiedEvent.getEndTime(), modifiedEvent.getSeats());
                cout << "Venue allocated: " << allocatedVenue << endl;
                modifiedEvent = Event(modifiedEvent.getName(), modifiedEvent.getOrganizer(), modifiedEvent.getCategory(), modifiedEvent.getDate(), modifiedEvent.getStartTime(), modifiedEvent.getEndTime(), modifiedEvent.getSeats(), allocatedVenue);
            } catch (const invalid_argument& e) {
                cerr << e.what() << endl;
                return;
            }
        }

        for (auto& event : events) {
            if (event.getName() == eventName) {
                event = modifiedEvent;
                break;
            }
        }

        ofstream file("events.txt");
        if (file.is_open()) {
            for (const auto& event : events) {
                file << event.getName() << "," << event.getOrganizer() << "," << event.getCategory() << ","
                     << event.getDate() << "," << event.getStartTime() << "," << event.getEndTime() << ","
                     << event.getSeats() << "," << event.getVenue() << endl;
            }
            file.close();
            cout << "Event modified successfully." << endl;
        } else {
         //   cerr << "Unable to open file to save events." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error modifying event: " << e.what() << endl;
    }
}

bool EventManagementSystem::isRoomAvailable(const vector<Event>& events, const string& date, const string& startTime, const string& endTime, const string& room) {
    for (const auto& event : events) {
        if (event.getDate() == date && event.getVenue() == room) {
            if (!(endTime <= event.getStartTime() || startTime >= event.getEndTime())) {
                return false;
            }
        }
    }
    return true;
}

void EventManagementSystem::displayEventsTable(const vector<Event>& events, const map<string, int>& participantCount) {
    cout << left << setw(20) << "Event Name" 
         << setw(20) << "Organizer" 
         << setw(12) << "Date" 
         << setw(20) << "Time" 
         << setw(20) << "Venue" 
         << setw(8) << "Seats" 
         << "Registered Participants" << endl;
    cout << string(20 + 20 + 12 + 20 + 20 + 8 + 25, '-') << endl;

    for (const auto& event : events) {
        int registeredParticipants = 0;
        auto it = participantCount.find(event.getName());
        if (it != participantCount.end()) {
            registeredParticipants = it->second;
        }
        cout << left << setw(20) << event.getName()
             << setw(20) << event.getOrganizer()
             << setw(12) << event.getDate()
             << setw(20) << (event.getStartTime() + " - " + event.getEndTime())
             << setw(20) << event.getVenue()
             << setw(8) << event.getSeats()
             << registeredParticipants << endl;
    }
}

void EventManagementSystem::printHeader(const string& title) {
    int terminalWidth = 80; // Assume default terminal width
    int titleWidth = title.size();
    int leftPadding = (terminalWidth - titleWidth) / 2;
    cout << setw(leftPadding) << "" << title << endl;
}

bool EventManagementSystem::isValidDateFormat(const string &date) {
    regex datePattern(R"(\d{4}-\d{2}-\d{2})");
    if (!regex_match(date, datePattern)) {
        return false;
    }

    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    if (month == 2) {
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if ((isLeapYear && day > 29) || (!isLeapYear && day > 28)) return false;
    }

    return true;
}

bool EventManagementSystem::isEventNameUnique(const string& eventName) {
    vector<Event> events = Event::loadAll();
    for (const auto& event : events) {
        if (event.getName() == eventName) {
            return false;
        }
    }
    return true;
}
