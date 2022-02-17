#pragma once
#include <tesla.hpp>
#include <dmntcht.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <random>
#define OVERLAY_VERSION "v1.0.2"
#define OVERLAY_TITLE "Ocean Modifier"

class WaveInfo
{
public:
    tsl::elm::NamedStepTrackBar *tide = new tsl::elm::NamedStepTrackBar("\u223F", {"High", "Normal", "Low"});
    tsl::elm::NamedStepTrackBar *event = new tsl::elm::NamedStepTrackBar("\u223F", {"No Event", "Rush", "Goldie Seeking", "Griller", "Mothership", "Fog", "Cohock Charge"});
};

struct Wave
{
    u8 event[3] = {0x0, 0x0, 0x0};
    u8 tide[3] = {0x0, 0x0, 0x0};
};

class Random
{
public:
    Random(u32 seed)
    {
        this->seed1 = 0x6C078965 * (seed ^ (seed >> 30)) + 1;
        this->seed2 = 0x6C078965 * (seed1 ^ (seed1 >> 30)) + 2;
        this->seed3 = 0x6C078965 * (seed2 ^ (seed2 >> 30)) + 3;
        this->seed4 = 0x6C078965 * (seed3 ^ (seed3 >> 30)) + 4;
    }

    u32 getU32()
    {
        u32 n = seed1 ^ (seed1 << 11);
        seed1 = seed2;
        seed2 = seed3;
        seed3 = seed4;
        seed4 = n ^ (n >> 8) ^ seed4 ^ (seed4 >> 19);
        return seed4;
    }

private:
    u32 seed1;
    u32 seed2;
    u32 seed3;
    u32 seed4;
};

class SeedSearch : public tsl::Gui
{
public:
    SeedSearch(u32 *game_random_seed)
    {
        this->game_random_seed = game_random_seed;
    }
    u32 *game_random_seed;

    void get_wave_info(u32 seed)
    {
        Random rnd = Random(seed);
        Wave mWave = Wave();
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
    }

    void find_target_seed()
    {

        std::random_device rnd;
        u32 random;
        for (u16 loop = 0; loop < 0xF; loop++)
        {
            random = static_cast<u32>(rnd());
        }
        *(this->game_random_seed) = random;
    }

    virtual tsl::elm::Element *createUI() override
    {
        auto frame = new tsl::elm::OverlayFrame("Ocean Modifier", "Seed Modifier");
        auto list = new tsl::elm::List();

        std::vector<WaveInfo> waves = {
            WaveInfo(),
            WaveInfo(),
            WaveInfo()};

        std::for_each(waves.begin(), waves.end(), [list](WaveInfo waveinfo)
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
};

class Ocean : public tsl::Gui
{
public:
    enum RomType : long unsigned int
    {
        JP = 0x01003C700009C000,
        US = 0x01003BC0000A0000,
        EU = 0x0100F8F0000A2000
    };

    u32 game_random_seed;
    bool fix_seed_flag;
    Ocean::RomType rom_type;
    std::string rom_type_str;

    // Ocean();
    Ocean(DmntCheatProcessMetadata);
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
