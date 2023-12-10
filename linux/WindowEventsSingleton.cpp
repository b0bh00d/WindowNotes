#include <QtCore/QMetaProperty>

#include "WindowEventsSingleton.h"

WindowEventsPtr win_events_;

DeclareSingleton(WindowEventsSingleton);

WindowEventsPtr WindowEventsSingleton::winEvents() const
{
    if(win_events_.isNull())
        win_events_ = WindowEventsPtr(new WindowEvents());
    return win_events_;
}
