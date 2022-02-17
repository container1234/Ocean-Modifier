#pragma once
#include <tesla.hpp>
#include <dmntcht.h>
#include <string.h>
#define OVERLAY_VERSION "v1.0.2"
#define OVERLAY_TITLE "Ocean Modifier"

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

    virtual tsl::elm::Element *createUI() override
    {
        auto frame = new tsl::elm::OverlayFrame(OVERLAY_TITLE, OVERLAY_VERSION);
        auto list = new tsl::elm::List();

        if (this->rom_type == NULL)
            return warning_frame();
        list->addItem(new tsl::elm::CategoryHeader(this->rom_type_str));
        frame->setContent(list);
        return frame;
    }
    virtual void update() override
    {
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
