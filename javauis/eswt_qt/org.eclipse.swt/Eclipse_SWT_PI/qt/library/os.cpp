/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <QApplication>
#include <QAction>
#include <QActionGroup>
#include <QEventLoop>
#include <QMainWindow>
#include <QString>
#include <QVariant>
#include <QWidget>
#include <QDesktopWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QToolButton>
#include <QCheckBox>
#include <QLabel>
#include <QPalette>
#include <QColor>
#include <QPixmap>
#include <QAbstractEventDispatcher>
#include <QThread>
#include <QVector>
#include <QTimer>
#include <QMenu>
#include <QMenuBar>
#include <QFrame>
#include <QScrollArea>
#include <QScrollBar>
#include <QListView>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>
#include <QTextOption>
#include <QClipboard>
#include <QScrollBar>
#include <QFontMetrics>
#include <QSlider>
#include <QProgressBar>
#include <QComboBox>
#include <QColorDialog>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTableWidget>
#include <QHeaderView>
#include <QFontDatabase>
#include <QListWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <new>
#include <QDesktopServices>
#include <QUrl>
#include <QTreeWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QUrl>
#include <QCalendarWidget>
#include <QProgressDialog>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QFontDialog>
#include <QLocale>
#include <QInputContextFactory>
#include <QChar>
#include <QLibrary>

#ifdef __SYMBIAN32__
#include <xqservicerequest.h>
#include <xqcallinfo.h>
#include <cntservicescontact.h>
#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration.h>
#include <hbinputsettingproxy.h>
#include <hbicon.h>
#include <XQAiwRequest.h>
#include <xqappmgr.h>
#include <xqaiwdecl.h>
#endif

#include <org_eclipse_swt_internal_qt_OS.h>
#include "eventcallback.h"
#include "slotcallback.h"
#include "swtlog.h"
#include "jniutils.h"
#include "swtfontcache.h"
#include "swt.h"
#include "swterror.h"
#include "listdatamodel.h"
#include "qcaptionedwidget.h"
#include "qswttimeedit.h"
#include "qtasktipbar.h"
#include "swtapplication.h"
#include "qswttabwidget.h"
#include "autorelease.h"
#include "graphics.h"

#ifdef __SYMBIAN32__
#include "swts60.h"
#include "swtmobiledevice.h"
#endif

using namespace Java::eSWT;

#define OS_NATIVE(func) Java_org_eclipse_swt_internal_qt_OS_##func

#ifdef Q_WS_X11
void extern qt_x11_wait_for_window_manager(QWidget* widget);
#endif

const uint KParagraphSeparator = 8233;
const int KTimedMessageBoxTimeout = 3000;

// Following lines duplicate 3 hard-coded values from QLineEdit.
// They are needed for computing client area size.
// But there is no way to access them directly
const int QLineEdit_minimumLineSpacing = 14;
const int QLineEdit_verticalMargin = 1;
const int QLineEdit_horizontalMargin = 2;

#ifdef __cplusplus
extern "C" {
#endif

//
// QApplication
//

JNIEXPORT jint JNICALL OS_NATIVE(QApplication_1swt_1new)
  (JNIEnv* aJniEnv, jclass , jint aJniUtils)
    {
    SwtApplication* app = NULL;
    HANDLE_TO_POINTER( JniUtils*, jniUtils, aJniUtils);

    class Args : public QObject
    {
    public:
        Args() : argc(0) {}
        virtual ~Args() {}
        char* argv[1];
        int argc;
    };
    Args* args = NULL;

    try
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("Compiled against Qt version %s", QT_VERSION_STR);

        args = new Args();
        QCoreApplication* oldApp = NULL;
        oldApp = QCoreApplication::instance();
        if( !oldApp )
            {
            app = new SwtApplication( args->argc, args->argv, jniUtils );
            }
        else
            {
            // QApplication already exists
            jniUtils->Throw( aJniEnv, ESwtErrorUnspecified );
            }
        args->setParent( app );

// Load qttestability plugin. Normally QApplication does this, but on Symbian it fails, so we need to
// load the plugin manually. Will be fixed to Qt 4.7.
#if !defined(QT_NO_LIBRARY)
        QLibrary testLib("qttestability");
        if (testLib.load())
            {
            typedef void (*TasInitialize)(void);
            TasInitialize initFunction = (TasInitialize)testLib.resolve("qt_testability_init");
#ifdef Q_OS_SYMBIAN
            //not found so use ordinal
            if (!initFunction)
                {
                initFunction = (TasInitialize)testLib.resolve("1");
                }
#endif
            if (initFunction)
                {
                initFunction();
                }
            }
#endif

        }
    catch(...)
        {
        delete args;
        delete app;
        jniUtils->Throw( aJniEnv, ESwtErrorNoHandles );
        }
    return POINTER_TO_HANDLE( app );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1swt_1palette_1new__ )
  (JNIEnv* aJniEnv, jclass)
    {
    QPalette* palette( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        palette = new QPalette( QApplication::palette () );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( palette );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1swt_1palette_1new__I )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    QPalette* palette( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        palette = new QPalette( QApplication::palette(widget) );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( palette );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1focusWidget )
  (JNIEnv* aJniEnv, jclass)
    {
    jint focusHandle = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();

        QApplication* app = static_cast<QApplication*>( QCoreApplication::instance() );
        if( app )
            {
            QWidget* focusWidget = app->focusWidget();
            focusHandle = POINTER_TO_HANDLE( focusWidget );
            }
        }
    SWT_CATCH
    return focusHandle;
    }

JNIEXPORT void JNICALL OS_NATIVE( QApplication_1setQuitOnLastWindowClosed )
  (JNIEnv* aJniEnv , jclass, jboolean aQuit)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        static_cast<QApplication*>(qApp)->setQuitOnLastWindowClosed( aQuit == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1desktop )
  (JNIEnv* aJniEnv , jclass)
    {
    QDesktopWidget* res = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        res = qApp->desktop();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( res );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1swt_1desktopPaintDevice )
  (JNIEnv* aJniEnv , jclass)
    {
    QPaintDevice* res = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        res = static_cast<QPaintDevice*>( QApplication::desktop() );
        }
    SWT_CATCH
    return reinterpret_cast<jint>( res );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1activeWindow )
  (JNIEnv* aJniEnv , jclass)
    {
    QWidget* active = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        active = QApplication::activeWindow();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( active );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1swt_1font_1new__ )
  (JNIEnv* aJniEnv , jclass)
    {
    QFont* font = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();

        // Implicitly shared, shallow copy
        font = new QFont( QApplication::font() );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( font );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1swt_1font_1new__I )
  (JNIEnv* aJniEnv, jclass, jint aWidgetHandle)
    {
    QFont* font = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QWidget*, widget, aWidgetHandle );

        // Implicitly shared, shallow copy
        font = new QFont( QApplication::font( widget ) );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( font );
    }

JNIEXPORT void JNICALL OS_NATIVE( QApplication_1beep )
  (JNIEnv* aJniEnv , jclass)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        static_cast<QApplication*>(qApp)->beep();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QApplication_1syncX )
  (JNIEnv* aJniEnv , jclass)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        static_cast<QApplication*>(qApp)->syncX();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1doubleClickInterval )
    ( JNIEnv* aJniEnv , jclass )
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        res = static_cast<jint>( qApp->doubleClickInterval() );
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1style )
  (JNIEnv* aJniEnv , jclass)
    {
    QStyle* style = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();

        QApplication* app = static_cast<QApplication*>( QCoreApplication::instance() );
        if( app )
            {
            style = app->style();
            }
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( style );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QApplication_1layoutDirection )
  (JNIEnv* aJniEnv, jclass)
    {
    Qt::LayoutDirection direction = Qt::LeftToRight;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        direction = qApp->layoutDirection();
        }
    SWT_CATCH
    return static_cast<int>( direction );
    }

JNIEXPORT void JNICALL OS_NATIVE( QApplication_1setLayoutDirection )
  (JNIEnv* aJniEnv, jclass, jint aDirection)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("direction=%d", aDirection);
        qApp->setLayoutDirection(static_cast<Qt::LayoutDirection>(aDirection));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QApplication_1setDoubleClickInterval )
  (JNIEnv * aJniEnv, jclass, jint aInterval)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("interval=%d", aInterval);
        qApp->setDoubleClickInterval( aInterval );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QApplication_1autoSipEnabled)
  (JNIEnv* aJniEnv, jclass)
    {
    jboolean enabled = JNI_FALSE;
#ifndef Q_WS_X11
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        enabled = qApp->autoSipEnabled();
        }
    SWT_CATCH
#endif
    return ( enabled ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT void JNICALL OS_NATIVE(QApplication_1setStyleSheet)
  (JNIEnv* aJniEnv, jclass, jstring aStyle)
  {
   SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        qApp->setStyleSheet( swtApp->jniUtils().JavaStringToQString( aJniEnv, aStyle ) );
        }
    SWT_CATCH
  }

JNIEXPORT void JNICALL OS_NATIVE(QApplication_1closeAllWindows)
  (JNIEnv* aJniEnv, jclass)
  {
   SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        qApp->closeAllWindows();
        }
    SWT_CATCH
  }

//
// QCoreApplication
//

JNIEXPORT jint JNICALL OS_NATIVE(QCoreApplication_1instance)
  ( JNIEnv* aJniEnv  , jclass )
    {
    QApplication* app = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        app = static_cast<QApplication*>(QCoreApplication::instance());
        }
    SWT_CATCH
    return POINTER_TO_HANDLE(app);
    }

JNIEXPORT void JNICALL OS_NATIVE( QCoreApplication_1setApplicationName)
  ( JNIEnv* aJniEnv, jclass, jint aHandle, jstring aName )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QApplication*, app, aHandle );
        app->setApplicationName( swtApp->jniUtils().JavaStringToQString( aJniEnv, aName ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QCoreApplication_1processEvents )
  ( JNIEnv* aJniEnv , jclass, jint handle, jint flags )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x flags=%x", handle, flags);

        HANDLE_TO_POINTER( QCoreApplication*, app, handle );
        app->processEvents( static_cast<QEventLoop::ProcessEventsFlags>( flags ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QCoreApplication_1sendPostedEvents__I)
  (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QCoreApplication*, app, aHandle );
        app->sendPostedEvents();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QCoreApplication_1sendPostedEvents__III)
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aReceiver, jint aEventType )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QCoreApplication*, app, aHandle );
        HANDLE_TO_POINTER( QObject*, receiver, aReceiver );
        app->sendPostedEvents( receiver, aEventType );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE(QCoreApplication_1hasPendingEvents)
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean hasEvents = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QCoreApplication*, app, aHandle );
        hasEvents = static_cast<int>(app->hasPendingEvents());
        }
    SWT_CATCH
    return hasEvents;
    }

JNIEXPORT void JNICALL OS_NATIVE( QCoreApplication_1postEvent )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aReceiver, jint aEventHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x receiver=%x event=%x", aHandle, aReceiver, aEventHandle);

        HANDLE_TO_POINTER( QCoreApplication*, app, aHandle );
        HANDLE_TO_POINTER( QObject*, receiver, aReceiver );

        // Not a QObject
        QEvent* event = reinterpret_cast<QEvent*>( aEventHandle );

        // Takes ownership of the event
        app->postEvent( receiver, event );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QCoreApplication_1sendEvent )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aReceiver, jint aEventHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x receiver=%x event=%x", aHandle, aReceiver, aEventHandle);

        HANDLE_TO_POINTER( QCoreApplication*, app, aHandle );
        HANDLE_TO_POINTER( QObject*, receiver, aReceiver );

        // Not a QObject
        QEvent* event = reinterpret_cast<QEvent*>( aEventHandle );

        // Does not take ownership of the event
        app->sendEvent( receiver, event );
        delete event;
        }
    SWT_CATCH
    }

//
// QAbstractEventDispatcher
//

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractEventDispatcher_1instance )
  ( JNIEnv* aJniEnv , jclass, jint aThreadHandle )
    {
    QAbstractEventDispatcher* dispatcher = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aThreadHandle );
        HANDLE_TO_POINTER( QThread*, thread, aThreadHandle );
        dispatcher = QAbstractEventDispatcher::instance( thread );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( dispatcher );
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractEventDispatcher_1wakeup)
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractEventDispatcher*, dispatcher, aHandle );
        dispatcher->wakeUp();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractEventDispatcher_1interrupt )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractEventDispatcher*, dispatcher, aHandle );
        dispatcher->interrupt();
        }
    SWT_CATCH
    }

//
// QMainWindow
//

JNIEXPORT jint JNICALL OS_NATIVE(QMainWindow_1new)
  ( JNIEnv* aJniEnv , jclass, jint aParent, jint aFlags )
    {
    QMainWindow* mainWindow = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("parent=%x flags=%x", aParent, aFlags);

        HANDLE_TO_POINTER( QWidget*, parent, aParent );
        mainWindow = new QMainWindow( parent, static_cast<Qt::WindowFlags>(aFlags) );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( mainWindow );
    }

JNIEXPORT void JNICALL OS_NATIVE( QMainWindow_1setMenuBar )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMenuBarHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x menubarHandle=%x", aHandle, aMenuBarHandle );
        HANDLE_TO_POINTER( QMainWindow*, mainWindow, aHandle );
        HANDLE_TO_POINTER( QMenuBar*, menuBar, aMenuBarHandle );
        mainWindow->setMenuBar( menuBar );
        }
    SWT_CATCH

    }

JNIEXPORT void JNICALL OS_NATIVE( QMainWindow_1setCentralWidget )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidget)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x widget=%x", aHandle, aWidget );
        HANDLE_TO_POINTER( QMainWindow*, mainWindow, aHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aWidget );
        mainWindow->setCentralWidget( widget );
        }
    SWT_CATCH
    }

//
// QPaintDevice
//

JNIEXPORT jint JNICALL OS_NATIVE( QPaintDevice_1depth )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        // Not a QObject
        QPaintDevice* dev = reinterpret_cast<QPaintDevice*>( static_cast<int>( aHandle ) );
        res = dev->depth();
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QPaintDevice_1physicalDpiX )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        // Not a QObject
        QPaintDevice* dev = reinterpret_cast<QPaintDevice*>( static_cast<int>( aHandle ) );
        res = static_cast<jint>( dev->physicalDpiX() );
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QPaintDevice_1physicalDpiY )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        // Not a QObject
        QPaintDevice* dev = reinterpret_cast<QPaintDevice*>( static_cast<int>( aHandle ) );
        res = static_cast<jint>( dev->physicalDpiY() );
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QPaintDevice_1width )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        // Not a QObject
        QPaintDevice* dev = reinterpret_cast<QPaintDevice*>( static_cast<int>( aHandle ) );
        res = static_cast<jint>( dev->width() );
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QPaintDevice_1height )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        // Not a QObject
        QPaintDevice* dev = reinterpret_cast<QPaintDevice*>( static_cast<int>( aHandle ) );
        res = static_cast<jint>( dev->height() );
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QPaintDevice_1paintingActive )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    jboolean res = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        // Not a QObject
        QPaintDevice* dev = reinterpret_cast<QPaintDevice*>( static_cast<int>( aHandle ) );
        res = (dev->paintingActive() ? JNI_TRUE : JNI_FALSE);
        }
    SWT_CATCH
    return res;
    }

//
// QWidget
//

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1new )
  (JNIEnv* aJniEnv , jclass, jint aParent, jint aFlags)
    {
    QWidget* widget = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("parent=%x flags=%x", aParent, aFlags);

        HANDLE_TO_POINTER( QWidget*, parent, aParent );
        widget = new QWidget( parent, static_cast<Qt::WindowFlags>(aFlags) );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( widget );
    }

JNIEXPORT void JNICALL OS_NATIVE(QWidget_1setVisible)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aVisible )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x visible=%d", aHandle, aVisible);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setVisible( aVisible == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1isVisible )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    bool visible = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        visible = widget->isVisible();
        }
    SWT_CATCH
    return ( visible ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1isVisibleTo )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aParentHandle)
    {
    bool visible = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x parentHandle=%x", aHandle, aParentHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QWidget*, parent, aParentHandle );
        visible = widget->isVisibleTo(parent);
        }
    SWT_CATCH
    return ( visible ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT void JNICALL OS_NATIVE(QWidget_1setParent)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aParent )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x parent=%x", aHandle, aParent);

        HANDLE_TO_POINTER( QWidget*, child, aHandle );
        HANDLE_TO_POINTER( QWidget*, parent, aParent );
        child->setParent( parent );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QWidget_1resize)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth, jint aHeight )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x width=%d height=%d", aHandle, aWidth, aHeight);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->resize( aWidth, aHeight );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1update__IIIII )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("handle=%x x=%d y=%d width=%d height=%d", aHandle, aX, aY, aWidth, aHeight);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->update( aX, aY, aWidth, aHeight );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1update__I )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->update();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1repaint )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("handle=%x x=%d y=%d width=%d height=%d", aHandle, aX, aY, aWidth, aHeight);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->repaint( aX, aY, aWidth, aHeight );
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE(QWidget_1rect)
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaRect = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QRect rect = widget->rect();
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        }
    SWT_CATCH
    return javaRect;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1sizeHint )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );

        QSize size = widget->sizeHint();
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, size );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1maximumSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );

        QSize size = widget->maximumSize();
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, size );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setMaximumSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMaxWidth, jint aMaxHeight)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x maxw=%d maxh=%d", aHandle, aMaxWidth, aMaxHeight);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setMaximumSize( aMaxWidth, aMaxHeight );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setMinimumSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMinWidth, jint aMinHeight)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x minw=%d minh=%d", aHandle, aMinWidth, aMinHeight);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setMinimumSize( aMinWidth, aMinHeight );
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1minimumSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );

        QSize size = widget->minimumSize();
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, size );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1frameSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );

        QSize size = widget->frameSize();
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, size );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1geometry )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaRect = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QRect rect = widget->geometry();
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        }
    SWT_CATCH
    return javaRect;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1frameGeometry )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaRect = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QRect rect = widget->frameGeometry();
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        }
    SWT_CATCH
    return javaRect;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setGeometry )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("handle=%x x=%d y=%d width=%d height=%d", aHandle, aX, aY, aWidth, aHeight);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setGeometry( aX, aY, aWidth, aHeight );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1move )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x x=%d y=%d", aHandle, aX, aY);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->move( aX, aY );
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1pos )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );

        QPoint pos = widget->pos();
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, pos );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1size )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );

        QSize size = widget->size();
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, size );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setFocus )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFocusReason)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setFocus( static_cast<Qt::FocusReason>( aFocusReason ) );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1swt_1palette_1new )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QPalette* palette( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        palette = new QPalette( widget->palette() );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( palette );
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setPalette )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPaletteHandle )
    {
     SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x paletteHandle=%x", aHandle, aPaletteHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QPalette* palette = reinterpret_cast< QPalette* >( aPaletteHandle );
        widget->setPalette( *palette );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1swt_1unsetPalette )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setPalette( QPalette() );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QWidget_1setAutoFillBackground)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aEnabled)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x enabled=%d", aHandle, aEnabled);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setAutoFillBackground( aEnabled == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1activateWindow )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->activateWindow();
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1isActiveWindow )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    bool active = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        active = widget->isActiveWindow();
        }
    SWT_CATCH
    return ( active ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setFont )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aFontHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x fonthandled=%x", aHandle, aFontHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QFont* font = reinterpret_cast< QFont* > ( aFontHandle );
        widget->setFont( *font );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1raise )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->raise();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1lower )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->lower();
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1mapTo )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aParent, jint aX, jint aY)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x parent=%x x=%d y=%d", aHandle, aParent, aX, aY);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QWidget*, parent, aParent );
        QPoint point( static_cast<int>(aX), static_cast<int>(aY) );
        QPoint pos = widget->mapTo( parent, point );
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, pos );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1mapToGlobal )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x x=%d y=%d", aHandle, aX, aY);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QPoint point(static_cast<int>(aX), static_cast<int>(aY));
        QPoint pos = widget->mapToGlobal( point );
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, pos );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QWidget_1mapFromGlobal )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x x=%d y=%d", aHandle, aX, aY);

        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QPoint point(static_cast<int>(aX), static_cast<int>(aY));
        QPoint pos = widget->mapFromGlobal( point );
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, pos );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QWidget_1actions )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jintArray javaArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, parent, aHandle );
        QList<QAction*> children = parent->actions();

        int childCount = children.size();
        QVector<int> handles(childCount);
        int* handleData = handles.data();
        for(int i = 0; i < childCount; ++i)
            {
            handleData[i] = POINTER_TO_HANDLE(children.at(i));
            }
        javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, handleData, childCount );
        }
    SWT_CATCH
    return javaArray;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1actions_1size )
    (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint size = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QWidget*, parent, aHandle );
        QList<QAction*> children = parent->actions();

        size = children.size();
        }
    SWT_CATCH
        return size;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1addAction )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aActionHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        SWT_LOG_DATA_1("actionHandle=%x", aActionHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        widget->addAction(action);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE ( QWidget_1insertAction )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aBeforeHandle, jint aActionHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x beforeHandle=%d actionHandle=%d", aHandle, aBeforeHandle, aActionHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QAction*, before, aBeforeHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        widget->insertAction( before, action );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_qt_OS_QWidget_1updateGeometry
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->updateGeometry();
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1isEnabled )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    bool enabled = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        enabled = widget->isEnabled();
        }
    SWT_CATCH
    return ( enabled ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1isEnabledTo )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aParentHandle)
    {
    bool enabled = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x parentHandle=%x", aHandle, aParentHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QWidget*, parent, aParentHandle );
        enabled = widget->isEnabledTo(parent);
        }
    SWT_CATCH
    return ( enabled ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setEnabled )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aEnabled)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x enabled=%d", aHandle, aEnabled);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setEnabled( aEnabled == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1isHidden )
    (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    bool hidden = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        hidden = widget->isHidden();
        }
    SWT_CATCH
    return ( hidden ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setContentsMargins )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aLeft, jint aTop, jint aRight, jint aBottom)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("handle=%x left=%d top=%d right=%d bottom=%d", aHandle, aLeft, aTop, aRight, aBottom);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setContentsMargins( aLeft, aTop, aRight, aBottom );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setToolTip )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aToolTip)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setToolTip( swtApp->jniUtils().JavaStringToQString( aJniEnv, aToolTip ) );
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QWidget_1toolTip )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QString toolTip = widget->toolTip();
        javaString = swtApp->jniUtils().QStringToJavaString( aJniEnv, toolTip );
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1stackUnder )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidget)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x widget=%x", aHandle, aWidget);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QWidget*, topWidget, aWidget );
        widget->stackUnder( topWidget );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setWindowTitle )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aTitle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setWindowTitle( swtApp->jniUtils().JavaStringToQString( aJniEnv, aTitle ) );
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QWidget_1windowTitle )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QString windowTitle = widget->windowTitle();
        javaString = swtApp->jniUtils().QStringToJavaString( aJniEnv, windowTitle );
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setWindowIcon )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIcon)
    {
    if(!aIcon) return;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, icon=%x", aHandle, aIcon);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle );
        const QIcon* icon = reinterpret_cast<QIcon*>(aIcon);
        widget->setWindowIcon(*icon);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setWindowIconText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setWindowIconText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setLayoutDirection )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aLayoutDirection)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x layoutDirection=%d ", aHandle, aLayoutDirection);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setLayout )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aLayoutHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x layout=%d ", aHandle, aLayoutHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QLayout*, layout, aLayoutHandle );
        widget->setLayout( layout );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QWidget_1layoutDirection)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint layoutDirection = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
        layoutDirection = widget->layoutDirection();
        }
    SWT_CATCH
    return layoutDirection;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1removeAction )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aActionHandle  )
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        SWT_LOG_DATA_1("actionHandle=%x", aActionHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        widget->removeAction(action);
        }
    SWT_CATCH
  }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1swt_1font_1new )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    QFont* font = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );

        // Implicitly shared, shallow copy
        font = new QFont(widget->font());
        }
    SWT_CATCH
    return reinterpret_cast< jint >( font );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1window )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QWidget* window = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        window = widget->window();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( window );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1isModal )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    bool modal = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        modal = widget->isModal();
        }
    SWT_CATCH
    return ( modal ? JNI_TRUE : JNI_FALSE );
    }


JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1heightForWidth )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth )
    {
    jint height = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x width=%d", aHandle, aWidth);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        height = widget->heightForWidth ( aWidth );
        }
    SWT_CATCH
    return height;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1windowState )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint state = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        state = widget->windowState();
        }
    SWT_CATCH
    return state;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setWindowState )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWindowState)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x windowState=%x", aHandle, aWindowState);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setWindowState( static_cast<Qt::WindowStates>( aWindowState ) );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1focusPolicy )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint policy = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        policy = widget->focusPolicy();
        }
    SWT_CATCH
    return policy;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setFocusPolicy )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPolicy)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x policy=%d ", aHandle, aPolicy);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setFocusPolicy( static_cast<Qt::FocusPolicy>(aPolicy) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setWindowModality )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aModality)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x modality=%d ", aHandle, aModality);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setWindowModality( static_cast<Qt::WindowModality>(aModality) );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1layout )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QLayout* layout = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        layout = widget->layout();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( layout );
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setFocusProxy )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aProxyHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x proxy=%d ", aHandle, aProxyHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        HANDLE_TO_POINTER( QWidget*, proxy, aProxyHandle );
        widget->setFocusProxy( proxy );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1focusProxy )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QWidget* proxy = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        proxy = widget->focusProxy();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( proxy );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1windowFlags )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint flags = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        flags = static_cast<jint>( widget->windowFlags() );
        }
    SWT_CATCH
    return flags;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setWindowFlags )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aType)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x flags=%x", aHandle, aType );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setWindowFlags( static_cast<Qt::WindowFlags>( aType ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setFixedSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth, jint aHeight)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x width=%d height=%d", aHandle, aWidth, aHeight );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QSize size( aWidth, aHeight );
        widget->setFixedSize( size );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setAttribute )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAttribute, jboolean aOn)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x attribute=%x", aHandle, aAttribute );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setAttribute( static_cast<Qt::WidgetAttribute>( aAttribute ),
            aOn == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setInputMethodHints )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aHints)
    {
#if QT_VERSION >= 0x040600
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x hints=%x", aHandle, aHints );
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setInputMethodHints( static_cast<Qt::InputMethodHints>( aHints ) );
        }
    SWT_CATCH
#endif
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1inputMethodHints )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint result = 0;
#if QT_VERSION >= 0x040600
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
        result = widget->inputMethodHints();
        }
    SWT_CATCH
#endif
    return result;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1clearFocus )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->clearFocus();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1scroll )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aDx, jint aDy, jint aX, jint aY, jint aW, jint aH)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x dx=%d dy=%d", aHandle, aDx, aDy);
        SWT_LOG_DATA_4("x=%d y=%d width=%d height=%d", aX, aY, aW, aH);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->scroll( aDx, aDy, QRect(aX, aY, aW, aH) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setWindowOpacity )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jdouble aLevel)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x level=%f", aHandle, aLevel);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setWindowOpacity( aLevel );
        }
    SWT_CATCH
    }

