#include <Config.hpp>

#include <fstream>

namespace ocean
{
    const char *Config::FILE_PATH = "sdmc:/switch/.overlays/OceanModifier/config.json";

    nlohmann::json Config::read()
    {
        std::fstream fs(FILE_PATH, std::fstream::in);
        if (fs.fail())
        {
            return initialize();
        }

        nlohmann::json json = nlohmann::json::parse(fs, nullptr, false);
        if (!json.is_discarded())
            return json;

        return initialize();
    }

    void Config::update(std::function<void(nlohmann::json &)> const &updater)
    {
        nlohmann::json json = read();

        updater(json);

        std::fstream fs(FILE_PATH, std::fstream::out);
        fs << json.dump(2);
    }

    nlohmann::json Config::initialize()
    {
        std::fstream fs(FILE_PATH, std::fstream::out | std::fstream::trunc);
        nlohmann::json json = nlohmann::json::object();
        fs << json;
        return json;
    }
}
