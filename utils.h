#ifndef UTILS_H
#define UTILS_H

#ifdef linux

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include <string>

std::string avahiBrowserEventToStdString(AvahiBrowserEvent event);
std::string avahiClientStateToStdString(AvahiClientState state);
std::string avahiLookupResultFlagsToStdString(AvahiLookupResultFlags flags);
std::string avahiProtocolToStdString(AvahiProtocol protocol);
std::string avahiResolveEventToStdString(AvahiResolverEvent event);

#endif // linux

#endif // UTILS_H