JNIEXPORT jdouble JNICALL OS_NATIVE( QWidget_1windowOpacity )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jdouble javaValue = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
        javaValue = widget->windowOpacity();
        }
    SWT_CATCH
    return javaValue;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setUpdatesEnabled )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aEnabled)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x enabled=%d", aHandle, aEnabled);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
        widget->setUpdatesEnabled(aEnabled == JNI_FALSE ? false : true);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1grabMouse )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
        widget->grabMouse();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1releaseMouse )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
        widget->releaseMouse();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1mouseGrabber )
  (JNIEnv* aJniEnv, jclass)
    {
    QWidget* grabber = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        grabber = QWidget::mouseGrabber();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( grabber );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QWidget_1swt_1paintDevice )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    QPaintDevice* result = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
        result = static_cast<QPaintDevice*>( widget );
        }
    SWT_CATCH
    return reinterpret_cast<jint>( result );
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setStyleSheet )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jstring aStyleSheet)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        widget->setStyleSheet( swtApp->jniUtils().JavaStringToQString( aJniEnv, aStyleSheet ) );
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QWidget_1styleSheet )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QString styleSheet = widget->styleSheet();
        javaString = swtApp->jniUtils().QStringToJavaString( aJniEnv, styleSheet );
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT void JNICALL OS_NATIVE( QWidget_1setInputContext )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aInputContext)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handel=%x inputContext=%x", aHandle, aInputContext);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        QInputContext* inputContext = reinterpret_cast<QInputContext*>(aInputContext);
        widget->setInputContext(inputContext);
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWidget_1testAttribute )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAttribute )
    {
    bool on = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x attribute=%d", aHandle, aAttribute);
        HANDLE_TO_POINTER( QWidget*, widget, aHandle );
        on = widget->testAttribute( static_cast<Qt::WidgetAttribute>(aAttribute));
        }
    SWT_CATCH
    return ( on ? JNI_TRUE : JNI_FALSE );
    }
//
// QFontMetrics
//

JNIEXPORT jint JNICALL OS_NATIVE(QWidget_1QFontMetrics_1lineSpacing)
    (JNIEnv* aJniEnv , jclass, jint aWidgetHandle)
{
    jint lineSpacing = 0;
    SWT_TRY
    {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aWidgetHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aWidgetHandle);
        lineSpacing = widget->fontMetrics().lineSpacing();
    }
    SWT_CATCH
    return lineSpacing;
}

//
// QDesktopWidget
//

JNIEXPORT jobject JNICALL OS_NATIVE( QDesktopWidget_1availableGeometry )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    jobject javaRect = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QDesktopWidget*, desktopWidget, aHandle );
        QRect rect = desktopWidget->availableGeometry( static_cast<int>( aIndex ) );
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        }
    SWT_CATCH
    return javaRect;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QDesktopWidget_1primaryScreen )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint primaryScreen=0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QDesktopWidget*, desktopWidget, aHandle );
        primaryScreen = desktopWidget->primaryScreen();
        }
    SWT_CATCH
    return primaryScreen;
    }
	
JNIEXPORT jint JNICALL OS_NATIVE( QDesktopWidget_1screenCount )
	(JNIEnv* aJniEnv, jclass, jint aHandle)
	{
	jint numScreens=0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QDesktopWidget*, desktopWidget, aHandle );
    #if QT_VERSION >= 0x040600
        numScreens = desktopWidget->screenCount();
    #else
        numScreens = desktopWidget->numScreens();
    #endif    
        }
    SWT_CATCH
    return numScreens;
	}

JNIEXPORT jint JNICALL OS_NATIVE( QDesktopWidget_1screen )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    QWidget* res = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER( QDesktopWidget*, desktopWidget, aHandle );
        res = desktopWidget->screen( aIndex );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( res );
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QDesktopWidget_1screenGeometry )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    jobject javaRect = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QDesktopWidget*, desktopWidget, aHandle );
        QRect rect = desktopWidget->screenGeometry( static_cast<int>( aIndex ) );
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        }
    SWT_CATCH
    return javaRect;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QDesktopWidget_1screenNumber )
    (JNIEnv* aJniEnv, jclass, jint aDesktopHandle, jint aWidgetHandle)
    {
    jint screenNum=0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("desktopHandle=%x widgetHandle=%x", aDesktopHandle, aWidgetHandle);
        HANDLE_TO_POINTER( QDesktopWidget*, desktopWidget, aDesktopHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aWidgetHandle );
        screenNum = desktopWidget->screenNumber( widget );
        }
    SWT_CATCH
    return screenNum;
    }

//
// QObject
//

JNIEXPORT void JNICALL OS_NATIVE(QObject_1installEventFilter)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aFilter )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x filter=%x", aHandle, aFilter);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        HANDLE_TO_POINTER( QObject*, eventFilter, aFilter );
        object->installEventFilter( eventFilter );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QObject_1removeEventFilter)
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFilter)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x filter=%x", aHandle, aFilter);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        HANDLE_TO_POINTER( QObject*, eventFilter, aFilter );
        object->removeEventFilter( eventFilter );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QObject_1setProperty)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aName, jint aData)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x data=%x", aHandle, aData);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        QString name = swtApp->jniUtils().JavaStringToQString( aJniEnv, aName );
        QByteArray chars = name.toLatin1();
        object->setProperty( chars.data(), QVariant(static_cast<int>( aData )) );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QObject_1property)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aName )
    {
    jint retVal = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        QString name = swtApp->jniUtils().JavaStringToQString( aJniEnv, aName );
        QByteArray chars = name.toLatin1();
        QVariant data = object->property(chars.data());
        if (data.isValid())
            {
            retVal = data.toInt();
            }
        }
    SWT_CATCH
    return retVal;
    }

JNIEXPORT void JNICALL OS_NATIVE(QObject_1delete)
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER(QObject*, object, aHandle);
        delete object;
        object = NULL;
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QObject_1setParent)
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aParent )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x parent=%x", aHandle, aParent);

        HANDLE_TO_POINTER( QObject*, child, aHandle );
        HANDLE_TO_POINTER( QObject*, parent, aParent );
        child->setParent( parent );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QObject_1parent )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QObject* parent = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QObject*, object, aHandle );
        parent = object->parent();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( parent );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE(QObject_1connect)
  (JNIEnv* aJniEnv , jclass, jint aSender, jstring aSignal, jint aReceiver, jstring aSlot,
    jint aConnectionType)
    {
    bool success = true;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("sender=%x receiver=%x", aSender, aReceiver);

        HANDLE_TO_POINTER( QObject*, sender, aSender );
        HANDLE_TO_POINTER( QObject*, receiver, aReceiver );

        QString signalName = swtApp->jniUtils().JavaStringToQString( aJniEnv, aSignal);
        QString slotName = swtApp->jniUtils().JavaStringToQString( aJniEnv, aSlot );

        // Prepare the signal and slot prototype strings for QObject.connect()
        // like using the SIGNAL and SLOT macros would do.
        const char* signalCodeString = SIGNAL("");
        const char* slotCodeString = SLOT("");
        signalName.prepend( QChar( signalCodeString[0] ) );
        slotName.prepend( QChar( slotCodeString[0] ) );
        QByteArray signalBytes = signalName.toLatin1();
        QByteArray slotBytes = slotName.toLatin1();
        const char* signalChars = signalBytes.data();
        const char* slotChars = slotBytes.data();

        SWT_LOG_DATA_2("signal=%s slot=%s", signalChars, slotChars);

        success = QObject::connect( sender, signalChars, receiver, slotChars,
            static_cast<Qt::ConnectionType>(aConnectionType) );
        }
    SWT_CATCH
    return ( success ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QObject_1thread )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QThread* thread = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER(QObject*, object, aHandle);
        thread = object->thread();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( thread );
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QObject_1children )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jintArray javaArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QObject*, parent, aHandle );
        QObjectList children = parent->children();

        int childCount = children.size();
        QVector<int> handles(childCount);
        int* handleData = handles.data();
        for(int i = 0; i < childCount; ++i)
            {
            handleData[i] = POINTER_TO_HANDLE(children.at(i));
            }
        javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, handleData, childCount );
        }
    SWT_CATCH
    return javaArray;
    }

JNIEXPORT void JNICALL OS_NATIVE( QObject_1dumpObjectInfo )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        object->dumpObjectInfo();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QObject_1dumpObjectTree )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        object->dumpObjectTree();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QObject_1deleteLater )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        object->deleteLater();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QObject_1setObjectName )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jstring aName)
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( QObject*, object, aHandle );
        QString name = swtApp->jniUtils().JavaStringToQString( aJniEnv, aName );
        object->setObjectName(name);
        }
    SWT_CATCH
  }

//
// QAbstractButton
//

JNIEXPORT void JNICALL OS_NATIVE( QAbstractButton_1setCheckable )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aCheckable)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x checkable=%d", aHandle, aCheckable);
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        button->setCheckable( aCheckable == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractButton_1setText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        button->setText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QAbstractButton_1text )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        QString text = button->text();
        javaString = swtApp->jniUtils().QStringToJavaString( aJniEnv, text );
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractButton_1setChecked )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aChecked)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x checked=%d", aHandle, aChecked);
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        button->setChecked( aChecked == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QAbstractButton_1isChecked )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    bool checked = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        checked = button->isChecked();
        }
    SWT_CATCH
    return ( checked ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractButton_1setIcon )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIconHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, aIconHandle=%x", aHandle, aIconHandle );
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        QIcon* icon = reinterpret_cast<QIcon*>(aIconHandle);
        button->setIcon( *icon );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractButton_1setIconSize )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth, jint aHeight )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        button->setIconSize( QSize( aWidth, aHeight ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractButton_1setAutoExclusive )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aAuto)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, autoExclusive=%d", aHandle, aAuto );
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        button->setAutoExclusive( aAuto == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractButton_1click )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractButton*, button, aHandle );
        button->click();
        }
    SWT_CATCH
    }

//
// QPushButton
//

JNIEXPORT jint JNICALL OS_NATIVE( QPushButton_1new )
  ( JNIEnv* aJniEnv , jclass )
    {
    QPushButton* button = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        button = new QPushButton();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( button );
    }

JNIEXPORT void JNICALL OS_NATIVE( QPushButton_1setFlat )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aFlat)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x flat=%d", aHandle, aFlat);
        HANDLE_TO_POINTER( QPushButton*, button, aHandle );
        button->setFlat( aFlat == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QPushButton_1setDefault )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aDefault)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x default=%d", aHandle, aDefault);
        HANDLE_TO_POINTER( QPushButton*, button, aHandle );
        button->setDefault( aDefault == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QPushButton_1isDefault )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    bool result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QPushButton*, button, aHandle );
        result = button->isDefault();
        }
    SWT_CATCH
    return ( result ? JNI_TRUE : JNI_FALSE );
    }

//
// QRadioButton
//

JNIEXPORT jint JNICALL OS_NATIVE( QRadioButton_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QRadioButton* button = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        button = new QRadioButton();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( button );
    }

//
// QCheckBox
//

JNIEXPORT jint JNICALL OS_NATIVE( QCheckBox_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QCheckBox* box = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        box = new QCheckBox();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( box );
    }

//
// QFrame
//

JNIEXPORT jint JNICALL JNICALL OS_NATIVE( QFrame_1new )
   ( JNIEnv* aJniEnv , jclass)
    {
    QFrame* frame( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        frame = new QFrame();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( frame );
    }

JNIEXPORT void JNICALL OS_NATIVE( QFrame_1setFrameStyle )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aStyle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x style=%x", aHandle, aStyle);
        HANDLE_TO_POINTER( QFrame*, frame, aHandle );
        frame->setFrameStyle( aStyle );
        }
    SWT_CATCH
}

JNIEXPORT jint JNICALL OS_NATIVE( QFrame_1frameWidth )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint width = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QFrame*, frame, aHandle );
        width = frame->frameWidth();
        }
    SWT_CATCH
    return width;
    }

JNIEXPORT void JNICALL OS_NATIVE( QFrame_1setLineWidth )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x width=%x ", aHandle, aWidth);
        HANDLE_TO_POINTER( QFrame*, frame, aHandle );
        frame->setLineWidth( aWidth );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QFrame_1frameShape )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint shape = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle);
        HANDLE_TO_POINTER( QFrame*, frame, aHandle );
        shape = frame->frameShape();
        }
    SWT_CATCH
    return shape;
    }

//
// QAbstractScrollArea
//

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractScrollArea_1viewPort )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QWidget* viewPort = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractScrollArea*, area, aHandle );
        viewPort = area->viewport();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( viewPort );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractScrollArea_1horizontalScrollBar )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QScrollBar* bar = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractScrollArea*, area, aHandle );
        bar = area->horizontalScrollBar();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( bar );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractScrollArea_1verticalScrollBar )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QScrollBar* bar = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractScrollArea*, area, aHandle );
        bar = area->verticalScrollBar();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( bar );
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractScrollArea_1setHorizontalScrollBarPolicy )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPolicy)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x policy=%x", aHandle, aPolicy);
        HANDLE_TO_POINTER( QAbstractScrollArea*, area, aHandle );
        area->setHorizontalScrollBarPolicy( static_cast<Qt::ScrollBarPolicy>(aPolicy) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractScrollArea_1setVerticalScrollBarPolicy )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPolicy)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x policy=%x", aHandle, aPolicy);
        HANDLE_TO_POINTER( QAbstractScrollArea*, area, aHandle );
        area->setVerticalScrollBarPolicy( static_cast<Qt::ScrollBarPolicy>(aPolicy) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractScrollArea_1setCornerWidget )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidget)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x widget=%x", aHandle, aWidget);
        HANDLE_TO_POINTER( QAbstractScrollArea*, area, aHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aWidget );
        area->setCornerWidget( widget );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractScrollArea_1cornerWidget )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QWidget* widget = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAbstractScrollArea*, area, aHandle );
        widget = area->cornerWidget();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( widget );
    }

//
// QScrollArea
//

JNIEXPORT jint JNICALL OS_NATIVE( QScrollArea_1new )
  (JNIEnv* aJniEnv , jclass, jint aParent)
    {
    QScrollArea* scrollArea = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("parent=%x", aParent);
        HANDLE_TO_POINTER( QWidget*, parent, aParent );
        scrollArea = new QScrollArea( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( scrollArea );
    }

//
// QAbstractSlider
//

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractSlider_1singleStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint singleStep = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        singleStep = slider->singleStep();
        }
    SWT_CATCH
    return singleStep;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractSlider_1maximum )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint maximum = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        maximum = slider->maximum();
        }
    SWT_CATCH
    return maximum;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractSlider_1minimum )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint minimum = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        minimum = slider->minimum();
        }
    SWT_CATCH
    return minimum;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractSlider_1pageStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint pageStep = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        pageStep = slider->pageStep();
        }
    SWT_CATCH
    return pageStep;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractSlider_1value )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint value = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        value = slider->value();
        }
    SWT_CATCH
    return value;
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractSlider_1setSingleStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        slider->setSingleStep( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractSlider_1setMaximum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        slider->setMaximum( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractSlider_1setMinimum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        slider->setMinimum( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractSlider_1setPageStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        slider->setPageStep( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractSlider_1setValue )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QAbstractSlider*, slider, aHandle );
        slider->setValue( aValue );
        }
    SWT_CATCH
    }

//
// QScrollBar
//

JNIEXPORT jint JNICALL OS_NATIVE(QScrollBar_1value)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint value = 0;
    SWT_TRY
    {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QScrollBar*, scrollBar, aHandle);
        value = scrollBar->value();
    }
    SWT_CATCH
    return value;
}

JNIEXPORT void JNICALL OS_NATIVE(QScrollBar_1setValue)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
{
    SWT_TRY
    {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, value=%d", aHandle, aValue);
        HANDLE_TO_POINTER(QScrollBar*, scrollBar, aHandle);
        scrollBar->setValue(aValue);
    }
    SWT_CATCH
}


//
// QLabel
//

JNIEXPORT jstring JNICALL OS_NATIVE( QLabel_1text )
  (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jstring txt = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        txt = swtApp->jniUtils().QStringToJavaString( aJniEnv, label->text() );
        }
    SWT_CATCH
    return txt;
}

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1setText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        label->setText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
}

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1setAlignment)
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAlignment)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x alignment=%x", aHandle, aAlignment);
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        label->setAlignment( static_cast< Qt::Alignment >( aAlignment) );
        }
    SWT_CATCH
}

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1setPixmap)
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPixmapHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x pixmapHandle=%x", aHandle, aPixmapHandle );
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        if(aPixmapHandle == 0 )
            {
            label->setPixmap(QPixmap());
            }
        else
            {
            QPixmap* image = static_cast< QPixmap* >( reinterpret_cast< QPaintDevice* >( aPixmapHandle ) );
            label->setPixmap( *image );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1swt_1setPixmap )
  ( JNIEnv* aJniEnv, jclass, jint aHandle, jint aImageHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x imageHandle=%x", aHandle, aImageHandle );
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        if(aImageHandle == 0 )
            {
            label->setPixmap(QPixmap());
            }
        else
            {
            Java::GFX::Image* image = reinterpret_cast< Java::GFX::Image* >( aImageHandle );
            label->setPixmap( image->toPixmap() );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1setWordWrap )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aWrap )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x wrap=%d", aHandle, aWrap);
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        label->setWordWrap ( aWrap == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL JNICALL OS_NATIVE( QLabel_1new )
   ( JNIEnv* aJniEnv , jclass)
    {
    QLabel* label = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        label = new QLabel();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( label );
    }

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1setBuddy )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aBuddy )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x buddy=%x", aHandle, aBuddy);
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        HANDLE_TO_POINTER( QWidget*, buddy, aBuddy );
        label->setBuddy( buddy );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1setTextInteractionFlags )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aFlags )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x flags=%x", aHandle, aFlags);
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        label->setTextInteractionFlags( static_cast<Qt::TextInteractionFlags>(aFlags) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QLabel_1setOpenExternalLinks )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aBool )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle);
        HANDLE_TO_POINTER( QLabel*, label, aHandle );
        SWT_LOG_DATA_1( "call setOpenExternalLinks(true) %d", 0 );
        label->setOpenExternalLinks( aBool == JNI_TRUE ? true : false  );
         }
    SWT_CATCH
    }

//
// QMenu
//

JNIEXPORT jint JNICALL OS_NATIVE( QMenu_1new )
    ( JNIEnv* aJniEnv , jclass, jint aParentHandle)
    {
    QMenu* menu = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("parentHandle=%x", aParentHandle );
        HANDLE_TO_POINTER( QWidget*, parent, aParentHandle );
        menu = new QMenu( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( menu );
    }

JNIEXPORT void JNICALL OS_NATIVE( QMenu_1addAction )
  (JNIEnv* aJniEnv , jclass, jint aMenuHandle, jint aActionHandle )
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("menuHandle=%x, actionHandle=%x", aMenuHandle, aActionHandle );
        HANDLE_TO_POINTER( QMenu*, menu, aMenuHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        menu->addAction( action );
        }
    SWT_CATCH
  }
JNIEXPORT jint JNICALL OS_NATIVE( QMenu_1exec )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aActionHandle )
    {
    QAction* selectedAction= NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4( "aHandle=%d x=%d y=%d aActionHandle=%d", aHandle, aX, aY, aActionHandle );
        HANDLE_TO_POINTER( QMenu*, menu, aHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        QPoint pos(aX,aY);
        AutoPopExecStack stackExec(menu);
        selectedAction = menu->exec( pos, action );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( selectedAction );
    }

JNIEXPORT void JNICALL OS_NATIVE( QMenu_1setDefaultAction )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aActionHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("aHandle=%x, actionHandle=%x", aHandle, aActionHandle );
        HANDLE_TO_POINTER( QMenu*, menu, aHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        menu->setDefaultAction( action );
        }
    SWT_CATCH

    }

JNIEXPORT jint JNICALL OS_NATIVE ( QMenu_1defaultAction )
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QAction* action = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("aHandle=%x", aHandle );
        HANDLE_TO_POINTER( QMenu*, menu, aHandle );
        action = menu->defaultAction();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( action );
    }

JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_qt_OS_QMenu_1popup
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aX, jint aY, jint aActionHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("aHandle=%x, aX=%d, aY=%d, aActionHandle=%x", aHandle, aX, aY, aActionHandle );
        HANDLE_TO_POINTER( QMenu*, menu, aHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        menu->popup(QPoint(aX, aY), action);
        }
    SWT_CATCH
    }

//
// QLineEdit
//
JNIEXPORT jint JNICALL OS_NATIVE(QLineEdit_1new)
(JNIEnv* aJniEnv , jclass)
    {
    QLineEdit* lineEdit = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        lineEdit = new QLineEdit();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE(lineEdit);
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setText)
(JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->setText(swtApp->jniUtils().JavaStringToQString(aJniEnv, aText));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1cut)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->cut();
        }
    SWT_CATCH
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1copy)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->copy();
        }
    SWT_CATCH
#endif
    }

JNIEXPORT jstring JNICALL OS_NATIVE(QLineEdit_1text)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, lineEdit->text());
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1insert)
(JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->insert(swtApp->jniUtils().JavaStringToQString(aJniEnv, aText));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setMaxLength)
(JNIEnv* aJniEnv , jclass, jint aHandle, jint aMaxLength)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, maxLength=%d", aHandle, aMaxLength);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->setMaxLength(aMaxLength);
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QLineEdit_1maxLength)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint maxLength = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        maxLength = lineEdit->maxLength();
        }
    SWT_CATCH
    return maxLength;
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setSelection)
(JNIEnv* aJniEnv , jclass, jint aHandle, jint aStart, jint aLength)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x, start=%d, length=%d", aHandle, aStart, aLength);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->setSelection(aStart, aLength);
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QLineEdit_1cursorPosition)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint cursorPosition = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        cursorPosition = lineEdit->cursorPosition();
        }
    SWT_CATCH
    return cursorPosition;
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setCursorPosition)
(JNIEnv* aJniEnv , jclass, jint aHandle, jint aPosition)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, position=%d", aHandle, aPosition);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->setCursorPosition(aPosition);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1deselect)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->deselect();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1end)
(JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aMark)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, mark=%d", aHandle, aMark);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->end(aMark == JNI_TRUE ? true : false);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setReadOnly)
(JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aReadOnly)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, readOnly=%d", aHandle, aReadOnly);
        HANDLE_TO_POINTER( QLineEdit*, lineEdit, aHandle );
        lineEdit->setReadOnly(aReadOnly == JNI_TRUE ? true : false);
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE(QLineEdit_1isReadOnly)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean readOnly = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        readOnly = lineEdit->isReadOnly() ? JNI_TRUE : JNI_FALSE;
        }
    SWT_CATCH
    return readOnly;
    }

JNIEXPORT jstring JNICALL OS_NATIVE(QLineEdit_1selectedText)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, lineEdit->selectedText());
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QLineEdit_1selectionStart)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint selectionStart = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        selectionStart = lineEdit->selectionStart();
        }
    SWT_CATCH
    return selectionStart;
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1selectAll)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->selectAll();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setEchoMode)
(JNIEnv* aJniEnv , jclass, jint aHandle, jint aEchoMode)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, echoMode=%d", aHandle, aEchoMode);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->setEchoMode(static_cast<QLineEdit::EchoMode>(aEchoMode));
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QLineEdit_1echoMode)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint echoMode = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        echoMode = lineEdit->echoMode();
        }
    SWT_CATCH
    return echoMode;
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setAlignment)
(JNIEnv* aJniEnv , jclass, jint aHandle, jint aAlignment)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, alignment=%d", aHandle, aAlignment);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->setAlignment(static_cast<Qt::Alignment>(aAlignment));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setFrame)
(JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aShowFrame)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, showFrame=%d", aHandle, aShowFrame);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        lineEdit->setFrame(aShowFrame == JNI_TRUE ? true : false);
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QLineEdit_1swt_1getBorderWidth)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    int width = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        QStyle* style = lineEdit->style();
        if (style)
            {
            width = style->pixelMetric(QStyle::PM_DefaultFrameWidth);
            }
        }
    SWT_CATCH
    return width;
    }

JNIEXPORT void JNICALL OS_NATIVE(QLineEdit_1setValidator)
(JNIEnv* aJniEnv , jclass, jint aHandle, jint aValidatorHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aHandle);
        HANDLE_TO_POINTER(QValidator*, validator, aValidatorHandle);
        lineEdit->setValidator( validator );
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE(QLineEdit_1swt_1preferredClientSize)
    (JNIEnv* aJniEnv , jclass, jint aLineEditHandle)
    {
    jobject size = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aLineEditHandle);
        HANDLE_TO_POINTER(QLineEdit*, lineEdit, aLineEditHandle);

        QFontMetrics fm(lineEdit->font());
        int left, top, right, bottom;
        lineEdit->getContentsMargins(&left, &top, &right, &bottom);
        int h = qMax(fm.lineSpacing(), QLineEdit_minimumLineSpacing) + 2*QLineEdit_verticalMargin
                + top + bottom;
        int w = fm.width(lineEdit->text()) + 2*QLineEdit_horizontalMargin
                + left + right;
        QPoint point(w, h);
        size = swtApp->jniUtils().NewJavaPoint( aJniEnv, point );
        }
    SWT_CATCH
    return size;
    }

//
// QTextEdit
//

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1new)
    (JNIEnv* aJniEnv , jclass)
    {
    QTextEdit* textEdit = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        textEdit = new QTextEdit();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE(textEdit);
    }

JNIEXPORT jstring JNICALL OS_NATIVE(QTextEdit_1toPlainText)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, textEdit->toPlainText());
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1setPlainText)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->setPlainText(swtApp->jniUtils().JavaStringToQString(aJniEnv, aText));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1setHtml)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aHtml)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->setHtml(swtApp->jniUtils().JavaStringToQString(aJniEnv, aHtml));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1insertPlainText)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->insertPlainText(swtApp->jniUtils().JavaStringToQString(aJniEnv, aText));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1cut)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->cut();
        }
    SWT_CATCH
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1copy)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->copy();
        }
    SWT_CATCH
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1setReadOnly)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aReadOnly)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->setReadOnly(aReadOnly == JNI_TRUE ? true : false);
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE(QTextEdit_1isReadOnly)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean readOnly = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        readOnly = textEdit->isReadOnly() ? JNI_TRUE : JNI_FALSE;
        }
    SWT_CATCH
    return readOnly;
    }

JNIEXPORT jobject JNICALL OS_NATIVE(QTextEdit_1cursorRect)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaRectangle = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        javaRectangle = swtApp->jniUtils().NewJavaRectangle(aJniEnv, textEdit->cursorRect());
        }
    SWT_CATCH
    return javaRectangle;
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1ensureCursorVisible)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->ensureCursorVisible();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1setAlignment)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAlignment)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, alignment=%d", aHandle, aAlignment);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->setAlignment(static_cast<Qt::Alignment>(aAlignment));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1setLineWrapMode)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWrapMode)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, wrapMode=%d", aHandle, aWrapMode);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        textEdit->setLineWrapMode(static_cast<QTextEdit::LineWrapMode>(aWrapMode));
        }
    SWT_CATCH
    }

