#include <iostream>
#include <string>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits>
#include <fstream>
#include <sstream>
#include <cstdio>

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
bool string_to_format(const std::string& text, Format& format);
bool parse_duration(const std::string& text, int& total_seconds);
int read_duration();
Album* create_album(const std::string& name, 
                    int year, 
                    Format format, 
                    int song_quantity, 
                    int duration_total_seconds, 
                    const std::string& artist);
void add_album(Album** head, Album* new_album);

void handle_print_albums(Album* head);
void print_album(Album* album,
                 int album_width = 0,
                 int artist_width = 0);
int print_albums(Album* head, std::string name = "", int search_year = 0);

void handle_clear_data(Album** head);
void handle_delete_album(Album** head);
void delete_album(Album** head, int position);

void handle_search_by_artist(Album* head);
std::string to_lower (std::string str);

void handle_display_albums_after_specified_year(Album* head);
void handle_longest_album_by_format(Album* head);

void handle_sorting_by_time(Album** head);
Album* get_middle(Album* head);
Album* merge(Album* list1, Album* list2);
Album* merge_sort(Album* head);

void handle_edit_album_field(Album* head);
void handle_edit_all_album_data(Album* head);

bool save_albums_to_file(Album* head,
                         const std::string& filename,
                         std::string& error);

bool load_albums_from_file(Album** head,
                           const std::string& filename,
                           std::string& error);

bool read_txt_filename(std::string& filename,
                       const std::string& action);
bool file_exists(const std::string& filename);
bool confirm_overwrite_if_needed(const std::string& filename);
void wait_to_go_back();

int main() {
    Album* head = nullptr;
    std::string active_filename = "albums.txt";
    std::string file_error;
    bool startup_file_exists = file_exists(active_filename);
    bool automatic_save_allowed = !startup_file_exists;

    if (load_albums_from_file(
            &head, active_filename, file_error)) {
        automatic_save_allowed = true;
        std::cout << "\nAlbum data loaded successfully.\n";
    } else {
        std::cout
            << "\n" << file_error
            << "\nStarting with an empty album list.\n";

        if (!automatic_save_allowed) {
            std::cout
                << "Automatic saving is disabled to protect the existing "
                << "file. Use option 12 to save to a valid file.\n";
        }
    }

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
            handle_edit_all_album_data(head);
            break;
        case 4:
            handle_edit_album_field(head);
            break;
        case 5:
            handle_sorting_by_time(&head);
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
        case 11: {
            std::string selected_filename;

            if (!read_txt_filename(selected_filename, "load")) {
                break;
            }

            char decision;

            std::cout
                << "\nLoading data will replace the current album list. "
                << "Continue (y/n): ";

            while (!(std::cin >> decision) ||
                   (decision != 'y' && decision != 'n')) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "Invalid option. Enter 'y' or 'n': ";
            }

            if (decision == 'y') {
                if (load_albums_from_file(
                        &head, selected_filename, file_error)) {
                    active_filename = selected_filename;
                    automatic_save_allowed = true;
                    std::cout
                        << "\nAlbum data loaded successfully.\n";
                } else {
                    std::cout << "\n" << file_error << '\n';
                }
            } else {
                std::cout << "\nLoading cancelled.\n";
            }

            wait_to_go_back();
            break;
        }
        case 12: {
            std::string selected_filename;

            if (!read_txt_filename(selected_filename, "save")) {
                break;
            }

            if (!confirm_overwrite_if_needed(selected_filename)) {
                std::cout << "\nSaving cancelled.\n";
                wait_to_go_back();
                break;
            }

            if (save_albums_to_file(
                    head, selected_filename, file_error)) {
                active_filename = selected_filename;
                automatic_save_allowed = true;
                std::cout << "\nAlbum data saved successfully.\n";
            } else {
                std::cout << "\n" << file_error << '\n';
            }

            wait_to_go_back();
            break;
        }
        case 0: {
            if (head != nullptr) {
                char decision;

                std::cout
                    << "\nSave album data to the default file "
                    << "\"albums.txt\" before exiting (y/n): ";

                while (!(std::cin >> decision) ||
                       (decision != 'y' && decision != 'n')) {
                    std::cin.clear();
                    std::cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n');

                    std::cout << "Invalid option. Enter 'y' or 'n': ";
                }

                if (decision == 'y') {
                    if (save_albums_to_file(
                            head, "albums.txt", file_error)) {
                        std::cout
                            << "\nAlbum data saved successfully.\n";
                    } else {
                        std::cout << "\n" << file_error << '\n';
                        break;
                    }
                }
            }

            running = false;
            while (head != nullptr) {
                delete_album(&head, 1);
            }
            break;
        }
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
        << "11. Load data from file\n"
        << "12. Save data to file\n"
        << "0. Exit\n\n"
        << "Select an option (0 - 12): ";

        while (!(std::cin >> opt) || opt < 0 || opt > 12) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );

            std::cout 
                << "\nInvalid option. Please enter a number from 0 to 12: ";
        }
        
    return opt;
}

