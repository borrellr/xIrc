bin_PROGRAMS = xIrc

AM_CPPFLAGS=@QT_INCLUDES@ -I./libX++ -DXIRCRESOURCEPATH=\"@libdir@/xIrc\" -DQT2

xIrc_LDFLAGS=-lqt @QT_LDFLAGS@

xIrc_SOURCES = xIrc.cpp \
xIrcBanDialog.cpp \
xIrcChanNickBox.cpp \
xIrcChannelQuery.cpp \
xIrcCommands.cpp \
xIrcConnect1.cpp \
xIrcConnect2.cpp \
xIrcConnect3.cpp \
xIrcConnectDialog.cpp \
xIrcDccChatFrame.cpp \
xIrcDccFile.cpp \
xIrcDccQuery.cpp \
xIrcDccSocket.cpp \
xIrcIgnoreQuery.cpp \
xIrcInviteBox.cpp \
xIrcMircServerParse.cpp \
xIrcMsgDispatch.cpp \
xIrcMsgDispatchEntry.cpp \
xIrcMsgFrame1.cpp \
xIrcMsgFrame2.cpp \
xIrcMsgQuery.cpp \
xIrcNickActionQuery.cpp \
xIrcNickCombo.cpp \
xIrcNickList.cpp \
xIrcNickListEntry.cpp \
xIrcNickQuery.cpp \
xIrcNotifyQuery.cpp \
xIrcNotifyTable.cpp \
xIrcPeopleEdit.cpp \
xIrcPeopleEntry.cpp \
xIrcPeopleList.cpp \
xIrcPeopleTable.cpp \
xIrcServerEdit.cpp \
xIrcServerEntry.cpp \
xIrcServerList.cpp \
xIrcServerQuery.cpp \
xIrcServerTable.cpp \
xIrcSocket.cpp

xIrc_LDADD = moc_xIrcBanDialog.o \
moc_xIrcChanNickBox.o \
moc_xIrcChannelQuery.o \
moc_xIrcConnect.o \
moc_xIrcConnectDialog.o \
moc_xIrcDccChatFrame.o \
moc_xIrcDccFile.o \
moc_xIrcDccQuery.o \
moc_xIrcDccSocket.o \
moc_xIrcIgnoreQuery.o \
moc_xIrcInviteBox.o \
moc_xIrcMsgDispatchEntry.o \
moc_xIrcMsgFrame.o \
moc_xIrcMsgQuery.o \
moc_xIrcNickActionQuery.o \
moc_xIrcNickCombo.o \
moc_xIrcNickQuery.o \
moc_xIrcNotifyQuery.o \
moc_xIrcNotifyTable.o \
moc_xIrcPeopleEdit.o \
moc_xIrcPeopleTable.o \
moc_xIrcServerEdit.o \
moc_xIrcServerQuery.o \
moc_xIrcServerTable.o \
moc_xIrcSocket.o \
-L./libX++ -lX++

datadir=$(libdir)/xIrc

data_DATA = servers.dat xIrc.xpm xIrc.defaults

CLEANFILES=moc_*

SUBDIRS=libX++

EXTRA_DIST=xIrc.defaults xIrc.doc xIrc.dvi xIrc.lsm xIrc.ps xIrc.tex xIrc.xpm \
servers.dat *.h

# QT Meta header files
moc_%.cpp: %.h
	$(MOC) -o $@ $<