//
// QTextEdit_swt
//
static jint getNumberOfPrecedingTextLines(const QTextBlock& aTextBlock)
    {
    QTextBlock block(aTextBlock.previous());
    jint lineCount = 0;
    while (block.isValid())
        {
        QTextLayout* textLayout = block.layout();
        if (textLayout)
            {
            // if block is valid, there is at least 1 line
            // even if layout would return 0
            int lines = textLayout->lineCount();
            lineCount += (lines> 0) ? lines : 1;
            }
        block = block.previous();
        }
    return lineCount;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1getCaretPosition)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jint position = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);
        position = textEdit->textCursor().position();
        }
    SWT_CATCH
    return position;
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1swt_1clearSelection)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);
        QTextCursor textCursor(textEdit->textCursor());
        textCursor.clearSelection();
        textEdit->setTextCursor(textCursor);
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1getCaretLineNumber)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jint lineNumber = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);

        QTextCursor textCursor = textEdit->textCursor();
        QTextBlock textBlock = textCursor.block();
        QTextLayout* textLayout = textBlock.layout();

        if (textLayout)
            {
            // lineNumber() returns the line number in the text block
            lineNumber = textLayout->lineForTextPosition(
                textCursor.position() - textBlock.position()).lineNumber();
            }

        lineNumber += getNumberOfPrecedingTextLines(textBlock);
        }
    SWT_CATCH
    return lineNumber;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1getLineHeight)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jint lineHeight = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);

        QTextLayout* textLayout = textEdit->textCursor().block().layout();

        if (textLayout)
            {
            lineHeight = qRound(textLayout->lineAt(0).height());
            }
        }
    SWT_CATCH
    return lineHeight;
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1swt_1setTopIndex)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle, jint aLine)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, line=%d", aTextEditHandle, aLine);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);

        QScrollBar* scrollBar = textEdit->verticalScrollBar();
        QTextDocument* textDocument = textEdit->document();
        if (scrollBar && textDocument)
            {
            if (aLine == 0)
                {
                scrollBar->setValue(0);
                }
            else
                {
                QTextBlock textBlock(textDocument->begin());
                int lineCount = 0;

                // Loop through the text blocks in the text document and check
                // which block contains the desired line. Then get the y-position of
                // the line and scroll the textEdit content accordingly.
                while(textBlock.isValid())
                    {
                    QTextLayout* layout = textBlock.layout();
                    if (layout)
                        {
                        int oldLineCount = lineCount;
                        int lines = layout->lineCount();
                        // if block is valid, there is at least 1 line
                        // even if layout would return 0
                        lineCount += (lines> 0) ? lines : 1;

                        if (aLine < lineCount)
                            {
                            int linePosition = qRound(layout->position().y() +
                                layout->lineAt(aLine - oldLineCount).y());
                            scrollBar->setValue(linePosition);
                            break;
                            }
                        }
                    textBlock = textBlock.next();
                    }
                }
            }
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1getTopIndex)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jint lineIndex = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);

        QScrollBar* scrollBar = textEdit->verticalScrollBar();

        if (scrollBar)
            {
            // First get the text block that at the top of visible viewport
            int top = scrollBar->value();

            QTextCursor textCursor(textEdit->cursorForPosition(QPoint(0, 0)));
            QTextBlock topBlock(textCursor.block());
            QTextLayout* layout = topBlock.layout();

            // Find out how many lines of the "top block" are above the visible area.
            if (layout && layout->position().y() < top)
                {
                int layoutPos = qRound(layout->position().y());

                // If less than half of the line height is visible, return next line
                for (int i = 0; i < layout->lineCount() &&
                    (layoutPos + layout->lineAt(i).rect().bottom()) <=
                    (top + layout->lineAt(i).rect().height() / 2); ++i)
                    {
                    ++lineIndex;
                    }
                }

            lineIndex += getNumberOfPrecedingTextLines(topBlock);
            }
        }
    SWT_CATCH
    return lineIndex;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1getLineCount)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jint lineCount = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);
        QTextDocument* textDocument = textEdit->document();

        if (textDocument)
            {
            lineCount = getNumberOfPrecedingTextLines(textDocument->end());
            }
        }
    SWT_CATCH
    return lineCount;
    }

JNIEXPORT jstring JNICALL OS_NATIVE(QTextEdit_1swt_1getSelectionText)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);
        QString string = textEdit->textCursor().selectedText();
        string.replace(KParagraphSeparator, "\n");
        javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, string);
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1selectionStart)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jint selectionStart = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);
        selectionStart = textEdit->textCursor().selectionStart();
        }
    SWT_CATCH
    return selectionStart;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1selectionEnd)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle)
    {
    jint selectionEnd = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aTextEditHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);
        selectionEnd = textEdit->textCursor().selectionEnd();
        }
    SWT_CATCH
    return selectionEnd;
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1swt_1setCursorPosition)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPosition, jboolean aMoveAnchor)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x, position=%x, moveAnchor=%x", aHandle, aPosition, aMoveAnchor);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        QTextCursor textCursor(textEdit->textCursor());
        textCursor.setPosition(aPosition, aMoveAnchor ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor);
        textEdit->setTextCursor(textCursor);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1swt_1selectAll)
(JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        QTextCursor textCursor(textEdit->textCursor());
        textCursor.select(QTextCursor::Document);
        textEdit->setTextCursor(textCursor);
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTextEdit_1swt_1getCharCount)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint length = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        QTextCursor textCursor(textEdit->textCursor());
        textCursor.movePosition(QTextCursor::End);
        length = textCursor.position();
        }
    SWT_CATCH
    return length;
    }

JNIEXPORT void JNICALL OS_NATIVE(QTextEdit_1swt_1append)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aHandle);
        QTextCursor textCursor(textEdit->textCursor());
        textCursor.movePosition(QTextCursor::End);
        textCursor.insertText(swtApp->jniUtils().JavaStringToQString(aJniEnv, aText));
        textEdit->setTextCursor(textCursor);
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE(QTextEdit_1swt_1preferredClientSize)
    (JNIEnv* aJniEnv , jclass, jint aTextEditHandle, jint aWHint)
    {
    jobject size = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x width=%x", aTextEditHandle, aWHint);
        HANDLE_TO_POINTER(QTextEdit*, textEdit, aTextEditHandle);
        QTextDocument* doc = textEdit->document();
        if( doc )
            {
            const qreal oldTextWidth = doc->textWidth();
             if (aWHint >= 0)
                {
                doc->setTextWidth(aWHint);
                }
            else
                {
                doc->adjustSize();
                }
            QSize  preferredSize= doc->size().toSize();
            doc->setTextWidth(oldTextWidth);
            QPoint point(preferredSize.width(), preferredSize.height());
            size = swtApp->jniUtils().NewJavaPoint( aJniEnv, point );
            }
        }
    SWT_CATCH
    return size;
    }


//
// QValidator
//
JNIEXPORT jint JNICALL OS_NATIVE( QRegExpValidator_1new )
    ( JNIEnv* aJniEnv , jclass, jint aParentHandle, jstring aRegExp )
    {
    QRegExpValidator* validator = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QObject*, parent, aParentHandle );
        validator = new QRegExpValidator(
            QRegExp( swtApp->jniUtils().JavaStringToQString( aJniEnv, aRegExp ) ), parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( validator );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QValidator_1validate )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aString )
    {
    jboolean res = true; // Intentionally validating null and empty strings
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QValidator*, validator, aHandle );
        if ( aString != NULL )
            {
            QString str = swtApp->jniUtils().JavaStringToQString( aJniEnv, aString );
            if (str.length() > 0)
                {
                int start = 0;
                res = validator->validate( str, start ) == QValidator::Acceptable;
                }
            }
        }
    SWT_CATCH
    return res;
    }

//
// QMenuBar
//
JNIEXPORT jint JNICALL OS_NATIVE( QMenuBar_1new )
    ( JNIEnv* aJniEnv , jclass, jint aParentHandle )
    {
    QMenuBar* menubar = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("parentHandle=%x", aParentHandle );
        HANDLE_TO_POINTER( QWidget*, parent, aParentHandle );
        menubar = new QMenuBar(parent);
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( menubar );
    }

JNIEXPORT void JNICALL OS_NATIVE( QMenuBar_1addAction )
  (JNIEnv* aJniEnv , jclass, jint aMenuBarHandle, jint aActionHandle)
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("menuBarHandle=%x, menuHandle=%x", aMenuBarHandle, aActionHandle );
        HANDLE_TO_POINTER( QMenuBar*, menuBar, aMenuBarHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        menuBar->addAction( action );
        }
    SWT_CATCH
  }

//
// QAction
//
JNIEXPORT jint JNICALL  OS_NATIVE( QAction_1new )
  (JNIEnv* aJniEnv , jclass, jint aParentHandle )
  {
    QAction* action = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QObject*, parent, aParentHandle );
        action = new QAction( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( action );
  }

JNIEXPORT void JNICALL OS_NATIVE( QAction_1setSeparator )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aBool)
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, bool=%x", aHandle, aBool );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setSeparator( aBool );
        }
    SWT_CATCH
  }

JNIEXPORT void JNICALL OS_NATIVE( QAction_1setCheckable )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aBool )
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, bool=%x", aHandle, aBool );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setCheckable( aBool );
        }
    SWT_CATCH
  }
JNIEXPORT void JNICALL  OS_NATIVE( QAction_1setText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAction_1setMenu )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMenuHandle )
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("aHandle=%x, aMenuHandle=%x", aHandle, aMenuHandle );
        HANDLE_TO_POINTER( QMenu*, menu, aMenuHandle );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setMenu( menu );
        }
    SWT_CATCH
  }

JNIEXPORT void JNICALL OS_NATIVE( QAction_1setShortcut )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aKeySequenceHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("aHandle=%x, aKeySequenceHandle=%x", aHandle, aKeySequenceHandle );
        QKeySequence* shortcut = reinterpret_cast<QKeySequence*>( aKeySequenceHandle );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setShortcut( *shortcut );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE ( QAction_1setEnabled )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aEnabled )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, aEnabled=%x", aHandle, aEnabled );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setEnabled( aEnabled );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QAction_1isEnabled )
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean enabled = JNI_TRUE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        enabled = action->isEnabled( );
        }
    SWT_CATCH
    return enabled;
    }

JNIEXPORT void JNICALL OS_NATIVE( QAction_1setChecked )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aChecked )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, aEnabled=%x", aHandle, aChecked );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setChecked( aChecked );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QAction_1isChecked )
    (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jboolean checked = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        checked = action->isChecked( );
        }
    SWT_CATCH
    return checked;
    }

JNIEXPORT void JNICALL OS_NATIVE( QAction_1setIcon )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIconHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, aIconHandle=%x", aHandle, aIconHandle );
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        QIcon* icon = reinterpret_cast<QIcon*>(aIconHandle);
        action->setIcon( *icon );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAction_1setIconText )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setIconText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE ( QAction_1setSoftKeyRole )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aRole )
    {
#if QT_VERSION >= 0x040600
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QAction*, action, aHandle );
        action->setSoftKeyRole( static_cast<QAction::SoftKeyRole>(aRole) );
        }
    SWT_CATCH
#endif
    }

//
// QActionGroup
//

JNIEXPORT jint JNICALL OS_NATIVE( QActionGroup_1new )
    ( JNIEnv* aJniEnv , jclass, jint aParentHandle )
    {
    QActionGroup* actionGroup = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QObject*, parent, aParentHandle );
        actionGroup = new QActionGroup( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( actionGroup );
    }


JNIEXPORT void JNICALL OS_NATIVE( QActionGroup_1setExclusive )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aExclusive )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, bool=%x", aHandle, aExclusive );
        HANDLE_TO_POINTER( QActionGroup*, actionGroup, aHandle );
        actionGroup->setExclusive( aExclusive );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QActionGroup_1addAction )
    ( JNIEnv* aJniEnv , jclass, jint aActionGroupHandle, jint aActionHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("actionGroupHandle=%x, actionHandle=%x", aActionGroupHandle, aActionHandle );
        HANDLE_TO_POINTER( QActionGroup*, actionGroup, aActionGroupHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        actionGroup->addAction( action );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QActionGroup_1removeAction )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aActionHandle )
  {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("actionGroupHandle=%x, actionHandle=%x", aHandle, aActionHandle );
        HANDLE_TO_POINTER( QActionGroup*, actionGroup, aHandle );
        HANDLE_TO_POINTER( QAction*, action, aActionHandle );
        actionGroup->removeAction( action );
        }
    SWT_CATCH
  }

//
// QPixmap
//

JNIEXPORT jboolean JNICALL OS_NATIVE( QPixmap_1load )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFileName )
    {
    jboolean result = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QPixmap* pixmap = static_cast< QPixmap* >( reinterpret_cast< QPaintDevice* >( aHandle ) );
        result = pixmap->load( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFileName ) ) == true ? JNI_TRUE : JNI_FALSE ;
        }
     SWT_CATCH
     return result;
    }


JNIEXPORT jint JNICALL JNICALL OS_NATIVE( QPixmap_1new )
   ( JNIEnv* aJniEnv , jclass)
    {
    QPixmap* pixmap = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        pixmap = new QPixmap();
        }
    SWT_CATCH
    return reinterpret_cast< jint >( static_cast< QPaintDevice* >( pixmap ) );
    }

JNIEXPORT void JNICALL OS_NATIVE( QPixmap_1delete )
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QPixmap* pixmap = reinterpret_cast< QPixmap* >( aHandle );
        delete pixmap;
        }
    SWT_CATCH

    }

//
// QPalette
//

JNIEXPORT void JNICALL OS_NATIVE( QPalette_1setColor__IIIIII )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aGroup, jint aRole, jint aRed, jint aGreen, jint aBlue )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x group=%x role=%x ", aHandle, aGroup, aRole );
        const int& red = static_cast<int>(aRed);
        const int& green = static_cast<int>(aGreen);
        const int& blue = static_cast<int>(aBlue);
        QPalette* palette = reinterpret_cast< QPalette* > ( aHandle );
        palette->setColor( static_cast< QPalette::ColorGroup> ( aGroup ), static_cast< QPalette::ColorRole> ( aRole ), QColor( red, green, blue ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QPalette_1setColor__IIIII )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRole, jint aRed, jint aGreen, jint aBlue )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5( "handle=%x rose=%x red=%x green=%x blue=%x", aHandle, aRole, aRed, aGreen, aBlue );
        const int& red = static_cast<int>(aRed);
        const int& green = static_cast<int>(aGreen);
        const int& blue = static_cast<int>(aBlue);
        QPalette* palette = reinterpret_cast< QPalette* > ( aHandle );
        palette->setColor( static_cast< QPalette::ColorRole> ( aRole ), QColor( red, green, blue ) );
        }
    SWT_CATCH
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QPalette_1color )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aGroup, jint aRole )
    {
    jintArray color(NULL);
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x role=%x ", aHandle, aRole );
        QPalette* palette = reinterpret_cast< QPalette* > ( aHandle );
        QColor osColor = palette->color( static_cast< QPalette::ColorGroup> ( aGroup ),
            static_cast< QPalette::ColorRole> ( aRole ) );
        int rgb[] = {osColor.red(), osColor.green(), osColor.blue()};
        color = swtApp->jniUtils().NewJavaIntArray( aJniEnv, rgb, 3 );
        }
     SWT_CATCH
     return color;
    }

JNIEXPORT void JNICALL OS_NATIVE( QPalette_1delete )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QPalette* palette = reinterpret_cast< QPalette* >( aHandle );
        delete palette;
        palette = NULL;
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QPalette_1swt_1setBrush )
   (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRole, jint aCgImage)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x role=%x cgImage=%x", aHandle, aRole, aCgImage );
        Java::GFX::Image* image = reinterpret_cast<Java::GFX::Image*>(aCgImage);
        QPalette* palette = reinterpret_cast< QPalette* > ( aHandle );
        if(aCgImage)
            {
            palette->setBrush( static_cast< QPalette::ColorRole> ( aRole ), QBrush( image->toPixmap() ) );
            }
        else
            {
            palette->setBrush( static_cast< QPalette::ColorRole> ( aRole ), QBrush() );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QPalette_1swt_1copyBrushFromPalette )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aSourceHandle, jint aRole)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x source=%x role=%x ", aHandle, aSourceHandle, aRole );
        QPalette* palette = reinterpret_cast<QPalette*> ( aHandle );
        QPalette* sourcePalette = reinterpret_cast<QPalette*> ( aSourceHandle );
        const QPalette::ColorRole& role = static_cast<QPalette::ColorRole> ( aRole );
        const QBrush& brush = sourcePalette->brush( role );
        palette->setBrush( role, brush );
        }
    SWT_CATCH
    }

//
// QTimer
//

JNIEXPORT jint JNICALL OS_NATIVE( QTimer_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QTimer* timer = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        timer = new QTimer();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( timer );
    }

JNIEXPORT void JNICALL OS_NATIVE( QTimer_1setSingleShot )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aSingleShot)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x singleShot=%d", aHandle, aSingleShot);
        HANDLE_TO_POINTER( QTimer*, timer, aHandle );
        timer->setSingleShot( aSingleShot == JNI_TRUE ? true : false );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTimer_1start )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMsec)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x msec=%d", aHandle, aMsec);
        HANDLE_TO_POINTER( QTimer*, timer, aHandle );
        timer->start( aMsec );
        }
    SWT_CATCH
    }

//
// QFont
//

JNIEXPORT jint JNICALL OS_NATIVE( QFont_1new__Ljava_lang_String_2IIZ )
( JNIEnv* aJniEnv , jclass, jstring aFamily, jint aPointSize, jint aWeight, jboolean aItalic )
    {
    QFont* font( NULL);
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4( "family=%s pointSize=%d weight=%d italic=%d", aFamily, aPointSize, aWeight, aItalic );
        font = new QFont( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFamily ), aPointSize, aWeight, aItalic );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( font );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QFont_1new__ )
( JNIEnv* aJniEnv , jclass )
    {
    QFont* font( NULL);
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
         font = new QFont();
        }
    SWT_CATCH
    return reinterpret_cast< jint >( font );
    }

 JNIEXPORT void JNICALL OS_NATIVE( QFont_1delete )
  (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFont* font = reinterpret_cast< QFont* >( aHandle );
        delete font;
        font = NULL;
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QFont_1family )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jstring javaString( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFont* font = reinterpret_cast< QFont* > ( aHandle );
        QString family = font->family();
        javaString = swtApp->jniUtils().QStringToJavaString( aJniEnv, family );
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QFont_1weight )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint weight = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFont* font = reinterpret_cast< QFont* > ( aHandle );
        weight = font->weight();
        }
    SWT_CATCH
    return weight;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QFont_1pointSize )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint pointSize = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFont* font = reinterpret_cast< QFont* > ( aHandle );
        pointSize = font->pointSize();
        }
    SWT_CATCH
    return pointSize;
    }


JNIEXPORT jboolean JNICALL OS_NATIVE( QFont_1italic )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jboolean italic = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFont* font = reinterpret_cast< QFont* > ( aHandle );
        italic = font->italic() ? JNI_TRUE : JNI_FALSE;
        }
    SWT_CATCH
    return italic;
    }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setUnderline )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jboolean aEnable)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setUnderline( aEnable == JNI_TRUE ? true : false );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setOverline )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jboolean aEnable)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setOverline( aEnable == JNI_TRUE ? true : false );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setStrikeOut )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jboolean aEnable)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setStrikeOut( aEnable == JNI_TRUE ? true : false );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setStretch )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jint aStretch)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_2("handle=%x streach=%d", aHandle, aStretch);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setStretch( aStretch );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setFixedPitch )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jboolean aEnable)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setFixedPitch( aEnable == JNI_TRUE ? true : false );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setStyle )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jint aStyle)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_2("handle=%x style=%x", aHandle, aStyle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setStyle( static_cast<QFont::Style>(aStyle) );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setWeight )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jint aWeight)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_2("handle=%x weight=%d", aHandle, aWeight);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setWeight( aWeight );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setStyleStrategy )
 (JNIEnv* aJniEnv, jclass, jint aHandle, jint aStrategy)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_2("handle=%x strategy=%x", aHandle, aStrategy);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       font->setStyleStrategy( static_cast<QFont::StyleStrategy>(aStrategy) );
       }
   SWT_CATCH
   }

JNIEXPORT void JNICALL OS_NATIVE( QFont_1setRawName )
 (JNIEnv* aJniEnv, jclass, jint
#ifdef Q_WS_X11
aHandle
#endif
, jstring
#ifdef Q_WS_X11
aRawName
#endif
)
   {
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
#ifdef Q_WS_X11
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
         font->setRawName( swtApp->jniUtils().JavaStringToQString( aJniEnv, aRawName ) );
#endif
       }
   SWT_CATCH
   }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFont_1underline )
 (JNIEnv* aJniEnv, jclass, jint aHandle)
   {
   jboolean enable = JNI_FALSE;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       enable = font->underline();
       }
   SWT_CATCH
   return enable;
   }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFont_1overline )
 (JNIEnv* aJniEnv, jclass, jint aHandle)
   {
   jboolean enable = JNI_FALSE;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       enable = font->overline();
       }
   SWT_CATCH
   return enable;
  }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFont_1strikeOut )
 (JNIEnv* aJniEnv, jclass, jint aHandle)
   {
   jboolean enable = JNI_FALSE;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       enable = font->strikeOut();
       }
   SWT_CATCH
   return enable;
   }


JNIEXPORT jint JNICALL OS_NATIVE( QFont_1stretch )
 (JNIEnv* aJniEnv, jclass, jint aHandle)
   {
   jint stretch = -1;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       stretch = font->stretch();
       }
   SWT_CATCH
   return stretch;
   }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFont_1fixedPitch )
 (JNIEnv* aJniEnv, jclass, jint aHandle)
   {
   jboolean enable = JNI_FALSE;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       enable = font->fixedPitch();
       }
   SWT_CATCH
   return enable;
   }

JNIEXPORT jint JNICALL OS_NATIVE( QFont_1style )
 (JNIEnv* aJniEnv, jclass, jint aHandle)
   {
   jint style = -1;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       style = font->style();
       }
   SWT_CATCH
   return style;
   }

JNIEXPORT jint JNICALL OS_NATIVE( QFont_1styleStrategy )
 (JNIEnv* aJniEnv, jclass, jint aHandle)
   {
   jint strategy = -1;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_1("handle=%x", aHandle);
       QFont* font = reinterpret_cast< QFont* >( aHandle );
       strategy = font->styleStrategy();
       }
   SWT_CATCH
   return strategy;
   }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFont_1swt_1equals )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aHandle2)
   {
   bool equals = false;
   SWT_TRY
       {
       SWT_LOG_JNI_CALL();
       SWT_LOG_DATA_2("handle=%x handle2=%x", aHandle, aHandle2);
       const QFont* const font = reinterpret_cast<QFont*>( aHandle );
       const QFont* const font2 = reinterpret_cast<QFont*>( aHandle2 );
       if(!font || !font2)
            {
            equals = (font == font2);
            }
       else
            {
            equals = (*font == *font2);
            }
       }
   SWT_CATCH
   return equals ? JNI_TRUE : JNI_FALSE;
   }

//
// QFontDatabase in Device
//

JNIEXPORT jint JNICALL OS_NATIVE( QFontDatabase_1new )
  ( JNIEnv* aJniEnv , jclass )
    {
    QFontDatabase* fontDB( NULL);
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        fontDB = new QFontDatabase();
        }
    SWT_CATCH
    return reinterpret_cast< jint >( fontDB );
    }

JNIEXPORT jobjectArray JNICALL OS_NATIVE( QFontDatabase_1families )
  ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jobjectArray familiesArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFontDatabase* fontDb = reinterpret_cast< QFontDatabase* > ( aHandle );
        familiesArray = swtApp->jniUtils().NewJavaStringArray( aJniEnv, fontDb->families() );
        }
    SWT_CATCH
    return familiesArray;
  }

 JNIEXPORT jintArray JNICALL OS_NATIVE( QFontDatabase_1swt_1heights )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFamily, jstring aStyle)
    {
    jintArray heightsArray = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x family=%x style=%x", aHandle, aFamily, aStyle );
        QFontDatabase* fontDb = reinterpret_cast< QFontDatabase* > ( aHandle );
        QList<int> heights = aStyle == NULL? fontDb->pointSizes( swtApp->jniUtils().JavaStringToQString(aJniEnv, aFamily) ):
            fontDb->pointSizes( swtApp->jniUtils().JavaStringToQString(aJniEnv, aFamily), swtApp->jniUtils().JavaStringToQString(aJniEnv, aStyle) );
        const int count = heights.size();
        if(count > 0)
            {
            QVector<int> handles(count);
            int* handleData = handles.data();
            for(int i = 0; i < count; ++i)
                {
                handleData[i] = heights.at(i);
                }
            heightsArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, handleData, count );
            }
        }
    SWT_CATCH
    return heightsArray;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFontDatabase_1isScalable )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFamily, jstring aStyle)
    {
    bool scalable = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x family=%x", aHandle, aFamily, aStyle );
        QFontDatabase* fontDb = reinterpret_cast< QFontDatabase* > ( aHandle );
        scalable = aStyle == NULL? fontDb->isScalable( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFamily ) ):
        fontDb->isScalable( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFamily ), swtApp->jniUtils().JavaStringToQString( aJniEnv, aStyle ) );
        }
    SWT_CATCH
    return scalable;
    }

JNIEXPORT jobjectArray JNICALL OS_NATIVE( QFontDatabase_1styles )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFamily )
    {
    jobjectArray stylesArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x family=%x", aHandle, aFamily);
        QFontDatabase* fontDb = reinterpret_cast< QFontDatabase* > ( aHandle );
        stylesArray = swtApp->jniUtils().NewJavaStringArray( aJniEnv, fontDb->styles( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFamily ) ) );
        }
    SWT_CATCH
    return stylesArray;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFontDatabase_1bold )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFamily, jstring aStyle)
    {
    bool bold = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x family=%x", aHandle, aFamily, aStyle );
        QFontDatabase* fontDb = reinterpret_cast< QFontDatabase* > ( aHandle );
        bold = fontDb->bold( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFamily ), swtApp->jniUtils().JavaStringToQString( aJniEnv, aStyle ) );
        }
    SWT_CATCH
    return bold;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QFontDatabase_1italic )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFamily, jstring aStyle)
    {
    bool italic = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x family=%x", aHandle, aFamily, aStyle );
        QFontDatabase* fontDb = reinterpret_cast< QFontDatabase* > ( aHandle );
        italic = fontDb->italic( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFamily ), swtApp->jniUtils().JavaStringToQString( aJniEnv, aStyle ) );
        }
    SWT_CATCH
    return italic;
    }

JNIEXPORT void JNICALL OS_NATIVE( QFontDatabase_1delete )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
  {
  SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFontDatabase* fontDB = reinterpret_cast< QFontDatabase* >( aHandle );
        delete fontDB;
        fontDB = NULL;
        }
    SWT_CATCH
  }

//
// ListModel
//

