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
bool is_blank(const std::string& text);
int get_year();
std::string format_to_string(Format format);
Album* create_album(const std::string& name, 
                    int year, 
                    Format format, 
                    int song_quantity, 
                    double duration_total_seconds, 
                    const std::string& artist);
void add_album(Album** head, Album* new_album);

void handle_print_albums(Album* head);
void print_album(Album* album);
int print_albums(Album* head, std::string name = "", int search_year = 0);

void handle_clear_data(Album** head);
void handle_delete_album(Album** head);
void delete_album(Album** head, int position);

void handle_search_by_artist(Album* head);
std::string to_lower (std:: string);

void handle_display_albums_after_specified_year(Album* head);
void handle_longest_album_by_format(Album* head);

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
            handle_search_by_artist(head);
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
            handle_display_albums_after_specified_year(head);
            break;
        case 7:
            handle_longest_album_by_format(head);
            break;
        case 8:
            handle_print_albums(head);
            break;
        case 9:
            handle_delete_album(&head);
            break;
        case 10:
            handle_clear_data(&head);
            break;
        case 0:
            running = false;
            break;
        }
    }

    return 0;
}

int menu() {
    int opt;

    std::cout
        << "\n====================================================\n"
        << "                MUSIC KALEIDOSCOPE\n"
        << "====================================================\n"
        << "         Music Album Collection Manager\n\n"
        << "1. Add an album\n"
        << "2. Search for albums by artist\n"
        << "3. Edit all album information\n"
        << "4. Edit a selected album field\n"
        << "5. Sort albums by duration\n"
        << "6. Display albums released after a specified year\n"
        << "7. Find the longest album in a selected format\n"
        << "8. Display all albums\n"
        << "9. Delete an album\n"
        << "10. Clear data\n"
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

void handle_add_album(Album** head) {
    std::string name;
    int year; 
    Format format;
    int song_quantity;
    int duration_total_seconds;
    std::string artist;

    std::cout 
        << "\n====================================================\n"
        << "                    ADD AN ALBUM\n"
        << "====================================================\n"
        << "Enter album name (maximum 100 characters): ";
    std::getline(std::cin >> std::ws, name);

    while (is_blank(name) || name.length() > 100) {
        if (is_blank(name)) {
            std::cout << "Name cannot be empty. Enter album name: ";
        } else {
            std::cout
                << "Name is too long. Enter a name containing up to 100 characters: ";
        }

        std::getline(std::cin, name);
    }
    

    const int current_year = get_year();

    std::cout << "Enter year: ";
      while (!(std::cin >> year) || year < 1909 || year > current_year) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid year. Please enter year from 1909 to " 
                    << current_year << ": ";
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
    format = static_cast<Format>(tmp - 1);

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
    do {
        std::cout << "Enter duration minutes: ";

        while (!(std::cin >> duration_minutes) ||
            duration_minutes < 0) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );

            std::cout
                << "Invalid value. Enter non-negative minutes: ";
        }

        std::cout << "Enter duration seconds (0-59): ";

        while (!(std::cin >> duration_seconds) ||
            duration_seconds < 0 ||
            duration_seconds > 59) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );

            std::cout << "Invalid value. Enter seconds from 0 to 59: ";
        }

        duration_total_seconds =
            duration_minutes * 60 + duration_seconds;

        if (duration_total_seconds == 0) {
            std::cout
                << "Album duration must be greater than 0:00.\n";
        }
    } while (duration_total_seconds == 0);

    std::cout << "Enter artist name (maximum 100 characters): ";
    std::getline(std::cin >> std::ws, artist);

    while (is_blank(artist) || artist.length() > 100) {
        if (is_blank(artist)) {
            std::cout << "Artist name cannot be empty. Enter artist name: ";
        } else {
            std::cout
                << "Artist name is too long. Enter a name containing up to 100 characters: ";
        }

        std::getline(std::cin, artist);
    }

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

bool is_blank(const std::string& text) {
    return text.find_first_not_of(" \t\r") == std::string::npos;
}

int get_year() {
    std::time_t t = std::time(nullptr);
    std::tm *const pTInfo = std::localtime(&t);

    return 1900 + pTInfo->tm_year;
}

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

