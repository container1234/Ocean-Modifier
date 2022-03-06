#include <tesla.hpp>
#include "status.hpp"
#include <fstream>
#include <iomanip>

namespace ocean
{
    namespace region
    {
        std::string rom_region_string(Region region)
        {
            switch (region)
            {
            case Region::JP:
                return "Splatoon2 JP";
            case Region::US:
                return "Splatoon2 US";
            case Region::EU:
                return "Splatoon2 EU";
            default:
                return "Unknown RomType";
            }
        }
    }

    namespace version
    {
        std::string rom_version_string(version::RomVer version)
        {
            switch (version)
            {
            case version::RomVer::v550:
                return "5.5.0";
            case version::RomVer::v540:
                return "5.4.0";
            case version::RomVer::v531:
                return "5.3.1";
            case version::RomVer::v530:
                return "5.3.0";
            case version::RomVer::v522:
                return "5.2.2";
            case version::RomVer::v521:
                return "5.2.1";
            case version::RomVer::v520:
                return "5.2.0";
            case version::RomVer::v510:
                return "5.1.0";
            case version::RomVer::v501:
                return "5.0.1";
            case version::RomVer::v500:
                return "5.0.0";
            case version::RomVer::v310:
                return "3.1.0";
            default:
                return "Unknown";
            }
        }

        u64 get_address_offset(version::RomVer version)
        {
            switch (version)
            {
            case version::RomVer::v550:
                return 0x0037F340;
            case version::RomVer::v540:
                return 0x0037F340;
            case version::RomVer::v531:
                return 0x0037F340;
            case version::RomVer::v530:
                return 0x003753B0;
            case version::RomVer::v522:
                return 0x003753B0;
            case version::RomVer::v521:
                return 0x003753B0;
            case version::RomVer::v520:
                return 0x003753B0;
            case version::RomVer::v510:
                return 0x0036C0C0;
            case version::RomVer::v501:
                return 0x00376810;
            case version::RomVer::v500:
                return 0x00363830;
            case version::RomVer::v310:
                return 0x00208C74;
            default:
                return 0x00000000;
            }
        }
    }

    namespace system
    {
        System::System(DmntCheatProcessMetadata metadata)
        {
            // Version/Region Enum
            this->region = static_cast<region::Region>(metadata.title_id);
            u64 build_id = 0;
            std::memcpy(&build_id, metadata.main_nso_build_id, sizeof(u64));
            this->build_id = __builtin_bswap64(build_id);
            this->version = static_cast<version::RomVer>(this->build_id);
            this->title_id = metadata.title_id;
            // Offset/Base
            this->base = metadata.main_nso_extents.base;
            this->offset = ocean::version::get_address_offset(this->version);
            // Version/Region Text
            this->rom_version_string = ocean::version::rom_version_string(this->version);
            this->rom_region_string = ocean::region::rom_region_string(this->region);
            this->rom_version_and_region = this->rom_region_string + " " + this->rom_version_string;
        }

        std::string System::convert_u64_to_hex(u64 value)
        {
            std::stringstream stream;
            stream << std::uppercase;
            stream << std::setfill('0') << std::setw(16) << std::hex << value;
            return "0x" + stream.str();
        }

        std::string System::convert_u32_to_hex(u32 value)
        {
            std::stringstream stream;
            stream << std::uppercase;
            stream << std::setfill('0') << std::setw(8) << std::hex << value;
            return "0x" + stream.str();
        }

        std::string System::getRegion()
        {
            return convert_u64_to_hex(static_cast<u64>(this->region));
        }

        region::Region System::getRegionType()
        {
            return this->region;
        }

        std::string System::getRegionText()
        {
            return this->rom_region_string;
        }

        std::string System::getVersion()
        {
            return convert_u64_to_hex(static_cast<u64>(this->version));
        }

        version::RomVer System::getVersionType()
        {
            return this->version;
        }

        std::string System::getVersionText()
        {
            return this->rom_version_string;
        }

        std::string System::getBuildId()
        {
            return convert_u64_to_hex(this->build_id);
        }

        std::string System::getRomTypeAndRegion()
        {
            return this->rom_version_and_region;
        }

        std::string System::getBaseAddress()
        {
            return convert_u64_to_hex(this->base);
        }

        u64 System::getTargetAddress()
        {
            return this->base + this->offset;
        }

        u32 System::getGameRandomSeed()
        {
            return this->game_random_seed;
        }

        std::string System::getGameRandomSeedText()
        {
            return convert_u32_to_hex(this->game_random_seed);
        }

        std::string System::getGameTitleIdText()
        {
            return convert_u64_to_hex(this->title_id);
        }

        u32 *System::getGameRandomSeedAddress()
        {
            return &this->game_random_seed;
        }

        u32 System::readDataFromMemory()
        {
            u64 target = this->getTargetAddress();
            u32 value = 0;
            u32 inst;
            dmntchtReadCheatProcessMemory(target, &inst, sizeof(inst));
            value |= (inst >> 5 & 0xFFFF) << 16;
            dmntchtReadCheatProcessMemory(target + 4, &inst, sizeof(inst));
            value |= (inst >> 5 & 0xFFFF);
            return value;
        }

        std::string System::getGameRandomSeedTextFromMemory()
        {
            return convert_u32_to_hex(this->readDataFromMemory());
        }

        void System::writeDataToMemory()
        {
            u32 game_random_seed = this->game_random_seed;
            u64 target = this->getTargetAddress();
            // Convert Int to Hex
            u32 inst = 0;
            inst = 0xD2A00000 | ((game_random_seed >> 16) << 5);
            dmntchtWriteCheatProcessMemory(target, &inst, sizeof(inst));
            inst = 0xF2800000 | ((game_random_seed & 0xFFFF) << 5);
            dmntchtWriteCheatProcessMemory(target + 4, &inst, sizeof(inst));
        }

        void System::restoreDataFromMemory()
        {
            u64 target = this->getTargetAddress();
            constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
            dmntchtWriteCheatProcessMemory(target, &inst_orig, sizeof(inst_orig));
        }

        bool System::isGameRunning()
        {
            return (this->region == region::Region::JP || this->region == region::Region::US || this->region == region::Region::EU);
        }
    }
}
