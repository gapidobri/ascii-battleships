#include <iostream>
#include <iomanip>
#include <math.h>
#include <ncurses.h>

#define ship_count 6

#define p_empty '.'
#define p_ship 'o'
#define p_sunk 'x'
#define p_miss '_'
#define p_shoot '*'

#define endl "\n\r"

using namespace std;

void clean(char a[][10]);
void join(char a[][10], char b[][10]);

void place_random_ship(char a[][10], int length);

int taken_f(char a[][10], int x, int y);
bool free_f(char a[][10], int x, int y);
char get_f(char a[][10], int x, int y);
void set_f(char a[][10], int x, int y, char field);

// Player
void player_set_ships(char pl[][10], int &x_last, int &y_last, int ships[ship_count]);
void player_set_ship(char pl[][10], int length, int &x_last, int &y_last);
bool player_shoot(char pc[][10], char pc_pl[][10], int x, int y);
void player_step(char pl[][10], char pc_pl[][10], char pc[][10], int &x_last, int &y_last);

// PC
void pc_set_ships(char pc[][10], int ships[ship_count]);
bool pc_shoot(char pl[][10], char pc[][10]);
void pc_step(char pl[][10], char pc[][10]);

bool place_ship(char a[][10], int &x, int &y, int ship_length, int direction);

void display(char a[][10]);
void display_boards(char pl[][10], char pc_pl[][10]);

float random(int min, int max);
void random_direction(int &x, int &y);
void set_direction(int &x, int &y, int direction);

void move_with_keys(int &x, int &y, int &direction, bool &place);

bool inbounds(int x, int y);
void set_inbounds(int &x, int &y, int x_ofs, int y_ofs);
bool all_sunk(char a[][10]);

void game(int ships[ship_count]);

int main() {

    int ships[ship_count] = {3, 2, 2, 1, 1, 1};

    srand(time(NULL));
    bool play;

    do {
        system("clear");
        game(ships);
        cout << "Do you want to play another game? [y/N]: ";
        char answer;
        play = false;
        cin >> answer;
        if (answer == 'y')
            play = true;
    } while (play);

    return 0;
    
}

void game(int ships[ship_count]) {
    
    char player[10][10], pc_player[10][10], pc[10][10];
    int x_last = 3, y_last = 5;
    clean(player);
    clean(pc_player);
    clean(pc);

    pc_set_ships(pc, ships);
    player_set_ships(player, x_last, y_last, ships);

    while (true) {

        player_step(player, pc_player, pc, x_last, y_last);
        if (all_sunk(pc))
            break;

        pc_step(player, pc);
        if (all_sunk(player))
            break;

    }

    if (all_sunk(player)) {
        cout << "You have lost!" << endl;
    } else if (all_sunk(pc)) {
        cout << "You have won!" << endl;
    }
}

void clean(char a[][10]) {
    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            set_f(a, x, y, p_empty);
}

void join(char a[][10], char b[][10]) {
    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            if (get_f(b, x, y) != p_empty)
                set_f(a, x, y, get_f(b, x, y));
}

void display_boards(char pl[][10], char pc_pl[][10]) {
    cout << "Your table:" << endl;
    display(pl);

    cout << endl << "PC's table:" << endl;
    display(pc_pl);
}

void player_step(char pl[][10], char pc_pl[][10], char pc[][10], int &x_last, int &y_last) {
    int x = x_last, y = y_last, direction = 0;
    bool place = false;
    char tmp[10][10];

    while (!place) {
        clean(tmp);
        join(tmp, pc_pl);
        set_inbounds(x, y, x, y);
        set_f(tmp, x, y, p_shoot);
        display_boards(pl, tmp);
        cout << endl;
        cout << "W, A, S, D -> Move" << endl;
        cout << "E -> Shoot" << endl << endl;
        move_with_keys(x, y, direction, place);

    }

    x_last = x;
    y_last = y;
    player_shoot(pc, pc_pl, x, y);
}

bool player_shoot(char pc[][10], char pc_pl[][10], int x, int y) {
    if (get_f(pc, x, y) == p_ship) {
        set_f(pc, x, y, p_sunk);
        set_f(pc_pl, x, y, p_sunk);
        return true;
    } else if (get_f(pc, x, y) == p_empty)
        set_f(pc_pl, x, y, p_miss);
    return false;
}

void player_set_ships(char pl[][10], int &x_last, int &y_last, int ships[]) {
    for (int i = 0; i < 6; i++)
        player_set_ship(pl, ships[i], x_last, y_last);
    system("clear");
}

void player_set_ship(char pl[][10], int length, int &x_last, int &y_last) {
    int x = x_last, y = y_last, direction = 0;
    bool place = false, valid = true;
    char tmp[10][10];

    while (!(place && valid)) {        

        cout << "W, A, S, D -> Move ship" << endl;
        cout << "Q -> Rotate ship" << endl;
        cout << "E -> Place ship" << endl;

        place = false;
        move_with_keys(x, y, direction, place);

        clean(tmp);
        join(tmp, pl);
        set_inbounds(x, y, x, y);
        valid = place_ship(tmp, x, y, length, direction);

        display(tmp);

        if (!valid) {
            cout << "Invalid position" << endl;
        }

    }

    x_last = x;
    y_last = y;
    join(pl, tmp);
}

