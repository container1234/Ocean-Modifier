#include <tesla.hpp>
#include <dmntcht.h>
#include <iomanip>
#include <random>
#include "wave.hpp"
#include "random.hpp"

#ifndef OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define OVERLAY_VERSION "v1.0.2"
#define OVERLAY_TITLE "Ocean Modifier"

namespace tesla
{
    class SeedSearch : public tsl::Gui
    {
    public:
        SeedSearch(u32 *game_random_seed)
        {
            this->game_random_seed = game_random_seed;
            this->mt = std::mt19937(time(NULL));
        }
        u32 *game_random_seed;
        std::mt19937 mt;

        std::vector<ocean::wave::WaveInfo> waves = {
            ocean::wave::WaveInfo(),
            ocean::wave::WaveInfo(),
            ocean::wave::WaveInfo()};

        u32 get_wave_info(u32 seed)
        {
            sead::random::Random rnd = sead::random::Random(seed);
            ocean::wave::WaveType mWave = ocean::wave::WaveType();
            const std::vector<u8> mEvent = {0x12, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
            const std::vector<u8> mTide = {0x01, 0x03, 0x01};

            for (u16 wave = 0; wave < 3; ++wave)
            {
                for (u64 event = 0, sum = 0; event < 7; ++event) // Calculate Wave Event
                {
                    if ((wave > 0) && (mWave.event[wave - 1] != 0) && (mWave.event[wave - 1] == event))
                        continue;
                    sum += mEvent[event];
                    if ((rnd.getU32() * sum >> 0x20) < mEvent[event])
                        mWave.event[wave] = event;
                }
                for (u64 tide = 0, sum = 0; tide < 3; ++tide) // Calculate Wave Tide
                {
                    if ((tide == 0) && (1 <= mWave.event[wave] && (mWave.event[wave] <= 3)))
                        continue;
                    sum += mTide[tide];
                    if ((rnd.getU32() * sum >> 0x20) < mTide[tide])
                        mWave.event[wave] == 6 ? mWave.tide[wave] = 0 : mWave.tide[wave] = tide;
                }
            }

            return mWave.tide[0] * 100000 + mWave.event[0] * 10000 + mWave.tide[1] * 1000 + mWave.event[1] * 100 + mWave.tide[2] * 10 + mWave.event[2] * 1;
        }

        void find_target_seed()
        {
            u32 random_seed = 0x00000000;
            u32 target_wave = this->get_target_seed();
            for (u32 loop = 0; loop <= 0x1000; loop++)
            {
                random_seed = static_cast<u32>(mt());
                if (target_wave == get_wave_info(random_seed))
                    break;
            }
            *(this->game_random_seed) = random_seed;
        }

        virtual tsl::elm::Element *createUI() override
        {
            auto frame = new tsl::elm::OverlayFrame("Ocean Modifier", "Seed Modifier");
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
                find_target_seed();
                return true;
            }
            return false; });
            list->addItem(search);
            return frame;
        }

    private:
        u32 get_target_seed()
        {
            return (u32)waves[0].tide->getProgress() * 100000 + (u32)waves[0].event->getProgress() * 10000 + (u32)waves[1].tide->getProgress() * 1000 + (u32)waves[1].event->getProgress() * 100 + (u32)waves[2].tide->getProgress() * 10 + (u32)waves[2].event->getProgress() * 1;
        }
    };

    class OceanModifier : public tsl::Gui
    {
    public:
        u32 game_random_seed;
        bool fix_seed_flag;
        ocean::rom::RomType rom_type;
        std::string rom_type_str;

        OceanModifier(DmntCheatProcessMetadata);
        bool check_if_seed_fixed();
        u32 read_seed_fixed();
        void write_game_random_seed(u32);
        void restore_game_random_seed();
        void update_seed();
        void fix_seed_toggle(bool);
        std::string convert_u32_to_hex(u32);
        tsl::elm::ListItem *target;

        virtual tsl::elm::Element *createUI() override
        {
            auto frame = new tsl::elm::OverlayFrame(OVERLAY_TITLE, OVERLAY_VERSION);
            auto list = new tsl::elm::List();

            if (this->rom_type == NULL)
                return warning_frame();

            auto *search = new tsl::elm::ListItem("Search");
            search->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SeedSearch>(&game_random_seed);
                return true;
            }

            return false; });
            list->addItem(new tsl::elm::CategoryHeader(this->rom_type_str));
            list->addItem(new tsl::elm::ToggleListItem("Seed Modifier", this->fix_seed_flag));
            list->addItem(search);
            // target = new tsl::elm::ListItem(convert_u32_to_hex(this->game_random_seed));
            target->setText(convert_u32_to_hex(this->game_random_seed));
            list->addItem(target);
            frame->setContent(list);
            return frame;
        }

        virtual void update() override
        {
            // target = new tsl::elm::ListItem(convert_u32_to_hex(this->game_random_seed));
            target->setText(convert_u32_to_hex(this->game_random_seed));
            // target
            return;
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
        // virtual tsl::elm::CustomDrawer *warning()
        // {
        //     return warning;
        // }
        // virtual bool handleInput(u64, u64, const HidTouchState, HidAnalogStickState, HidAnalogStickState) override;

    private:
        u32 base;
    };
}
#endif // OCEAN_MODIFIER_INCLUDE_OCEAN_H_
