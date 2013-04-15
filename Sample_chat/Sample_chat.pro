APP_NAME = Sample_chat

CONFIG += qt warn_on cascades10

INCLUDEPATH += ../qxmpp/include

DEPENDPATH += ../qxmpp/include

LIBS += -L../qxmpp

LIBS += -lbbdata
LIBS += -lbbsystem
LIBS += -lbbdevice
LIBS += -DQXMPP_SHARED

QT += network

include(config.pri)

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
        LIBS += -lqxmpp_armv7
    }

    CONFIG(release, debug|release) {
        # Device-Release custom configuration
        LIBS += -lqxmpp_armv7
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
        LIBS += -lqxmpp_x86
    }
}
