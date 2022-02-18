#include "menu.hpp"

namespace tesla
{
    bool check_if_seed_fixed()
    {
        char inst[8];
        constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
        dmntchtReadCheatProcessMemory(base + SEED_INST_OFFSET, &inst, sizeof(inst));
        return !((inst == inst_orig) | (inst[3] != 0xD2) | (inst[7] != 0xF2));
    }

    u32 read_seed_inst()
    {
        u32 value = 0;
        u32 inst;
        dmntchtReadCheatProcessMemory(base + SEED_INST_OFFSET, &inst, sizeof(inst));
        value |= (inst >> 5 & 0xFFFF) << 16;
        dmntchtReadCheatProcessMemory(base + SEED_INST_OFFSET + 4, &inst, sizeof(inst));
        value |= (inst >> 5 & 0xFFFF);
        return value;
    }

    void OceanModifier::write_seed_inst(u32 value)
    {
        u32 inst = 0;
        inst = 0xD2A00000 | ((value >> 16) << 5);
        dmntchtWriteCheatProcessMemory(base + SEED_INST_OFFSET, &inst, sizeof(inst));
        inst = 0xF2800000 | ((value & 0xFFFF) << 5);
        dmntchtWriteCheatProcessMemory(base + SEED_INST_OFFSET + 4, &inst, sizeof(inst));
    }

    void OceanModifier::restore_seed_inst()
    {
        constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
        dmntchtWriteCheatProcessMemory(base + SEED_INST_OFFSET, &inst_orig, sizeof(inst_orig));
    }

    void OceanModifier::update_seed_inst()
    {
        if (fix_seed_flag)
            write_seed_inst(game_random_seed);
        else
            restore_seed_inst();
    }

    void OceanModifier::fix_seed_toggle(bool state)
    {
        fix_seed_flag = state;
        update_seed_inst();
    }

    OceanModifier::OceanModifier(DmntCheatProcessMetadata metadata)
    {
        // this->metadata = metadata;
        base = metadata.main_nso_extents.base;
        title_id = metadata.title_id;
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

    std::string OceanModifier::convert_u64_to_hex(u64 address)
    {
        std::stringstream stream;
        stream << std::uppercase;
        stream << std::setfill('0') << std::setw(16) << std::hex << address;
        return "0x" + stream.str();
    }

    bool OceanModifier::fix_seed_flag = false;
}
