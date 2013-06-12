# Auto-generated by IDE. Any changes made by user will be lost!
BASEDIR =  $$quote($$_PRO_FILE_PWD_)

device {
    CONFIG(debug, debug|release) {
        SOURCES +=  $$quote($$BASEDIR/src/ApplicationUI.cpp) \
                 $$quote($$BASEDIR/src/PushContentController.cpp) \
                 $$quote($$BASEDIR/src/dao/BaseDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/ConfigurationDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/PushDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/PushHistoryDAO.cpp) \
                 $$quote($$BASEDIR/src/main.cpp) \
                 $$quote($$BASEDIR/src/service/ConfigurationService.cpp) \
                 $$quote($$BASEDIR/src/service/PushHandler.cpp) \
                 $$quote($$BASEDIR/src/service/PushNotificationService.cpp) \
                 $$quote($$BASEDIR/src/service/QBNetwork.cpp) \
                 $$quote($$BASEDIR/src/vo/Configuration.cpp) \
                 $$quote($$BASEDIR/src/vo/Push.cpp) \
                 $$quote($$BASEDIR/src/vo/PushHistoryItem.cpp)

        HEADERS +=  $$quote($$BASEDIR/src/ApplicationUI.hpp) \
                 $$quote($$BASEDIR/src/PushContentController.hpp) \
                 $$quote($$BASEDIR/src/dao/BaseDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/ConfigurationDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/PushDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/PushHistoryDAO.hpp) \
                 $$quote($$BASEDIR/src/service/ConfigurationService.hpp) \
                 $$quote($$BASEDIR/src/service/PushHandler.hpp) \
                 $$quote($$BASEDIR/src/service/PushNotificationService.hpp) \
                 $$quote($$BASEDIR/src/service/QBNetwork.hpp) \
                 $$quote($$BASEDIR/src/vo/Configuration.hpp) \
                 $$quote($$BASEDIR/src/vo/Push.hpp) \
                 $$quote($$BASEDIR/src/vo/PushHistoryItem.hpp)
    }

    CONFIG(release, debug|release) {
        SOURCES +=  $$quote($$BASEDIR/src/ApplicationUI.cpp) \
                 $$quote($$BASEDIR/src/PushContentController.cpp) \
                 $$quote($$BASEDIR/src/dao/BaseDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/ConfigurationDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/PushDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/PushHistoryDAO.cpp) \
                 $$quote($$BASEDIR/src/main.cpp) \
                 $$quote($$BASEDIR/src/service/ConfigurationService.cpp) \
                 $$quote($$BASEDIR/src/service/PushHandler.cpp) \
                 $$quote($$BASEDIR/src/service/PushNotificationService.cpp) \
                 $$quote($$BASEDIR/src/service/QBNetwork.cpp) \
                 $$quote($$BASEDIR/src/vo/Configuration.cpp) \
                 $$quote($$BASEDIR/src/vo/Push.cpp) \
                 $$quote($$BASEDIR/src/vo/PushHistoryItem.cpp)

        HEADERS +=  $$quote($$BASEDIR/src/ApplicationUI.hpp) \
                 $$quote($$BASEDIR/src/PushContentController.hpp) \
                 $$quote($$BASEDIR/src/dao/BaseDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/ConfigurationDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/PushDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/PushHistoryDAO.hpp) \
                 $$quote($$BASEDIR/src/service/ConfigurationService.hpp) \
                 $$quote($$BASEDIR/src/service/PushHandler.hpp) \
                 $$quote($$BASEDIR/src/service/PushNotificationService.hpp) \
                 $$quote($$BASEDIR/src/service/QBNetwork.hpp) \
                 $$quote($$BASEDIR/src/vo/Configuration.hpp) \
                 $$quote($$BASEDIR/src/vo/Push.hpp) \
                 $$quote($$BASEDIR/src/vo/PushHistoryItem.hpp)
    }
}

simulator {
    CONFIG(debug, debug|release) {
        SOURCES +=  $$quote($$BASEDIR/src/ApplicationUI.cpp) \
                 $$quote($$BASEDIR/src/PushContentController.cpp) \
                 $$quote($$BASEDIR/src/dao/BaseDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/ConfigurationDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/PushDAO.cpp) \
                 $$quote($$BASEDIR/src/dao/PushHistoryDAO.cpp) \
                 $$quote($$BASEDIR/src/main.cpp) \
                 $$quote($$BASEDIR/src/service/ConfigurationService.cpp) \
                 $$quote($$BASEDIR/src/service/PushHandler.cpp) \
                 $$quote($$BASEDIR/src/service/PushNotificationService.cpp) \
                 $$quote($$BASEDIR/src/service/QBNetwork.cpp) \
                 $$quote($$BASEDIR/src/vo/Configuration.cpp) \
                 $$quote($$BASEDIR/src/vo/Push.cpp) \
                 $$quote($$BASEDIR/src/vo/PushHistoryItem.cpp)

        HEADERS +=  $$quote($$BASEDIR/src/ApplicationUI.hpp) \
                 $$quote($$BASEDIR/src/PushContentController.hpp) \
                 $$quote($$BASEDIR/src/dao/BaseDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/ConfigurationDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/PushDAO.hpp) \
                 $$quote($$BASEDIR/src/dao/PushHistoryDAO.hpp) \
                 $$quote($$BASEDIR/src/service/ConfigurationService.hpp) \
                 $$quote($$BASEDIR/src/service/PushHandler.hpp) \
                 $$quote($$BASEDIR/src/service/PushNotificationService.hpp) \
                 $$quote($$BASEDIR/src/service/QBNetwork.hpp) \
                 $$quote($$BASEDIR/src/vo/Configuration.hpp) \
                 $$quote($$BASEDIR/src/vo/Push.hpp) \
                 $$quote($$BASEDIR/src/vo/PushHistoryItem.hpp)
    }
}

INCLUDEPATH +=  $$quote($$BASEDIR/src/dao) \
         $$quote($$BASEDIR/src/service) \
         $$quote($$BASEDIR/src/vo) \
         $$quote($$BASEDIR/src)

CONFIG += precompile_header

PRECOMPILED_HEADER =  $$quote($$BASEDIR/precompiled.h)

lupdate_inclusion {
    SOURCES +=  $$quote($$BASEDIR/../src/*.c) \
             $$quote($$BASEDIR/../src/*.c++) \
             $$quote($$BASEDIR/../src/*.cc) \
             $$quote($$BASEDIR/../src/*.cpp) \
             $$quote($$BASEDIR/../src/*.cxx) \
             $$quote($$BASEDIR/../assets/*.qml) \
             $$quote($$BASEDIR/../assets/*.js) \
             $$quote($$BASEDIR/../assets/*.qs)

    HEADERS +=  $$quote($$BASEDIR/../src/*.h) \
             $$quote($$BASEDIR/../src/*.h++) \
             $$quote($$BASEDIR/../src/*.hh) \
             $$quote($$BASEDIR/../src/*.hpp) \
             $$quote($$BASEDIR/../src/*.hxx)
}

TRANSLATIONS =  $$quote($${TARGET}.ts)
