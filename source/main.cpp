#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include "ocean.h"      // The Ocean Header
#include <dmntcht.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#define GAME_BUILD_ID 0x1729DD426870976B

static DmntCheatProcessMetadata metadata;
static tsl::elm::ListItem *modify_seed;

class GuiOcean : public tsl::Gui
{
public:
    GuiOcean() {}
    Ocean ocean = Ocean(metadata);

    // Called when this Gui gets loaded to create the UI
    // Allocate all elements on the heap. libtesla will make sure to clean them up when not needed anymore
    virtual tsl::elm::Element *createUI() override
    {
        // A OverlayFrame is the base element every overlay consists of. This will draw the default Title and Subtitle.
        // If you need more information in the header or want to change it's look, use a HeaderOverlayFrame.
        auto frame = new tsl::elm::OverlayFrame("Ocean Modifier", "v1.0.1");

        // A list that can contain sub elements and handles scrolling
        auto list = new tsl::elm::List();

        // if Splatoon 2 is not running
        auto warning = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h)
                                                  {
                renderer->drawString("\uE150", false, 180, 250, 90, renderer->a(0xFFFF));
                renderer->drawString("Splatoon 2 is not running!", false, 70, 340, 25, renderer->a(0xFFFF)); });
        frame->setContent(warning);
        return frame;

        // auto *fix_seed = new tsl::elm::ToggleListItem("Fix Seed", check_if_seed_fixed());
        // fix_seed->setStateChangedListener(fix_seed_toggle);
        // list->addItem(fix_seed);

        /*
        auto *reset_seed = new tsl::elm::ToggleListItem("Fix Seed", fix_seed_flag);
        reset_seed->setClickListener([](u64 keys){
            if(keys == HidNpadButton_A)
            {
                restore_seed_inst();
                return true;
            }
            return false;
        });
        list->addItem(reset_seed);
        */

        // modify_seed = new tsl::elm::ListItem("Fix Seed:");
        // modify_seed->setClickListener(change_seed_value);
        // list->addItem(modify_seed);

        // list->addItem(new tsl::elm::CategoryHeader("Seed Information"));

        // Add the list to the frame for it to be drawn
        frame->setContent(list);

        // Return the frame to have it become the top level element of this Gui
        return frame;
    }

    // Called once every frame to update values
    virtual void update() override
    {
        // if Splatoon 2 is not running
        // std::stringstream stream;
        // stream << std::uppercase;
        // stream << std::setfill('0') << std::setw(8) << std::hex << seed;
        // modify_seed->setText("Seed: 0x" + stream.str());
    }

    // Called once every frame to handle inputs not handled by other UI elements
    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override
    {
        return false; // Return true here to signal the inputs have been consumed
    }
};

class OverlayOcean : public tsl::Overlay
{
public:
    // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override
    {
        dmntchtInitialize();
        dmntchtForceOpenCheatProcess();
        dmntchtGetCheatProcessMetadata(&metadata);
    } // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override
    {
        dmntchtExit();
    } // Called at the end to clean up all services previously initialized

    virtual void onShow() override
    {
        // if Splatoon 2 is not running
        // fix_seed_flag = check_if_seed_fixed();
        // if (fix_seed_flag)
        //     seed = read_seed_inst();
    }                                 // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {} // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<GuiOcean>(); // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

int main(int argc, char **argv)
{
    return tsl::loop<OverlayOcean>(argc, argv);
}
