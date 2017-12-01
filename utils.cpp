#include "utils.h"

#ifdef linux

std::string avahiBrowserEventToStdString(AvahiBrowserEvent event) {
    if (event == AVAHI_BROWSER_NEW) {
        return std::string("New");
    } else if (event == AVAHI_BROWSER_REMOVE) {
        return std::string("Remove");
    } else if (event == AVAHI_BROWSER_FAILURE) {
        return std::string("Failure");
    } else if (event == AVAHI_BROWSER_ALL_FOR_NOW) {
        return std::string("All for now");
    } else if (event == AVAHI_BROWSER_CACHE_EXHAUSTED) {
        return std::string("Cache Exhausted");
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(event)) + ")";
    }
}

std::string avahiClientStateToStdString(AvahiClientState state) {
    if (state == AVAHI_CLIENT_S_RUNNING) {
        return std::string("Running");
    } else if (state == AVAHI_CLIENT_S_REGISTERING) {
        return std::string("Registering");
    } else if (state == AVAHI_CLIENT_S_COLLISION) {
        return std::string("Collision");
    } else if (state == AVAHI_CLIENT_FAILURE) {
        return std::string("Failure");
    } else if (state == AVAHI_CLIENT_CONNECTING) {
        return std::string("Connecting");
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(state)) + ")";
    }
}

std::string avahiLookupResultFlagsToStdString(AvahiLookupResultFlags flags) {
    std::string s;

    if (flags & AVAHI_LOOKUP_RESULT_CACHED) {
        s += "cached,";
    }

    if (flags & AVAHI_LOOKUP_RESULT_WIDE_AREA) {
        s += "wide,";
    }

    if (flags & AVAHI_LOOKUP_RESULT_MULTICAST) {
        s += "multi";
    }

    if (flags & AVAHI_LOOKUP_RESULT_LOCAL) {
        s += "local";
    }

    if (flags & AVAHI_LOOKUP_RESULT_OUR_OWN) {
        s += "self";
    }

    if (flags & AVAHI_LOOKUP_RESULT_STATIC) {
        s += "static,";
    }

    if (s.empty()) {
        s += "none";
    }

    return s;
}

std::string avahiResolveEventToStdString(AvahiResolverEvent event) {
    if (event == AVAHI_RESOLVER_FOUND) {
        return "Found";
    } else if (event == AVAHI_RESOLVER_FAILURE) {
        return "Failure";
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(event)) + ")";
    }
}

std::string avahiProtocolToStdString(AvahiProtocol protocol) {
    if (protocol == AVAHI_PROTO_INET) {
        return std::string("IPv4");
    } else if (protocol == AVAHI_PROTO_INET6) {
        return std::string("IPv6");
    } else if (protocol == AVAHI_PROTO_UNSPEC) {
        return std::string("Unspecified");
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(protocol)) + ")";
    }
}


#endif
