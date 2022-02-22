#include "menu.hpp"
#include <fstream>

namespace tesla
{
    u32 SeedSearch::get_target_wave()
    {
        return config.tide[0] * 100000 + config.event[0] * 10000 + config.tide[1] * 1000 + config.event[1] * 100 + config.tide[2] * 10 + config.event[2];
    }

    std::string SeedSearch::get_target_wave_string()
    {
        std::stringstream stream;
        stream << std::uppercase;
        stream << std::setfill('0') << std::setw(6) << get_target_wave();
        return stream.str();
    }

    // Check game random seed is fixed
    bool check_if_seed_fixed()
    {
        char inst[8];
        constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
        dmntchtReadCheatProcessMemory(base + SEED_INST_OFFSET, &inst, sizeof(inst));
        return !((inst == inst_orig) | (inst[3] != 0xD2) | (inst[7] != 0xF2));
    }

    // Read data from memory
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

    SeedSearch::SeedSearch()
    {
        this->mt = std::mt19937(time(NULL));
        // Load config.json
        this->load();
    }

    // Save config.json
    void SeedSearch::write()
    {
        const char *CONFIG_FILE = "/config/ocean/config.json";
        tsl::hlp::doWithSDCardHandle([&]()
                                     {
            std::ofstream ofs(CONFIG_FILE, std::ofstream::out | std::ofstream::trunc);
            nlohmann::json j = config;
            std::string jsonString = j.dump(4);
            ofs << jsonString << std::endl; });
    }

    // Load config.json
    void SeedSearch::load()
    {
        const char *CONFIG_FILE = "/config/ocean/config.json";
        tsl::hlp::doWithSDCardHandle([&]()
                                     { 
            std::ifstream ifs(CONFIG_FILE);
            std::string data = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
            nlohmann::json json = nlohmann::json::parse(data);
            this->config = json.get<ocean::Config>(); });
    }

    // Search target seed
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

    // Calculate wave info
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

    // Write data to memory
    void OceanModifier::write_seed_inst(u32 value)
    {
        u32 inst = 0;
        inst = 0xD2A00000 | ((value >> 16) << 5);
        dmntchtWriteCheatProcessMemory(base + SEED_INST_OFFSET, &inst, sizeof(inst));
        inst = 0xF2800000 | ((value & 0xFFFF) << 5);
        dmntchtWriteCheatProcessMemory(base + SEED_INST_OFFSET + 4, &inst, sizeof(inst));
    }

    // Restore original data
    void OceanModifier::restore_seed_inst()
    {
        constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
        dmntchtWriteCheatProcessMemory(base + SEED_INST_OFFSET, &inst_orig, sizeof(inst_orig));
    }

    // Update game random seed
    void OceanModifier::update_seed_inst()
    {
        if (fix_seed_flag)
            write_seed_inst(game_random_seed);
        else
            restore_seed_inst();
    }

    // Toggle fix seed flag
    void OceanModifier::fix_seed_toggle(bool state)
    {
        fix_seed_flag = state;
        update_seed_inst();
    }

    OceanModifier::OceanModifier(DmntCheatProcessMetadata metadata)
    {
        base = metadata.main_nso_extents.base;
        title_id = metadata.title_id;
        this->status = new ocean::status::Status(metadata);
    }

    // Get Hex string
    std::string OceanModifier::convert_u32_to_hex(u32 game_random_seed)
    {
        std::stringstream stream;
        stream << std::uppercase;
        stream << std::setfill('0') << std::setw(8) << std::hex << game_random_seed;
        return "0x" + stream.str();
    }

    // Get Hex string
    std::string OceanModifier::convert_u64_to_hex(u64 address)
    {
        std::stringstream stream;
        stream << std::uppercase;
        stream << std::setfill('0') << std::setw(16) << std::hex << address;
        return "0x" + stream.str();
    }

    bool OceanModifier::fix_seed_flag = false;
}