JNIEXPORT jint JNICALL OS_NATIVE( ListModel_1new )
( JNIEnv* aJniEnv , jclass, jint aLayoutStyle, jint aParent, jint aListType )
    {
    ListModel* dataModel(NULL);
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "layoutStyle=%d parent=%x listType=%d", aLayoutStyle, aParent, aListType );
        HANDLE_TO_POINTER( QListView*, listView, aParent );
        dataModel = ListModel::createDataModel( aListType, aLayoutStyle, listView );
        }
    SWT_CATCH
    return reinterpret_cast<jint>(dataModel);
    }

JNIEXPORT void JNICALL OS_NATIVE( ListModel_1beginInsertRows )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aParentIndexHandle, jint aFirst, jint aLast )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x parentIdxHandle=%x first=%d last=%d ", aHandle, aParentIndexHandle, aFirst, aLast);
        ListModel* listModel = reinterpret_cast< ListModel* > ( aHandle );
        if ( aParentIndexHandle!= 0 )
            {
            QModelIndex* modelIndex = reinterpret_cast< QModelIndex* > ( aHandle );
            listModel->beginInsertRows( *modelIndex, aFirst, aLast );
            }
        else
            {
            listModel->beginInsertRows( QModelIndex(), aFirst, aLast );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1endInsertRows )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle);
        ListModel* listModel = reinterpret_cast< ListModel* > ( aHandle );
        listModel->endInsertRows();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( ListModel_1beginRemoveRows )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aParentIndexHandle, jint aFirst, jint aLast )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x parentIdxHandle=%x first=%d last=%d ", aHandle, aParentIndexHandle, aFirst, aLast);
        ListModel* listModel = reinterpret_cast< ListModel* > ( aHandle );
        if ( aParentIndexHandle!= 0 )
            {
            QModelIndex* modelIndex = reinterpret_cast< QModelIndex* > ( aHandle );
            listModel->beginRemoveRows( *modelIndex, aFirst, aLast );
            }
        else
            {
            listModel->beginRemoveRows( QModelIndex(), aFirst, aLast );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1endRemoveRows )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle);
        ListModel* listModel = reinterpret_cast< ListModel* > ( aHandle );
        listModel->endRemoveRows();
        }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1setCheckState )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aSelectionHandle, jint aState )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x selectionHandle=%x checkState=%x  ", aHandle, aSelectionHandle, aState );
        ListModel* listDataModel = reinterpret_cast< ListModel* > ( aHandle );
        QItemSelection* selection = reinterpret_cast<QItemSelection* > ( aSelectionHandle );
        listDataModel->setCheckState( *selection, static_cast< Qt::CheckState > ( aState ) );
           }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1append__ILjava_lang_String_2 )
( JNIEnv* aJniEnv , jclass,  jint aHandle, jstring aString )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x  ", aHandle );
        ListModel* listDataModel = reinterpret_cast< ListModel* > ( aHandle );
        listDataModel->appendItem( swtApp->jniUtils().JavaStringToQString( aJniEnv, aString ) );
        }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1append__ILjava_lang_String_2I )
( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aString, jint aImageHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x imageHandle=%x", aHandle, aImageHandle);
        ListModel* listDataModel = reinterpret_cast< ListModel* > ( aHandle );
        QPixmap* pixmap = reinterpret_cast< QPixmap* >( aImageHandle );
        listDataModel->appendItem( swtApp->jniUtils().JavaStringToQString( aJniEnv, aString ), pixmap );
        }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1append__ILjava_lang_String_2_3IILjava_lang_String_2_3II )
( JNIEnv* aJniEnv, jclass, jint aHandle, jstring aDetailText, jintArray aDetailImageHandles, jint aDetailImageCount, jstring aHeadingText, jintArray aHeadingImageHandles, jint aHeadingImageCount )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle );

        jboolean isCopy;
        jint* imagesHandles = NULL;

        QPixmap* detailImages = new QPixmap[aDetailImageCount];
        if( !detailImages )
            {
            throw std::bad_alloc();
            }
        if(aDetailImageHandles)
            {
            imagesHandles = aJniEnv->GetIntArrayElements(aDetailImageHandles, &isCopy);
            if( !imagesHandles )
                {
                delete [] detailImages;
                throw std::bad_alloc();
                }
            for(int i = 0; i < aDetailImageCount; i++)
                {
                detailImages[i] = (reinterpret_cast< Java::GFX::Image* >( imagesHandles[i] ))->toPixmap();
                }
            aJniEnv->ReleaseIntArrayElements(aDetailImageHandles, imagesHandles, JNI_ABORT);
            }

        QPixmap* headingImages = new QPixmap[aHeadingImageCount];
        if( !headingImages )
            {
            delete [] detailImages; // allocated earlier
            throw std::bad_alloc();
            }
        if(aHeadingImageHandles)
            {
            imagesHandles = aJniEnv->GetIntArrayElements(aHeadingImageHandles, &isCopy);
            if( !imagesHandles )
                {
                delete [] detailImages;
                delete [] headingImages;
                throw std::bad_alloc();
                }
            for(int i = 0; i < aHeadingImageCount; i++)
                {
                headingImages[i] = (reinterpret_cast< Java::GFX::Image* >( imagesHandles[i] ))->toPixmap();
                }
            aJniEnv->ReleaseIntArrayElements(aHeadingImageHandles, imagesHandles, JNI_ABORT);
            }

        ListModel* listDataModel = reinterpret_cast< ListModel* > ( aHandle );
        listDataModel->appendItem( swtApp->jniUtils().JavaStringToQString( aJniEnv, aDetailText ), 
            const_cast<const QPixmap*>(detailImages), aDetailImageCount,
            swtApp->jniUtils().JavaStringToQString( aJniEnv, aHeadingText ), 
            const_cast<const QPixmap*>(headingImages), aHeadingImageCount );
        }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1insert__ILjava_lang_String_2I )
( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aString, jint aIndex )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%x ", aHandle, aIndex );
        ListModel* listDataModel = reinterpret_cast< ListModel* > ( aHandle );
        listDataModel->insertItem( aIndex, swtApp->jniUtils().JavaStringToQString( aJniEnv, aString ) );
        }
    SWT_CATCH
    }

JNIEXPORT void  JNICALL OS_NATIVE( ListModel_1insert__ILjava_lang_String_2II )
( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aString, jint aIndex, jint aImageHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x index=%x  imageHandle=%x ", aHandle, aIndex,  aImageHandle);
        ListModel* listDataModel = reinterpret_cast< ListModel* > ( aHandle );
        QPixmap* pixmap = reinterpret_cast< QPixmap* >( aImageHandle );
        listDataModel->insertItem( aIndex, swtApp->jniUtils().JavaStringToQString( aJniEnv, aString ), pixmap );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( ListModel_1layoutAboutToBeChanged )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        ListModel* dataModel = reinterpret_cast< ListModel* > ( aHandle );
        dataModel->emitLayoutAboutToBeChanged();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( ListModel_1layoutChanged )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        ListModel* dataModdel = reinterpret_cast< ListModel* > ( aHandle );
        dataModdel->emitLayoutChanged();
        }
    SWT_CATCH
    }


JNIEXPORT jstring JNICALL OS_NATIVE(ListModel_1itemString)
      (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d ", aHandle, aIndex );
        ListModel* dataModel = reinterpret_cast< ListModel* > ( aHandle );
        javaString = swtApp->jniUtils().QStringToJavaString( aJniEnv, dataModel->itemString(aIndex) );
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT jint JNICALL OS_NATIVE( ListModel_1itemHeight )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint height = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        ListModel* dataModel = reinterpret_cast< ListModel* > ( aHandle );
        height = dataModel->itemHeight();
        }
    SWT_CATCH
    return height;
    }

JNIEXPORT jint JNICALL OS_NATIVE( ListModel_1insertPosition )
( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aString, jboolean aAscent )
    {
    jint pos = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        ListModel* dataModel = reinterpret_cast< ListModel* > ( aHandle );
        pos = dataModel->insertPosition(swtApp->jniUtils().JavaStringToQString(aJniEnv, aString), aAscent == JNI_TRUE ? true : false);
        }
    SWT_CATCH
    return pos;
    }

JNIEXPORT jobjectArray JNICALL OS_NATIVE( ListModel_1itemStrings )
( JNIEnv* aJniEnv, jclass, jint aHandle )
    {
    jobjectArray stringArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        ListModel* dataModel = reinterpret_cast< ListModel* > ( aHandle );
        stringArray = dataModel->itemStrings(aJniEnv);
        }
    SWT_CATCH
    return stringArray;
    }

JNIEXPORT jint JNICALL OS_NATIVE( ListModel_1indexOf )
( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aString, jint aStart )
    {
    jint index = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x start=%d", aHandle, aStart );
        ListModel* dataModel = reinterpret_cast< ListModel* > ( aHandle );
        index = dataModel->indexOf(swtApp->jniUtils().JavaStringToQString(aJniEnv, aString), aStart);
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT void JNICALL OS_NATIVE( ListModel_1remove )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d ", aHandle, aIndex );
        ListModel* dataModdel = reinterpret_cast< ListModel* > ( aHandle );
        dataModdel->remove(aIndex);
        }
    SWT_CATCH
    }
JNIEXPORT void JNICALL OS_NATIVE( ListModel_1setItemContentsToNull )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d ", aHandle, aIndex );
        ListModel* dataModel = reinterpret_cast< ListModel* > ( aHandle );
        dataModel->setItemContentsToNull( aIndex );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( ListModel_1clearList )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle  );
        ListModel* dataModdel = reinterpret_cast< ListModel* > ( aHandle );
        dataModdel->clearList();
        }
    SWT_CATCH
    }

//
// ListItemDelegate
//

JNIEXPORT jint JNICALL OS_NATIVE( ListBoxItemDelegate_1new )
( JNIEnv* aJniEnv , jclass, jint aParent )
    {
    ListBoxItemDelegate* itemDelegate( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("parent=%x ", aParent);
        HANDLE_TO_POINTER( QObject*, parentObject, aParent );
        itemDelegate = new ListBoxItemDelegate( parentObject );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( itemDelegate );
    }

JNIEXPORT void JNICALL OS_NATIVE( ListBoxItemDelegate_1setHeadingFont )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aFontHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x fontHandle=%x ", aHandle, aFontHandle );
        HANDLE_TO_POINTER( ListBoxItemDelegate*, itemDelegate, aHandle );
        QFont* font = reinterpret_cast< QFont* > ( aFontHandle );
        itemDelegate->setHeadingFont( font );
        }
    SWT_CATCH
    }

//
// QAbstractItemModel
//

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractItemModel_1index )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow, jint aColumn, jint aParentIndexHandle )
    {
    QModelIndex* modelIndex( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4( "handle=%x row=%d column=%d parentIndexHandle=%x ", aHandle, aRow, aColumn, aParentIndexHandle );
        QAbstractItemModel* itemModdel = reinterpret_cast< QAbstractItemModel* > ( aHandle );
        QModelIndex* parentIndex;
        if( aParentIndexHandle == 0 )
            {
            modelIndex = new QModelIndex( itemModdel->index(aRow, aColumn, QModelIndex() ) );
            }
        else
            {
            parentIndex =  reinterpret_cast< QModelIndex* > ( aParentIndexHandle );
            modelIndex = new QModelIndex( itemModdel->index(aRow, aColumn, *parentIndex ) );
            }
         }
    SWT_CATCH
    return reinterpret_cast< jint >( modelIndex );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractItemModel_1rowCount )
( JNIEnv* aJniEnv , jclass, jint aHandle  )
    {
    jint count = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        QAbstractItemModel* itemModdel = reinterpret_cast< QAbstractItemModel* > ( aHandle );
        count = itemModdel->rowCount();
        }
    SWT_CATCH
    return count;
    }

//
// QModelIndex
//

JNIEXPORT jint JNICALL OS_NATIVE( QModelIndex_1row )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint row(-1);
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        QModelIndex* modelIndex = reinterpret_cast< QModelIndex* >( aHandle );
        if(modelIndex != NULL && modelIndex->isValid())
            {
            row = modelIndex->row();
            }
        }
    SWT_CATCH
    return row;
    }

JNIEXPORT void JNICALL OS_NATIVE( QModelIndex_1delete )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        QModelIndex* modelIndex = reinterpret_cast< QModelIndex* >( aHandle );
        delete modelIndex;
        }
    SWT_CATCH
    }

//
// QItemSelectionModel
//

JNIEXPORT void JNICALL OS_NATIVE( QItemSelectionModel_1select__III )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndexHandle, jint aCommand )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x indexHandle=%d, command=%x ", aHandle, aIndexHandle, aCommand);
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        QModelIndex* modelIndex = reinterpret_cast<QModelIndex* > ( aIndexHandle );
        itemSelectionModel->select( *modelIndex, static_cast< QItemSelectionModel::SelectionFlags > ( aCommand ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QItemSelectionModel_1select_1_1set )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aSelectionHandle, jint aCommand )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x selectionHandle=%d, command=%x ", aHandle, aSelectionHandle, aCommand);
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        QItemSelection* selection = reinterpret_cast<QItemSelection* > ( aSelectionHandle );
        itemSelectionModel->select( *selection, static_cast< QItemSelectionModel::SelectionFlags > ( aCommand ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QItemSelectionModel_1select__IIII )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aStartIndexHandle, jint aEndIndexHandle, jint aCommand )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x startIndexHandle=%d, endIndexHandle=%d, command=%x ", aHandle, aStartIndexHandle, aEndIndexHandle, aCommand);
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        QModelIndex* startModelIndex = reinterpret_cast<QModelIndex* > ( aStartIndexHandle );
        QModelIndex* endModelIndex = reinterpret_cast<QModelIndex* > ( aEndIndexHandle );
        QItemSelection selection( *startModelIndex, *endModelIndex );
        itemSelectionModel->select( selection, static_cast< QItemSelectionModel::SelectionFlags > ( aCommand ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QItemSelectionModel_1clearSelection )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle );
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        itemSelectionModel->clearSelection();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QItemSelectionModel_1currentIndex )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    QModelIndex* currentModelIndex = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle );
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        currentModelIndex = new QModelIndex( itemSelectionModel->currentIndex() );
         }
    SWT_CATCH
    return reinterpret_cast< jint >( currentModelIndex );
    }

JNIEXPORT void JNICALL OS_NATIVE( QItemSelectionModel_1setCurrentIndex )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndexHandle, jint aCommand )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x indexHandle=%d, command=%x ", aHandle, aIndexHandle, aCommand);
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        QModelIndex* modelIndex = reinterpret_cast<QModelIndex* > ( aIndexHandle );
        itemSelectionModel->setCurrentIndex( *modelIndex, static_cast< QItemSelectionModel::SelectionFlags > ( aCommand ) );
        }
    SWT_CATCH
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QItemSelectionModel_1selectedRows )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn )
    {
    jintArray javaArray( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x column=%d ", aHandle, aColumn );
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        QModelIndexList indexList = itemSelectionModel->selectedRows( aColumn );
        const int count = indexList.count();
        if( count > 0 )
            {
            QVector<int> indexHandles( count );
            int* indexData = indexHandles.data();
            for(int i = 0; i < count; ++i)
                {
                indexData[i] =  indexList.value( i ).row();
                }
            javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, indexData, count );
            }
        }
    SWT_CATCH
    return javaArray;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE(QItemSelectionModel_1isSelected)
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndexHandle )
    {
    jboolean isSelected = JNI_FALSE;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x indexHandle=x ", aHandle, aIndexHandle);
        QItemSelectionModel* itemSelectionModel = reinterpret_cast<QItemSelectionModel* > ( aHandle );
        QModelIndex* modelIndex = reinterpret_cast<QModelIndex* > ( aIndexHandle );
        isSelected = itemSelectionModel->isSelected( *modelIndex );
        }
    SWT_CATCH
    return isSelected;
    }

//
// QAbstractItemView
//

JNIEXPORT  void JNICALL OS_NATIVE( QAbstractItemView_1setSelectionBehavior )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aBehavior )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x behavior=%x ", aHandle,  aBehavior );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        itemView->setSelectionBehavior( static_cast< QAbstractItemView::SelectionBehavior >( aBehavior ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1setSelectionMode )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aMode )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x mode=%x ", aHandle,  aMode );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        itemView->setSelectionMode( static_cast< QAbstractItemView::SelectionMode >( aMode ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1setModel )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aDataModelHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x dataModel=%x ", aHandle,  aDataModelHandle );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        QAbstractItemModel* dataModel = reinterpret_cast< QAbstractItemModel*>( aDataModelHandle );
        itemView->setModel( dataModel );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractItemView_1selectionModel )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    QItemSelectionModel* selectionModel( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        selectionModel = itemView->selectionModel();
        }
    SWT_CATCH
    return reinterpret_cast< jint >( selectionModel );
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1scrollTo )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndexHandle, jint aScrollHint )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x aIndex=%x scrollHint=%d ", aHandle, aIndexHandle, aScrollHint );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        QModelIndex* modelIndex = reinterpret_cast< QModelIndex* > ( aIndexHandle );
        itemView->scrollTo( *modelIndex, static_cast< QAbstractItemView::ScrollHint >( aScrollHint ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1clearSelection )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        itemView->clearSelection();
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1selectAll )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        itemView->selectAll();
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1setIconSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x x=%d y=%d", aHandle, aX, aY );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        itemView->setIconSize(QSize(aX,aY));
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QAbstractItemView_1iconSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        QSize size = itemView->iconSize();
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, size );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractItemView_1sizeHintForColumn )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn )
    {
    jint sizeHint = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x column=%d", aHandle, aColumn );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        sizeHint = itemView->sizeHintForColumn( aColumn );
        }
    SWT_CATCH
    return sizeHint;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QAbstractItemView_1sizeHintForIndex )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow, jint aColumn )
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x row=%d column=%d", aHandle, aRow, aColumn );
        HANDLE_TO_POINTER( QAbstractItemView*, widget, aHandle );
        QModelIndex* modelIndex = new QModelIndex( widget->model()->index( aRow, aColumn ) );
        QSize size( widget->sizeHintForIndex( *modelIndex ) );
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, size );
        delete modelIndex;
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractItemView_1sizeHintForRow )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow )
    {
    jint height = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x row=%d", aHandle, aRow );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        height = itemView->sizeHintForRow( aRow );
        }
    SWT_CATCH
    return height;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractItemView_1swt_1indexAt )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY )
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x x=%d y=%d", aHandle, aX, aY );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        res = itemView->indexAt( QPoint( aX, aY ) ).row();
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1swt_1removeRows )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aRow, jint aCount)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x row=%d count=%d", aHandle, aRow, aCount );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        QAbstractItemModel* model = itemView->model();
        model->removeRows(aRow, aCount);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractItemView_1update )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndexHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x aIndex=%x ", aHandle, aIndexHandle );
        HANDLE_TO_POINTER( QAbstractItemView*, itemView, aHandle );
        QModelIndex* modelIndex = reinterpret_cast< QModelIndex* > ( aIndexHandle );
        itemView->update( *modelIndex );
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QAbstractItemView_1visualRect )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow, jint aColumn )
    {
    jobject javaRect = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x row=%d column=%d", aHandle, aRow, aColumn );
        HANDLE_TO_POINTER( QAbstractItemView*, widget, aHandle );
        QModelIndex* modelIndex = new QModelIndex( widget->model()->index( aRow, aColumn ) );
        QRect rect( widget->visualRect( *modelIndex ) );
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        delete modelIndex;
        }
    SWT_CATCH
    return javaRect;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractItemView_1currentIndex )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint currentIndex = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x ", aHandle );
        HANDLE_TO_POINTER( QAbstractItemView*, widget, aHandle );
        currentIndex = widget->currentIndex().row();
        }
    SWT_CATCH
    return currentIndex;
    }

//
// QListView
//

JNIEXPORT jint JNICALL OS_NATIVE( QListView_1new )
    ( JNIEnv* aJniEnv , jclass )
    {
    QListView* listView( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        listView= new QListView();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( listView );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QListView_1flow )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        res = listView->flow();
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QListView_1gridSize )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QListView*, widget, aHandle );
        javaPoint = swtApp->jniUtils().NewJavaPoint( aJniEnv, widget->gridSize() );
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QListView_1resizeMode )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        res = listView->resizeMode();
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT void JNICALL OS_NATIVE( QListView_1setFlow )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aFlow )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x ", aHandle, aFlow );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        listView->setFlow( static_cast<QListView::Flow>(aFlow) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListView_1setItemDelegate )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aDelegateHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x delegateHandle=%x ", aHandle, aDelegateHandle );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        HANDLE_TO_POINTER( QAbstractItemDelegate*, itemDelegate, aDelegateHandle );
        listView->setItemDelegate( itemDelegate );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListView_1setMovement )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aMovement )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x value=%x", aHandle, aMovement );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        listView->setMovement( static_cast<QListView::Movement>( aMovement ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListView_1setViewMode )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aViewMode )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x value=%x", aHandle, aViewMode );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        listView->setViewMode( static_cast<QListView::ViewMode>( aViewMode ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListView_1setGridSize )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth, jint aHeight )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x width=%d height=%d", aHandle, aWidth, aHeight );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        listView->setGridSize( QSize( aWidth, aHeight ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListView_1setSpacing )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aSpacing )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x value=%d", aHandle, aSpacing );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        listView->setSpacing( aSpacing );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListView_1setResizeMode )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aMode )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x value=%d", aHandle, aMode );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        listView->setResizeMode( static_cast<QListView::ResizeMode>(aMode) );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QListView_1spacing )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint res = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QListView*, listView, aHandle );
        res = listView->spacing();
        }
    SWT_CATCH
    return res;
    }

//
//QModelIndexList
//

JNIEXPORT jint JNICALL OS_NATIVE( QModelIndexList_1size )
( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint size = 0;
     SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle );
        //HANDLE_TO_POINTER( QList*, list, aHandle );
        QModelIndexList* list = reinterpret_cast < QModelIndexList* > ( aHandle );
        size = list->size();
        }
    SWT_CATCH
    return size;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QModelIndexList_1at )
( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    const QModelIndex* modelIndex( NULL );
     SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%d ", aHandle, aIndex);
        QModelIndexList* list = reinterpret_cast < QModelIndexList* > ( aHandle );
        modelIndex =  &list->at( aIndex );
        }
    SWT_CATCH
    return reinterpret_cast < jint > ( modelIndex );
    }



//
//QKeySequence
//
JNIEXPORT jint JNICALL OS_NATIVE( QKeySequence_1new )
    ( JNIEnv* aJniEnv , jclass, jint aInteger )
    {
    QKeySequence* keySequence = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("aInteger=%x", aInteger);
        keySequence = new QKeySequence( aInteger );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( static_cast< QKeySequence* >( keySequence ) );
    }

JNIEXPORT void JNICALL OS_NATIVE( QKeySequence_1delete )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QKeySequence* keySequence = reinterpret_cast< QKeySequence* >( aHandle );
        delete keySequence;
        keySequence = NULL;
        }
    SWT_CATCH
    }
//
//QIcon
//
JNIEXPORT jint JNICALL OS_NATIVE( QIcon_1swt_1new )
    ( JNIEnv* aJniEnv , jclass, jint aImageHandle )
    {
    QIcon* icon = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("image=%x", aImageHandle);
        Java::GFX::Image* image = reinterpret_cast< Java::GFX::Image* >( aImageHandle );
        icon = new QIcon(image->toPixmap());
        }
    SWT_CATCH
    return reinterpret_cast< jint >( static_cast< QIcon* >( icon ) );
    }

JNIEXPORT jint JNICALL OS_NATIVE ( QIcon_1new )
    (JNIEnv* aJniEnv , jclass)
    {
    QIcon* icon = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        icon = new QIcon();
        }
    SWT_CATCH
    return reinterpret_cast< jint >( static_cast< QIcon* >( icon ) );
    }

JNIEXPORT void JNICALL OS_NATIVE( QIcon_1delete )
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QIcon* icon = reinterpret_cast< QIcon* >( aHandle );
        delete icon;
        icon = NULL;
        }
    SWT_CATCH

    }

JNIEXPORT jint JNICALL OS_NATIVE( QIcon_1pixmap )
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth, jint aHeight)
    {
    QPixmap* pixmap = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x width=%d height=%d", aHandle, aWidth, aHeight);
        QIcon* icon = reinterpret_cast< QIcon* >( aHandle );
        pixmap = new QPixmap(icon->pixmap(aWidth, aHeight));
        }
    SWT_CATCH
    return reinterpret_cast< jint >(pixmap);
    }

//
// HbIcon
//

JNIEXPORT jint JNICALL OS_NATIVE ( HbIcon_1new )
    (JNIEnv* aJniEnv , jclass, jstring aName)
    {
#ifdef __SYMBIAN32__
    HbIcon* icon = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        icon = new HbIcon(swtApp->jniUtils().JavaStringToQString(aJniEnv, aName));
        }
    SWT_CATCH
    return reinterpret_cast< jint >( static_cast< HbIcon* >( icon ) );
#else
    return 0;
#endif
    }

JNIEXPORT jint JNICALL OS_NATIVE( HbIcon_1pixmap )
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
#ifdef __SYMBIAN32__
    QPixmap* pixmap = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HbIcon* icon = reinterpret_cast< HbIcon* >( aHandle );
        pixmap = new QPixmap(icon->pixmap());
        }
    SWT_CATCH
    return reinterpret_cast< jint >(pixmap);
#else
    return 0;
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE( HbIcon_1delete )
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HbIcon* icon = reinterpret_cast< HbIcon* >( aHandle );
        delete icon;
        icon = NULL;
        }
    SWT_CATCH
#endif
    }

//
// QSlider
//

JNIEXPORT jint JNICALL OS_NATIVE( QSlider_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QSlider* slider = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
            slider = new QSlider();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( slider );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QSlider_1singleStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint singleStep = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        singleStep = slider->singleStep();
        }
    SWT_CATCH
    return singleStep;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QSlider_1maximum )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint maximum = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        maximum = slider->maximum();
        }
    SWT_CATCH
    return maximum;
    }


JNIEXPORT jint JNICALL OS_NATIVE( QSlider_1minimum )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint minimum = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        minimum = slider->minimum();
        }
    SWT_CATCH
    return minimum;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QSlider_1pageStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint pageStep = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        pageStep = slider->pageStep();
        }
    SWT_CATCH
    return pageStep;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QSlider_1value )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint value = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        value = slider->value();
        }
    SWT_CATCH
    return value;
    }

JNIEXPORT void JNICALL OS_NATIVE( QSlider_1setSingleStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        slider->setSingleStep( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QSlider_1setMaximum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        slider->setMaximum( aValue );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QSlider_1setMinimum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        slider->setMinimum( aValue );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QSlider_1setPageStep )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        slider->setPageStep( aValue );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QSlider_1setValue )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        slider->setValue( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QSlider_1setOrientation )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_POINTER( QSlider*, slider, aHandle );
        slider->setOrientation( static_cast<Qt::Orientation>( aValue ) );
        }
    SWT_CATCH
    }

//
// QColor
//

