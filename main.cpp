#include <iostream>
#include <string>
#include <cstdint>
#include <ctime>
#include <iomanip>

enum Format {
    CD,
    Vinyl,
    Cassette,
    Digital,
    DVDAudio,
    Other
};

typedef struct Data {
    std::string name;
    int year;
    Format format;
    int song_quantity;
    int duration_total_seconds;
    std::string artist;
} Data;

typedef struct Album {
    Album* prev;
    Data data;
    Album* next;
} Album;

int menu();
void handle_add_album(Album** head);
void handle_print_albums(Album* head);
std::string format_to_string(Format format);

Album* create_album(const std::string& name, 
                    int year, 
                    Format format, 
                    int song_quantity, 
                    double duration_total_seconds, 
                    const std::string& artist);
void add_album(Album** head, Album* new_album);
void print_albums(Album* head);
void print_album(Album* album);

int main() {
    Album* head = nullptr;
    bool running = true;

    while (running) {
        int option = menu();

        switch (option)
        {
        case 1:
            handle_add_album(&head);
            break;
        case 2:
            /* code */
            break;
        case 3:
            /* code */
            break;
        case 4:
            /* code */
            break;
        case 5:
            /* code */
            break;
        case 6:
            /* code */
            break;
        case 7:
            /* code */
            break;
        case 8:
            handle_print_albums(head);
            break;
        case 9:
            /* code */
            break;
        case 10:
            /* code */
            break;
        case 0:
            running = false;
            break;
        }
    }

    return 0;
}

int get_year() {
    std::time_t t = std::time(nullptr);
    std::tm *const pTInfo = std::localtime(&t);

    return 1900 + pTInfo->tm_year;
}

void handle_add_album(Album** head) {
    std::string name;
    int year; 
    Format format;
    int song_quantity;
    double duration_total_seconds;
    std::string artist;

    std::cout 
        << "\n====================================================\n"
        << "                    ADD AN ALBUM\n"
        << "====================================================\n"
        << "Enter name of album: ";
    std::getline(std::cin >> std::ws, name);

    std::cout << "Enter year: ";
      while (!(std::cin >> year) || year < 1909 || year > get_year()) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid year. Please enter year from 1909 to " 
                    << get_year() << ": ";
    }

    std::cout 
        << "Format:\n"
        << "1. CD\n"
        << "2. Vinyl\n"
        << "3. Cassette\n"
        << "4. Digital\n"
        << "5. DVDAudio\n"
        << "6. Other\n"
        << "Enter a number from 1 to 6: ";
    int tmp;
    while (!(std::cin >> tmp) || tmp < 1 || tmp > 6) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout 
                << "\nInvalid option. Please enter a number from 1 to 6: ";
        }
    format = static_cast<Format>(tmp-1);

    std::cout << "Enter song quantity: ";
    while (!(std::cin >> song_quantity) || song_quantity < 1) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout 
                << "\nInvalid option. Please enter a positive number: ";
        }

    int duration_minutes;
    int duration_seconds;

    std::cout << "Enter duration minutes: ";
    while (!(std::cin >> duration_minutes) || duration_minutes < 0) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
        std::cout << "Invalid value. Enter non-negative minutes: ";
    }

    std::cout << "Enter duration seconds (0-59): ";
    while (!(std::cin >> duration_seconds) || duration_seconds < 0 || duration_seconds > 59) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
        std::cout << "Invalid value. Enter seconds from 0 to 59: ";
    }

    duration_total_seconds = duration_minutes * 60 + duration_seconds;
    
    std::cout << "Enter artist name: ";
    std::getline(std::cin >> std::ws, artist);

    Album* album = create_album(
        name, 
        year, 
        format, 
        song_quantity, 
        duration_total_seconds, 
        artist);
    std::cout << "Album you want to add:\n";
    print_album(album);

    char decision;

    std::cout << "\nAdd album? (y/n): ";
    while (!(std::cin >> decision) || (decision != 'y' && decision != 'n')) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "\nInvalid option. Enter 'y' or 'n': ";
        }
    if (decision == 'y') {
        add_album(head, album);
        std::cout << "\nAlbum added successfully.\n";
    } else {
        delete album;
        std::cout << "\nChanges weren't saved.\n";
    }
}

