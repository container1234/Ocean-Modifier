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

        u64 address_offset(version::RomVer version)
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

    namespace status
    {
        Status::Status(DmntCheatProcessMetadata metadata)
        {
            this->region = static_cast<region::Region>(metadata.title_id);
            u64 build_id = 0;
            std::memcpy(&build_id, metadata.main_nso_build_id, sizeof(u64));
            this->build_id = __builtin_bswap64(build_id);
            this->version = static_cast<version::RomVer>(this->build_id);
            // this->offset = this->version->address_offset();
            this->rom_version_string = ocean::version::rom_version_string(this->version);
            this->rom_region_string = ocean::region::rom_region_string(this->region);
            this->rom_version_and_region = this->rom_region_string + " " + this->rom_version_string;
        }

        std::string Status::convert_u64_to_hex(u64 value)
        {
            std::stringstream stream;
            stream << std::uppercase;
            stream << std::setfill('0') << std::setw(16) << std::hex << value;
            return "0x" + stream.str();
        }

        std::string Status::getRegion()
        {
            return convert_u64_to_hex(static_cast<u64>(this->region));
        }

        std::string Status::getRegionText()
        {
            return this->rom_region_string;
        }

        std::string Status::getVersion()
        {
            return convert_u64_to_hex(static_cast<u64>(this->version));
        }

        std::string Status::getVersionText()
        {
            return this->rom_version_string;
        }

        std::string Status::getBuildId()
        {
            return convert_u64_to_hex(this->build_id);
        }

        std::string Status::getRomTypeAndRegion()
        {
            return this->rom_version_and_region;
        }

        u64 Status::getOffset()
        {
            return this->offset;
        }
    }
}