JNIEXPORT jint JNICALL OS_NATIVE( QColor_1new__ )
  (JNIEnv* aJniEnv , jclass)
    {
    QColor* color = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        color = new QColor();
        }
    SWT_CATCH
    return QCOLOR_TO_HANDLE( color );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QColor_1new__III )
  (JNIEnv* aJniEnv , jclass, jint aRed, jint aGreen, jint aBlue)
    {
    const int& red = static_cast<int>(aRed);
    const int& green = static_cast<int>(aGreen);
    const int& blue = static_cast<int>(aBlue);
    QColor* color = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        color = new QColor(red, green, blue);
        }
    SWT_CATCH
    return QCOLOR_TO_HANDLE( color );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QColor_1red )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint red = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QCOLOR( color, aHandle );
        red = color->red();
        }
    SWT_CATCH
    return red;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QColor_1green )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint green = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QCOLOR( color, aHandle );
        green = color->green();
        }
    SWT_CATCH
    return green;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QColor_1blue )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint blue = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QCOLOR( color, aHandle );
        blue = color->blue();
        }
    SWT_CATCH
    return blue;
    }

JNIEXPORT void JNICALL OS_NATIVE( QColor_1setRed )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_QCOLOR( color, aHandle );
        color->setRed( aValue );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QColor_1setGreen )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_QCOLOR( color, aHandle );
        color->setGreen( aValue );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QColor_1setBlue )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x", aHandle, aValue );
        HANDLE_TO_QCOLOR( color, aHandle );
        color->setBlue( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QColor_1delete )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QCOLOR( color, aHandle );
        delete color;
        color = NULL;
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QColor_1equals )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aCompareHandle)
    {
    jboolean result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle1=%x handle2=%x", aHandle, aCompareHandle);
        HANDLE_TO_QCOLOR( color1, aHandle );
        HANDLE_TO_QCOLOR( color2, aCompareHandle );
        result = (*color1 == *color2);
        }
    SWT_CATCH
    return result;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QColor_1isValid)
    (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jboolean result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle1=%x ", aHandle);
        HANDLE_TO_QCOLOR( color, aHandle );
        result = color->isValid();
        }
    SWT_CATCH
    return result;
    }
//
//QColorDialog
//
JNIEXPORT jint JNICALL OS_NATIVE( QColorDialog_1getColor )
    (JNIEnv* aJniEnv , jclass, jint aRed, jint aGreen, jint aBlue, jint aParentHandle, jstring aDialogID, jint aLayoutDirection)
    {
    QColor* color = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("parentHandle=%x layoutDirection=%d", aParentHandle, aLayoutDirection);
        HANDLE_TO_POINTER(QWidget*, parent, aParentHandle);
        QColorDialog dialog(QColor(aRed, aGreen, aBlue), parent);
        dialog.setObjectName(swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID));
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection));
        AutoPopExecStack stackExec(&dialog);
        int code = dialog.exec();
        if (code == QDialog::Accepted)
            {
            color = new QColor(dialog.selectedColor());
            }
        }
    SWT_CATCH
    return QCOLOR_TO_HANDLE( color );
    }




//
// QTableWidget
//

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QTableWidget* table = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        table = new QTableWidget();
        table->setWordWrap( false );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( table );
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1clearContents )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->clearContents();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1rowCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint rowCount = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        rowCount = table->rowCount();
        }
    SWT_CATCH
    return rowCount;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1columnCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint columnCount = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        columnCount = table->columnCount();
        }
    SWT_CATCH
    return columnCount;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1swt_1headerHeight )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aHorizontalHeader)
    {
    jint height = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x horizontalHeader=%d", aHandle, aHorizontalHeader);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        if (aHorizontalHeader)
            {
            height = table->horizontalHeader()->height();
            }
        else
            {
            height = table->verticalHeader()->height();
            }
        }
    SWT_CATCH
    return height;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1column )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumnHandle)
    {
    jint column = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x columnHandle=%x", aHandle, aColumnHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aColumnHandle );
        column = table->column( item );
        }
    SWT_CATCH
    return column;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1row )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRowHandle)
    {
    jint row = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x rowHandle=%x", aHandle, aRowHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aRowHandle );
        row = table->row( item );
        }
    SWT_CATCH
    return row;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1currentColumn )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint column = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        column = table->currentColumn();
        }
    SWT_CATCH
    return column;
    }


JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1currentRow )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint row = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        row = table->currentRow();
        }
    SWT_CATCH
    return row;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1swt_1selectionCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint count = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        QList<QTableWidgetItem*> items = table->selectedItems();

        int itemCount = items.size();
        QSet<int> rows;
        for(int i = 0; i < itemCount; ++i)
            {
            rows.insert(table->row(items.at(i)));
            }
        count = rows.size();
        }
    SWT_CATCH
    return count;
    }


JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1swt_1headerSize )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aHorizontalHeader, jint aIndex)
    {
    jint size = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x horizontalHeader=%d index=%d", aHandle, aHorizontalHeader, aIndex);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        if (aHorizontalHeader)
            {
            size = table->horizontalHeader()->sectionSize(aIndex);
            }
        else
            {
            size = table->verticalHeader()->sectionSize(aIndex);
            }
        }
    SWT_CATCH
    return size;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1topIndex )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint topIndex = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        topIndex = table->rowAt(1);
        }
    SWT_CATCH
    return topIndex;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1swt_1header )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aHorizontalHeader)
    {
    jint header = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x horizontalHeader=%d", aHandle, aHorizontalHeader);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        if (aHorizontalHeader)
            {
            header = POINTER_TO_HANDLE( table->horizontalHeader() );
            }
        else
            {
            header = POINTER_TO_HANDLE( table->horizontalHeader() );
            }
        }
    SWT_CATCH
    return header;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1itemAt )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
    {
    jint item = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x x=%d y=%d", aHandle, aX, aY);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        item = QTABLEWIDGETITEM_TO_HANDLE( table->itemAt( aX, aY ) );
        }
    SWT_CATCH
    return item;
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QTableWidget_1swt_1selectedItems )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jintArray javaArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        QList<QTableWidgetItem*> items = table->selectedItems();

        int itemCount = items.size();
        QSet<int> rows;
        for(int i = 0; i < itemCount; ++i)
            {
            rows.insert(table->row(items.at(i)));
            }
        QList<int> rowList = rows.toList();
        javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, rowList.toVector().data(), rows.size() );
        }
    SWT_CATCH
    return javaArray;
    }


JNIEXPORT jboolean JNICALL OS_NATIVE( QTableWidget_1swt_1headerVisible )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aHorizontalHeader)
    {
    jboolean visible = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x horizontalHeader=%d", aHandle, aHorizontalHeader);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        if (aHorizontalHeader)
            {
            visible = !table->horizontalHeader()->isHidden();
            }
        else
            {
            visible = !table->verticalHeader()->isHidden();
            }
        }
    SWT_CATCH
    return visible;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1setCellWidget )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow, jint aColumn, jint aWidgetHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x row=%d column=%d widgetHandle=%x", aHandle, aRow, aColumn, aWidgetHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        HANDLE_TO_POINTER( QWidget*,widget, aWidgetHandle );
        table->setCellWidget(aRow, aColumn, widget);
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1setItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow, jint aColumn, jint aItemHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x row=%d column=%d itemHandle=%x", aHandle, aRow, aColumn, aItemHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aItemHandle );
        table->setItem(aRow, aColumn, item);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1setRowCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRowCount)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x rowCount=%d", aHandle, aRowCount);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->setRowCount(aRowCount);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1setColumnCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumnCount)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x columnCount=%d", aHandle, aColumnCount);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->setColumnCount(aColumnCount);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1insertRow )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x row=%d", aHandle, aRow);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->insertRow(aRow);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1insertColumn )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x column=%d", aHandle, aColumn);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->insertColumn(aColumn);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1removeRow )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x row=%d", aHandle, aRow);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->removeRow(aRow);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1removeColumn )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x column=%d", aHandle, aColumn);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->removeColumn(aColumn);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1swt_1setHeaderVisible )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aHorizontalHeader, jboolean aVisible)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x horizontalHeader=%d visible=%d", aHandle, aHorizontalHeader, aVisible);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        if (aHorizontalHeader)
            {
            table->horizontalHeader()->setVisible(aVisible);
            }
        else
            {
            table->verticalHeader()->setVisible(aVisible);
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1swt_1setHeaderText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x column=%d", aHandle, aColumn);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        QTableWidgetItem* item = table->horizontalHeaderItem(aColumn);
        if (item == 0)
            {
            item = new QTableWidgetItem();
            }
        item->setText(swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ));
        table->setHorizontalHeaderItem(aColumn, item);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1scrollToItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle, jint aScrollHint)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x itemhandle=%x", aHandle, aItemHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aItemHandle );
        table->scrollToItem( item, static_cast<QAbstractItemView::ScrollHint>( aScrollHint ) );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1swt_1setHeaderIcon )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jint aIconHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x index=%d iconHandle=%d", aHandle, aIndex, aIconHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        QIcon* icon = reinterpret_cast<QIcon*>(aIconHandle);
        QTableWidgetItem* item = table->horizontalHeaderItem(aIndex);
        if (item == 0)
            {
            item = new QTableWidgetItem();
            }
        item->setIcon(*icon);
        table->setHorizontalHeaderItem(aIndex, item);
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1swt_1resizeHeaderSection )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aHorizontalHeader, jint aIndex, jint aSize)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x horizontalHeader=%d index=%d size=%d", aHandle, aHorizontalHeader, aIndex, aSize);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        if (aHorizontalHeader)
            {
            table->horizontalHeader()->resizeSection( aIndex, aSize );
            }
        else
            {
            table->verticalHeader()->resizeSection( aIndex, aSize );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1setCurrentItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x itemhandle=%x", aHandle, aItemHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aItemHandle );
        table->setCurrentItem( item );
        }
    SWT_CATCH
    }


JNIEXPORT jobject JNICALL OS_NATIVE( QTableWidget_1visualItemRect )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle)
    {
    jobject javaRect( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x itemhandle=%x", aHandle, aItemHandle);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aItemHandle );
        QRect rect = table->visualItemRect( item );
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        }
    SWT_CATCH
    return javaRect;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidget_1setCurrentCell )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow, jint aColumn)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x row=%d column=%d", aHandle, aRow, aColumn);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        table->setCurrentCell( aRow, aColumn );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidget_1swt_1headerItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    jint itemHandle = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%x", aHandle, aIndex);
        HANDLE_TO_POINTER( QTableWidget*, table, aHandle );
        QTableWidgetItem* item = table->horizontalHeaderItem(aIndex);
        if (item == 0)
            {
            item = new QTableWidgetItem();
            table->setHorizontalHeaderItem(aIndex, item);
            }
        itemHandle = QTABLEWIDGETITEM_TO_HANDLE(item);
        }
    SWT_CATCH
    return itemHandle;
    }


//
// QTableView
//

JNIEXPORT jint JNICALL OS_NATIVE( QTableView_1columnWidth )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn)
    {
    jint width = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x column=%d", aHandle, aColumn);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        width = table->columnWidth(aColumn);
        }
    SWT_CATCH
    return width;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableView_1rowHeight )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow)
    {
    jint height = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x row=%d", aHandle, aRow);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        height = table->rowHeight(aRow);
        }
    SWT_CATCH
    return height;
    }


JNIEXPORT jboolean JNICALL OS_NATIVE( QTableView_1showGrid )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean show = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        show = table->showGrid();
        }
    SWT_CATCH
    return show;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableView_1setShowGrid )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aShow)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x show=%d", aHandle, aShow);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        table->setShowGrid(aShow);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableView_1setWordWrap )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jboolean aOn)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x on=%d", aHandle, aOn);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        table->setWordWrap(aOn);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableView_1setColumnWidth )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn, jint aWidth)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x width=%d", aHandle, aWidth);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        table->setColumnWidth(aColumn, aWidth);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableView_1showRow )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x row=%d", aHandle, aRow);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        table->showRow( aRow );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QTableView_1showColumn )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x column=%d", aHandle, aColumn);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        table->showColumn( aColumn );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableView_1setRowHeight )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRow, jint aHeight)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x row=%d height=%d", aHandle, aRow, aHeight );
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        table->setRowHeight( aRow, aHeight );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableView_1setColumnHidden )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn, jboolean aHide)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x column=%d hide=%d", aHandle, aColumn, aHide );
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        table->setColumnHidden( aColumn, aHide );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableView_1rowAt )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aY)
    {
    jint row = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x y=%d", aHandle, aY);
        HANDLE_TO_POINTER( QTableView*, table, aHandle );
        row = table->rowAt( aY );
        }
    SWT_CATCH
    return row;
    }
//
// QTableWidgetItem
//
JNIEXPORT jint JNICALL OS_NATIVE( QTableWidgetItem_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QTableWidgetItem* item = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        item = new QTableWidgetItem();
        }
    SWT_CATCH
    return QTABLEWIDGETITEM_TO_HANDLE( item );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidgetItem_1flags )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint flags = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        flags = item->flags();
        }
    SWT_CATCH
    return flags;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTableWidgetItem_1data )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRole)
    {
    jint data = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x role=%d", aHandle, aRole );
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        data = item->data( aRole ).toInt();
        }
    SWT_CATCH
    return data;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QTableWidgetItem_1grayed )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean grayed = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        grayed = !(item->flags() & Qt::ItemIsEnabled);
        }
    SWT_CATCH
    return grayed;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QTableWidgetItem_1isSelected )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean selected = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        selected = item->isSelected();
        }
    SWT_CATCH
    return selected;
    }



JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1delete )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        delete item;
        item = NULL;
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1setFlags )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFlags)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x flags=%d", aHandle, aFlags);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        item->setFlags(static_cast<Qt::ItemFlags>( aFlags ));
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1setData )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRole, jint aData)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x role=%d data=%d", aHandle, aRole, aData);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        item->setData(aRole, QVariant(static_cast<int>(aData)));
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1setText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        item->setText(swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1swt_1setGrayed )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aGrayed)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x grayed=%d", aHandle, aGrayed);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        if (aGrayed)
            {
            item->setFlags(item->flags() &~ Qt::ItemIsEnabled);
            }
        else
            {
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1setSelected )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aSelected)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x selected=%d", aHandle, aSelected);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        item->setSelected(aSelected);
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1setIcon )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIconHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x iconHandle=%d", aHandle, aIconHandle);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        QIcon* icon = reinterpret_cast<QIcon*>(aIconHandle);
        item->setIcon(*icon);
        }
    SWT_CATCH
    }


JNIEXPORT jstring JNICALL OS_NATIVE( QTableWidgetItem_1text )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, item->text());
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1swt_1setBackground )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRed, jint aGreen, jint aBlue, jboolean aRestore)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x red=%d green=%d blue=%d", aHandle, aRed, aGreen, aBlue);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        const int& red = static_cast<int>(aRed);
        const int& green = static_cast<int>(aGreen);
        const int& blue = static_cast<int>(aBlue);
        QBrush brush;
        if( aRestore == JNI_FALSE )
            {
            brush.setColor( QColor(red, green, blue) );
            brush.setStyle( Qt::SolidPattern );
            }
        item->setBackground( brush );
        }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1swt_1setForeground )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRed, jint aGreen, jint aBlue, jboolean aRestore)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x red=%d green=%d blue=%d", aHandle, aRed, aGreen, aBlue);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        const int& red = static_cast<int>(aRed);
        const int& green = static_cast<int>(aGreen);
        const int& blue = static_cast<int>(aBlue);
        QBrush brush;
        brush.setStyle( Qt::NoBrush );
        if( aRestore == JNI_FALSE )
            {
            brush.setColor( QColor(red, green, blue) );
            brush.setStyle( Qt::SolidPattern );
            }
        item->setForeground( brush );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1swt_1setFont )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFontHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        QFont* font = reinterpret_cast<QFont*>( aFontHandle );
        if ( font )
            {
            item->setFont( *font );
            }
        else
            {
            item->setFont( QFont() );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTableWidgetItem_1setTextAlignment )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAlignment)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x alignment=%x", aHandle, aAlignment);
        HANDLE_TO_QTABLEWIDGETITEM( item, aHandle );
        item->setTextAlignment(aAlignment);
        }
    SWT_CATCH
    }

//
// QHeaderView
//
JNIEXPORT void JNICALL OS_NATIVE( QHeaderView_1setResizeMode )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jint aResizeMode)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x index=%d resizeMode=%d", aHandle, aIndex, aResizeMode);
        HANDLE_TO_POINTER( QHeaderView*, header, aHandle );
        header->setResizeMode(aIndex, static_cast<QHeaderView::ResizeMode>( aResizeMode ));
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QHeaderView_1resizeMode )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    jint resizeMode = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER( QHeaderView*, header, aHandle );
        resizeMode = static_cast<int>( header->resizeMode(aIndex) );
        }
    SWT_CATCH
    return resizeMode;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QHeaderView_1sectionSizeHint )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    jint sizeHint = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER( QHeaderView*, header, aHandle );
        sizeHint = header->sectionSizeHint(aIndex);
        }
    SWT_CATCH
    return sizeHint;
    }

//
// QProgressBar
//

JNIEXPORT jint JNICALL OS_NATIVE( QProgressBar_1new )
   ( JNIEnv* aJniEnv , jclass)
    {
    QProgressBar* pb = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
    pb = new QProgressBar();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( pb );
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressBar_1setOrientation )
   ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aOrientation)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x ", aHandle, aOrientation);
        HANDLE_TO_POINTER( QProgressBar*, pb, aHandle );
        pb->setOrientation( static_cast< Qt::Orientation >( aOrientation) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressBar_1setTextInVisible )
   ( JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle );
        HANDLE_TO_POINTER( QProgressBar*, pb, aHandle );
        pb->setTextVisible( false );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressBar_1setValue )
   ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x ", aHandle, aValue);
        HANDLE_TO_POINTER( QProgressBar*, pb, aHandle );
        pb->setValue( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressBar_1setMinimum )
   ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x ", aHandle, aValue);
        HANDLE_TO_POINTER( QProgressBar*, pb, aHandle );
        pb->setMinimum( aValue );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressBar_1setMaximum )
   ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x value=%x ", aHandle, aValue);
        HANDLE_TO_POINTER( QProgressBar*, pb, aHandle );
        pb->setMaximum( aValue );
        }
    SWT_CATCH
    }

//
// QComboBox
//
JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QComboBox* cb = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        cb = new QComboBox( );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( cb );
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1clear )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->clear();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1count )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint index=0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        index = cb->count();
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1currentIndex )
  (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint index=-1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        index = cb->currentIndex();
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1findText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    jint index=-1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x text=%x", aHandle, aText);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        index = cb->findText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1lineEdit )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QLineEdit* lineEdit = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        lineEdit = cb->lineEdit();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( lineEdit );
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QComboBox_1view
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QAbstractItemView* view = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        view = cb->view();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( view );
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1insertItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aItem, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x value=%x index=%x", aHandle, aItem, aIndex);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        int count = cb->count();
        cb->insertItem( aIndex, swtApp->jniUtils().JavaStringToQString( aJniEnv, aItem ) );
        if(  count == 0)
            {
            cb->setCurrentIndex( -1 );
            }
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QComboBox_1itemText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    jstring itemText=NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%x", aHandle, aIndex);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        itemText = swtApp->jniUtils().QStringToJavaString( aJniEnv, cb->itemText( aIndex ) );
        }
    SWT_CATCH
    return itemText;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1maxVisibleItems )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint visibleItems = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QComboBox*, cb, aHandle);
        visibleItems = cb->maxVisibleItems();
        }
    SWT_CATCH
    return visibleItems;
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1removeItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%x", aHandle, aIndex);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->removeItem( aIndex );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1setCurrentIndex )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x index=%x", aHandle, aIndex);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->setCurrentIndex( aIndex );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1setEditable )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aEditable)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x editable=%x", aHandle, aEditable);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->setEditable( aEditable );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1setInsertPolicy )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPolicy)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x policy=%x", aHandle, aPolicy);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->setInsertPolicy( static_cast<QComboBox::InsertPolicy>(aPolicy) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1setItemText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x text=%x index=%x", aHandle, aText, aIndex);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->setItemText( aIndex, swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1setMaxVisibleItems )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aCount)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x count=%x", aHandle, aCount);
        HANDLE_TO_POINTER(QComboBox*, cb, aHandle);
        cb->setMaxVisibleItems( aCount );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL  OS_NATIVE( QComboBox_1setSizeAdjustPolicy )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAdjustPolicy)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x policy=%x", aHandle, aAdjustPolicy);
        HANDLE_TO_POINTER(QComboBox*, cb, aHandle);
        cb->setSizeAdjustPolicy( static_cast<QComboBox::SizeAdjustPolicy> ( aAdjustPolicy ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1swt_1deselect )
  ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x ", aHandle);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->setCurrentIndex( -1 );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1swt_1getItemHeight )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint height = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QComboBox*, cb, aHandle);
        height = cb->view()->sizeHintForRow(0);
        }
    SWT_CATCH
    return height;
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QComboBox_1swt_1getSelection )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject selection = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QComboBox*, cb, aHandle);
        int end = cb->lineEdit()->cursorPosition();
        int start = cb->lineEdit()->selectionStart();
        selection = swtApp->jniUtils().NewJavaPoint( aJniEnv, QPoint((start == -1) ? end : start, end));
        }
    SWT_CATCH
    return selection;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QComboBox_1swt_1getTextHeight )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint height = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QComboBox*, cb, aHandle);
        height = cb->lineEdit()->sizeHint().height();
        }
    SWT_CATCH
    return height;
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1swt_1setItems )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jobjectArray aItems)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x items=%x", aHandle, aItems);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->clear();
        cb->addItems( swtApp->jniUtils().JavaStringArrayToQStringList( aJniEnv, aItems ) );
        cb->setCurrentIndex( -1 );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QComboBox_1swt_1setText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x text=%x", aHandle, aText);
        HANDLE_TO_POINTER( QComboBox*, cb, aHandle );
        cb->setCurrentIndex( -1 );
        cb->lineEdit()->setText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    }


//
// QListWidget
//
JNIEXPORT jint JNICALL OS_NATIVE( QListWidget_1new )
    ( JNIEnv* aJniEnv , jclass )
    {
    QListWidget* listWidget = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        listWidget = new QListWidget;
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( listWidget );
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1addItem )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aItem, jint aIconHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x text=%x icon=%x", aHandle, aItem, aIconHandle );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        if ( aIconHandle != 0 )
            {
            QListWidgetItem* item = new QListWidgetItem;
            item->setText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aItem ) );
            item->setIcon( *( reinterpret_cast<QIcon*>( aIconHandle ) ) );
            listWidget->addItem( item );
            }
        else
            {
            listWidget->addItem( swtApp->jniUtils().JavaStringToQString( aJniEnv, aItem ) );
            }
        }
     SWT_CATCH_1( ESwtErrorItemNotAdded );
     }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1addItems )
   ( JNIEnv* aJniEnv, jclass, jint aHandle, jobjectArray aItems, jintArray aIconHandles )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x items=%x icons=%x", aHandle, aItems, aIconHandles);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        QStringList items = swtApp->jniUtils().JavaStringArrayToQStringList( aJniEnv, aItems );
        jint* icons = NULL;
        if ( aIconHandles != 0 )
            {
            jboolean isCopy;
            icons = aJniEnv->GetIntArrayElements( aIconHandles, &isCopy );
            }
        int count = items.size();
        for (int i = 0; i < count; i++)
            {
            QListWidgetItem* item = new QListWidgetItem;
            item->setText( items.at( i ) );
            if ( icons != NULL )
                {
                QIcon* icon = reinterpret_cast<QIcon*>( icons[i] );
                if ( icon )
                    {
                    item->setIcon( *( icon ) );
                    }
                }
            listWidget->addItem( item );
            }
        }
    SWT_CATCH_1( ESwtErrorItemNotAdded )
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1clear )
   ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        listWidget->clear();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QListWidget_1count )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint index=0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        index = listWidget->count();
        }
    SWT_CATCH_1( ESwtErrorCannotGetCount )
    return index;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QListWidget_1currentRow )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jint index=-1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        index = listWidget->currentRow();
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1insertItem )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jstring aItem, jint aIconHandle )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x text=%x icon=%x index=%x", aHandle, aItem, aIconHandle, aIndex);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        if ( aIconHandle != 0 )
            {
            QListWidgetItem* item = new QListWidgetItem;
            item->setText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aItem ) );
            item->setIcon( *( reinterpret_cast<QIcon*>( aIconHandle ) ) );
            listWidget->insertItem( aIndex, item );
            }
        else
            {
            listWidget->insertItem( aIndex, swtApp->jniUtils().JavaStringToQString( aJniEnv, aItem ) );
            }
        }
     SWT_CATCH_1( ESwtErrorItemNotAdded )
     }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1scrollToItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jint aScrollHint)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x index=%d scroll=%d", aHandle, aIndex, aScrollHint );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        listWidget->scrollToItem( listWidget->item( aIndex ), static_cast<QAbstractItemView::ScrollHint>(aScrollHint));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1setSortingEnabled )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aStatus )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x value=%d", aHandle, aStatus );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        listWidget->setSortingEnabled( aStatus );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1sortItems )
  ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aOrder )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x value=%d", aHandle, aOrder );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        listWidget->sortItems( static_cast<Qt::SortOrder>( aOrder ) );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(  QListWidget_1swt_1indexOf )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText, jint aStartIndex )
    {
    jint index=-1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x text=%x index=%x", aHandle, aText, aStartIndex);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        QString text = swtApp->jniUtils().JavaStringToQString( aJniEnv, aText );
        for (int i = aStartIndex; i < listWidget->count(); ++i)
            {
            if((listWidget->item(i)->text()).compare( text, Qt::CaseSensitive ) == 0 )
                {
                index = i;
                break;
                }
            }
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QListWidget_1swt_1isItemSelected )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    jboolean res = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        QListWidgetItem* item = listWidget->item( aIndex );
        if ( item != NULL )
            {
            res = item->isSelected();
            }
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QListWidget_1swt_1isItemVisible )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    jboolean res = true;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        QListWidgetItem* item = listWidget->item( aIndex );
        if ( item != NULL )
            {
            res = !item->isHidden();
            }
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jstring JNICALL  OS_NATIVE( QListWidget_1swt_1itemText )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        javaString = swtApp->jniUtils().QStringToJavaString( aJniEnv, listWidget->item( aIndex )->text() );
        }
    SWT_CATCH_1( ESwtErrorCannotGetItem )
    return javaString;
    }

JNIEXPORT jobjectArray JNICALL  OS_NATIVE( QListWidget_1swt_1itemTexts )
    ( JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    jobjectArray javaStringArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        QStringList stringList;
        int count = listWidget->count();
        for ( int i = 0; i < count; i++ )
            {
            stringList << listWidget->item( i )->text();
            }
        javaStringArray = swtApp->jniUtils().NewJavaStringArray( aJniEnv, stringList );
        }
    SWT_CATCH
    return javaStringArray;
    }

