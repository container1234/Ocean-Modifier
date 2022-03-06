#include <tesla.hpp>
#include <iomanip>
#include <random>
#include "wave.hpp"
#include "random.hpp"
#include "config.hpp"
#include "status.hpp"
#include <thread>

#ifndef OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define SEED_INST_OFFSET 0x0037F340
#define OVERLAY_VERSION "v1.0.4"
#define OVERLAY_TITLE "Ocean Modifier"

namespace tesla
{
    // u32 read_seed_inst();
    // bool check_if_seed_fixed();
    static ocean::wave::WaveType wave;

    class SeedSearch : public tsl::Gui
    {
    public:
        SeedSearch(u32 *);
        std::mt19937 mt;
        u32 *game_random_seed;
        ocean::Config config;
        void load();
        void write();
        std::vector<ocean::wave::WaveInfo> waves = {
            ocean::wave::WaveInfo(),
            ocean::wave::WaveInfo(),
            ocean::wave::WaveInfo()};
        u32 get_wave_info(u32);
        u32 search_target_seed();
        std::string get_target_wave_string();
        u32 get_target_wave();
        tsl::elm::ListItem *search;
        tsl::elm::ListItem *logger;

        virtual tsl::elm::Element *createUI() override
        {
            auto frame = new tsl::elm::OverlayFrame("Ocean Modifier", "Seed Search");
            auto list = new tsl::elm::List();

            for (auto itr = waves.begin(); itr != waves.end(); ++itr)
            {
                const int index = std::distance(waves.begin(), itr);
                itr->tide->setProgress(config.tide[index]);
                itr->event->setProgress(config.event[index]);
                itr->tide->setValueChangedListener([this, index](int value)
                                                   { config.tide[index] = value; });
                itr->event->setValueChangedListener([this, index](int value)
                                                    { config.event[index] = value; });
                list->addItem(new tsl::elm::CategoryHeader("WAVE" + std::to_string(index + 1)));
                list->addItem(itr->tide);
                list->addItem(itr->event);
            }

            frame->setContent(list);

            search = new tsl::elm::ListItem("Search");
            logger = new tsl::elm::ListItem("Save");
            search->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                std::thread thread([this]
                                   { *(this->game_random_seed) = search_target_seed(); });
                thread.join();
                return true;
            }
            return false; });
            logger->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                this->write();
                return true;
            }
            return false; });
            search->setValue(get_target_wave_string());
            list->addItem(search);
            list->addItem(logger);
            return frame;
        }

        virtual void update() override
        {
            search->setValue(get_target_wave_string());
        };
    };

    class SeedInput : public tsl::Gui, ocean::system::System
    {
        using ocean::system::System::System;

    public:
        SeedInput(u32 *);
        u32 *game_random_seed;
        int cursor = 7;
        virtual tsl::elm::Element *createUI() override
        {
            auto frame = new tsl::elm::OverlayFrame("Ocean Modifier", "Seed Modifier");
            auto seed = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h)
                                                      {
                std::stringstream stream;
                stream << std::setfill(' ') << std::setw(8 - cursor) << ".";
                std::string cursor_str = stream.str();
                renderer->drawString(cursor_str.c_str(), false, 70, 310, 25, renderer->a(0xFFFF));
                renderer->drawString(this->getGameRandomSeedText().c_str(), false, 70, 340, 25, renderer->a(0xFFFF)); });
            
            seed->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_Left) {
                cursor++;
                if (cursor > 7)
                    cursor = 0;
                return true;
            }
            if (keys & HidNpadButton_Right) {
                cursor--;
                if (cursor < 0)
                    cursor = 7;
                return true;
            }
            if (keys & HidNpadButton_Up) {
                u32 new_seed = this->getGameRandomSeed();
                if (((new_seed >> (cursor * 4)) & 0xF) == 0xF)
                {
                    new_seed &= ~(0xF << (cursor * 4));
                } else {
                    new_seed += 1 << (cursor * 4);
                }
                *(this->game_random_seed) = new_seed;
                return true;
            }
            if (keys & HidNpadButton_Down) {
                u32 new_seed = this->getGameRandomSeed();
                if (((new_seed >> (cursor * 4)) & 0xF) == 0x0)
                {
                    new_seed |= (0xF << cursor * 4);
                } else {
                    new_seed -= 1 << (cursor * 4);
                }
                *(this->game_random_seed) = new_seed;
                return true;
            }
            return false; });
            frame->setContent(seed);
            return frame;
        }
        
        virtual void update() override {};
    
    };

    class OceanModifier : public tsl::Gui, ocean::system::System
    {
        using ocean::system::System::System;

    public:
        tsl::elm::ListItem *target;
        tsl::elm::ListItem *memory;
        tsl::elm::ListItem *search;
        static bool fix_seed_flag;
        static void fix_seed_toggle(bool);
        static void update_seed_inst();

        virtual tsl::elm::Element *createUI() override
        {
            // IF SPLATOON2 IS NOT RUNNING
            if (!isGameRunning())
                return warning_frame();

            auto frame = new tsl::elm::OverlayFrame(OVERLAY_TITLE, OVERLAY_VERSION);
            auto list = new tsl::elm::List();
            search = new tsl::elm::ListItem("Search");
            target = new tsl::elm::ListItem("Target", getGameRandomSeedText());
            auto *writer = new tsl::elm::ListItem("Write");

            // Method
            search->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SeedSearch>(getGameRandomSeedAddress());
                return true;
            }
            return false; });
            
            target->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SeedInput>(getGameRandomSeedAddress());
                return true;
            }
            return false; });
            
            writer->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                writeDataToMemory();
                return true;
            }
            return false; });
            // RomType
            list->addItem(new tsl::elm::CategoryHeader(getRomTypeAndRegion()));
            auto *build = new tsl::elm::ListItem("BID", getBuildId());
            // Modifier
            auto *offset = new tsl::elm::ListItem("Base", getBaseAddress());
            auto *title = new tsl::elm::ListItem("ID", getGameTitleIdText());
            // auto *modifier = new tsl::elm::ToggleListItem("Modifier", check_if_seed_fixed());
            // modifier->setStateChangedListener(fix_seed_toggle);
            list->addItem(build);
            list->addItem(title);
            list->addItem(offset);
            // list->addItem(modifier);
            // Seed
            list->addItem(new tsl::elm::CategoryHeader("Seed"));
            list->addItem(search);
            memory = new tsl::elm::ListItem("Memory", getGameRandomSeedText());
            list->addItem(target);
            list->addItem(memory);
            list->addItem(writer);
            frame->setContent(list);

            // Credits
            list->addItem(new tsl::elm::CategoryHeader("Credits"));
            list->addItem(new tsl::elm::ListItem("Author", "@container12345"));
            list->addItem(new tsl::elm::ListItem("Contributor", "@tkgling"));
            return frame;
        }

        virtual void update() override
        {
            if (!isGameRunning())
                return;
            // Update Target Seed
            target->setValue(getGameRandomSeedText());

            // Read Memory Seed
            memory->setValue(getGameRandomSeedTextFromMemory());
        }

        virtual tsl::elm::OverlayFrame *warning_frame()
        {
            auto frame = new tsl::elm::OverlayFrame(OVERLAY_TITLE, OVERLAY_VERSION);
            auto warning = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h)
                                                      {
                renderer->drawString("\uE150", false, 180, 250, 90, renderer->a(0xFFFF));
                renderer->drawString("Splatoon 2 is not running!", false, 70, 340, 25, renderer->a(0xFFFF)); });
            frame->setContent(warning);
            return frame;
        }

        // virtual void onHide() override {}

        // virtual bool handleInput(u64, u64, const HidTouchState, HidAnalogStickState, HidAnalogStickState) override;
    };
}
#endif // OCEAN_MODIFIER_INCLUDE_OCEAN_H_
