#include <tesla.hpp>
#include <dmntcht.h>
#include <iomanip>
#include <random>
#include "wave.hpp"
#include "random.hpp"
#include "config.hpp"

#ifndef OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define SEED_INST_OFFSET 0x0037F340
#define OVERLAY_VERSION "v1.0.3"
#define OVERLAY_TITLE "Ocean Modifier"

namespace tesla
{
    u32 read_seed_inst();
    bool check_if_seed_fixed();
    static u64 base;
    static u64 title_id;
    static u32 game_random_seed;
    static ocean::wave::WaveType wave;

    std::string get_target_wave_string();
    u32 get_target_wave();

    class SeedSearch : public tsl::Gui
    {
    public:
        SeedSearch()
        {
            this->mt = std::mt19937(time(NULL));
        }
        std::mt19937 mt;
        std::vector<ocean::wave::WaveInfo> waves = {
            ocean::wave::WaveInfo(),
            ocean::wave::WaveInfo(),
            ocean::wave::WaveInfo()};
        u32 get_wave_info(u32);
        u32 search_target_seed();

        virtual tsl::elm::Element *createUI() override
        {
            auto frame = new tsl::elm::OverlayFrame("Ocean Modifier", "Seed Search");
            auto list = new tsl::elm::List();

            std::for_each(waves.begin(), waves.end(), [list](ocean::wave::WaveInfo waveinfo)
                          {
            list->addItem(new tsl::elm::CategoryHeader("WAVE"));
            list->addItem(waveinfo.tide);
            list->addItem(waveinfo.event); });
            frame->setContent(list);

            auto search = new tsl::elm::ListItem("Search", "...");
            search->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                game_random_seed = search_target_seed();
                return true;
            }
            return false; });
            list->addItem(search);
            return frame;
        }
    };

    class OceanModifier : public tsl::Gui
    {
    public:
        OceanModifier(DmntCheatProcessMetadata);
        ocean::rom::RomType rom_type;
        std::string rom_type_str;
        tsl::elm::ListItem *target;
        tsl::elm::ListItem *memory;
        tsl::elm::ListItem *search;
        static bool fix_seed_flag;
        static void fix_seed_toggle(bool);
        static void update_seed_inst();
        static void write_seed_inst(u32);
        static void restore_seed_inst();
        std::string convert_u32_to_hex(u32);
        std::string convert_u64_to_hex(u64);

        virtual tsl::elm::Element *createUI() override
        {
            if (this->rom_type == NULL)
                return warning_frame();

            auto frame = new tsl::elm::OverlayFrame(OVERLAY_TITLE, OVERLAY_VERSION);
            auto list = new tsl::elm::List();
            search = new tsl::elm::ListItem("Search", get_target_wave_string());
            auto *writer = new tsl::elm::ListItem("Write");

            // Method
            search->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SeedSearch>();
                return true;
            }
            return false; });
            writer->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                write_seed_inst(game_random_seed);
                return true;
            }
            return false; });
            // RomType
            list->addItem(new tsl::elm::CategoryHeader(rom_type_str));
            // Modifier
            auto *offset = new tsl::elm::ListItem("Base", convert_u64_to_hex(base));
            auto *title = new tsl::elm::ListItem("ID", convert_u64_to_hex(title_id));
            auto *modifier = new tsl::elm::ToggleListItem("Modifier", check_if_seed_fixed());
            modifier->setStateChangedListener(fix_seed_toggle);
            list->addItem(title);
            list->addItem(offset);
            list->addItem(modifier);
            list->addItem(new tsl::elm::CategoryHeader("Seed"));
            list->addItem(search);
            target = new tsl::elm::ListItem("Target", convert_u32_to_hex(game_random_seed));
            memory = new tsl::elm::ListItem("Memory", convert_u32_to_hex(game_random_seed));
            list->addItem(target);
            list->addItem(memory);
            list->addItem(writer);
            frame->setContent(list);
            return frame;
        }

        virtual void update() override
        {
            if (this->rom_type == NULL)
                return;
            // Update Search Wave
            // search->setValue(get_target_wave_string());
            // Update Target Seed
            target->setValue(convert_u32_to_hex(game_random_seed));

            // Read Memory Seed
            if (check_if_seed_fixed())
                memory->setValue(convert_u32_to_hex(read_seed_inst()));
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