#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include <dmntcht.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#define SEED_INST_OFFSET 0x0037F340


static DmntCheatProcessMetadata metadata;
static bool fix_seed_flag=false;
static u32 seed=0;
static int cursor=0;
static tsl::elm::ListItem *modify_seed;

void write_seed_inst(u32 value)
{
    u32 inst;
    inst = 0xD2A00000 | ((value >> 16) << 5);
    dmntchtWriteCheatProcessMemory(metadata.main_nso_extents.base + SEED_INST_OFFSET, &inst, sizeof(inst));
    inst = 0xF2800000 | ((value & 0xFFFF) << 5);
    dmntchtWriteCheatProcessMemory(metadata.main_nso_extents.base + SEED_INST_OFFSET + 4, &inst, sizeof(inst));
}

void restore_seed_inst()
{
    constexpr char inst_orig[8] = {0x00, 0x01, 0x40, 0xF9, 0x93, 0xE1, 0x4E, 0x94};
    dmntchtWriteCheatProcessMemory(metadata.main_nso_extents.base + SEED_INST_OFFSET, &inst_orig, sizeof(inst_orig));
}

void fix_seed_toggle(bool state)
{
    fix_seed_flag = state;
}

bool change_seed_value(u64 button)
{
    if(button == HidNpadButton_L)
    {
        cursor++;
        if(cursor > 7)
        {
            cursor = 0;
        }
    } else if(button == HidNpadButton_R){
        cursor--;
        if(cursor < 0)
        {
            cursor = 7;
        }
    } else if(button == HidNpadButton_X){
        seed += 1 << (cursor * 4);
    } else if(button == HidNpadButton_Y){
        seed -= 1 << (cursor * 4);
    } else if(button == HidNpadButton_A){
        write_seed_inst(seed);
    } else {
        return false;
    }
    return true;
}

class GuiOcean : public tsl::Gui {
public:
    GuiOcean() { }

    // Called when this Gui gets loaded to create the UI
    // Allocate all elements on the heap. libtesla will make sure to clean them up when not needed anymore
    virtual tsl::elm::Element* createUI() override {
        // A OverlayFrame is the base element every overlay consists of. This will draw the default Title and Subtitle.
        // If you need more information in the header or want to change it's look, use a HeaderOverlayFrame.
        auto frame = new tsl::elm::OverlayFrame("Ocean Modifier", "v1.0.0");

        // A list that can contain sub elements and handles scrolling
        auto list = new tsl::elm::List();

        // auto *fix_seed = new tsl::elm::ToggleListItem("Fix Seed", fix_seed_flag);
        // fix_seed->setStateChangedListener(fix_seed_toggle);
        // list->addItem(fix_seed);
        
        auto *reset_seed = new tsl::elm::ListItem("Random Seed");
        reset_seed->setClickListener([](u64 keys){
            if(keys == HidNpadButton_A)
            {
                restore_seed_inst();
                return true;
            }
            return false;
        });
        list->addItem(reset_seed);
        
        modify_seed = new tsl::elm::ListItem("Fix Seed:");
        modify_seed->setClickListener(change_seed_value);
        list->addItem(modify_seed);
        
        // list->addItem(new tsl::elm::CategoryHeader("Seed Information"));
        
        // Add the list to the frame for it to be drawn
        frame->setContent(list);

        // Return the frame to have it become the top level element of this Gui
        return frame;
    }

    // Called once every frame to update values
    virtual void update() override {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(8) << std::hex << seed;
        modify_seed->setText("Seed: " + stream.str());
        /*
        if(fix_seed_flag)
        {
            write_seed_inst(seed);
        } else {
            restore_seed_inst();
        }
        */
    }

    // Called once every frame to handle inputs not handled by other UI elements
    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        return false;   // Return true here to signal the inputs have been consumed
    }
};

class OverlayOcean : public tsl::Overlay {
public:
                                             // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override {
        dmntchtInitialize();
        dmntchtForceOpenCheatProcess();
        dmntchtGetCheatProcessMetadata(&metadata);
    }  // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override {
        dmntchtExit();
    }  // Called at the end to clean up all services previously initialized

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {}    // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiOcean>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

int main(int argc, char **argv) {
    return tsl::loop<OverlayOcean>(argc, argv);
}
