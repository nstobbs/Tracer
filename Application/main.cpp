#include "Application/Application.hpp"
#include <iostream>

int main(int argc, char **argv) {
    Application App(ApplicationSettings(argc, argv));
    return EXIT_SUCCESS;
}