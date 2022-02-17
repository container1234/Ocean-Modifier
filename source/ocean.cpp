#include "ocean.h"
#define SEED_INST_OFFSET 0x0037F340

Ocean::Ocean(DmntCheatProcessMetadata metadata)
{
    this->base = metadata.main_nso_extents.base;
    this->rom_type = static_cast<Ocean::RomType>(metadata.title_id);
    this->game_random_seed = 0;
    this->fix_seed_flag = false;
}

bool Ocean::check_if_seed_fixed()
{
    char inst[8];
    constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
    dmntchtReadCheatProcessMemory(this->base + SEED_INST_OFFSET, &inst, sizeof(inst));
    return !((inst == inst_orig) | (inst[3] != 0xD2) | (inst[7] != 0xF2));
}

u32 Ocean::read_seed_fixed()
{
    u32 value = 0;
    u32 inst;
    dmntchtReadCheatProcessMemory(this->base + SEED_INST_OFFSET, &inst, sizeof(inst));
    value |= (inst >> 5 & 0xFFFF) << 16;
    dmntchtReadCheatProcessMemory(this->base + SEED_INST_OFFSET + 4, &inst, sizeof(inst));
    value |= (inst >> 5 & 0xFFFF);
    return value;
}

void Ocean::write_game_random_seed(u32 value)
{
    u32 inst = 0;
    inst = 0xD2A00000 | ((value >> 16) << 5);
    dmntchtWriteCheatProcessMemory(this->base + SEED_INST_OFFSET, &inst, sizeof(inst));
    inst = 0xF2800000 | ((value & 0xFFFF) << 5);
    dmntchtWriteCheatProcessMemory(this->base + SEED_INST_OFFSET + 4, &inst, sizeof(inst));
}

void Ocean::update_seed()
{
    if (fix_seed_flag)
        write_game_random_seed(game_random_seed);
    else
        restore_game_random_seed();
}

void Ocean::fix_seed_toggle(bool state)
{
    fix_seed_flag = state;
    update_seed();
}

void Ocean::restore_game_random_seed()
{
    constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
    dmntchtWriteCheatProcessMemory(this->base + SEED_INST_OFFSET, &inst_orig, sizeof(inst_orig));
}
