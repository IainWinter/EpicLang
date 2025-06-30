struct Player {
    int x;
    int y;
    int z;
}

Player make_player() {
    return Player {
        x = 0;
        y = 0;
        z = 0;
    };
}

void main(int x) {
    Player p = make_player();
    p.x = 1;

    p = Player {
        x = 0;
        y = 0;
        z = 0;
    };

    while (x < 10) {
        print(to_string(x));
        x = x + 1;
    }
}