#include <tesla.hpp>
#include <random>
#include <time.h>

#ifndef OCEAN_MODIFIER_INCLUDE_WAVE_H_
#define OCEAN_MODIFIER_INCLUDE_WAVE_H_

namespace ocean
{
    namespace rom
    {
        enum RomType : long unsigned int
        {
            JP = 0x01003C700009C000,
            US = 0x01003BC0000A0000,
            EU = 0x0100F8F0000A2000
        };
    }

    namespace wave
    {
        enum EventType
        {
            None,
            Rush,
            Goldie,
            Griller,
            Fog,
            Rally,
            Canon
        };

        enum WaterLevel
        {
            Low,
            Middle,
            High
        };

        WaterLevel WaterLevelName(std::string);
        EventType ToEventType(std::string);

        class WaveInfo
        {
        public:
            tsl::elm::NamedStepTrackBar *tide = new tsl::elm::NamedStepTrackBar("\u2248", {"Low", "Middle", "High"});
            tsl::elm::NamedStepTrackBar *event = new tsl::elm::NamedStepTrackBar("\uE140", {"None", "Rush", "Goldie", "Griller", "Rally", "Fog", "Canon"});
        };

        struct WaveType
        {
            u32 event[3] = {0x0, 0x0, 0x0};
            u32 tide[3] = {0x0, 0x0, 0x0};
        };
    }
}
#endif // OCEAN_MODIFIER_INCLUDE_WAVE_H_
