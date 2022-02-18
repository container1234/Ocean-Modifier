#include "menu.hpp"

namespace tesla
{
    u32 get_target_wave()
    {
        return wave.tide[0] * 100000 + wave.event[0] * 10000 + wave.tide[1] * 1000 + wave.event[1] * 100 + wave.tide[2] * 10 + wave.event[2];
    }

    std::string get_target_wave_string()
    {
        std::stringstream stream;
        stream << std::uppercase;
        stream << std::setfill('0') << std::setw(6) << get_target_wave();
        return stream.str();
    }

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

    u32 SeedSearch::search_target_seed()
    {
        u32 target_wave = get_target_wave();
        for (u32 loop = 0; loop <= 0x10000; loop++)
        {
            u32 random_seed = static_cast<u32>(mt());
            if (target_wave == get_wave_info(random_seed))
                return random_seed;
        }
        return 0x00000000;
    }

    u32 SeedSearch::get_wave_info(u32 seed)
    {
        sead::random::Random rnd = sead::random::Random(seed);
        ocean::wave::WaveType mWave = ocean::wave::WaveType();
        const std::vector<u8> mEvent = {0x12, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
        const std::vector<u8> mTide = {0x01, 0x03, 0x01};

        for (u16 wave = 0; wave < 3; ++wave)
        {
            for (u64 event = 0, sum = 0; event < 7; ++event) // Calculate Wave Event
            {
                if ((wave > 0) && (mWave.event[wave - 1] != 0) && (mWave.event[wave - 1] == event))
                    continue;
                sum += mEvent[event];
                if ((rnd.getU32() * sum >> 0x20) < mEvent[event])
                    mWave.event[wave] = event;
            }
            for (u64 tide = 0, sum = 0; tide < 3; ++tide) // Calculate Wave Tide
            {
                if ((tide == 0) && (1 <= mWave.event[wave] && (mWave.event[wave] <= 3)))
                    continue;
                sum += mTide[tide];
                if ((rnd.getU32() * sum >> 0x20) < mTide[tide])
                    mWave.event[wave] == 6 ? mWave.tide[wave] = 0 : mWave.tide[wave] = tide;
            }
        }
        return mWave.tide[0] * 100000 + mWave.event[0] * 10000 + mWave.tide[1] * 1000 + mWave.event[1] * 100 + mWave.tide[2] * 10 + mWave.event[2] * 1;
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
