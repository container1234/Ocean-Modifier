#include <tesla.hpp>
#include <dmntcht.h>
#include <iomanip>
#include <random>
#include "wave.hpp"
#include "random.hpp"

#ifndef OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define OCEAN_MODIFIER_INCLUDE_OCEAN_H_
#define SEED_INST_OFFSET 0x0037F340
#define OVERLAY_VERSION "v1.0.2"
#define OVERLAY_TITLE "Ocean Modifier"
static DmntCheatProcessMetadata metadata;

namespace tesla
{
    static u32 offset;
    static bool fix_seed_flag = false;
    static u32 game_random_seed = 0x00000000;
    void update_inst(bool);
    void restore_game_random_seed();
    void write_seed_to_memory(u32);
    u32 read_fixed_seed();
    bool check_seed_is_fixed();

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

        void search_target_seed()
        {
            u32 random_seed = 0x00000000;
            u32 target_wave = this->get_target_wave();
            for (u32 loop = 0; loop <= 0x1000; loop++)
            {
                random_seed = static_cast<u32>(mt());
                if (target_wave == get_wave_info(random_seed))
                    break;
            }
            game_random_seed = random_seed;
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
                search_target_seed();
                return true;
            }
            return false; });
            list->addItem(search);
            return frame;
        }

    private:
        u32 get_target_wave()
        {
            return (u32)waves[0].tide->getProgress() * 100000 + (u32)waves[0].event->getProgress() * 10000 + (u32)waves[1].tide->getProgress() * 1000 + (u32)waves[1].event->getProgress() * 100 + (u32)waves[2].tide->getProgress() * 10 + (u32)waves[2].event->getProgress() * 1;
        }
    };

    class OceanModifier : public tsl::Gui
    {
    public:
        ocean::rom::RomType rom_type;
        std::string rom_type_str;
        tsl::elm::ListItem *target;
        tsl::elm::ListItem *memory;
        tsl::elm::ToggleListItem *modifier;

        OceanModifier(DmntCheatProcessMetadata);
        std::string convert_u32_to_hex(u32);

        virtual tsl::elm::Element *createUI() override
        {
            offset = metadata.main_nso_extents.base + SEED_INST_OFFSET;
            auto frame = new tsl::elm::OverlayFrame(OVERLAY_TITLE, OVERLAY_VERSION);
            auto list = new tsl::elm::List();

            if (this->rom_type == NULL)
                return warning_frame();

            modifier = new tsl::elm::ToggleListItem("Modifier", check_seed_is_fixed());
            modifier->setStateChangedListener(update_inst);
            list->addItem(modifier);
            auto *search = new tsl::elm::ListItem("Search");
            search->setClickListener([this](u64 keys)
                                     {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SeedSearch>();
                return true;
            }
            return false; });
            list->addItem(new tsl::elm::CategoryHeader(rom_type_str));
            list->addItem(search);
            list->addItem(new tsl::elm::CategoryHeader("Seed"));
            target = new tsl::elm::ListItem("Target", convert_u32_to_hex(game_random_seed));
            memory = new tsl::elm::ListItem("Memory", convert_u32_to_hex(game_random_seed));
            list->addItem(target);
            list->addItem(memory);
            frame->setContent(list);
            return frame;
        }

        virtual void update() override
        {
            // Update Target Seed
            target->setValue(convert_u32_to_hex(game_random_seed));

            // Read Memory Seed
            if (check_seed_is_fixed())
                memory->setValue(convert_u32_to_hex(read_fixed_seed()));
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

    private:
        u32 offset;
    };
}
#endif // OCEAN_MODIFIER_INCLUDE_OCEAN_H_
