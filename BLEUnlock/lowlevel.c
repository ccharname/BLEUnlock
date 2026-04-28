#include "lowlevel.h"
#include <dlfcn.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOKitLib.h>

void wakeDisplay(void)
{
    static IOPMAssertionID assertionID;
    IOPMAssertionDeclareUserActivity(CFSTR("BLEUnlock"), kIOPMUserActiveLocal, &assertionID);
}

void sleepDisplay(void)
{
    mach_port_t mainPort;
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 120000
    if (__builtin_available(macOS 12.0, *)) {
        mainPort = kIOMainPortDefault;
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        mainPort = kIOMasterPortDefault;
#pragma clang diagnostic pop
    }
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    mainPort = kIOMasterPortDefault;
#pragma clang diagnostic pop
#endif

    io_registry_entry_t reg = IORegistryEntryFromPath(mainPort, "IOService:/IOResources/IODisplayWrangler");
    if (reg) {
        IORegistryEntrySetCFProperty(reg, CFSTR("IORequestIdle"), kCFBooleanTrue);
        IOObjectRelease(reg);
    }
}

int lockScreenImmediate(void)
{
    typedef int (*SACLockScreenImmediateFunction)(void);
    static SACLockScreenImmediateFunction lockScreen = NULL;
    static bool didLookup = false;

    if (!didLookup) {
        didLookup = true;
        void *handle = dlopen("/System/Library/PrivateFrameworks/login.framework/login", RTLD_LAZY);
        if (handle) {
            lockScreen = (SACLockScreenImmediateFunction)dlsym(handle, "SACLockScreenImmediate");
        }
    }

    if (!lockScreen) {
        return -1;
    }
    return lockScreen();
}
