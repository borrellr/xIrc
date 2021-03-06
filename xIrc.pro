SOURCES += xIrc.cpp xIrcBanDialog.cpp xIrcChannelQuery.cpp xIrcChanNickBox.cpp \
xIrcCommands.cpp xIrcConnect1.cpp xIrcConnect2.cpp xIrcConnect3.cpp \
xIrcConnectDialog.cpp xIrcDccChatFrame.cpp xIrcDccFile.cpp xIrcDccQuery.cpp \
xIrcDccSocket.cpp xIrcIgnoreQuery.cpp xIrcInviteBox.cpp \
xIrcMircServerParse.cpp xIrcMsgDispatch.cpp xIrcMsgDispatchEntry.cpp \
xIrcMsgFrame1.cpp xIrcMsgFrame2.cpp xIrcMsgQuery.cpp xIrcNickActionQuery.cpp \
xIrcNickCombo.cpp xIrcNickList.cpp xIrcNickListEntry.cpp xIrcNickQuery.cpp \
xIrcNotifyQuery.cpp xIrcNotifyTable.cpp xIrcPeopleEdit.cpp xIrcPeopleEntry.cpp \
xIrcPeopleList.cpp xIrcPeopleTable.cpp xIrcServerEdit.cpp xIrcServerEntry.cpp \
xIrcServerList.cpp xIrcServerQuery.cpp xIrcServerTable.cpp xIrcSocket.cpp
HEADERS += ircreply.h xIrcBanDialog.h xIrcChannelQuery.h xIrcChanNickBox.h \
xIrcCommands.h xIrcConnectDialog.h xIrcConnect.h xIrcDccChatFrame.h \
xIrcDccFile.h xIrcDccQuery.h xIrcDccSocket.h xIrcIgnoreQuery.h xIrcInviteBox.h \
xIrcLineEditQuery.h xIrcMircServerParse.h xIrcMsgDispatchEntry.h \
xIrcMsgDispatch.h xIrcMsgFrame.h xIrcMsgQuery.h xIrcNickActionQuery.h \
xIrcNickCombo.h xIrcNickListEntry.h xIrcNickList.h xIrcNickQuery.h \
xIrcNotifyQuery.h xIrcNotifyTable.h xIrcPeopleEdit.h xIrcPeopleEntry.h \
xIrcPeopleList.h xIrcPeopleTable.h xIrcServerEdit.h xIrcServerEntry.h \
xIrcServerList.h xIrcServerQuery.h xIrcServerTable.h xIrcSocket.h
INCLUDEPATH += ./libX++
LIBS += -lX++ -L./libX++
CONFIG += qt warn_on release
TARGET += xIrc
