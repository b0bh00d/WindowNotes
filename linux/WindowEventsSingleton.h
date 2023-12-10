#pragma once

#include "WindowEvents.h"

#include "singleton.h"
#include "events_types.h"

class WindowEventsSingleton : public Singleton<WindowEventsSingleton>, public no_copy_or_move
{
public:     // methods
    WindowEventsPtr  winEvents() const;
};
