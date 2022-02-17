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

        // int WaterLevelName(std::string tide)
        // {
        //     if (tide == "Low")
        //     {
        //         return 0;
        //     }

        //     if (tide == "Middle")
        //     {
        //         return 0;
        //     }

        //     if (tide == "High")
        //     {
        //         return 0;
        //     }

        //     return 0;
        // }

        // EventType ToEventType(std::string event)
        // {
        //     if (event == "None")
        //     {
        //         return EventType::None;
        //     }
        //     if (event == "Rush")
        //     {
        //         return EventType::Rush;
        //     }
        //     if (event == "Goldie")
        //     {
        //         return EventType::Goldie;
        //     }
        //     if (event == "Griller")
        //     {
        //         return EventType::Griller;
        //     }
        //     if (event == "Rally")
        //     {
        //         return EventType::Rally;
        //     }
        //     if (event == "Fog")
        //     {
        //         return EventType::Fog;
        //     }
        //     if (event == "Caonon")
        //     {
        //         return EventType::Canon;
        //     }
        //     return EventType::None;
        // }

        class WaveInfo
        {
        public:
            tsl::elm::NamedStepTrackBar *tide = new tsl::elm::NamedStepTrackBar("\u223F", {"Low", "Middle", "High"});
            tsl::elm::NamedStepTrackBar *event = new tsl::elm::NamedStepTrackBar("\u223F", {"None", "Rush", "Goldie", "Griller", "Rally", "Fog", "Canon"});
        };

        struct WaveType
        {
            u8 event[3] = {0x0, 0x0, 0x0};
            u8 tide[3] = {0x0, 0x0, 0x0};
        };
    }
}
#endif // OCEAN_MODIFIER_INCLUDE_WAVE_H_