void doQListWidgetSelect( QListWidget* aListWidget, int aStart, int aEnd, QItemSelectionModel::SelectionFlags aCommand )
    {
    QModelIndex modelIndexTL = QModelIndex( aListWidget->model()->index( aStart, 0, QModelIndex() ) );
    QModelIndex modelIndexBR = QModelIndex( aListWidget->model()->index( aEnd, 0, QModelIndex() ) );
    QItemSelection itemSelection= QItemSelection( modelIndexTL, modelIndexBR );
    aListWidget->selectionModel()->select( itemSelection, aCommand );
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1selectIndex )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jint aCommand )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x index=%d command=%x", aHandle, aIndex, aCommand);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        doQListWidgetSelect( listWidget, aIndex, aIndex, static_cast<QItemSelectionModel::SelectionFlags> ( aCommand ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1selectRange )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aStart, jint aEnd, jint aCommand )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4("handle=%x start=%x end=%x command=%x", aHandle, aStart, aEnd, aCommand);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        doQListWidgetSelect( listWidget, aStart, aEnd, static_cast<QItemSelectionModel::SelectionFlags> ( aCommand ) );
        }
    SWT_CATCH
    }

void doQListWidgetSetFocusIndex( QListWidget* aListWidget, int aIndex )
    {
    QModelIndex modelIndex = QModelIndex( aListWidget->model()->index( aIndex, 0, QModelIndex() ) );
    aListWidget->selectionModel()->setCurrentIndex( modelIndex, QItemSelectionModel::Current );
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1selectArray )
    ( JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aIndexArr,
            jint aIndexArrCount, jint aCommand, jboolean aScrollFocus )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x indexArrCount=%d focus=%d",
                aHandle, aIndexArrCount, aScrollFocus);
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );

        jboolean isCopy;
        jint* indices = aJniEnv->GetIntArrayElements( aIndexArr, &isCopy );
        QItemSelectionModel::SelectionFlags cmd = static_cast<QItemSelectionModel::SelectionFlags> ( aCommand );
        int count = listWidget->count();
        int focusIndex = listWidget->currentRow();
        for ( int i = 0; i < aIndexArrCount; i++ )
            {
            if ( indices[i] >= 0 && indices[i] < count)
                {
                doQListWidgetSelect( listWidget, indices[i], indices[i], cmd );
                focusIndex = indices[i];
                }
            }
        if( aScrollFocus)
            {
            listWidget->scrollToItem( listWidget->item( focusIndex ) );
            doQListWidgetSetFocusIndex( listWidget, focusIndex );
            }
        }
    SWT_CATCH
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QListWidget_1swt_1selectedRows )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn )
    {
    jintArray javaArray( NULL );
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x column=%d ", aHandle, aColumn );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        QModelIndexList indexList = listWidget->selectionModel()->selectedRows( aColumn );
        const int count = indexList.count();
        if( count > 0 )
            {
            QVector<int> indexHandles( count );
            int* indexData = indexHandles.data();
            for ( int i = 0; i < count; ++i )
                {
                indexData[i] = indexList.value( i ).row();
                }
            javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, indexData, count );
            }
        }
    SWT_CATCH_1( ESwtErrorCannotGetSelection )
    return javaArray;
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1removeIndex )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        delete listWidget->takeItem( aIndex );
        }
    SWT_CATCH_1( ESwtErrorItemNotRemoved )
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1removeRange )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aStart, jint aEnd )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x start=%d end=%d", aHandle, aStart, aEnd );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        for ( int i = aEnd; i >= aStart; --i )
            {
            delete listWidget->takeItem( i );
            }
        }
    SWT_CATCH_1( ESwtErrorItemNotRemoved )
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1removeArray )
    ( JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aIndexArr, jint aIndexArrCount )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x indexArrCount=%d", aHandle, aIndexArrCount );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        jboolean isCopy;
        jint* indices = aJniEnv->GetIntArrayElements( aIndexArr, &isCopy );
        if( indices != NULL  )
            {
            for ( int i = aIndexArrCount - 1; i >= 0; i-- )
                {
                delete listWidget->takeItem( indices[i] );
                }
            }
        }
    SWT_CATCH_1( ESwtErrorItemNotRemoved )
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1setFocusIndex )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        doQListWidgetSetFocusIndex( listWidget, aIndex );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1setItemVisible )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint index, jboolean aStatus )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x index=%d status=%d", aHandle, index, aStatus );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        QListWidgetItem* item = listWidget->item( index );
        if ( item != NULL )
            {
            item->setHidden( !aStatus );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QListWidget_1swt_1setText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x index=%d text=%d", aHandle, aIndex, aText );
        HANDLE_TO_POINTER( QListWidget*, listWidget, aHandle );
        listWidget->item( aIndex )->setText(swtApp->jniUtils().JavaStringToQString(aJniEnv, aText));
        }
    SWT_CATCH
    }

//
// QTreeWidget
//

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidget_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QTreeWidget* treeWidget = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        treeWidget = new QTreeWidget;
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( treeWidget );
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1addTopLevelItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x item=%x", aHandle, aItemHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aItemHandle );
        treeWidget->addTopLevelItem( treeWidgetItem );
        }
    SWT_CATCH
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QTreeWidget_1addTopLevelItems )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aCount)
    {
    jintArray javaArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x count=%x", aHandle, aCount );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        QList<QTreeWidgetItem *> items;
        QVector<int> handles( aCount );
        int* handleData = handles.data();
        for (int i = 0; i < aCount; ++i)
             {
             QTreeWidgetItem* item =  new QTreeWidgetItem();
             handleData[i] = QTREEWIDGETITEM_TO_HANDLE( item );
             items.append( item );
             }
        treeWidget->addTopLevelItems( items );
        javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, handleData, aCount );
        }
    SWT_CATCH
    return javaArray;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1clear )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        treeWidget->clear();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidget_1currentItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle )
    {
    QTreeWidgetItem* treeWidgetItem = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        treeWidgetItem = treeWidget->currentItem();
        }
    SWT_CATCH
    return reinterpret_cast< jint >( treeWidgetItem );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidget_1indexOfTopLevelItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle)
    {
    jint index = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x item=%x", aHandle, aItemHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aItemHandle );
        index = treeWidget->indexOfTopLevelItem(treeWidgetItem );
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1insertTopLevelItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x item=%x index=%d", aHandle, aItemHandle, aIndex );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aItemHandle );
        treeWidget->insertTopLevelItem( aIndex,treeWidgetItem );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidget_1itemAt )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
    {
    QTreeWidgetItem* treeWidgetItem = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x x=%x y=%d", aHandle, aX, aY );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        treeWidgetItem = treeWidget->itemAt( QPoint( aX, aY ) );
        }
    SWT_CATCH
    return reinterpret_cast< jint >( treeWidgetItem );
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1scrollToItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle, jint aScrollHint)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x item=%x index=%d", aHandle, aItemHandle, aScrollHint );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aItemHandle );
        treeWidget->scrollToItem( treeWidgetItem, static_cast<QAbstractItemView::ScrollHint>( aScrollHint ) );
        }
    SWT_CATCH
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QTreeWidget_1selectedItems )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jintArray javaArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );

        QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
        int selectedItemsCount = selectedItems.size();
        QVector<int> handles(selectedItemsCount);
        int* handleData = handles.data();

        for(int i = 0; i < selectedItemsCount; ++i)
            {
            handleData[i] = QTREEWIDGETITEM_TO_HANDLE(selectedItems.at(i));
            }
        javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, handleData, selectedItemsCount );
        }
    SWT_CATCH
    return javaArray;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1setColumnCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aCount)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x count=%x", aHandle, aCount );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        treeWidget->setColumnCount( aCount );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1setCurrentItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x count=%x", aHandle, aItemHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aItemHandle );
        treeWidget->setCurrentItem( treeWidgetItem );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidget_1swt_1rowAt )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint xPos, jint yPos)
    {
    jint index = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        index = treeWidget->indexOfTopLevelItem( treeWidget->itemAt( xPos, yPos ) );
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidget_1swt_1selectionCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint count = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        count = treeWidget->selectedItems().size();
        }
    SWT_CATCH
    return count;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1swt_1setHeaderStretch )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aVisible)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x visible=%x", aHandle, aVisible );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        treeWidget->header()->setStretchLastSection( aVisible );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1swt_1setHeaderVisible )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aVisible)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x visible=%x", aHandle, aVisible );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        treeWidget->header()->setVisible( aVisible );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidget_1takeTopLevelItem )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x index=%d", aHandle, aIndex );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        QTreeWidgetItem* item = treeWidget->takeTopLevelItem(aIndex);
        delete item;
        item = NULL;
        }
    SWT_CATCH
    }

JNIEXPORT jobject JNICALL OS_NATIVE( QTreeWidget_1visualItemRect )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aItemHandle)
    {
    jobject javaRect = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x item=%x", aHandle, aItemHandle );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        HANDLE_TO_QTREEWIDGETITEM( item, aItemHandle );
        QRect rect = treeWidget->visualItemRect( item );
        javaRect = swtApp->jniUtils().NewJavaRectangle( aJniEnv, rect );
        }
    SWT_CATCH
    return javaRect;
    }

//
// QTreeWidgetItem
//
JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidgetItem_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QTreeWidgetItem* treeWidgetItem = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        treeWidgetItem = new QTreeWidgetItem();
        }
    SWT_CATCH
    return reinterpret_cast< jint >( treeWidgetItem );
    }

JNIEXPORT jintArray JNICALL OS_NATIVE( QTreeWidgetItem_1addChildren )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aCount)
    {
    jintArray javaArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x count=%x", aHandle, aCount );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        QList<QTreeWidgetItem *> items;
        QVector<int> handles( aCount );
        int* handleData = handles.data();
        for (int i = 0; i < aCount; ++i)
            {
            QTreeWidgetItem* item =  new QTreeWidgetItem();
            handleData[i] = QTREEWIDGETITEM_TO_HANDLE( item );
            items.append( item );
            }
        treeWidgetItem->addChildren( items );
        javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, handleData, aCount );
        }
    SWT_CATCH
    return javaArray;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidgetItem_1checkState )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint checkState = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        checkState = treeWidgetItem->checkState( 0 );
        }
    SWT_CATCH
    return checkState;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidgetItem_1indexOfChild )
  (JNIEnv* aJniEnv , jclass, jint aParentHandle, jint aChildHandle)
    {
    int index = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "parentHandle=%x childHandle=%x", aParentHandle, aChildHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aParentHandle );
        HANDLE_TO_QTREEWIDGETITEM( childItem, aChildHandle );
        index = treeWidgetItem->indexOfChild( childItem );
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1insertChild )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aChildHandle, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3( "handle=%x child=%x index=%d", aHandle, aChildHandle, aIndex );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        HANDLE_TO_QTREEWIDGETITEM( childItem, aChildHandle );
        treeWidgetItem->insertChild( aIndex, childItem );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QTreeWidgetItem_1isExpanded )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jboolean expanded = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        expanded = treeWidgetItem->isExpanded();
        }
    SWT_CATCH
    return expanded;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1removeChild )
  (JNIEnv* aJniEnv , jclass, jint aParentHandle, jint aChildHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x child=%x ", aParentHandle, aChildHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aParentHandle );
        HANDLE_TO_QTREEWIDGETITEM( childItem, aChildHandle );
        treeWidgetItem->removeChild( childItem );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1setCheckState )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFlags)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x flags=%x", aHandle, aFlags );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        treeWidgetItem->setCheckState(0, static_cast<Qt::CheckState>( aFlags ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1setExpanded )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aExpanded)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x expanded=%x", aHandle, aExpanded );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        treeWidgetItem->setExpanded( aExpanded );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1setFlags )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFlags)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x flags=%x", aHandle, aFlags );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        treeWidgetItem->setFlags( static_cast<Qt::ItemFlags>( aFlags ) );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1setIcon )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aImageHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x iconHandle=%x", aHandle, aImageHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        QIcon* icon = reinterpret_cast<QIcon*>( aImageHandle );
        treeWidgetItem->setIcon( 0, *icon );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1setSelected )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aSelected)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x text=%x", aHandle, aSelected );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        treeWidgetItem->setSelected( aSelected );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1setText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x text=%x", aHandle, aText );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        treeWidgetItem->setText( 0, swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidgetItem_1swt_1isSameParent )
  (JNIEnv* aJniEnv , jclass, jint aFirstHandle, jint aLastHandle)
    {
    jint handle = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x handle=%x ", aFirstHandle, aLastHandle );
        HANDLE_TO_QTREEWIDGETITEM( firstTreeWidgetItem, aFirstHandle );
        HANDLE_TO_QTREEWIDGETITEM( lastTreeWidgetItem, aLastHandle );

        if(firstTreeWidgetItem->parent()==lastTreeWidgetItem->parent())
            {
            handle = QTREEWIDGETITEM_TO_HANDLE( firstTreeWidgetItem->parent() );
            }
        }
    SWT_CATCH
    return handle;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QTreeWidget_1swt_1parentChildCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint count = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        count = treeWidgetItem->parent()->childCount();
        }
    SWT_CATCH
    return count;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1swt_1setBackground )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRed, jint aGreen, jint aBlue, jboolean aRestore)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4( "handle=%x red=%d green=%d blue=%d", aHandle, aRed, aGreen, aBlue );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        const int& red = static_cast<int>(aRed);
        const int& green = static_cast<int>(aGreen);
        const int& blue = static_cast<int>(aBlue);
        QBrush brush;
        if( aRestore == JNI_FALSE )
            {
            brush.setColor( QColor(red, green, blue) );
            brush.setStyle( Qt::SolidPattern );
            }
        treeWidgetItem->setBackground( 0, brush );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1swt_1setFont )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFontHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x fontHandle=%x", aHandle, aFontHandle );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        QFont* font = reinterpret_cast<QFont*>( aFontHandle );
        if ( font )
            {
            treeWidgetItem->setFont( 0, *font );
            }
        else
            {
            treeWidgetItem->setFont( 0, QFont() );
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeWidgetItem_1swt_1setForeground )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aRed, jint aGreen, jint aBlue, jboolean aRestore)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_4( "handle=%x red=%d green=%d blue=%d", aHandle, aRed, aGreen, aBlue );
        HANDLE_TO_QTREEWIDGETITEM( treeWidgetItem, aHandle );
        const int& red = static_cast<int>(aRed);
        const int& green = static_cast<int>(aGreen);
        const int& blue = static_cast<int>(aBlue);
        QBrush brush;
        if ( aRestore == JNI_FALSE )
            {
            brush.setColor( QColor(red, green, blue) );
            brush.setStyle( Qt::SolidPattern );
            }
        treeWidgetItem->setForeground( 0, brush );
        }
    SWT_CATCH
    }

//
// QTreeView
//
JNIEXPORT jint JNICALL  OS_NATIVE( QTreeView_1columnWidth )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn)
    {
    jint columnWidth = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x column=%x", aHandle, aColumn );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        columnWidth = treeWidget->columnWidth( aColumn );
        }
    SWT_CATCH
    return columnWidth;
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeView_1resizeColumnToContents )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aColumn)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x column=%x", aHandle, aColumn );
        HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
        treeWidget->resizeColumnToContents( aColumn );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QTreeView_1selectAll )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
            HANDLE_TO_POINTER( QTreeWidget*, treeWidget, aHandle );
            treeWidget->selectAll();
        }
    SWT_CATCH
    }

//
// QDateTimeEdit
//
JNIEXPORT jint JNICALL OS_NATIVE( QDateTimeEdit_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QDateTimeEdit* editor = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        editor = new QDateTimeEdit();
        editor->setTimeSpec( Qt::UTC );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( editor );
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QDateTimeEdit_1calendarWidget
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QCalendarWidget* calendar = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
        calendar = editor->calendarWidget();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( calendar );
    }


JNIEXPORT void JNICALL OS_NATIVE( QDateTimeEdit_1setCalendarPopup )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aEnable)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x enable=%d", aHandle, aEnable );
         HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
         editor->setCalendarPopup( aEnable );
         }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QDateTimeEdit_1setDisplayFormat )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFormat)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
         editor->setDisplayFormat( swtApp->jniUtils().JavaStringToQString( aJniEnv, aFormat ) );
         }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QDateTimeEdit_1currentSectionIndex )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint index = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
         index = editor->currentSectionIndex();
         }
    SWT_CATCH
    return index;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QDateTimeEdit_1sectionCount )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint count = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
         count = editor->sectionCount();
         }
    SWT_CATCH
    return count;
    }


JNIEXPORT jintArray JNICALL OS_NATIVE( QDateTimeEdit_1swt_1dateTime )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jintArray javaArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );

        QDateTime dateTime = editor->dateTime();

        QDate date = dateTime.date();
        QTime time = dateTime.time();
        int result[6] = {0,0,0,0,0,0};

        result[0] = date.year();
        result[1] = date.month();
        result[2] = date.day();
        result[3] = time.hour();
        result[4] = time.minute();
        result[5] = time.second();

        javaArray = swtApp->jniUtils().NewJavaIntArray( aJniEnv, result, 6 );
        }
    SWT_CATCH
    return javaArray;
    }

JNIEXPORT void JNICALL OS_NATIVE( QDateTimeEdit_1swt_1setDate )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aDate)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
        jint* dateArray = 0;
        if (aDate != 0)
            {
            jboolean isCopy;
            dateArray = aJniEnv->GetIntArrayElements( aDate, &isCopy );

            if (dateArray != 0)
                {
                QDate date(dateArray[0], dateArray[1], dateArray[2]);
                editor->setDate(date);
                }
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QDateTimeEdit_1swt_1setTime )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aTime)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
        jint* timeArray = 0;
        if (aTime != 0)
            {
            jboolean isCopy;
            timeArray = aJniEnv->GetIntArrayElements( aTime, &isCopy );

            if (timeArray != 0)
                {
                QTime time(timeArray[0], timeArray[1], timeArray[2]);
                editor->setTime(time);
                }
            }
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QDateTimeEdit_1swt_1setDateTime )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aDateTime)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle );
        HANDLE_TO_POINTER( QDateTimeEdit*, editor, aHandle );
        jint* dateTimeArray = 0;
        if (aDateTime != 0)
            {
            jboolean isCopy;
            dateTimeArray = aJniEnv->GetIntArrayElements( aDateTime, &isCopy );

            if (dateTimeArray != 0)
                {
                QDate date(dateTimeArray[0], dateTimeArray[1], dateTimeArray[2]);
                QTime time(dateTimeArray[3], dateTimeArray[4], dateTimeArray[5]);
                QDateTime dateTime(date, time, Qt::UTC);
                editor->setDateTime(dateTime);
                }
            }
        }
    SWT_CATCH
    }

//
// QSwtTimeEdit
//
JNIEXPORT jint JNICALL OS_NATIVE( QSwtTimeEdit_1swt_1new )
  (JNIEnv* aJniEnv , jclass, jint aStyle)
    {
    QSwtTimeEdit* editor = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        editor = new QSwtTimeEdit();

        // Get default formatting for the editor from application locale for
        // TIME, DATE and DATE_TIME styles. DURATION and OFFSET are handled
        // on the Java side.
        QLocale locale;
        switch (aStyle)
            {
            case 1:     // DATE
                editor->setDisplayFormat(locale.dateFormat(QLocale::ShortFormat));
                editor->setTimeSpec( Qt::UTC );
                break;
            case 2:     // TIME
                editor->setTimeRange(QTime(0,0,0), QTime(23,59,59));
                editor->setDisplayFormat(locale.timeFormat(QLocale::LongFormat));
                break;
            case 3:     // DATE_TIME
                editor->setDisplayFormat(locale.dateTimeFormat(QLocale::ShortFormat));
                editor->setTimeSpec( Qt::UTC );
                break;
            }
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( editor );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QSwtTimeEdit_1editor )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint editorHandle = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QSwtTimeEdit*, editor, aHandle );
         editorHandle = POINTER_TO_HANDLE( editor->editor() );
         }
    SWT_CATCH
    return editorHandle;
    }


JNIEXPORT jint JNICALL OS_NATIVE( QSwtTimeEdit_1cursorPosition )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint pos = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QSwtTimeEdit*, editor, aHandle );
         pos = editor->cursorPosition();
         }
    SWT_CATCH
    return pos;
    }


JNIEXPORT jint JNICALL OS_NATIVE( QSwtTimeEdit_1textLength )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint length = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QSwtTimeEdit*, editor, aHandle );
         length = editor->textLength();
         }
    SWT_CATCH
    return length;
    }

JNIEXPORT void JNICALL OS_NATIVE( QSwtTimeEdit_1swt_1setTextVisible )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aEnable)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x enable=%x", aHandle, aEnable );
        HANDLE_TO_POINTER( QSwtTimeEdit*, editor, aHandle );
        editor->setTextVisible( aEnable );
        }
    SWT_CATCH
    }

//
// QOffsetTimeEdit
//

JNIEXPORT jint JNICALL OS_NATIVE( QOffsetTimeEdit_1new )
  (JNIEnv* aJniEnv , jclass)
    {
    QOffsetTimeEdit* editor = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        editor = new QOffsetTimeEdit();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( editor );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QOffsetTimeEdit_1offset )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint offset = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QOffsetTimeEdit*, editor, aHandle );
         offset = editor->getOffset();
         }
    SWT_CATCH
    return offset;
    }

JNIEXPORT void JNICALL OS_NATIVE( QOffsetTimeEdit_1setOffset )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aOffset)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x offset=%d", aHandle, aOffset );
         HANDLE_TO_POINTER( QOffsetTimeEdit*, editor, aHandle );
         editor->setOffset( aOffset );
         }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QOffsetTimeEdit_1setMinimum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMinimum)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x minimum=%d", aHandle, aMinimum );
         HANDLE_TO_POINTER( QOffsetTimeEdit*, editor, aHandle );
         editor->setMinimum( aMinimum );
         }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QOffsetTimeEdit_1setMaximum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMaximum)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x maximum=%d", aHandle, aMaximum );
         HANDLE_TO_POINTER( QOffsetTimeEdit*, editor, aHandle );
         editor->setMaximum( aMaximum );
         }
    SWT_CATCH
    }

//
// QProgressDialog
//
JNIEXPORT jint JNICALL OS_NATIVE( QProgressDialog_1swt_1new )
  (JNIEnv* aJniEnv , jclass, jint aParentHandle, jboolean aShowBar )
    {
    QProgressDialog* dialog = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QWidget*, parent, aParentHandle );
        dialog = new QProgressDialog( parent, Qt::ToolTip );
        if (!aShowBar)
            {
            // Create a custom TaskTipBar whose preferred size
            // is (0,0) and then set it to the dialog to make
            // ProgressBar disappear
            QTaskTipBar* bar = new QTaskTipBar(dialog);
            dialog->setBar(bar);
            }
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( dialog );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QProgressDialog_1maximum )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint maximum = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         maximum = dialog->maximum();
         }
    SWT_CATCH
    return maximum;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QProgressDialog_1minimum )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint minimum = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         minimum = dialog->minimum();
         }
    SWT_CATCH
    return minimum;
    }

JNIEXPORT jint JNICALL OS_NATIVE( QProgressDialog_1value )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint value = 0;
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         value = dialog->value();
         }
    SWT_CATCH
    return value;
    }


JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setAutoClose )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aClose)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x close=%d", aHandle, aClose );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         dialog->setAutoClose( aClose );
         }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setAutoReset )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jboolean aReset)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x reset=%d", aHandle, aReset );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         dialog->setAutoReset( aReset );
         }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setMaximum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMaximum)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x maximum=%d", aHandle, aMaximum );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         dialog->setMaximum( aMaximum );
         }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setMinimum )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMinimum)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x minimum=%d", aHandle, aMinimum );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         dialog->setMinimum( aMinimum );
         }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setValue )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aValue)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x value=%d", aHandle, aValue );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         dialog->setValue( aValue );
         }
    SWT_CATCH
    }


JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setLabelText )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aText)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_1("handle=%x", aHandle );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         dialog->setLabelText( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
         }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setBar )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aBarHandle)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x barHande=%x", aHandle, aBarHandle );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         HANDLE_TO_POINTER( QProgressBar*, bar, aBarHandle );
         dialog->setBar( bar );
         }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QProgressDialog_1setCancelButton )
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aButtonHandle)
    {
    SWT_TRY
         {
         SWT_LOG_JNI_CALL();
         SWT_LOG_DATA_2("handle=%x buttonHande=%x", aHandle, aButtonHandle );
         HANDLE_TO_POINTER( QProgressDialog*, dialog, aHandle );
         HANDLE_TO_POINTER( QPushButton*, button, aButtonHandle );
         dialog->setCancelButton( button );
         }
    SWT_CATCH
    }


//
JNIEXPORT jint JNICALL OS_NATIVE( QEvent_1new )
  (JNIEnv* aJniEnv , jclass, jint aType)
    {
    QEvent* event = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("type=%d", aType);

        // Ownership is passed to the caller
        event = new QEvent( static_cast<QEvent::Type>( aType ) );
        }
    SWT_CATCH
    QEvent* handle = static_cast<QEvent*>( event );
    return reinterpret_cast<jint>( handle );
    }
// QMouseEvent
//

JNIEXPORT jint JNICALL OS_NATIVE( QMouseEvent_1new )
  (JNIEnv* aJniEnv , jclass, jint aType, jint aXPos, jint aYPos, jint aXGlobalPos, jint aYGlobalPos, jint aButton, jint aButtons, jint aModifiers)
    {
    QMouseEvent* event = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("type=%d", aType);

        // Ownership is passed to the caller
        event = new QMouseEvent( static_cast<QEvent::Type>( aType ),
                                 QPoint( aXPos, aYPos ),
                                 QPoint( aXGlobalPos, aYGlobalPos ),
                                 static_cast<Qt::MouseButton>( aButton ),
                                 static_cast<Qt::MouseButtons>( aButtons ),
                                 static_cast<Qt::KeyboardModifiers>( aModifiers ) );
        }
    SWT_CATCH
    // Not a QObject
    QEvent* handle = static_cast<QEvent*>( event );
    return reinterpret_cast<jint>( handle );
    }

//
// QKeyEvent
//

JNIEXPORT jint JNICALL OS_NATIVE( QKeyEvent_1new )
  (JNIEnv* aJniEnv , jclass, jint aType, jint aKey, jint aModifiers, jstring aText)
    {
    QKeyEvent* event = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("type=%d", aType);

        // Ownership is passed to the caller
        event = new QKeyEvent( static_cast<QEvent::Type>( aType ),
                               aKey,
                               static_cast<Qt::KeyboardModifiers>( aModifiers ),
                               swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
        }
    SWT_CATCH
    // Not a QObject
    QEvent* handle = static_cast<QEvent*>( event );
    return reinterpret_cast<jint>( handle );
    }

//
// QCaptionedWidget
//

JNIEXPORT jint JNICALL OS_NATIVE( QCaptionedWidget_1new )
    ( JNIEnv* aJniEnv , jclass )
    {
    QCaptionedWidget* widget = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        widget = new QCaptionedWidget;
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( widget );
    }



