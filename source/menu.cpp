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

    SeedSearch::SeedSearch(u32 *game_random_seed)
    {
        this->mt = std::mt19937(time(NULL));
        this->game_random_seed = game_random_seed;
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
        for (u32 loop = 0; loop <= 0x100000; loop++)
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

    // Update game random seed
    void OceanModifier::update_seed_inst()
    {
        // if (fix_seed_flag)
        //     writeDataToMemory();
        // else
        //     restoreDataFromMemory();
    }

    // Toggle fix seed flag
    void OceanModifier::fix_seed_toggle(bool state)
    {
        fix_seed_flag = state;
        update_seed_inst();
    }

    // OceanModifier::OceanModifier(DmntCheatProcessMetadata metadata)
    // {
    // }

    // // Get Hex string
    // std::string OceanModifier::convert_u32_to_hex(u32 game_random_seed)
    // {
    //     std::stringstream stream;
    //     stream << std::uppercase;
    //     stream << std::setfill('0') << std::setw(8) << std::hex << game_random_seed;
    //     return "0x" + stream.str();
    // }

    // // Get Hex string
    // std::string OceanModifier::convert_u64_to_hex(u64 address)
    // {
    //     std::stringstream stream;
    //     stream << std::uppercase;
    //     stream << std::setfill('0') << std::setw(16) << std::hex << address;
    //     return "0x" + stream.str();
    // }

    bool OceanModifier::fix_seed_flag = false;
}
