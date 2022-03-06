#include <tesla.hpp>
#include <dmntcht.h>
#include <string>
#include <vector>

namespace ocean
{
    namespace region
    {
        enum class Region : long unsigned int
        {
            JP = 0x01003C700009C000,
            US = 0x01003BC0000A0000,
            EU = 0x0100F8F0000A2000
        };

        std::string rom_region_string(Region);
    }

    namespace version
    {
        enum class RomVer : long unsigned int
        {
            v550 = 0x1729DD426870976B,
            v540 = 0x32A8F031861B76DF,
            v531 = 0xCF91518983FCB18D,
            v530 = 0xB9D6759587833635,
            v522 = 0x8A7F4570B0D5D94C,
            v521 = 0x2FA49C6FAC6BEBFA,
            v520 = 0xDA404117DA4A7D6C,
            v510 = 0xD3C62FCA532DB432,
            v501 = 0xEFCC50C2257D55E2,
            v500 = 0x26D72E09076C3B11,
            v310 = 0x034C8FA7A63B7A87,
        };

        std::string rom_version_string(RomVer);
        u64 get_address_offset(RomVer);
    }

    namespace system
    {
        class System
        {
        public:
            System(DmntCheatProcessMetadata);
            std::string getRegion();
            region::Region getRegionType();
            std::string getRegionText();
            std::string getVersion();
            version::RomVer getVersionType();
            std::string getVersionText();
            std::string getBuildId();
            std::string getRomTypeAndRegion();
            std::string getBaseAddress();
            u64 getTargetAddress();
            u32 getGameRandomSeed();
            std::string getGameTitleIdText();
            std::string getGameRandomSeedText();
            u32 *getGameRandomSeedAddress();
            bool isGameRunning();

            // Memory
            void writeDataToMemory();
            void restoreDataFromMemory();
            u32 readDataFromMemory();
            std::string getGameRandomSeedTextFromMemory();

        protected:
            u64 base;

        private:
            ocean::region::Region region;
            ocean::version::RomVer version;
            std::string rom_version_string;
            std::string rom_region_string;
            std::string rom_version_and_region;
            u64 offset;
            u64 build_id;
            u64 title_id;
            u32 game_random_seed;
            std::string convert_u64_to_hex(u64);
            std::string convert_u32_to_hex(u32);
        };
    }
    // u64 ocean::system::System::base = 0x00000000;
}
