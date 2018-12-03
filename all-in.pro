QT += quick
QT += sql
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Users/zhenl/AppData/Local/Continuum/miniconda3/Library/lib/ -larrow -lparquet
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Users/zhenl/AppData/Local/Continuum/miniconda3/Library/lib/ larrow -lparquet

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third-party/levelDB/release -lleveldb
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third-party/levelDB/debug -lleveldb

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third-party/sqlite3/release -lsqlite3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third-party/sqlite3/debug -lsqlite3

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third-party/arrow/release -larrow -lparquet
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third-party/arrow/debug -larrow -lparquet

#INCLUDEPATH += $$PWD/../../Users/zhenl/AppData/Local/Continuum/miniconda3/Library/include
INCLUDEPATH += $$PWD/third-party/sqlite3/include
INCLUDEPATH += $$PWD/third-party/levelDB/include
INCLUDEPATH += $$PWD/third-party/arrow/include

RESOURCES += qml\qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/sql_parser/CsvSQLParser.h \
    include/view/DataViewQML.h   \
    include/view/DataTableModel.h \
    include/file_extractor/CsvExtractor.h   \
    include/utility/CVDate.h    \
    include/common/EmptyValue.h \
    include/common/MetaData.h \
    include/view/MetaDataQML.h  \
    include/column_storage/ColumnStorageReader.h  \
    include/column_storage/ColumnStorageWriter.h \
    include/Extractor.h \
    include/common/DBInterface.h \
    include/sql_parser/SQLiteParserAdapter.h \
    include/view/QueryViewQML.h \
    include/common/NodeInfo.h


SOURCES += \
    src/view/DataViewQML.cpp \
    src/view/DataTableModel.cpp \
    src/file_extractor/CsvExtractor.cpp \
    src/utility/CVDate.cpp \
    src/view/MetaDataQML.cpp    \
    src/column_storage/ColumnStorageReader.cpp \
    src/column_storage/ColumnStorageWriter.cpp \
    src/Extractor.cpp \
    src/common/MetaData.cpp \
    src/common/DBInterface.cpp \
    src\main.cpp \
    src/sql_parser/CsvSQLParser.cpp \
    src/common/NodeInfo.cpp

DISTFILES +=
