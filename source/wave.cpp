#include "wave.hpp"

namespace ocean
{
    namespace wave
    {
        WaterLevel WaterLevelName(std::string tide)
        {
            if (tide == "Low")
            {
                return WaterLevel::Low;
            }

            if (tide == "Middle")
            {
                return WaterLevel::Middle;
            }

            if (tide == "High")
            {
                return WaterLevel::High;
            }

            return WaterLevel::Middle;
        }

        EventType ToEventType(std::string event)
        {
            if (event == "None")
            {
                return EventType::None;
            }
            if (event == "Rush")
            {
                return EventType::Rush;
            }
            if (event == "Goldie")
            {
                return EventType::Goldie;
            }
            if (event == "Griller")
            {
                return EventType::Griller;
            }
            if (event == "Rally")
            {
                return EventType::Rally;
            }
            if (event == "Fog")
            {
                return EventType::Fog;
            }
            if (event == "Caonon")
            {
                return EventType::Canon;
            }
            return EventType::None;
        }
    }
}
