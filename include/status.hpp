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
        u64 address_offset();
    }

    namespace status
    {
        class Status
        {
        public:
            Status(DmntCheatProcessMetadata);
            ocean::region::Region region;
            ocean::version::RomVer version;
            std::string rom_version_and_region;
            u64 offset;
            u64 build_id;

        private:
            std::string rom_version_string;
            std::string rom_region_string;
        };
    }
}
