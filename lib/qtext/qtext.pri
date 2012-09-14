# qtext.pri
# 6/28/2011

DEFINES += WITH_LIB_QTEXT
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/actionwithid.h \
    $$PWD/algorithm.h \
    $$PWD/bitwise.h \
    $$PWD/bytearray.h \
    $$PWD/checkbox.h \
    $$PWD/combobox.h \
    $$PWD/compactstackedlayout.h \
    $$PWD/countdowntimer.h \
    $$PWD/dateedit.h \
    $$PWD/datetime.h \
    $$PWD/datetimeedit.h \
    $$PWD/doublespinbox.h \
    $$PWD/dialog.h \
    $$PWD/draggablemainwindow.h \
    $$PWD/draggablewidget.h \
    $$PWD/eventforwarder.h \
    $$PWD/filedeleter.h \
    $$PWD/filesystem.h \
    $$PWD/fontcombobox.h \
    $$PWD/hash.h \
    $$PWD/highlighttextedit.h \
    $$PWD/htmltag.h \
    $$PWD/layoutwidget.h \
    $$PWD/namedcheckbox.h \
    $$PWD/namedradiobutton.h \
    $$PWD/network.h \
    $$PWD/networkcookie.h \
    $$PWD/os.h \
    $$PWD/overlaylayout.h \
    $$PWD/radiobutton.h \
    $$PWD/rubberband.h \
    $$PWD/slider.h \
    $$PWD/spinbox.h \
    $$PWD/ss.h \
    $$PWD/stoppable.h \
    $$PWD/string.h \
    $$PWD/systemtrayicon.h \
    $$PWD/texthighlighter.h \
    $$PWD/textedit.h \
    $$PWD/timeedit.h \
    $$PWD/toolbutton.h \
    $$PWD/toolbuttonwithid.h \
    $$PWD/webview.h \
    $$PWD/webpage.h \
    $$PWD/webpluginfactory.h \
    $$PWD/withsizehint.h

SOURCES += \
    $$PWD/actionwithid.cc \
    $$PWD/checkbox.cc \
    $$PWD/combobox.cc \
    $$PWD/countdowntimer.cc \
    $$PWD/dateedit.cc \
    $$PWD/datetime.cc \
    $$PWD/datetimeedit.cc \
    $$PWD/dialog.cc \
    $$PWD/doublespinbox.cc \
    $$PWD/draggablemainwindow.cc \
    $$PWD/draggablewidget.cc \
    $$PWD/filedeleter.cc \
    $$PWD/filesystem.cc \
    $$PWD/fontcombobox.cc \
    $$PWD/highlighttextedit.cc \
    $$PWD/networkcookie.cc \
    $$PWD/os.cc \
    $$PWD/radiobutton.cc \
    $$PWD/rubberband.cc \
    $$PWD/slider.cc \
    $$PWD/spinbox.cc \
    $$PWD/string.cc \
    $$PWD/texthighlighter.cc \
    $$PWD/textedit.cc \
    $$PWD/timeedit.cc \
    $$PWD/toolbutton.cc \
    $$PWD/toolbuttonwithid.cc \
    $$PWD/webview.cc \
    $$PWD/webpage.cc \
    $$PWD/webpluginfactory.cc

QT +=   core gui network webkit

DOC_FILES += \
    $$PWD/doc/start.html

IMAGE_FILES += \
    $$PWD/images/favicon.ico \
    $$PWD/images/null.png

OTHER_FILES += \
    $$DOC_FILES \
    $$IMAGE_FILES

# EOF