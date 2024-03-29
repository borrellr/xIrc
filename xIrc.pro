SOURCES += xIrc.cpp \
xIrcCommands.cpp xIrcConnect1.cpp xIrcConnect2.cpp xIrcConnect3.cpp \
xIrcDccChatFrame.cpp xIrcDccFile.cpp xIrcDccQuery.cpp xIrcDccSocket.cpp \
xIrcMsgDispatch.cpp xIrcMsgDispatchEntry.cpp xIrcMsgFrame1.cpp \
xIrcMsgFrame2.cpp xIrcMsgQuery.cpp xIrcNickCombo.cpp xIrcNickList.cpp \
xIrcNickListEntry.cpp xIrcNotifyQuery.cpp xIrcNotifyTable.cpp \
xIrcPeopleEntry.cpp xIrcPeopleList.cpp xIrcPeopleTable.cpp xIrcSocket.cpp


HEADERS += ircreply.h xIrcChannelQuery.h \
xIrcCommands.h xIrcConnect.h xIrcDccChatFrame.h \
xIrcDccFile.h xIrcDccQuery.h xIrcDccSocket.h \
xIrcLineEditQuery.h xIrcMsgDispatchEntry.h \
xIrcMsgDispatch.h xIrcMsgFrame.h xIrcMsgQuery.h \
xIrcNickCombo.h xIrcNickListEntry.h xIrcNickList.h \
xIrcNotifyQuery.h xIrcNotifyTable.h xIrcPeopleEntry.h \
xIrcPeopleList.h xIrcPeopleTable.h xIrcSocket.h


INTERFACES += servereditdialog.ui \
           serverconnectdialog.ui \
           xservquery.ui \
           quitform.ui \
           kickmsgdialog.ui \
           nicknameform.ui \
           channeldialog.ui \
           peopleedit.ui \
           nickaction.ui \
           bandialog.ui \
           channickbox.ui \
           invitebox.ui \
           peopledialog.ui

INCLUDEPATH += /usr/local/include/libX
LIBS += -lX++ -L/usr/local/lib/xIrc
DEFINES += QT2 QT3 QT_NO_ASCII_CAST
CONFIG += qt warn_on release thread
QMAKE_CXXFLAGS += -fpermissive -Wwrite-strings
TARGET += xIrc
