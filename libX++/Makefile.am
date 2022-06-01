noinst_LIBRARIES = libX++.a

AM_CXXFLAGS=@QT_INCLUDES@ -DQT2

libX___a_SOURCES = \
xApp.cpp \
xCheckBox.cpp \
xComboBox.cpp \
xConfirmation.cpp \
xDefaults.cpp \
xDialog.cpp \
xEdit.cpp \
xEditList.cpp \
xFileDialog.cpp \
xFrame.cpp \
xGauge.cpp \
xInputFrame.cpp \
xLabel.cpp \
xLabeledList.cpp \
xLineEdit.cpp \
xLineEditQuery.cpp \
xMessageBox.cpp \
xMisc.cpp \
xMultiLineBuf.cpp \
xMultiLineEdit.cpp \
xMultiLineFrame.cpp \
xMultiLineWin.cpp \
xProgressBar.cpp \
xPshBtn.cpp \
xPshBtnFrame.cpp \
xPshBtnTable.cpp \
xResources.cpp \
xSQLBase.cpp \
xSQLField.cpp \
xSQLIndex.cpp \
xSQLTable.cpp \
xSQLView.cpp \
xSlider.cpp \
xSocket.cpp \
xSocketDialog.cpp \
xTable.cpp

libX___a_LIBADD = moc_xCheckBox.o \
moc_xComboBox.o \
moc_xConfirmation.o \
moc_xDialog.o \
moc_xEdit.o \
moc_xEditList.o \
moc_xFileDialog.o \
moc_xFrame.o \
moc_xGauge.o \
moc_xInputFrame.o \
moc_xLabel.o \
moc_xLabeledList.o \
moc_xLineEdit.o \
moc_xLineEditQuery.o \
moc_xMessageBox.o \
moc_xMultiLineEdit.o \
moc_xMultiLineFrame.o \
moc_xMultiLineWin.o \
moc_xProgressBar.o \
moc_xPshBtn.o \
moc_xPshBtnFrame.o \
moc_xPshBtnTable.o \
moc_xSQLField.o \
moc_xSQLTable.o \
moc_xSQLView.o \
moc_xSlider.o \
moc_xSocket.o \
moc_xSocketDialog.o \
moc_xTable.o

OBJECTS=$(libX___a_OBJECTS) $(METAOBJS)

CLEANFILES=moc*

EXTRA_DIST=*.h

# QT Meta header files
moc_%.cpp: %.h
	$(MOC) -o $@ $<
