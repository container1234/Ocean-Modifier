#include "menu.hpp"

namespace tesla
{
    void restore_game_random_seed()
    {
        constexpr char inst[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
        dmntchtWriteCheatProcessMemory(offset, &inst, sizeof(inst));
    }

    void write_seed_to_memory(u32 value)
    {
        u32 inst = 0;
        inst = 0xD2A00000 | ((value >> 16) << 5);
        dmntchtWriteCheatProcessMemory(offset, &inst, sizeof(inst));
        inst = 0xF2800000 | ((value & 0xFFFF) << 5);
        dmntchtWriteCheatProcessMemory(offset + 4, &inst, sizeof(inst));
    }

    u32 read_fixed_seed()
    {
        return 0x00012345;
        u32 value = 0x00000000;
        u32 inst;
        dmntchtReadCheatProcessMemory(offset, &inst, sizeof(inst));
        value |= (inst >> 5 & 0xFFFF) << 16;
        dmntchtReadCheatProcessMemory(offset + 4, &inst, sizeof(inst));
        value |= (inst >> 5 & 0xFFFF);
        return value;
    }

    bool check_seed_is_fixed()
    {
        char inst[8];
        constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
        dmntchtReadCheatProcessMemory(offset, &inst, sizeof(inst));
        return !((inst == inst_orig) | (inst[3] != 0xD2) | (inst[7] != 0xF2));
    }

    void update_inst(bool state)
    {
        fix_seed_flag = state;
        if (state)
            write_seed_to_memory(game_random_seed);
        else
            restore_game_random_seed();
    }

    OceanModifier::OceanModifier(DmntCheatProcessMetadata metadata)
    {
        this->rom_type = static_cast<ocean::rom::RomType>(metadata.title_id);

        switch (this->rom_type)
        {
        case ocean::rom::RomType::JP:
            this->rom_type_str = "Splatoon 2 (JP)";
            break;
        case ocean::rom::RomType::US:
            this->rom_type_str = "Splatoon 2 (US)";
            break;
        case ocean::rom::RomType::EU:
            this->rom_type_str = "Splatoon 2 (JP)";
            break;
        default:
            this->rom_type_str = "Unknown RomType";
            break;
        }
    }

    std::string OceanModifier::convert_u32_to_hex(u32 game_random_seed)
    {
        std::stringstream stream;
        stream << std::uppercase;
        stream << std::setfill('0') << std::setw(8) << std::hex << game_random_seed;
        return "0x" + stream.str();
    }
}