bool read_txt_filename(
    std::string& filename,
    const std::string& action) {

    std::cout
        << "\nEnter the TXT file name to " << action
        << " (for example, albums.txt), or 0 to go back: ";

    std::getline(std::cin >> std::ws, filename);

    while (true) {
        if (filename == "0") {
            return false;
        }

        bool valid_extension =
            filename.length() > 4 &&
            to_lower(filename.substr(filename.length() - 4)) == ".txt";

        bool valid_name =
            !is_blank(filename) &&
            filename.length() <= 100 &&
            filename.find('/') == std::string::npos &&
            filename.find('\\') == std::string::npos &&
            valid_extension;

        if (valid_name) {
            return true;
        }

        std::cout
            << "Invalid file name. Enter a simple .txt file name "
            << "or 0 to go back: ";

        std::getline(std::cin, filename);
    }
}

bool file_exists(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();
}

bool confirm_overwrite_if_needed(const std::string& filename) {
    if (!file_exists(filename)) {
        return true;
    }

    char decision;

    std::cout
        << "\nThe file \"" << filename
        << "\" already exists. Overwriting it cannot be undone. "
        << "Continue (y/n): ";

    while (!(std::cin >> decision) ||
           (decision != 'y' && decision != 'n')) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid option. Enter 'y' or 'n': ";
    }

    return decision == 'y';
}

void wait_to_go_back() {
    int option;

    std::cout << "\nEnter 0 to go back: ";

    while (!(std::cin >> option) || option != 0) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid option. Please enter 0: ";
    }
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

    duration_total_seconds = read_duration();

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

    return "Unknown";
}

bool string_to_format(const std::string& text, Format& format) {
    if (text == "CD") {
        format = CD;
    } else if (text == "Vinyl") {
        format = Vinyl;
    } else if (text == "Cassette") {
        format = Cassette;
    } else if (text == "Digital") {
        format = Digital;
    } else if (text == "DVD-Audio") {
        format = DVDAudio;
    } else if (text == "Other") {
        format = Other;
    } else {
        return false;
    }

    return true;
}

bool parse_duration(const std::string& text, int& total_seconds) {
    std::istringstream input(text);
    long long minutes;
    int seconds;
    char separator;

    if (!(input >> minutes >> separator >> seconds) ||
        separator != ':' ||
        minutes < 0 ||
        seconds < 0 ||
        seconds > 59) {
        return false;
    }

    input >> std::ws;

    if (!input.eof()) {
        return false;
    }

    const long long maximum = std::numeric_limits<int>::max();

    if (minutes > (maximum - seconds) / 60) {
        return false;
    }

    long long duration = minutes * 60 + seconds;

    if (duration == 0) {
        return false;
    }

    total_seconds = static_cast<int>(duration);
    return true;
}

int read_duration() {
    while (true) {
        long long duration_minutes;
        int duration_seconds;

        std::cout << "Enter duration minutes: ";

        while (!(std::cin >> duration_minutes) ||
               duration_minutes < 0) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout
                << "Invalid value. Enter non-negative minutes: ";
        }

        std::cout << "Enter duration seconds (0-59): ";

        while (!(std::cin >> duration_seconds) ||
               duration_seconds < 0 ||
               duration_seconds > 59) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Invalid value. Enter seconds from 0 to 59: ";
        }

        const long long maximum = std::numeric_limits<int>::max();

        if (duration_minutes >
            (maximum - duration_seconds) / 60) {
            std::cout
                << "Duration is too large. Please enter a smaller value.\n";
            continue;
        }

        long long total_seconds =
            duration_minutes * 60 + duration_seconds;

        if (total_seconds == 0) {
            std::cout
                << "Album duration must be greater than 0:00.\n";
            continue;
        }

        return static_cast<int>(total_seconds);
    }
}

