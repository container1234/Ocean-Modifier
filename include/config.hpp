#include <json.hpp>
#include <tesla.hpp>
#include <functional>

#ifndef OCEAN_MODIFIER_INCLUDE_CONFIG_H_
#define OCEAN_MODIFIER_INCLUDE_CONFIG_H_

namespace ocean
{
    class Config
    {
    public:
        static nlohmann::json read();
        static void update(std::function<void(nlohmann::json &)> const &updator);

        u32 wave;
        u8 *tide[3];
        u8 *event[3];

    private:
        Config() {}
        static const char *FILE_PATH;
    };
}
#endif // OCEAN_MODIFIER_INCLUDE_CONFIG_H_
