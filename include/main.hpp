#include <tesla.hpp>
#include "menu.hpp"
#include "wave.hpp"
#include <dmntcht.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#ifndef OCEAN_MODIFIER_INCLUDE_MAIN_H_
#define OCEAN_MODIFIER_INCLUDE_MAIN_H_
#define GAME_BUILD_ID 0x1729DD426870976B
static DmntCheatProcessMetadata metadata;

class OverlayOcean : public tsl::Overlay
{
public:
    // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override
    {
        dmntchtInitialize();
        dmntchtForceOpenCheatProcess();
        dmntchtGetCheatProcessMetadata(&metadata);
    }

    // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override
    {
        dmntchtExit();
    }

    // Called at the end to clean up all services previously initialized
    virtual void onShow() override
    {
    }

    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {} // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<tesla::OceanModifier>(metadata); // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

#endif // OCEAN_MODIFIER_INCLUDE_MAIN_H_
