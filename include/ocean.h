#pragma once
#include <tesla.hpp>
#include <dmntcht.h>

class Ocean
{
public:
    enum RomType : long unsigned int
    {
        JP = 0x01003C700009C000,
        US = 0x01003BC0000A0000,
        EU = 0x0100F8F0000A2000
    };

    u32 game_random_seed;
    bool fix_seed_flag;
    Ocean::RomType rom_type;

    Ocean(DmntCheatProcessMetadata);
    bool check_if_seed_fixed();
    u32 read_seed_fixed();
    void write_game_random_seed(u32);
    void restore_game_random_seed();
    void update_seed();
    void fix_seed_toggle(bool);

private:
    u32 base;
};
