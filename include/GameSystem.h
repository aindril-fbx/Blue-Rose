#pragma once

struct Game {
    const char* Fname;
    const char* Lname;
    const unsigned char* Icon;
    void (*init)();
    void (*update)();
    void (*cleanup)();
};