void move_with_keys(int &x, int &y, int &direction, bool &place) {
    initscr();
    int key = getch();
    system("clear");

    switch (key) {
        case 'W':
        case 'w':
            y--;
            break;
        case 'S':
        case 's':
            y++;
            break;
        case 'A':
        case 'a':
            x--;
            break;
        case 'D':
        case 'd':
            x++;
            break;
        case 'E':
        case 'e':
            place = true;
            break;
        case 'Q':
        case 'q':
            direction++;
            if (direction >= 4)
                direction = 0;
            break;
    }

    cout << endl;
}

void pc_step(char pl[][10], char pc[][10]) {
    pc_shoot(pl, pc);
}

void pc_set_ships(char pc[][10], int ships[ship_count]) {
    for (int i = 0; i < 6; i++)
        place_random_ship(pc, ships[i]);
}

bool pc_shoot(char pl[][10], char pc[][10]) {
    int x, y;
    do {
        x = round(random(0, 9));
        y = round(random(0, 9));
    } while (get_f(pl, x, y) == p_miss || get_f(pl, x, y) == p_sunk);

    if (get_f(pl, x, y) == p_ship) {
        set_f(pl, x, y, p_sunk);
        return true;
    }
    else if (get_f(pl, x, y) == p_empty)
        set_f(pl, x, y, p_miss);
    return false;
}

void place_random_ship(char a[][10], int ship_length) {
    int x, y, x_dir, y_dir, field_count = 0;
    char tmp[10][10];
    clean(tmp);

    random_direction(x_dir, y_dir);
    
    do {
        x = round(random(0, 9));
        y = round(random(0, 9));
    } while (taken_f(a, x, y));

    while (field_count < ship_length) {
        set_f(tmp, x, y, p_ship);
        field_count++;

        x += x_dir;
        y += y_dir;

        if (taken_f(a, x, y) || !inbounds(x, y))
            break;
    }

    if (field_count < ship_length) {
        place_random_ship(a, ship_length);
    } else {
        join(a, tmp);
    }
}

bool place_ship(char a[][10], int &x, int &y, int ship_length, int direction) {
    int xi = x, yi = y, x_dir, y_dir, field_count = 0;
    bool valid = true;
    char tmp[10][10];
    clean(tmp);

    set_direction(x_dir, y_dir, direction);

    while (field_count < ship_length) {
        set_f(tmp, xi, yi, p_ship);
       
        field_count++;

        if (taken_f(a, xi, yi))
            valid = false;

        xi += x_dir;
        yi += y_dir;


        if (!inbounds(xi, yi))
            break;
    }

    if (field_count < ship_length) {
        set_inbounds(x, y, xi, yi);
        return place_ship(a, x, y, ship_length, direction);
    } else {
        join(a, tmp);
        return valid;
    }
}

void display(char a[][10]) {
    for (int y = -1; y < 11; y++) {
        for (int x = -1; x < 11; x++) {

            if (x == 10)
                if (y == 10)
                    cout << "_|";
                else if (y == -1)
                    cout << "_";
                else
                    cout << setw(2) << "|";
            else if (y == -1)
                cout << "__";
            else if (x == -1)
                cout << "|";
            else if (y == 10)
                cout << "__";
            else
                cout << setw(2) << get_f(a, x, y);
        }
        cout << endl;
    }
}

char get_f(char a[][10], int x, int y) {
    return a[y][x];
}

void set_f(char a[][10], int x, int y, char field) {
    a[y][x] = field;
}

bool free_f(char a[][10], int x, int y) {
    return a[y][x] == p_empty;
}

int taken_f(char a[][10], int x, int y) {
    int fields = 0;
    for (int yi = (y - 1); yi < (y + 2); yi++)
        for (int xi = (x - 1); xi < (x + 2); xi++)
            if (get_f(a, xi, yi) == p_ship)
                fields++;
    return fields;
}

float random(int min, int max) {
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}

void random_direction(int &x, int &y) {
    set_direction(x, y, round(random(0, 2)));
}

// Direction: 0 - right, 1 - diagonal right, 2 - up, 3 - diagonal left
void set_direction(int &x, int &y, int direction) {
    switch (direction) {
        case 1:
            x = 1;
            y = -1;
            break;
        case 2:
            x = 0;
            y = -1;
            break;
        case 3:
            x = -1;
            y = -1;
            break;
        default:
            x = 1;
            y = 0;
    }
}

bool inbounds(int x, int y) {
    return x >= 0 && x <= 9 && y >= 0 && y <= 9;
}

void set_inbounds(int &x, int &y, int x_ofs, int y_ofs) {
    int x_inb = max(min(x_ofs, 9), 0);
    int y_inb = max(min(y_ofs, 9), 0);

    x += x_inb - x_ofs;
    y += y_inb - y_ofs;
}

bool all_sunk(char a[][10]) {
    bool all_sunk = true;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++)
            if (a[i][j] == p_ship)
                all_sunk = false;
    }
    return all_sunk;
}