//
// QMessageBox
//
JNIEXPORT jint JNICALL OS_NATIVE(QMessageBox_1swt_1exec)
   (JNIEnv* aJniEnv , jclass, jint aIcon, jstring aTitle, jstring aText,
    jint aButtons, jint aParent, jint aModality, jstring aDialogID, jint aLayoutDirection)
    {
    jint retValue = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("dialogID=%s, parent=%x icon=%d, buttons=%x, modality=%d", aDialogID, aParent, aIcon, aButtons, aModality);
        HANDLE_TO_POINTER(QWidget*, parent, aParent);
        QMessageBox msgBox( static_cast<QMessageBox::Icon>(aIcon),
                            swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle),
                            swtApp->jniUtils().JavaStringToQString(aJniEnv, aText),
                            static_cast<QMessageBox::StandardButtons>(aButtons),
                            parent );
        msgBox.setObjectName( swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID) );
        msgBox.setWindowModality( static_cast<Qt::WindowModality>(aModality) );
        msgBox.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        AutoPopExecStack stackExec(&msgBox);
        retValue = static_cast<jint>(msgBox.exec());
        }
    SWT_CATCH
    return retValue;
    }

JNIEXPORT void JNICALL OS_NATIVE(QMessageBox_1swt_1execTimer)
   (JNIEnv* aJniEnv , jclass, jint aIcon, jstring aTitle, jstring aText,
    jint aParent, jstring aDialogID, jint aLayoutDirection, jint aModality, jint aCgImageHandle)
    {
    SWT_TRY
        {
        // Define event filter which closes QMessageBox
        // on any key press or mouse button release events.
        class TimedMsgBoxEventFilter : public QObject
            {
        public:
            TimedMsgBoxEventFilter(QMessageBox& aMsgBox) :
                QObject(), msgBox(aMsgBox)
                {
                QObjectList list = aMsgBox.children();
                int count = list.count();
                for (int i = 0; i < count; ++i)
                    {
                    list.at(i)->installEventFilter(this);
                    }
                aMsgBox.installEventFilter(this);
                }

            virtual bool eventFilter(QObject* /*aObject*/, QEvent* aEvent)
                {
                if (aEvent->type() == QEvent::KeyPress ||
                    aEvent->type() == QEvent::MouseButtonRelease)
                    {
                    msgBox.reject();
                    return true;
                    }
                return false;
                }
        private:
            QMessageBox& msgBox;
            };


        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("dialogID=%s, parent=%x icon=%d, cgImage=%x, modality=%d", aDialogID, aParent, aIcon, aCgImageHandle, aModality);
        HANDLE_TO_POINTER(QWidget*, parent, aParent);

        QMessageBox msgBox( static_cast<QMessageBox::Icon>(aIcon),
                            swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle),
                            swtApp->jniUtils().JavaStringToQString(aJniEnv, aText),
                            QMessageBox::NoButton,
                            parent );

        // Following call is needed to hide the auto-added OK button
        msgBox.setStandardButtons(QMessageBox::NoButton);
        msgBox.setObjectName( swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID) );
        msgBox.setWindowModality( static_cast<Qt::WindowModality>(aModality) );
        msgBox.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );

        Java::GFX::Image* image = reinterpret_cast<Java::GFX::Image*>(aCgImageHandle);
        if(image)
            {
            QPixmap pixmap = image->toPixmap();
            if (!pixmap.isNull()) 
                {
                msgBox.setIconPixmap( pixmap );
                }
            }

        TimedMsgBoxEventFilter filter(msgBox);
        QTimer::singleShot(KTimedMessageBoxTimeout, &msgBox, SLOT(reject()));
        AutoPopExecStack stackExec(&msgBox);
        msgBox.exec();
        }
    SWT_CATCH
    }


//
// QInputDialog
//


JNIEXPORT jstring JNICALL OS_NATIVE(QInputDialog_1swt_1getText)
    (JNIEnv* aJniEnv , jclass, jint aParentHandle, jstring aTitle, jstring aLabel, jint aEchoMode, jstring aDefaultText, jstring aDialogId, jint aLayoutDirection)
    {
    jstring text = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("parent handle=%x echo mode=%d layoutDirection=%d", aParentHandle, aEchoMode, aLayoutDirection);
        HANDLE_TO_POINTER(QWidget*, parent, aParentHandle);
        QInputDialog dialog(parent);
        dialog.setObjectName(swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogId));
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        dialog.setInputMode(QInputDialog::TextInput);
        dialog.setWindowTitle(swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle));
        dialog.setLabelText(swtApp->jniUtils().JavaStringToQString(aJniEnv, aLabel));
        dialog.setTextEchoMode(static_cast<QLineEdit::EchoMode>(aEchoMode));
        dialog.setTextValue(swtApp->jniUtils().JavaStringToQString(aJniEnv, aDefaultText));
        AutoPopExecStack stackExec(&dialog);
        int result = dialog.exec();
        if (result == QDialog::Accepted)
            {
            text = swtApp->jniUtils().QStringToJavaString(aJniEnv, dialog.textValue());
            }
        }
    SWT_CATCH
    return text;
    }

JNIEXPORT jstring JNICALL OS_NATIVE(QInputDialog_1swt_1getInteger)
  (JNIEnv *aJniEnv, jclass, jint aParentHandle, jstring aTitle, jstring aLabel, 
  jint aMin, jint aMax, jint aDefaultValue, jstring aDialogId, jint aLayoutDirection)
    {
    jstring integerString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("parent handle=%x min=%d, max=%d, default=%d, layoutDirection=%d",
                        aParentHandle, aMin, aMax, aDefaultValue, aLayoutDirection);
        HANDLE_TO_POINTER(QWidget*, parent, aParentHandle);
        QInputDialog dialog(parent);
        dialog.setObjectName(swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogId));
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        dialog.setInputMode(QInputDialog::IntInput);
        dialog.setWindowTitle(swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle));
        dialog.setLabelText(swtApp->jniUtils().JavaStringToQString(aJniEnv, aLabel));
        dialog.setIntRange(aMin, aMax);
        // Order of this call matters, causes problems if done before setting range
        dialog.setIntValue(aDefaultValue);
        AutoPopExecStack stackExec(&dialog);
        int result = dialog.exec();
        if (result == QDialog::Accepted)
            {
            QString str = QString("%1").arg(dialog.intValue());
            integerString = swtApp->jniUtils().QStringToJavaString(aJniEnv, str);
            }
        }
    SWT_CATCH
    return integerString;
}
//
// QVBoxLayout
//

JNIEXPORT jint JNICALL OS_NATIVE( QVBoxLayout_1new )
    ( JNIEnv* aJniEnv , jclass, jint aParent )
    {
    QVBoxLayout* layout = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HANDLE_TO_POINTER( QWidget*, parent, aParent );
        layout = new QVBoxLayout( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( layout );
    }


//
// QLayout
//

JNIEXPORT void JNICALL OS_NATIVE( QLayout_1addWidget )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidget )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x widget=%d", aHandle, aWidget);
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aWidget );
        layout->addWidget( widget );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QLayout_1setContentsMargins )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aLeft, jint aTop, jint aRight, jint aBottom )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("handle=%x left=%d top=%d right=%d bottom=%d", aHandle, aLeft, aTop, aRight, aBottom);
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        layout->setContentsMargins( aLeft, aTop, aRight, aBottom );
        }
    SWT_CATCH
    }
JNIEXPORT void JNICALL OS_NATIVE( QLayout_1setMenuBar )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aMenuBar )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x menubar=%d", aHandle, aMenuBar );
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        HANDLE_TO_POINTER( QWidget*, widget, aMenuBar );
        layout->setMenuBar( widget );
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QLayout_1setSpacing )
    ( JNIEnv* aJniEnv , jclass, jint aHandle, jint aSpacing )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x spacing=%d", aHandle, aSpacing );
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        layout->setSpacing( aSpacing );
        }
    SWT_CATCH
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QLayout_1activate )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    bool result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        result = layout->activate();
        }
    SWT_CATCH
    return (result ? JNI_TRUE : JNI_FALSE);
    }

JNIEXPORT void JNICALL OS_NATIVE( QLayout_1update )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        layout->update();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE( QLayout_1sizeConstraint )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    jint result = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        result = layout->sizeConstraint();
        }
    SWT_CATCH
    return result;
    }

JNIEXPORT void JNICALL OS_NATIVE( QLayout_1setSizeConstraint )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aConstraint)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x constraint=%x", aHandle, aConstraint);
        HANDLE_TO_POINTER( QLayout*, layout, aHandle );
        layout->setSizeConstraint((QLayout::SizeConstraint)aConstraint);
        }
    SWT_CATCH
    }

//
// QDesktopServices
//
JNIEXPORT jboolean JNICALL OS_NATIVE( QDesktopServices_1openUrl )
(JNIEnv* aJniEnv , jclass, jstring aUrl)
{
    bool result = false;
#ifndef QT_NO_DESKTOPSERVICES
    SWT_TRY
    {
        SWT_LOG_JNI_CALL();
        QString url = swtApp->jniUtils().JavaStringToQString( aJniEnv, aUrl );
        SWT_LOG_DATA_1( "call QDesktopSrvices::openUrl %d", 0 );
        result = QDesktopServices::openUrl( QUrl( url ) );
    }
    SWT_CATCH
#endif
    return ( result ? JNI_TRUE : JNI_FALSE );
}

//
// Special purpose global functions exported by Qt
//

JNIEXPORT void JNICALL OS_NATIVE( qt_1x11_1wait_1for_1window_1manager )
  (JNIEnv* aJniEnv , jclass, jint
#ifdef Q_WS_X11
aHandle
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifdef Q_WS_X11
          SWT_LOG_DATA_1("handle=%d", aHandle);
        HANDLE_TO_POINTER(QWidget*, widget, aHandle);
            qt_x11_wait_for_window_manager( widget );
#endif
        }
    SWT_CATCH
    }

//
// QSystemTrayIcon
//

JNIEXPORT jboolean JNICALL OS_NATIVE( QSystemTrayIcon_1isSystemTrayAvailable )
  (JNIEnv* aJniEnv , jclass)
{
    bool result = false;
    SWT_TRY
    {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_SYSTEMTRAYICON
        SWT_LOG_DATA_1( "call QSystemTrayIcon::isSystemTrayAvailable %d", 0 );
        result = QSystemTrayIcon::isSystemTrayAvailable();
#endif
    }
    SWT_CATCH
    return ( result ? JNI_TRUE : JNI_FALSE );
}

JNIEXPORT jint JNICALL OS_NATIVE( QSystemTrayIcon_1new )
  (JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_SYSTEMTRAYICON
aParent
#endif
)
   {
   jint retVal = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_SYSTEMTRAYICON
        HANDLE_TO_POINTER( QObject*, parent, aParent );
        QSystemTrayIcon* tray = NULL;
        tray = new QSystemTrayIcon( parent );
        retVal = POINTER_TO_HANDLE( tray );
#endif
        }
    SWT_CATCH
    return retVal;
   }

JNIEXPORT void JNICALL OS_NATIVE( QSystemTrayIcon_1setIcon )
  (JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_SYSTEMTRAYICON
aHandle
#endif
, jint
#ifndef QT_NO_SYSTEMTRAYICON
aIconHandle
#endif
)
  {
  SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_SYSTEMTRAYICON
        SWT_LOG_DATA_2("handle=%x, aIconHandle=%x", aHandle, aIconHandle );
        HANDLE_TO_POINTER( QSystemTrayIcon*, tray, aHandle );
        QIcon* icon = reinterpret_cast<QIcon*>(aIconHandle);
        tray->setIcon( *icon );
#endif
        }
  SWT_CATCH
  }

JNIEXPORT jboolean JNICALL OS_NATIVE(QSystemTrayIcon_1isVisible)
    (JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_SYSTEMTRAYICON
aHandle
#endif
)
    {
    bool visible = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_SYSTEMTRAYICON
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QSystemTrayIcon*, tray, aHandle );
        visible = tray->isVisible();
#endif
        }
    SWT_CATCH
    return ( visible ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT void JNICALL OS_NATIVE(QSystemTrayIcon_1setVisible)
    (JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_SYSTEMTRAYICON
aHandle
#endif
, jboolean
#ifndef QT_NO_SYSTEMTRAYICON
aVisible
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_SYSTEMTRAYICON
        SWT_LOG_DATA_2("handle=%x visible=%d", aHandle, aVisible);
        HANDLE_TO_POINTER( QSystemTrayIcon*, tray, aHandle );
        tray->setVisible( aVisible == JNI_TRUE ? true : false );
#endif
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE (QSystemTrayIcon_1setToolTip)
    (JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_SYSTEMTRAYICON
aHandle
#endif
, jstring
#ifndef QT_NO_SYSTEMTRAYICON
aText
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_SYSTEMTRAYICON
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER( QSystemTrayIcon*, tray, aHandle );
        tray->setToolTip( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
#endif
        }
    SWT_CATCH
    }
//
// QFileDialog
//

JNIEXPORT jobjectArray JNICALL OS_NATIVE (QFileDialog_1swt_1getOpenFileName)
( JNIEnv* aJniEnv , jclass, jint aParent, jstring aTitle, jstring aDirectory, jstring aFilter, jstring aSelectedFilter, jstring aDialogID, jint aLayoutDirection )
    {
    jobjectArray javaStringArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "parent=%x direction=%d ", aParent, aLayoutDirection );
        HANDLE_TO_POINTER(QWidget*, parent, aParent);
        QFileDialog dialog( parent,
            aTitle != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle) : QString(),
            aDirectory != NULL ?swtApp->jniUtils().JavaStringToQString(aJniEnv, aDirectory) : QString(),
            aFilter != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aFilter) : QString(static_cast<const char*>(0)) );
        dialog.setObjectName( swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID ) );
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setAcceptMode( static_cast<QFileDialog::AcceptMode>(QFileDialog::AcceptOpen) );
        if(aSelectedFilter != NULL)
            {
            dialog.selectFilter (swtApp->jniUtils().JavaStringToQString(aJniEnv, aSelectedFilter));
            }
        AutoPopExecStack stackExec(&dialog);
        int code = dialog.exec();
        if( code == QDialog::Accepted )
            {
            QStringList stringList = dialog.selectedFiles();
            stringList.append( dialog.selectedFilter() );
            javaStringArray = swtApp->jniUtils().NewJavaStringArray(aJniEnv, stringList);
            }
        }
    SWT_CATCH
    return javaStringArray;
    }

JNIEXPORT jobjectArray JNICALL OS_NATIVE (QFileDialog_1swt_1getOpenFileNames)
( JNIEnv* aJniEnv , jclass, jint aParent, jstring aTitle, jstring aDirectory, jstring aFilter, jstring aSelectedFilter, jstring aDialogID, jint aLayoutDirection )
    {
    jobjectArray javaStringArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "parent=%x direction=%d ", aParent, aLayoutDirection );
        HANDLE_TO_POINTER(QWidget*, parent, aParent);
        QFileDialog dialog( parent,
            aTitle != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle) : QString(),
            aDirectory != NULL ?swtApp->jniUtils().JavaStringToQString(aJniEnv, aDirectory) : QString(),
            aFilter != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aFilter) : QString(static_cast<const char*>(0)) );
        dialog.setObjectName( swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID ) );
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        dialog.setFileMode(QFileDialog::ExistingFiles);
        dialog.setAcceptMode( static_cast<QFileDialog::AcceptMode>(QFileDialog::AcceptOpen) );
        if(aSelectedFilter != NULL)
            {
            dialog.selectFilter (swtApp->jniUtils().JavaStringToQString(aJniEnv, aSelectedFilter));
            }
        AutoPopExecStack stackExec(&dialog);
        int code = dialog.exec();
        if( code == QDialog::Accepted )
            {
            QStringList stringList = dialog.selectedFiles();
            stringList.append( dialog.selectedFilter() );
            javaStringArray = swtApp->jniUtils().NewJavaStringArray(aJniEnv, stringList);
            }
        }
    SWT_CATCH
    return javaStringArray;
    }

JNIEXPORT jobjectArray JNICALL OS_NATIVE (QFileDialog_1swt_1getSaveFileName)
( JNIEnv* aJniEnv , jclass, jint aParent, jstring aTitle, jstring aDirectory, jstring aFilter, jstring aSelectedFilter, jstring aDialogID, jint aLayoutDirection )
    {
    jobjectArray javaStringArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "parent=%x direction=%d ", aParent, aLayoutDirection );
        HANDLE_TO_POINTER(QWidget*, parent, aParent);
        QFileDialog dialog( parent,
            aTitle != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle) : QString(),
            aDirectory != NULL ?swtApp->jniUtils().JavaStringToQString(aJniEnv, aDirectory) : QString(),
            aFilter != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aFilter) : QString(static_cast<const char*>(0)) );
        dialog.setObjectName( swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID ) );
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        dialog.setAcceptMode( static_cast<QFileDialog::AcceptMode>(QFileDialog::AcceptSave) );
        dialog.setConfirmOverwrite ( false );
        dialog.setFileMode(QFileDialog::AnyFile);
        if(aSelectedFilter != NULL)
            {
            dialog.selectFilter (swtApp->jniUtils().JavaStringToQString(aJniEnv, aSelectedFilter));
            }
        AutoPopExecStack stackExec(&dialog);
        int code = dialog.exec();
        if( code == QDialog::Accepted )
            {
            QStringList stringList = dialog.selectedFiles();
            stringList.append( dialog.selectedFilter() );
            javaStringArray = swtApp->jniUtils().NewJavaStringArray(aJniEnv, stringList);
            }
        }
    SWT_CATCH
    return javaStringArray;
    }

JNIEXPORT jstring JNICALL OS_NATIVE (QFileDialog_1swt_1getOpenDirectoryName)
( JNIEnv* aJniEnv , jclass, jint aParent, jstring aTitle, jstring aDirectory, jstring aDialogID, jint aLayoutDirection )
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "parent=%x direction=%d ", aParent, aLayoutDirection );
        HANDLE_TO_POINTER(QWidget*, parent, aParent);
        QFileDialog dialog( parent,
            aTitle != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle) : QString(),
            aDirectory != NULL ? swtApp->jniUtils().JavaStringToQString(aJniEnv, aDirectory) : QString(), NULL );
        dialog.setObjectName( swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID ) );
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        dialog.setFileMode( QFileDialog::DirectoryOnly );
        dialog.setAcceptMode( static_cast<QFileDialog::AcceptMode>(QFileDialog::AcceptOpen) );
        AutoPopExecStack stackExec(&dialog);
        int code = dialog.exec();
        if( code == QDialog::Accepted )
            {
            QStringList stringList = dialog.selectedFiles();
            if( stringList.count() > 0 )
                {
                javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, stringList.at(0));
                }
            }
        }
    SWT_CATCH
    return javaString;
    }


JNIEXPORT void JNICALL OS_NATIVE (QDialog_1swt_1closeDialogs)
( JNIEnv* aJniEnv , jclass, jint aObject, jstring aDialogID )
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "object=%x ", aObject );
        HANDLE_TO_POINTER(QObject*, object, aObject);
        QObjectList children = object->children();
        if( aDialogID )
            {
            // close the dialog which is identified by dialog ID
            QString name = swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID );
            for(int i = 0; i < children.size(); ++i)
                {
                QDialog* dialog = qobject_cast<QDialog*>(children.at(i));
                if( dialog && dialog->objectName() == name )
                    {
                    dialog->setParent(NULL);
                    dialog->reject();
                    break;
                    }
                }
            }
        else
            {
            // close all dialogs
             for(int i = 0; i < children.size(); ++i)
                {
                QDialog* dialog = qobject_cast<QDialog*>(children.at(i));
                if( dialog )
                    {
                    // next line can cause memory leak if the dialog was created in heap
                    dialog->setParent(NULL);
                    dialog->reject();
                    }
                }
            }
        }
     SWT_CATCH
    }

//
// QDialog
//
JNIEXPORT jint JNICALL OS_NATIVE( QDialog_1new )
  (JNIEnv* aJniEnv, jclass, jint aParent, jint aFlags)
    {
    QDialog* dialog = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("parent=%x flags=%x", aParent, aFlags);

        HANDLE_TO_POINTER( QWidget*, parent, aParent );
        dialog = new QDialog( parent, static_cast<Qt::WindowFlags>(aFlags) );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE(dialog);
    }

//
// QTabWidget
//
JNIEXPORT jint JNICALL OS_NATIVE(QSwtTabWidget_1new)
    (JNIEnv* aJniEnv , jclass)
    {
    QTabWidget* tabWidget = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        tabWidget = new QSwtTabWidget();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE(tabWidget);
    }

JNIEXPORT void JNICALL OS_NATIVE(QTabWidget_1addTab)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aPageHandle, jint aIconHandle, jstring aLabel)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x, pageHandle=%x, iconHandle=%x", aHandle, aPageHandle, aIconHandle);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle );
        HANDLE_TO_POINTER(QWidget*, page, aPageHandle);
        QIcon* icon = reinterpret_cast<QIcon*>(aIconHandle);
        tabWidget->addTab(page, *icon, swtApp->jniUtils().JavaStringToQString(aJniEnv, aLabel));
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTabWidget_1count)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint count = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle );
        count = static_cast<jint>(tabWidget->count());
        }
    SWT_CATCH
    return count;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTabWidget_1currentIndex)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint index = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle );
        index = static_cast<jint>(tabWidget->currentIndex());
        }
    SWT_CATCH
    return index;
    }

JNIEXPORT void JNICALL OS_NATIVE(QTabWidget_1removeTab)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle);
        tabWidget->removeTab(static_cast<int>(aIndex));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTabWidget_1setCurrentIndex)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle);
        tabWidget->setCurrentIndex(static_cast<int>(aIndex));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTabWidget_1setTabIcon)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jint aIconHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_3("handle=%x, index=%d, iconHandle=%x", aHandle, aIndex, aIconHandle);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle);
        tabWidget->setTabIcon(static_cast<int>(aIndex), *(reinterpret_cast<QIcon*>(aIconHandle)));
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE(QTabWidget_1setTabText)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex, jstring aText)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle);
        tabWidget->setTabText(static_cast<int>(aIndex), swtApp->jniUtils().JavaStringToQString(aJniEnv, aText));
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE(QTabWidget_1tabText)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    jstring javaString = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle);
        javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, tabWidget->tabText(static_cast<int>(aIndex)));
        }
    SWT_CATCH
    return javaString;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTabWidget_1widget)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aIndex)
    {
    jint handle = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x, index=%d", aHandle, aIndex);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle );
        handle = POINTER_TO_HANDLE(tabWidget->widget(static_cast<int>(aIndex)));
        }
    SWT_CATCH
    return handle;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTabWidget_1swt_1stackedWidgetHandle)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint handle = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle );
        QObjectList children = tabWidget->children();
        int childCount = children.size();
        for(int i = 0; i < childCount; ++i)
            {
            if (qobject_cast<QStackedWidget*>(children.at(i)))
                {
                handle = POINTER_TO_HANDLE(children.at(i));
                break;
                }
            }
        }
    SWT_CATCH
    return handle;
    }

JNIEXPORT jobject JNICALL OS_NATIVE(QSwtTabWidget_1stackedWidgetSize)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jobject javaPoint = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QSwtTabWidget*, tabWidget, aHandle);
        javaPoint = swtApp->jniUtils().NewJavaPoint(aJniEnv, tabWidget->stackedWidgetSize());
        }
    SWT_CATCH
    return javaPoint;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QTabWidget_1tabBar)
    (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    jint handle = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(QTabWidget*, tabWidget, aHandle );
        QObjectList children = tabWidget->children();
        int childCount = children.size();
        for(int i = 0; i < childCount; ++i)
            {
            if (qobject_cast<QTabBar*>(children.at(i)))
                {
                handle = POINTER_TO_HANDLE(children.at(i));
                break;
                }
            }
         }
    SWT_CATCH
    return handle;
    }


//
// QClipboard
//
JNIEXPORT void JNICALL OS_NATIVE( QClipboard_1clear )
    ( JNIEnv* aJniEnv , jclass )
    {
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        QApplication::clipboard()->clear();
        }
    SWT_CATCH
#endif
    }

JNIEXPORT jbyteArray JNICALL OS_NATIVE( QClipboard_1mimeData )
    ( JNIEnv* aJniEnv , jclass, jstring aMimeType)
    {
    jbyteArray res = NULL;
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        const QMimeData* mimeData = QApplication::clipboard()->mimeData();
        QString dataType = swtApp->jniUtils().JavaStringToQString( aJniEnv, aMimeType );
        QByteArray byteArr = mimeData->data( dataType );
        SWT_LOG_DATA_2("Getting %d bytes, type %s", byteArr.count(), dataType.toLatin1().data());
        res = swtApp->jniUtils().NewJavaByteArray( aJniEnv, byteArr );
        }
    SWT_CATCH
#endif
    return res;
    }

JNIEXPORT void JNICALL OS_NATIVE( QClipboard_1setMimeData )
    ( JNIEnv* aJniEnv , jclass, jstring aMimeType, jbyteArray aByteArray )
    {
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        QByteArray byteArr = swtApp->jniUtils().JavaByteArrayToQByteArray( aJniEnv, aByteArray );
        QString dataType = swtApp->jniUtils().JavaStringToQString( aJniEnv, aMimeType );
        QMimeData* data = new QMimeData();
        data->setData( dataType, byteArr );
        SWT_LOG_DATA_2("Setting %d bytes, type %s", byteArr.count(), dataType.toLatin1().data());
        QApplication::clipboard()->setMimeData( data );
        }
    SWT_CATCH
#endif
    }

JNIEXPORT jobjectArray JNICALL OS_NATIVE( QClipboard_1swt_1formats )
    ( JNIEnv* aJniEnv , jclass )
    {
    jobjectArray res = NULL;
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        QClipboard* clip = QApplication::clipboard();
        const QMimeData* mimeData = clip->mimeData();
        res = swtApp->jniUtils().NewJavaStringArray( aJniEnv, mimeData->formats() );
        }
    SWT_CATCH
#endif
    return res;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QClipboard_1swt_1isEmptyText )
    ( JNIEnv* aJniEnv , jclass )
    {
    jboolean res = JNI_TRUE;
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        const QMimeData* mimeData = QApplication::clipboard()->mimeData();
        if(!mimeData->hasText() || !mimeData->text().isEmpty())
            {
            res = JNI_FALSE;
            }
        }
    SWT_CATCH
#endif
    return res;
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QClipboard_1text )
    ( JNIEnv* aJniEnv , jclass)
    {
    jstring res = NULL;
#ifndef QT_NO_CLIPBOARD
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        QClipboard* clip = QApplication::clipboard();
        res = swtApp->jniUtils().QStringToJavaString( aJniEnv, clip->text() );
        }
    SWT_CATCH
#endif
    return res;
    }

