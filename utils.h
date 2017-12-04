#ifndef UTILS_H
#define UTILS_H

#include <QString>

#ifdef linux

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

QString avahiBrowserEventToQString(AvahiBrowserEvent event);
QString avahiClientStateToQString(AvahiClientState state);
QString avahiLookupResultFlagsToQString(AvahiLookupResultFlags flags);
QString avahiProtocolToQString(AvahiProtocol protocol);
QString avahiResolveEventToQString(AvahiResolverEvent event);

#endif // linux

#endif // UTILS_H
