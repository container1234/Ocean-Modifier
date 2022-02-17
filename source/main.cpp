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
        return initially<Ocean>(metadata); // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

int main(int argc, char **argv)
{
    return tsl::loop<OverlayOcean>(argc, argv);
}