void handle_print_albums(Album* head) {
    std::cout 
        << "\n====================================================\n"
        << "                    LIST OF ALBUMS\n"
        << "====================================================\n\n";

    if (head == nullptr) {
        std::cout << "The album list is empty.\n";
    } else {
        std::cout
            << std::left
            << std::setw(5)  << "No."
            << std::setw(30) << "Album"
            << std::setw(8)  << "Year"
            << std::setw(15) << "Format"
            << std::setw(10) << "Songs"
            << std::setw(12) << "Duration"
            << std::setw(25) << "Artist"
            << '\n';

        std::cout << std::string(105, '-') << '\n';
    }
    
    int no = 1;
    Album* tmp = head;
    while (tmp != nullptr) {
        std::cout << std::setw(5)  << no;
        print_album(tmp);
        tmp = tmp->next;
        ++no;
    }

    int option;
    std::cout << "\nEnter 0 to go back: ";
    while (!(std::cin >> option) || option != 0) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid option. Please enter 0: ";
    }
}

Album* create_album(const std::string& name, 
                    int year, 
                    Format format, 
                    int song_quantity, 
                    double duration_total_seconds, 
                    const std::string& artist) {
        
    Album* newAlbum = new Album{};
    
    newAlbum->data.name = name;
    newAlbum->data.year = year;
    newAlbum->data.format = format;
    newAlbum->data.song_quantity = song_quantity;
    newAlbum->data.duration_total_seconds = duration_total_seconds;
    newAlbum->data.artist = artist;
    newAlbum->next = nullptr;
    newAlbum->prev = nullptr;
    
    return newAlbum;
}
    
void add_album(Album** head, Album* new_album) {
    if (*head == nullptr) {
        *head = new_album;
        return;
    } 
    
    new_album->next = *head;
    (*head)->prev = new_album;
    *head = new_album;
}

void print_album(Album* album) {
    double duration = (album->data.duration_total_seconds / 60) + ((album->data.duration_total_seconds % 60) / 100.0);
    std::cout
        << std::left
        << std::setw(30) << album->data.name
        << std::setw(8)  << album->data.year
        << std::setw(15) << format_to_string(album->data.format)
        << std::setw(10) << album->data.song_quantity
        << std::setw(12) << std::fixed << std::setprecision(2) << duration
        << std::setw(25) << album->data.artist
        << '\n';
}

void delete_album(Album* head);
void delete_all_albums(Album* head);

void change_album_string_part(Album** head, std::string data_type, std::string chenges);
void change_album_int_part(Album** head, std::string data_type, int chenges);

std::string format_to_string(Format format) {
    switch (format) {
        case CD:       return "CD";
        case Vinyl:    return "Vinyl";
        case Cassette: return "Cassette";
        case Digital:  return "Digital";
        case DVDAudio: return "DVD-Audio";
        case Other:    return "Other";
    }
}


void print_albums(Album* head) {
    if (head == nullptr) {
        return;
    }

    Album* tmp = head;

    std::cout << "|\tназва диска\t|\tрік випуску\t|\tформат\t|\tкількість пісень\t|\tтривалість звучання\t|\tвиконавець\t|\n";
    do {
        print_album(tmp);
        tmp = tmp->next;
    } while (tmp != nullptr);
}
void print_albums_by_author(Album* head, const std::string& author);

int menu() {
    int opt;

    std::cout
        << "\n====================================================\n"
        << "                MUSIC KALEIDOSCOPE\n"
        << "====================================================\n"
        << "         Music Album Collection Manager\n\n"
        << "1. Add an album\n"
        << "2. Delete an album\n"
        << "3. Edit album information\n"
        << "4. Search for albums by artist\n"
        << "5. Display albums released after a specified year\n"
        << "6. Find the longest album in a selected format\n"
        << "7. Sort albums by duration\n"
        << "8. Display all albums\n"
        << "9. Load data from a file\n"
        << "10. Save data to a file\n"
        << "0. Exit\n\n"
        << "Select an option (0 - 10): ";

        while (!(std::cin >> opt) || opt < 0 || opt > 10) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );

            std::cout 
                << "\nInvalid option. Please enter a number from 0 to 10: ";
        }
        
    return opt;
}