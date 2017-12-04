#include "utils.h"

#ifdef linux

QString avahiBrowserEventToQString(AvahiBrowserEvent event) {
    if (event == AVAHI_BROWSER_NEW) {
        return QString("New");
    } else if (event == AVAHI_BROWSER_REMOVE) {
        return QString("Remove");
    } else if (event == AVAHI_BROWSER_FAILURE) {
        return QString("Failure");
    } else if (event == AVAHI_BROWSER_ALL_FOR_NOW) {
        return QString("All for now");
    } else if (event == AVAHI_BROWSER_CACHE_EXHAUSTED) {
        return QString("Cache Exhausted");
    } else {
        return QString("Unknown") + " (" + QString::number(static_cast<int>(event)) + ")";
    }
}

QString avahiClientStateToQString(AvahiClientState state) {
    if (state == AVAHI_CLIENT_S_RUNNING) {
        return QString("Running");
    } else if (state == AVAHI_CLIENT_S_REGISTERING) {
        return QString("Registering");
    } else if (state == AVAHI_CLIENT_S_COLLISION) {
        return QString("Collision");
    } else if (state == AVAHI_CLIENT_FAILURE) {
        return QString("Failure");
    } else if (state == AVAHI_CLIENT_CONNECTING) {
        return QString("Connecting");
    } else {
        return QString("Unknown") + " (" + QString::number(static_cast<int>(state)) + ")";
    }
}

QString avahiLookupResultFlagsToQString(AvahiLookupResultFlags flags) {
    QString s;

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

    if (s.length() == 0) {
        s += "none";
    }

    return s;
}

QString avahiResolveEventToQString(AvahiResolverEvent event) {
    if (event == AVAHI_RESOLVER_FOUND) {
        return QString("Found");
    } else if (event == AVAHI_RESOLVER_FAILURE) {
        return QString("Failure");
    } else {
        return QString("Unknown") + " (" + QString::number(static_cast<int>(event)) + ")";
    }
}

QString avahiProtocolToQString(AvahiProtocol protocol) {
    if (protocol == AVAHI_PROTO_INET) {
        return QString("IPv4");
    } else if (protocol == AVAHI_PROTO_INET6) {
        return QString("IPv6");
    } else if (protocol == AVAHI_PROTO_UNSPEC) {
        return QString("Unspecified");
    } else {
        return QString("Unknown") + " (" + QString::number(static_cast<int>(protocol)) + ")";
    }
}


#endif
