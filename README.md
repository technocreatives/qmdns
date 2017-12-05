# QmDNS

A multiplatform Qt component for mDNS (Bonjour/ZeroConf) service discovery.

## Requirements

* Qt5
* libavahi-client-dev (Linux)

## Build Sample

```
qmake

make

./qmDNS
```

## Importing as Library

```
# Replace <QmDNS_DIR> by the location of this project directory.

include(<QmDNS_DIR>/QmDNS.pri)
INCLUDEPATH += <QmDNS_DIR>
HEADERS     += $$files(<QmDNS_DIR>/*h)

```

## Exporting to QML

This library can accessed directly from QML to do mDNS service discovery.

```
#include "qmdns.h"

int main(int argc, char** agv) {
    QtGuiApplication app(argc, argv);
    ...
    qmlRegisterType<QmDNS>("com.technocreatives.qmdns", 0, 1, "QmDNS");
    qmlRegisterType<QmDNSService>("com.technocreatives.qmdns", 0, 1, "QmDNSService");
    ...
    return app.exec();
}

```

## Using on QML

```
import com.technocreatives.qmdns 0.1

QmDNS {
    Component.onCompleted: {
        try {
            init(); // required to setup safely
            startServiceBrowse("_http._tcp", QmDNS.IPV4);
        } catch (e) {
            // logging
        }
    }

    onServiceFound: {
        // Called when service was discovered but not populated
    }

    onServiceLost: {
        // Called when service left
    }

    onServiceResolved: {
        // Called when discovered service fully populated
    }

    onServiceNotResolved: {
        // Called when an error prevent service from being populated
    }
}
```

## Author

Emiliano Firmino