Album* create_album(const std::string& name, 
                    int year, 
                    Format format, 
                    int song_quantity, 
                    int duration_total_seconds, 
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

void print_album(Album* album, int album_width, int artist_width) {
    const int minimum_album_width = 30;
    const int minimum_artist_width = 25;
    const int songs_width = 12;
    const int duration_width = 15;

    if (album_width == 0) {
        album_width = std::max(
            minimum_album_width,
            static_cast<int>(album->data.name.length()) + 2);
    }

    if (artist_width == 0) {
        artist_width = std::max(
            minimum_artist_width,
            static_cast<int>(album->data.artist.length()) + 2);
    }

    int duration_min = (album->data.duration_total_seconds / 60);
    int duration_sec = (album->data.duration_total_seconds % 60);
    
    std::string duration = std::to_string(duration_min) + ":" +
                            (duration_sec < 10 ? "0" : "") +
                            std::to_string(duration_sec);
    std::cout
        << std::left
        << std::setw(album_width) << album->data.name
        << std::setw(8)  << album->data.year
        << std::setw(15) << format_to_string(album->data.format)
        << std::setw(songs_width) << album->data.song_quantity
        << std::setw(duration_width) << duration
        << std::setw(artist_width) << album->data.artist
        << '\n';
}

int print_albums(Album* head, std::string name, int search_year) {
    const int number_width = 5;
    const int minimum_album_width = 30;
    const int year_width = 8;
    const int format_width = 15;
    const int songs_width = 12;
    const int duration_width = 15;
    const int minimum_artist_width = 25;
    int no = 1;

    std::cout 
        << "\n====================================================\n"
        << "                    LIST OF ALBUMS\n"
        << "====================================================\n\n";

    if (head == nullptr) {
        std::cout << "The album list is empty.\n";
        return 0;
    }

    std::string search_name = to_lower(name);
    auto should_print = [&](Album* album) {
        if (!name.empty()) {
            return to_lower(album->data.artist) == search_name;
        }

        if (search_year != 0) {
            return album->data.year > search_year;
        }

        return true;
    };

    int album_width = minimum_album_width;
    int artist_width = minimum_artist_width;

    for (Album* current = head;
         current != nullptr;
         current = current->next) {
        if (should_print(current)) {
            album_width = std::max(
                album_width,
                static_cast<int>(current->data.name.length()) + 2);
            artist_width = std::max(
                artist_width,
                static_cast<int>(current->data.artist.length()) + 2);
        }
    }

    std::cout
        << std::left
        << std::setw(number_width) << "No."
        << std::setw(album_width) << "Album"
        << std::setw(year_width) << "Year"
        << std::setw(format_width) << "Format"
        << std::setw(songs_width) << "Songs"
        << std::setw(duration_width) << "Duration"
        << std::setw(artist_width) << "Artist"
        << '\n';

    int table_width =
        number_width + album_width + year_width + format_width +
        songs_width + duration_width + artist_width;

    std::cout
        << std::string(static_cast<std::size_t>(table_width), '-')
        << '\n';

    for (Album* current = head;
         current != nullptr;
         current = current->next) {
        if (should_print(current)) {
            std::cout << std::setw(number_width) << no;
            print_album(current, album_width, artist_width);
            ++no;
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

void handle_sorting_by_time(Album** head) {
    char decision = ' ';
    if (head == nullptr || *head == nullptr) {
        std::cout << "\nThe album list is empty.\n";
    } else {
        std::cout << "\nAre you sure you want to sort the albums? This action will change their order and cannot be undone (y/n): ";
        while (!(std::cin >> decision) || (decision != 'y' && decision != 'n')) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "\nInvalid option. Enter 'y' or 'n': ";
        }
    
        if (decision == 'y') {
            *head = merge_sort(*head);
            std::cout << "\nAlbums were sorted successfully.\n";
        }
        
    }

    if (decision != 'n') {
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

Album* merge_sort(Album* head) {
    if (head == nullptr || head->next == nullptr) {
        return head;
    }

    Album* left = head;
    Album* right = get_middle(head);
    Album* tmp = right->next;
    right->next = nullptr;
    right = tmp;
    right->prev = nullptr;

    left = merge_sort(left);
    right = merge_sort(right);

    return merge(left, right);
}
Album* get_middle(Album* head) {
    Album* slow = head;
    Album* fast = head->next;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}
Album* merge(Album* list1, Album* list2) {
    Album dummy{};
    Album* tail = &dummy;

    while (list1 != nullptr && list2 != nullptr) {
        if (list1->data.duration_total_seconds > list2->data.duration_total_seconds) {
            tail->next = list1;
            list1->prev = tail;
            list1 = list1->next;
        } else {
            tail->next = list2;
            list2->prev = tail;
            list2 = list2->next;
        }
        tail = tail->next;
    }
    if (list1 != nullptr) {
        tail->next = list1;
        list1->prev = tail;
    }
    if (list2 != nullptr) {
        tail->next = list2;
        list2->prev = tail;
    }

    Album* result = dummy.next;
    if (result != nullptr) {
        result->prev = nullptr;
    }

    return result;
}

void handle_edit_album_field(Album* head) {
    if (head == nullptr) {
        std::cout << "\nThe album list is empty.\n";
        return;
    }

    int count = print_albums(head);
    int position;

    std::cout << "\nEnter the album position to edit, or 0 to go back: ";

    while (!(std::cin >> position) || position < 0 || position > count) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout
            << "Invalid option. Enter a number between 0 and "
            << count << ": ";
    }

    if (position == 0) {
        return;
    }

    Album* album = head;

    for (int i = 1; i < position; ++i) {
        album = album->next;
    }

    int field;

    std::cout
        << "\nSelect the field you want to change:\n"
        << "1. Name\n"
        << "2. Year\n"
        << "3. Format\n"
        << "4. Song quantity\n"
        << "5. Duration\n"
        << "6. Artist\n"
        << "0. Go back\n"
        << "Enter a number from 0 to 6: ";

    while (!(std::cin >> field) || field < 0 || field > 6) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout
            << "Invalid option. Enter a number from 0 to 6: ";
    }

    if (field == 0) {
        return;
    }

    Data updated_data = album->data;

    switch (field) {
        case 1: {
            std::string name;

            std::cout << "Enter the new album name: ";
            std::getline(std::cin >> std::ws, name);

            while (is_blank(name) || name.length() > 100) {
                if (is_blank(name)) {
                    std::cout
                        << "Name cannot be empty. Enter album name: ";
                } else {
                    std::cout
                        << "Name is too long. Enter up to 100 characters: ";
                }

                std::getline(std::cin, name);
            }

            updated_data.name = name;
            break;
        }

        case 2: {
            int year;
            int current_year = get_year();

            std::cout << "Enter the new year: ";

            while (!(std::cin >> year) ||
                   year < 1909 ||
                   year > current_year) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout
                    << "Invalid year. Enter a year from 1909 to "
                    << current_year << ": ";
            }

            updated_data.year = year;
            break;
        }

        case 3: {
            int format_option;

            std::cout
                << "Select the new format:\n"
                << "1. CD\n"
                << "2. Vinyl\n"
                << "3. Cassette\n"
                << "4. Digital\n"
                << "5. DVD-Audio\n"
                << "6. Other\n"
                << "Enter a number from 1 to 6: ";

            while (!(std::cin >> format_option) ||
                   format_option < 1 ||
                   format_option > 6) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout
                    << "Invalid option. Enter a number from 1 to 6: ";
            }

            updated_data.format =
                static_cast<Format>(format_option - 1);
            break;
        }

        case 4: {
            int song_quantity;

            std::cout << "Enter the new song quantity: ";

            while (!(std::cin >> song_quantity) ||
                   song_quantity < 1) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout
                    << "Invalid value. Enter a positive number: ";
            }

            updated_data.song_quantity = song_quantity;
            break;
        }

        case 5: {
            updated_data.duration_total_seconds = read_duration();
            break;
        }

        case 6: {
            std::string artist;

            std::cout << "Enter the new artist name: ";
            std::getline(std::cin >> std::ws, artist);

            while (is_blank(artist) || artist.length() > 100) {
                if (is_blank(artist)) {
                    std::cout
                        << "Artist name cannot be empty. "
                        << "Enter artist name: ";
                } else {
                    std::cout
                        << "Artist name is too long. "
                        << "Enter up to 100 characters: ";
                }

                std::getline(std::cin, artist);
            }

            updated_data.artist = artist;
            break;
        }
    }

    Album preview{nullptr, updated_data, nullptr};

    std::cout << "\nUpdated album information:\n";
    print_album(&preview);

    char decision;

    std::cout << "\nSave these changes (y/n): ";

    while (!(std::cin >> decision) ||
           (decision != 'y' && decision != 'n')) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid option. Enter 'y' or 'n': ";
    }

    if (decision == 'y') {
        album->data = updated_data;
        std::cout << "\nAlbum information updated successfully.\n";
    } else {
        std::cout << "\nChanges were not saved.\n";
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

void handle_edit_all_album_data(Album* head) {
    if (head == nullptr) {
        std::cout << "\nThe album list is empty.\n";
        return;
    }

    int count = print_albums(head);
    int position;

    std::cout
        << "\nEnter the album position to change, or 0 to go back: ";

    while (!(std::cin >> position) ||
           position < 0 ||
           position > count) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout
            << "Invalid option. Enter a number between 0 and "
            << count << ": ";
    }

    if (position == 0) {
        return;
    }

    Album* album = head;

    for (int i = 1; i < position; ++i) {
        album = album->next;
    }

    Data updated_data;

    std::cout << "\nEnter the new album name: ";
    std::getline(std::cin >> std::ws, updated_data.name);

    while (is_blank(updated_data.name) ||
           updated_data.name.length() > 100) {
        if (is_blank(updated_data.name)) {
            std::cout
                << "Name cannot be empty. Enter album name: ";
        } else {
            std::cout
                << "Name is too long. Enter up to 100 characters: ";
        }

        std::getline(std::cin, updated_data.name);
    }

    int current_year = get_year();

    std::cout << "Enter the new year: ";

    while (!(std::cin >> updated_data.year) ||
           updated_data.year < 1909 ||
           updated_data.year > current_year) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout
            << "Invalid year. Enter a year from 1909 to "
            << current_year << ": ";
    }

    int format_option;

    std::cout
        << "Select the new format:\n"
        << "1. CD\n"
        << "2. Vinyl\n"
        << "3. Cassette\n"
        << "4. Digital\n"
        << "5. DVD-Audio\n"
        << "6. Other\n"
        << "Enter a number from 1 to 6: ";

    while (!(std::cin >> format_option) ||
           format_option < 1 ||
           format_option > 6) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout
            << "Invalid option. Enter a number from 1 to 6: ";
    }

    updated_data.format =
        static_cast<Format>(format_option - 1);

    std::cout << "Enter the new song quantity: ";

    while (!(std::cin >> updated_data.song_quantity) ||
           updated_data.song_quantity < 1) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout
            << "Invalid value. Enter a positive number: ";
    }

    updated_data.duration_total_seconds = read_duration();

    std::cout << "Enter the new artist name: ";
    std::getline(std::cin >> std::ws, updated_data.artist);

    while (is_blank(updated_data.artist) ||
           updated_data.artist.length() > 100) {
        if (is_blank(updated_data.artist)) {
            std::cout
                << "Artist name cannot be empty. Enter artist name: ";
        } else {
            std::cout
                << "Artist name is too long. "
                << "Enter up to 100 characters: ";
        }

        std::getline(std::cin, updated_data.artist);
    }

    Album preview{nullptr, updated_data, nullptr};

    std::cout << "\nNew album information:\n";
    print_album(&preview);

    char decision;

    std::cout << "\nSave these changes (y/n): ";

    while (!(std::cin >> decision) ||
           (decision != 'y' && decision != 'n')) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid option. Enter 'y' or 'n': ";
    }

    if (decision == 'y') {
        album->data = updated_data;
        std::cout << "\nAlbum information updated successfully.\n";
    } else {
        std::cout << "\nChanges were not saved.\n";
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

bool save_albums_to_file(
    Album* head,
    const std::string& filename,
    std::string& error) {

    const std::string temporary_filename = filename + ".tmp";
    std::ofstream file(
        temporary_filename,
        std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        error = "Could not create a temporary file for saving \"" +
                filename + "\".";
        return false;
    }

    file << "Album Year Format Songs Duration Artist\n";

    Album* current = head;

    while (current != nullptr) {
        int duration_minutes =
            current->data.duration_total_seconds / 60;
        int duration_seconds =
            current->data.duration_total_seconds % 60;
        std::string duration =
            std::to_string(duration_minutes) + ":" +
            (duration_seconds < 10 ? "0" : "") +
            std::to_string(duration_seconds);

        file
            << std::quoted(current->data.name) << ' '
            << current->data.year << ' '
            << format_to_string(current->data.format) << ' '
            << current->data.song_quantity << ' '
            << duration << ' '
            << std::quoted(current->data.artist) << '\n';

        if (!file) {
            file.close();
            std::remove(temporary_filename.c_str());
            error = "An error occurred while writing to \"" +
                    filename + "\".";
            return false;
        }

        current = current->next;
    }

    file.close();

    if (!file) {
        std::remove(temporary_filename.c_str());
        error = "Could not finish writing to \"" + filename + "\".";
        return false;
    }

    if (std::rename(
            temporary_filename.c_str(),
            filename.c_str()) != 0) {
        std::remove(temporary_filename.c_str());
        error = "Could not replace \"" + filename +
                "\" with the saved data.";
        return false;
    }

    error.clear();
    return true;
}

bool load_albums_from_file(
    Album** head,
    const std::string& filename,
    std::string& error) {

    if (head == nullptr) {
        error = "Invalid list pointer.";
        return false;
    }

    std::ifstream file(filename);

    if (!file.is_open()) {
        error = "The data file \"" + filename +
                "\" does not exist or cannot be opened.";
        return false;
    }

    std::string header;

    if (!std::getline(file, header) ||
        header != "Album Year Format Songs Duration Artist") {
        error = "The data file has an invalid header.";
        return false;
    }

    Album* loaded_head = nullptr;
    Album* loaded_tail = nullptr;

    std::string line;
    int line_number = 1;
    const int current_year = get_year();

    while (std::getline(file, line)) {
        ++line_number;

        if (is_blank(line)) {
            continue;
        }

        std::istringstream input(line);

        std::string name;
        std::string artist;
        std::string format_text;
        std::string duration_text;
        int year;
        int song_quantity;
        int duration_total_seconds = 0;
        Format format = Other;

        bool valid = static_cast<bool>(
            input
            >> std::quoted(name)
            >> year
            >> format_text
            >> song_quantity
            >> duration_text
            >> std::quoted(artist));

        if (valid) {
            valid =
                string_to_format(format_text, format) &&
                parse_duration(
                    duration_text, duration_total_seconds);
        }

        if (valid) {
            input >> std::ws;

            if (!input.eof()) {
                valid = false;
            }
        }

        if (valid) {
            valid =
                !is_blank(name) &&
                name.length() <= 100 &&
                year >= 1909 &&
                year <= current_year &&
                song_quantity > 0 &&
                duration_total_seconds > 0 &&
                !is_blank(artist) &&
                artist.length() <= 100;
        }

        if (!valid) {
            while (loaded_head != nullptr) {
                delete_album(&loaded_head, 1);
            }

            error =
                "Malformed album data on line " +
                std::to_string(line_number) + ".";

            return false;
        }

        Album* album = create_album(
            name,
            year,
            format,
            song_quantity,
            duration_total_seconds,
            artist);

        if (loaded_head == nullptr) {
            loaded_head = album;
            loaded_tail = album;
        } else {
            loaded_tail->next = album;
            album->prev = loaded_tail;
            loaded_tail = album;
        }
    }

    if (file.bad()) {
        while (loaded_head != nullptr) {
            delete_album(&loaded_head, 1);
        }

        error = "An error occurred while reading \"" +
                filename + "\".";

        return false;
    }

    while (*head != nullptr) {
        delete_album(head, 1);
    }

    *head = loaded_head;

    error.clear();
    return true;
}
