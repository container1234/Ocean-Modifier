#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include "menu.hpp"     // The Menu Header
#include "main.hpp"     // The Main Header
#include <dmntcht.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#define GAME_BUILD_ID 0x1729DD426870976B

int main(int argc, char **argv)
{
    return tsl::loop<OverlayOcean>(argc, argv);
}