//
// QFontDialog
//
JNIEXPORT jint JNICALL OS_NATIVE(QFontDialog_1swt_1getFont)
    ( JNIEnv* aJniEnv , jclass, jint aParentHandle, jstring aTitle,
      jstring aFamily, jint aPointSize, jint aWeight, jboolean aItalic, jstring aDialogID, jint aLayoutDirection)
    {
    QFont* font(NULL);
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_5("parent=%x, size=%d, weight=%d, italic=%d, layoutDirection=%d", aParentHandle, aPointSize, aWeight, aItalic, aLayoutDirection);
        HANDLE_TO_POINTER(QWidget*, parent, aParentHandle );
        QFont initialFont;
        if (static_cast<int>(aPointSize) > -1)
            {
            initialFont = QFont(swtApp->jniUtils().JavaStringToQString(aJniEnv, aFamily),
                                static_cast<int>(aPointSize),
                                static_cast<int>(aWeight),
                                aItalic == JNI_TRUE ? true : false);
            }

        QFontDialog dialog( initialFont, parent );
        dialog.setObjectName( swtApp->jniUtils().JavaStringToQString(aJniEnv, aDialogID ) );
        dialog.setLayoutDirection( static_cast<Qt::LayoutDirection>(aLayoutDirection) );
        dialog.setWindowTitle(swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle));
        AutoPopExecStack stackExec(&dialog);
        int code = dialog.exec();
        if( code == QDialog::Accepted )
            {
            QFont selected = dialog.selectedFont();
            font = new QFont( selected );
            }
        }
    SWT_CATCH
    return reinterpret_cast<jint>(font);
    }


//
// QStyle
//
JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1pixelMetric
    (JNIEnv *aJniEnv, jclass, jint aId)
    {
    jint metric = -1;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("id=%d", aId);
        QStyle* style = QApplication::style();
        if (style)
            {
            metric = static_cast<jint>(style->pixelMetric(static_cast<QStyle::PixelMetric>(aId)));
            }
        }
    SWT_CATCH
    return metric;
    }

JNIEXPORT jint JNICALL OS_NATIVE(QStyle_1standardIcon)
    (JNIEnv* aJniEnv , jclass, jint aHandle, jint aStandardIconId)
    {
    QIcon* icon = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x iconId=%d", aHandle, aStandardIconId);
        HANDLE_TO_POINTER(QStyle*,style, aHandle);
        icon = new QIcon(style->standardIcon(static_cast<QStyle::StandardPixmap>(aStandardIconId)));
        }
    SWT_CATCH
    return reinterpret_cast<jint>(icon);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1dockWidgetTitleBarButtonMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_DockWidgetTitleBarButtonMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1mdiSubWindowMinimizedWidth
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_MdiSubWindowMinimizedWidth);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1layoutLeftMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_LayoutLeftMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1layoutTopMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_LayoutTopMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1layoutRightMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_LayoutRightMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1layoutBottomMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_LayoutBottomMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1layoutHorizontalSpacing
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_LayoutHorizontalSpacing);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1layoutVerticalSpacing
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_LayoutVerticalSpacing);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1dockWidgetTitleMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_DockWidgetTitleMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolBarFrameWidth
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolBarFrameWidth);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolBarHandleExtent
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolBarHandleExtent);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolBarItemMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolBarItemMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolBarItemSpacing
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolBarItemSpacing);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolBarSeparatorExtent
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolBarSeparatorExtent);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolBarExtensionExtent
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolBarExtensionExtent);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1tabBarScrollButtonWidth
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TabBarScrollButtonWidth);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1tabBarTabShiftHorizontal
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TabBarTabShiftHorizontal);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1tabBarTabShiftVertical
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TabBarTabShiftVertical);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1headerMarkSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_HeaderMarkSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1headerGripMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_HeaderGripMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1headerMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_HeaderMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1SpinBoxSliderHeight
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_SpinBoxSliderHeight);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolBarIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolBarIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1smallIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_SmallIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1largeIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_LargeIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1focusFrameHMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_FocusFrameHMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1focusFrameVMargin
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_FocusFrameVMargin);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1iconViewIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_IconViewIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1listViewIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ListViewIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1toolTipLabelFrameWidth
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ToolTipLabelFrameWidth);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1checkBoxLabelSpacing
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_CheckBoxLabelSpacing);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1radioButtonLabelSpacing
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_RadioButtonLabelSpacing);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1tabBarIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TabBarIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1sizeGripSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_SizeGripSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1messageBoxIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_MessageBoxIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1buttonIconSize
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_ButtonIconSize);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1textCursorWidth
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TextCursorWidth);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1tabBarScrollButtonOverlap
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TabBar_ScrollButtonOverlap);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1tabCloseIndicatorWidth
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TabCloseIndicatorWidth);
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QStyle_1swt_1pm_1tabCloseIndicatorHeight
  (JNIEnv *, jclass)
    {
    return static_cast<jint>(QStyle::PM_TabCloseIndicatorHeight);
    }

//
// QLocale
//

JNIEXPORT jint JNICALL OS_NATIVE( QLocale_1system )
  (JNIEnv* aJniEnv, jclass)
    {
    QLocale* locale = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        locale = new QLocale( QLocale::system() );
        }
    SWT_CATCH
    return reinterpret_cast<int>( locale );
    }

JNIEXPORT jint JNICALL OS_NATIVE( QLocale_1language )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    QLocale::Language language = QLocale::C;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QLocale* locale = reinterpret_cast<QLocale*>(aHandle);
        language = locale->language();
        }
    SWT_CATCH
    return static_cast<int>( language );
    }

JNIEXPORT void JNICALL OS_NATIVE( QLocale_1delete )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QLocale* locale = reinterpret_cast<QLocale*>(aHandle);
        delete locale;
        }
    SWT_CATCH
    }

//
// QChar
//
JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_OS_QChar_1direction
  (JNIEnv *aJniEnv, jclass, jchar aChar)
	{
		int result=-1;
	    SWT_TRY
	        {
	        SWT_LOG_JNI_CALL();
	        result = QChar::direction(aChar);
	        }
	    SWT_CATCH
	    return result;
	}

//
// SwtFontCache
//

JNIEXPORT jint JNICALL OS_NATIVE( SwtFontCache_1cache )
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    QFont* result = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFont* font = reinterpret_cast<QFont*> ( aHandle );
        result = swtApp->fontCache().cache( font );
        }
    SWT_CATCH
    return reinterpret_cast<jint>( result );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( SwtFontCache_1isCached )
  (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
    bool result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QFont* font = reinterpret_cast<QFont*> ( aHandle );
        result = swtApp->fontCache().isCached( font );
        }
    SWT_CATCH
    return result ? JNI_TRUE : JNI_FALSE;
    }

//
// QInputContextFactory
//

JNIEXPORT jint JNICALL OS_NATIVE( QInputContextFactory_1create )
  (JNIEnv* aJniEnv, jclass, jstring aKey, jint aParent)
    {
    QInputContext* inputContext = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("parent=%x", aParent);
        QObject* parent = reinterpret_cast<QObject*>(aParent);
        inputContext = QInputContextFactory::create(swtApp->jniUtils().JavaStringToQString(aJniEnv, aKey), parent);
        }
    SWT_CATCH
    return reinterpret_cast<jint>(inputContext);
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QInputContextFactory_1swt_1key )
    (JNIEnv* aJniEnv, jclass, jstring aLanguage)
    {
    jstring strKey = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        QStringList keys = QInputContextFactory::keys();
        QStringList languages;
        QString TargetedLanguage = swtApp->jniUtils().JavaStringToQString(aJniEnv, aLanguage);
        for (int i = 0; i < keys.count(); i++)
            {
            const QString& key = keys.at(i);
            languages = QInputContextFactory::languages(key);
            for (int j = 0; j < languages.count(); j++)
                {
                const QString& language = languages.at(j);
                if (TargetedLanguage == language
                        || language.contains(TargetedLanguage)
                        || TargetedLanguage.contains(language))
                    {
                    strKey = swtApp->jniUtils().QStringToJavaString(aJniEnv, key);
                    break;
                    }
                }
            if (strKey != NULL)
                break;
            }
        }
    SWT_CATCH
    return strKey;
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_extension_OS_HbInputSettingProxy_1availableHwKeyboard
    (JNIEnv* aJniEnv, jclass)
    {
    jint result = 0;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        HbInputSettingProxy* hbisp = HbInputSettingProxy::instance();
        QList<HbKeyboardType> keyBoards;
        hbisp->availableHwKeyboard(keyBoards);
        if(keyBoards.size()>0)
            {
            result = keyBoards[0];
            }
        }
    SWT_CATCH
#endif
    return result;
    }

//
// SwtApplication
//

JNIEXPORT jint JNICALL OS_NATIVE( SwtApplication_1javaCallbackCount )
  (JNIEnv* aJniEnv, jclass)
    {
    jint result = 0;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        result = static_cast<jint>(swtApp->jniUtils().javaCallbackCount());
        }
    SWT_CATCH
    return result;
    }

JNIEXPORT void JNICALL OS_NATIVE( SwtApplication_1postDeferredEvents )
  (JNIEnv* aJniEnv, jclass)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        swtApp->postDeferredEvents();
        }
    SWT_CATCH
    }

//
// XQServiceRequest
//

JNIEXPORT jint JNICALL OS_NATIVE( XQServiceRequest_1new )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jstring aService, jstring aOperation, jboolean aSynchronous)
#else
(JNIEnv* aJniEnv, jclass, jstring, jstring, jboolean)
#endif
    {
#ifdef __SYMBIAN32__
    XQServiceRequest* request = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        request = new XQServiceRequest(swtApp->jniUtils().JavaStringToQString(aJniEnv, aService), swtApp->jniUtils().JavaStringToQString(aJniEnv, aOperation), aSynchronous  == JNI_TRUE ? true : false);
        }
    SWT_CATCH
    return reinterpret_cast<jint>(request);
#else
return NULL;
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE( XQServiceRequest_1swt_1setArgumentsForFetchEmail )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aTitle, jstring aAction, jstring)
#else
(JNIEnv* aJniEnv, jclass, jint, jstring, jstring, jstring)
#endif
    {
#ifdef __SYMBIAN32__    
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        XQServiceRequest* request = reinterpret_cast<XQServiceRequest*>(aHandle);
        *request << swtApp->jniUtils().JavaStringToQString(aJniEnv, aTitle);
        *request << swtApp->jniUtils().JavaStringToQString(aJniEnv, aAction);
        }
    SWT_CATCH
#endif    
    }

JNIEXPORT void JNICALL OS_NATIVE( XQServiceRequest_1swt_1setArgumentsForDial )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aNumber, jboolean aAsyncAnswer)
#else
(JNIEnv* aJniEnv, jclass, jint, jstring, jboolean)
#endif
    {
#ifdef __SYMBIAN32__    
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        XQServiceRequest* request = reinterpret_cast<XQServiceRequest*>(aHandle);
        *request << swtApp->jniUtils().JavaStringToQString(aJniEnv, aNumber);
        *request << (aAsyncAnswer == JNI_TRUE ? true : false);
        }
    SWT_CATCH
#endif    
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( XQServiceRequest_1send )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle)
#else
(JNIEnv* aJniEnv, jclass, jint)
#endif
    {
#ifdef __SYMBIAN32__    
    bool result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        XQServiceRequest* client = reinterpret_cast<XQServiceRequest*>(aHandle);
        QVariant retValue;
        result = client->send(retValue);
        }
    SWT_CATCH
    return result ? JNI_TRUE : JNI_FALSE;
#else
    return JNI_FALSE;
#endif    
    }

//
// XQApplicationManager
//

JNIEXPORT jint JNICALL OS_NATIVE( XQApplicationManager_1new )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass)
#else
(JNIEnv* aJniEnv, jclass)
#endif
    {
#ifdef __SYMBIAN32__    
    XQApplicationManager* aiwMgr = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        aiwMgr = new XQApplicationManager();
        }
    SWT_CATCH
    return reinterpret_cast<jint>(aiwMgr);
#else
    return NULL;
#endif    
    }

JNIEXPORT jint JNICALL OS_NATIVE( XQApplicationManager_1create )
#ifdef __SYMBIAN32__
  (JNIEnv * aJniEnv, jclass,jint aHandle, jstring aService, jstring aInterface, jstring aOperation, jboolean aSynchronous)
#else
(JNIEnv *, jclass, jint, jstring, jstring, jstring, jboolean)
#endif
    {
#ifdef __SYMBIAN32__
    XQAiwRequest* request = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        XQApplicationManager* aiwmgr = reinterpret_cast<XQApplicationManager*>(aHandle);
        request = aiwmgr->create(swtApp->jniUtils().JavaStringToQString(aJniEnv, aService),
                swtApp->jniUtils().JavaStringToQString(aJniEnv, aInterface), 
                swtApp->jniUtils().JavaStringToQString(aJniEnv, aOperation), 
                aSynchronous  == JNI_TRUE ? true : false);
        }
    SWT_CATCH
    return reinterpret_cast<jint>(request);
#else
return NULL;
#endif
    }

//
// XQAiwRequest
//
JNIEXPORT void JNICALL OS_NATIVE( XQAiwRequest_1setArguments )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aNumber)
#else
(JNIEnv* aJniEnv, jclass, jint, jstring)
#endif
    {
#ifdef __SYMBIAN32__    
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        XQAiwRequest* request = reinterpret_cast<XQAiwRequest*>(aHandle);
        if (request)
            {
            QList<QVariant> args;
            args << swtApp->jniUtils().JavaStringToQString(aJniEnv,
                    aNumber);
            request->setArguments(args);
            }
        }
    SWT_CATCH
#endif    
    }

JNIEXPORT void JNICALL OS_NATIVE( XQAiwRequest_1swtDialer_1setArguments )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aNumber)
#else
(JNIEnv* aJniEnv, jclass, jint, jstring)
#endif
    {
#ifdef __SYMBIAN32__    
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        XQAiwRequest* request = reinterpret_cast<XQAiwRequest*>(aHandle);
        if (request)
            {
            QList<QVariant> args;
            QVariantMap map;
            map.insert(XQLOGS_VIEW_INDEX, QVariant(int(XQService::LogsViewAll)));
            map.insert(XQLOGS_SHOW_DIALPAD, QVariant(true));
            map.insert(XQLOGS_DIALPAD_TEXT, QVariant(swtApp->jniUtils().JavaStringToQString(aJniEnv,
                    aNumber)));
            args.append(QVariant(map));
            request->setArguments(args);
            }
        }
    SWT_CATCH
#endif    
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( XQAiwRequest_1send )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle)
#else
(JNIEnv* aJniEnv, jclass, jint)
#endif
    {
#ifdef __SYMBIAN32__    
    bool result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        XQAiwRequest* request= reinterpret_cast<XQAiwRequest*>(aHandle);
        if (request)
            {
            result = request->send();
            }
        }
    SWT_CATCH
    return result ? JNI_TRUE : JNI_FALSE;
#else
    return JNI_FALSE;
#endif    
    }

//
// CntServicesContactList
//

JNIEXPORT jobjectArray  JNICALL OS_NATIVE( CntServicesContactList_1swt_1contacts )
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle)
#else
(JNIEnv* aJniEnv, jclass, jint)
#endif
    {
#ifdef __SYMBIAN32__        
    jobjectArray javaStringArray = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        QVariant* variant = reinterpret_cast<QVariant*>(aHandle);
        CntServicesContactList contacts = qVariantValue<CntServicesContactList>(*variant);
        QStringList list;
        for(int i = 0; i < contacts.count(); i++)
            {
            list.append(contacts.at(i).mDisplayName);
            list.append(contacts.at(i).mPhoneNumber);
            list.append(contacts.at(i).mEmailAddress);
            
            }
        if(list.size() > 0)
            {
            javaStringArray = swtApp->jniUtils().NewJavaStringArray(aJniEnv, list);
            }
        }
    SWT_CATCH
    return javaStringArray;
#else
    return NULL;
#endif    
    }

//
// MobileDevice, Screen, Input
//

JNIEXPORT jint JNICALL OS_NATIVE( MobileDevice_1new )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass)
#else
  (JNIEnv*, jclass)
#endif
    {
    jint handle = 0;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        handle =  swtApp->initializeMobileDevice();
        }
    SWT_CATCH
#endif
    return handle;    
    }

JNIEXPORT void JNICALL OS_NATIVE( MobileDevice_1createFlipWatch )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jint aHandle)
#else
  (JNIEnv*, jclass, jint)
#endif
    {
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        CSwtMobileDevice* mobileDevice =  reinterpret_cast<CSwtMobileDevice*>(aHandle);
        mobileDevice->CreateFlipWatchL();
        }
    SWT_CATCH
#endif
    }
    
JNIEXPORT void JNICALL OS_NATIVE( MobileDevice_1destroy )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jint /*aHandle*/)
#else
  (JNIEnv*, jclass, jint)
#endif
    {
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", swtApp );
        swtApp->destroyMobileDevice();
        }
    SWT_CATCH
#endif
    }

JNIEXPORT jint JNICALL OS_NATIVE( MobileDevice_1getUserInactivityTime )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jint aHandle)
#else
  (JNIEnv*, jclass, jint)
#endif
    {
    jint time=0;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        CSwtMobileDevice* mobileDevice =  reinterpret_cast<CSwtMobileDevice*>(aHandle);
        time = mobileDevice->GetUserInactivityTime();
        }
    SWT_CATCH
#endif
    return time;
    }

JNIEXPORT void JNICALL OS_NATIVE( MobileDevice_1resetUserInactivityTime )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jint aHandle)
#else
  (JNIEnv*, jclass, jint)
#endif
    {
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1( "handle=%x", aHandle );
        CSwtMobileDevice* mobileDevice =  reinterpret_cast<CSwtMobileDevice*>(aHandle);
        mobileDevice->ResetUserInactivityTime();
        }
    SWT_CATCH
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE( MobileDevice_1setLight )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aDuration)
#else
  (JNIEnv*, jclass, jint, jint)
#endif
    {
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x duration=%x", aHandle, aDuration );
        CSwtMobileDevice* mobileDevice =  reinterpret_cast<CSwtMobileDevice*>(aHandle);
        mobileDevice->SetLights(static_cast<TInt>(aDuration));
        }
    SWT_CATCH
#endif
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( MobileDevice_1flashLights )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aDuration)
#else
  (JNIEnv*, jclass, jint, jint)
#endif
    {
    jboolean flashLights = JNI_FALSE;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x duration=%x", aHandle, aDuration );
        CSwtMobileDevice* mobileDevice =  reinterpret_cast<CSwtMobileDevice*>(aHandle);
        TRAP_IGNORE(flashLights = mobileDevice->FlashBacklightL(static_cast<TInt>(aDuration)));
        }
    SWT_CATCH
#endif
    return ( flashLights ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( MobileDevice_1vibration )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aDuration)
#else
  (JNIEnv*, jclass, jint, jint)
#endif
    {
    jboolean vibraSupport = JNI_FALSE;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2( "handle=%x duration=%x", aHandle, aDuration );
        CSwtMobileDevice* mobileDevice =  reinterpret_cast<CSwtMobileDevice*>(aHandle);
        vibraSupport = mobileDevice->Vibrate(static_cast<TInt>(aDuration));
        }
    SWT_CATCH
#endif
    return ( vibraSupport ? JNI_TRUE : JNI_FALSE );
    }

JNIEXPORT jint JNICALL OS_NATIVE( getScreenDeviceNumber )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass)
#else
  (JNIEnv*, jclass)
#endif
    {
    jint screenNumber = -1;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        screenNumber = SymbianUtils::GetScreenDeviceNumber();
        }
    SWT_CATCH
#endif
    return screenNumber;    
    }

JNIEXPORT jint JNICALL OS_NATIVE( getColorDepth )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass)
#else
  (JNIEnv*, jclass)
#endif
    {
    jint colorDepth = 24;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        colorDepth = SymbianUtils::GetColorDepth();
        }
    SWT_CATCH
#endif
    return colorDepth;    
    }

JNIEXPORT jint JNICALL OS_NATIVE( getHwInputs )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass)
#else
  (JNIEnv*, jclass)
#endif
    {
    jint hwInputs = 0;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        hwInputs = SymbianUtils::GetHwInputs();
        }
    SWT_CATCH
#endif
    return hwInputs;    
    }

//
// QNetworkConfigurationManager
//

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_extension_OS_QNetworkConfigurationManager_1new
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aParent)
#else
(JNIEnv*, jclass, jint)
#endif
{
    jint result = 0;
#ifdef __SYMBIAN32__    
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("parent=%x", aParent);
        HANDLE_TO_POINTER(QObject*, parent, aParent);
        result = POINTER_TO_HANDLE(new QtMobility::QNetworkConfigurationManager(parent));
        }
    SWT_CATCH
#endif
    return result;
}

JNIEXPORT jintArray JNICALL Java_org_eclipse_swt_internal_extension_OS_QNetworkConfigurationManager_1allConfigurations
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle, jint aFilter)
#else
(JNIEnv*, jclass, jint, jint)
#endif
{
    jintArray javaArray = NULL;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x filter=%x", aHandle, aFilter);

        HANDLE_TO_POINTER(QtMobility::QNetworkConfigurationManager*, manager, aHandle);
        
        QList<QtMobility::QNetworkConfiguration> configs = 
                manager->allConfigurations(static_cast<QtMobility::QNetworkConfiguration::StateFlags>(aFilter));

        int count = configs.size();
        QVector<int> handles(count);
        int* handleData = handles.data();
        for(int i = 0; i < count; ++i)
            {
            handleData[i] = reinterpret_cast<int>(new QtMobility::QNetworkConfiguration(configs.at(i)));
            }
        javaArray = swtApp->jniUtils().NewJavaIntArray(aJniEnv, handleData, count);
        }
    SWT_CATCH
#endif
    return javaArray;
}

//
// QNetworkConfiguration
//

JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_extension_OS_QNetworkConfiguration_1bearerName
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle)
#else
(JNIEnv *, jclass, jint)
#endif
{
#ifdef __SYMBIAN32__
    jstring result = NULL;
#ifdef __SYMBIAN32__    
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        QtMobility::QNetworkConfiguration* config = reinterpret_cast<QtMobility::QNetworkConfiguration*>(aHandle);
        result = swtApp->jniUtils().QStringToJavaString(aJniEnv, config->bearerName());
        }
    SWT_CATCH
#endif
    return result;
#endif
}

JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_extension_OS_QNetworkConfiguration_1delete
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle)
#else
(JNIEnv *, jclass, jint)
#endif
{
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        delete reinterpret_cast<QtMobility::QNetworkConfiguration*>(aHandle);
        }
    SWT_CATCH
#endif
}

//
// XQCallInfo
//

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_extension_OS_XQCallInfo_1create
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass)
#else
(JNIEnv *, jclass)
#endif
{
    jint result = 0;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef __WINSCW__
        result = POINTER_TO_HANDLE(XQCallInfo::create());
#endif
        }
    SWT_CATCH
#endif
    return result;
}

JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_extension_OS_XQCallInfo_1swt_1hasCalls
#ifdef __SYMBIAN32__
(JNIEnv* aJniEnv, jclass, jint aHandle)
#else
(JNIEnv *, jclass, jint)
#endif
{
    jboolean result = JNI_FALSE;
#ifdef __SYMBIAN32__
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);
        HANDLE_TO_POINTER(XQCallInfo*, callInfo, aHandle);
        QList<CallInfo> callInfos;
        callInfo->getCalls(callInfos);
        result = callInfos.empty() ? JNI_FALSE : JNI_TRUE;
        }
    SWT_CATCH
#endif
    return result;
}

//
// Other
//

JNIEXPORT jint JNICALL OS_NATIVE(EventHandler_1new)
  ( JNIEnv* aJniEnv, jclass)
    {
    EventCallback* cb = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();

        // Event handler is owned by the Java peer (Display instance)
        cb = new EventCallback();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( cb );
    }

JNIEXPORT void JNICALL OS_NATIVE(EventHandler_1destroy)
  (JNIEnv* aJniEnv , jclass, jint aHandle)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("handle=%x", aHandle);

        HANDLE_TO_POINTER( EventCallback*, cb, aHandle );
        cb->Destroy();
        }
    SWT_CATCH
    }

JNIEXPORT jint JNICALL OS_NATIVE(SignalHandler_1new)
  (JNIEnv* aJniEnv, jclass, jint aWidget, jint aSignalId )
    {
    SlotCallback* cb = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("widget=%x signalId=%d", aWidget, aSignalId);

        HANDLE_TO_POINTER( QObject*, widget, aWidget );

        // Widget takes ownership of the signal handler instance
        cb = new SlotCallback( aJniEnv, NULL, widget, aSignalId );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( cb );
    }

JNIEXPORT jint JNICALL OS_NATIVE(SignalForwarder_1new)
  (JNIEnv* aJniEnv, jclass, jint aWidget, jobject aPeer, jint aSignalId )
    {
    SlotCallback* cb = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_1("peer=%x", aPeer);

        HANDLE_TO_POINTER( QObject*, widget, aWidget );

        // Widget takes ownership of the signal handler instance
        cb = new SlotCallback( aJniEnv, aPeer, widget, aSignalId );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( cb );
    }

JNIEXPORT jint JNICALL OS_NATIVE( JniUtils_1new )
  (JNIEnv* aJniEnv, jclass)
    {
    // Note that JNI callbacks are not possible before first successfully creating jniUtils
    JniUtils* jniUtils = NULL;
    try {
        jniUtils = new JniUtils(aJniEnv);
        }
    catch(...)
        {
        // Can't throw an exception because there's no jniUtils
        }
    return POINTER_TO_HANDLE( jniUtils );
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( JniUtils_1safeToDelete )
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aQObjectHandle)
{
    bool result = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        SWT_LOG_DATA_2("handle=%x qobject=%x", aHandle, aQObjectHandle);

        HANDLE_TO_POINTER( JniUtils*, jniUtils, aHandle);
        HANDLE_TO_POINTER( QObject*, object, aQObjectHandle );

        result = jniUtils->safeToDelete(object);
        }
    SWT_CATCH
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL OS_NATIVE( windowServer )
  (JNIEnv* aJniEnv , jclass)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        }
    SWT_CATCH
#ifndef Q_WS_X11
    return 1;
#else
    return 2;
#endif
    }

JNIEXPORT jint JNICALL OS_NATIVE( initUiThread )
  (JNIEnv*
#ifdef __SYMBIAN32__
          aJniEnv
#endif
          , jclass, jint
#ifdef __SYMBIAN32__
          aUid
#endif
          )
    {
    jint retVal = 0;
#ifdef __SYMBIAN32__
    retVal = static_cast<jint>(SymbianUtils::initUiThread(aJniEnv, static_cast<TInt>(aUid)));
#endif
    return retVal;
    }

JNIEXPORT void JNICALL OS_NATIVE( cleanUpUiThread )
  (JNIEnv*, jclass)
    {
#ifdef __SYMBIAN32__
    SymbianUtils::cleanupUiThread();
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE( setSymbianAppName )
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jstring aName)
#else
  (JNIEnv*, jclass, jstring)
#endif
    {
#ifdef __SYMBIAN32__
        SymbianUtils::setAppName(aJniEnv, aName);
#endif
    }

JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_qt_s60_OS__1startUI
#ifdef __SYMBIAN32__
  (JNIEnv* aJniEnv, jclass, jobject aRunner, jint aUid)
#else
(JNIEnv*, jclass, jobject, jint)
#endif
    {
    jint retVal = -1;
#ifdef __SYMBIAN32__
    retVal = static_cast<jint>(SymbianUtils::startUI( aJniEnv, aRunner,  aUid));
#endif
    return retVal;
    }

// Add new stuff above the category "Other", that is the last category

#ifdef __cplusplus
}
#endif