void handle_print_albums(Album* head) {
    print_albums(head);

    int option;
    std::cout << "\nEnter 0 to go back: ";
    while (!(std::cin >> option) || option != 0) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid option. Please enter 0: ";
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void print_album(Album* album) {
    double duration = (album->data.duration_total_seconds / 60) + 
                      ((album->data.duration_total_seconds % 60) / 100.0);
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

int print_albums(Album* head, std::string name, int search_year) {
    int no = 1;

    std::cout 
        << "\n====================================================\n"
        << "                    LIST OF ALBUMS\n"
        << "====================================================\n\n";

    if (head == nullptr) {
        std::cout << "The album list is empty.\n";
        return 0;
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
            
            Album* tmp = head;
            std::string search_name = to_lower(name);

            while (tmp != nullptr) {
                bool should_print;

                if (!name.empty()) {
                    should_print = to_lower(tmp->data.artist) == search_name;
                } else if (search_year != 0) {
                    should_print = tmp->data.year > search_year;
                } else {
                    should_print = true;
                }

                if (should_print) {
                    std::cout << std::setw(5)  << no;    
                    print_album(tmp);
                    ++no;
                }
                tmp = tmp->next;
            }
    }
    return no - 1;
}

void handle_delete_album(Album** head) {
    int count = print_albums(*head);

    if (count == 0) {
        int option;
        std::cout << "\nEnter 0 to go back: ";
        while (!(std::cin >> option) || option != 0) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Invalid option. Please enter 0: ";
        }
    } else {
        int position;
        std::cout << "\nEnter the album position to delete, or 0 to go back: ";
        while (!(std::cin >> position) || position < 0 || position > count) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Invalid option. Please enter a number between 0 and " << count  << ": ";
        }
        if (position == 0) {
            return;
        }
        char decision;
    
        std::cout << "\nAre you sure? This action cannot be undone (y/n): ";
        while (!(std::cin >> decision) || (decision != 'y' && decision != 'n')) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "\nInvalid option. Enter 'y' or 'n': ";
            }
        if (decision == 'y') {
            delete_album(head, position);
    
            std::cout << "\nAlbum deleted successfully.\n";
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
    
}

void delete_album(Album** head, int position) {
    if (*head == nullptr) {
        return;
    }
    Album* tmp = *head;
    if (position == 1) {
        *head = (*head)->next;
        if (*head != NULL) {
            (*head)->prev = NULL;
        }
        delete tmp;
        return;
    }
    for (int i = 1; tmp != nullptr && i < position; i++) {
        tmp = tmp->next;
    }
    if (tmp == nullptr) {
        return;
    }
    if (tmp->next != nullptr) {
        tmp->next->prev = tmp->prev;
    }
    if (tmp->prev != nullptr) {
        tmp->prev->next = tmp->next;
    }
    delete tmp;
}

void handle_clear_data(Album** head) {
    char decision;
    
    std::cout << "Are you sure? This action cannot be undone (y/n): ";
    while (!(std::cin >> decision) || (decision != 'y' && decision != 'n')) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "\nInvalid option. Enter 'y' or 'n': ";
        }

    if (decision == 'y') {
        while (*head != nullptr) {
            delete_album(head, 1);
        }

        std::cout << "\nData cleared successfully.\n";
        int option;
        std::cout << "\nEnter 0 to go back: ";
        while (!(std::cin >> option) || option != 0) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Invalid option. Please enter 0: ";
        }
    }
}

void handle_search_by_artist(Album* head) {
    if (head == nullptr) {
        std::cout << "\nNo albums available.\n";
    } else {
        std::string artist;
        std::cout << "\nEnter the artist’s name to search for, or 0 to go back: ";
        std::getline(std::cin >> std::ws, artist);
    
        while (is_blank(artist) || artist.length() > 100) {
            if (is_blank(artist)) {
                std::cout << "Name cannot be empty. Enter artist’s name: ";
            } else {
                std::cout
                    << "Name is too long. Enter a name containing up to 100 characters: ";
            }
    
            std::getline(std::cin, artist);
        }
        
        if (artist == "0") {
            return;
        }
        
        int count = print_albums(head, artist);
        if (count == 0) {
            std::cout << "\nNo albums found for this artist.\n";
        }
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

std::string to_lower(std::string str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return std::tolower(c); });

    return lower;
}

void handle_display_albums_after_specified_year(Album* head) {
    if (head == nullptr) {
        std::cout << "\nNo albums available.\n";
        return;
    } else {
        int year;
        int current_year = get_year();
        std::cout << "\nEnter the year after which to display albums, or 0 to go back: ";
        while (!(std::cin >> year) || (year != 0 && (year < 1909 || year > current_year))) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid year. Please enter year from 1909 to " 
                    << current_year 
                    << ", or 0 to go back: ";
        }
        
        if (year == 0) {
            return;
        }
        int count = print_albums(head, "", year);
        if (count == 0) {
            std::cout << "\nNo albums were found after the specified year.\n";
        }
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

void handle_longest_album_by_format(Album* head) {
    if (head == nullptr) {
        std::cout << "\nNo albums available.\n";
        return;
    } else {
        std::cout 
            << "Format:\n"
            << "1. CD\n"
            << "2. Vinyl\n"
            << "3. Cassette\n"
            << "4. Digital\n"
            << "5. DVDAudio\n"
            << "6. Other\n"
            << "0. Go back\n"
            << "\nEnter a number from 0 to 6: ";
        int option;
        while (!(std::cin >> option) || option < 0 || option > 6) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout 
                    << "\nInvalid option. Please enter a number from 0 to 6: ";
            }
        if (option == 0) {
            return;
        }
        Format format = static_cast<Format>(option - 1);

        Album* curr = head;
        Album* longest = nullptr;
        int longest_time = -1;

        while (curr != nullptr) {
            if (curr->data.format == format && 
                curr->data.duration_total_seconds > longest_time) {

                longest = curr;
                longest_time = curr->data.duration_total_seconds;

            }
            curr = curr->next;
        }

        if (longest == nullptr) {
            std::cout << "\nNo albums found in the selected format.\n";
        } else {
            std::cout << "\nThe longest album in the selected format is:\n";
            print_album(longest);
        }
    }

    int opt;
    std::cout << "\nEnter 0 to go back: ";
    while (!(std::cin >> opt) || opt != 0) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid option. Please enter 0: ";
    }
}