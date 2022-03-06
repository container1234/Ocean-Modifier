#include <json.hpp>
#include <tesla.hpp>
#include <functional>
#include <vector>
#include <string>

#ifndef OCEAN_MODIFIER_INCLUDE_CONFIG_H_
#define OCEAN_MODIFIER_INCLUDE_CONFIG_H_

namespace ocean
{
    class Config
    {
    public:
        u32 seed;
        std::vector<u8> tide;
        std::vector<u8> event;
    };

    void to_json(nlohmann::json &, const Config &);
    void from_json(const nlohmann::json &, Config &);
}
#endif // OCEAN_MODIFIER_INCLUDE_CONFIG_H_
