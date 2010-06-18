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

#include <eikenv.h>
#include <apgwgnam.h> // For CApaWindowGroupName
#include <coemain.h>
#include <eikappui.h>
#include <w32std.h>
#include <avkon.hrh>
#include <QVariant>
#include <AknDef.h>
#include <apgtask.h>
#include <hal.h>
#include <org_eclipse_swt_internal_qt_OS.h>
#include "eventcallback.h"
#include "swts60.h"
#include "autorelease.h"

using namespace Java::eSWT;

// Name of the UI thread.
_LIT(KSwtUiThreadName, "SwtQtUiThread");

// Name of the support thread.
_LIT(KSwtSupportThreadName, "SwtQtSupportThread");

// Name of the shared library
_LIT(KSwtDllName, "eswtqt.dll");

// Stack size for the UI thread, 0x14000 = 80kB
const TInt KSwtUiThreadStackSize = 0x14000;

// Stack size for the support thread, 0x1F40 = 8kB
const TInt KSwtSupportThreadStackSize = 0x1F40;

static const char* const EVENT_FILTER = "swt_event_filter";

// Data stored to thread/dll specific Symbian thread local storage of the UI 
// thread. 
typedef struct
    {
    JavaVM* vm;
    jint uid;
    jobject runner;
    TRequestStatus initStatus;
    TThreadId initThreadId;
    TThreadId uiThreadId;
    TBool doFreeTLSInCleanupUIThread;
    }
SwtTlsData;

inline static void freeTLSData(SwtTlsData*& aData)
    {
    Dll::SetTls(NULL);
    delete aData;
    aData = NULL;
    }

TUid SwtQtS60MainApplication::AppDllUid() const
    {
    SwtTlsData* data = reinterpret_cast<SwtTlsData*>(Dll::Tls());
    TUid uid;
    uid.iUid = static_cast<TInt>(data ? data->uid : 0);
    return uid;
    }

int SymbianUtils::startUI(JNIEnv* aJniEnv, jobject aRunner, jint aUid)
    {
    // Get a VM pointer using the current thread context
    JavaVM* javaVM = NULL;
    jint getVMStatus = aJniEnv->GetJavaVM(&javaVM);
    if(getVMStatus < 0) return KErrNoMemory;
    
    // Add a global reference to callback object so that in can be used from 
    // the UI thread. 
    jobject globalRef = aJniEnv->NewGlobalRef(aRunner);
    if(!globalRef) return KErrNoMemory;
    
    // Put needed data to a stucture for TLS storing that will be done later 
    // in the UI thread
    SwtTlsData* data = new (std::nothrow) SwtTlsData();
    if(!data) return KErrNoMemory;
    data->vm = javaVM;
    data->uid = aUid;
    data->runner = globalRef;
    data->initStatus = KRequestPending;
    RThread thread;
    data->initThreadId = thread.Id();
    
    // Create a new thread that will be the UI thread
    TName uiThreadName(KSwtUiThreadName);
    RThread uiThread;
    TInt createStatus = uiThread.Create( uiThreadName, SymbianUtils::uiThreadEntryPoint,
            KSwtUiThreadStackSize, NULL, reinterpret_cast<TAny*>(data) );
    data->uiThreadId = uiThread.Id();
    if(createStatus != KErrNone)
        {
        delete data;
        return createStatus;
        }
    
    // Resume the UI thread and wait until it reports back the initialization 
    // status
    uiThread.Resume();
    User::WaitForRequest(data->initStatus);
    
    // Launch the support thread
    if(data->initStatus == KErrNone)
        {
        startSupportThread(reinterpret_cast<TAny*>(data));
        }
    
    // Return the thread initialization status
    return data->initStatus.Int(); 
    }

int SymbianUtils::initUiThread(JNIEnv* aJniEnv, const TInt& aUid)
    {
    // This thread may or may not be a UI thread started by calling startUI. 
    // It's concluded that this thread was started by startUI if TLS is set by 
    // this DLL. If this is already the UI thread then not much initialization 
    // is left to do. If this isn't a UI thread then everything has to be 
    // initialized. In the latter case the stack size has already been fixed 
    // and that might cause problems. 
    TBool isUiThread = ETrue;
    SwtTlsData* data = reinterpret_cast<SwtTlsData*>(Dll::Tls());
    if(!data) isUiThread = EFalse;
    
    // If this is already initialized as a UI thread by startUI the do nothing
    // more. Otherwise continue with the initialization. 
    if(isUiThread)
        {
        return KErrNone;
        }
    
    // Create and set the TLS data structure. 
    data = new (std::nothrow) SwtTlsData();
    if(!data) return KErrNoMemory;
    data->doFreeTLSInCleanupUIThread = ETrue;
    Dll::SetTls(data);

    // This is the UI thread now, store its id
    RThread uiThread;
    data->uiThreadId = uiThread.Id();
    
    // Get a VM pointer using the current thread context
    JavaVM* javaVM = NULL;
    jint getVMStatus = aJniEnv->GetJavaVM(&javaVM);
    if(getVMStatus < 0) return KErrNoMemory;
    data->vm = javaVM;
    
    // Store the MIDlet uid we got as a parameter
    data->uid = aUid;
    
    // Launch the support thread
    startSupportThread(reinterpret_cast<TAny*>(data));

    return KErrNone;
    }

void SymbianUtils::cleanupUiThread()
    {
    SwtTlsData* data = reinterpret_cast<SwtTlsData*>(Dll::Tls());
    if(!data) return;
    if(data->doFreeTLSInCleanupUIThread)
        {
        freeTLSData(data);
        }
    }

void SymbianUtils::setAppName(JNIEnv* aJniEnv, jstring aName)
    {
    // Convert name from jstring to Symbian descriptor
    HBufC16* buffer = NULL;
    if(aName != NULL)
        {
        jboolean isCopy;
        const jchar* javaChars = aJniEnv->GetStringChars(aName, &isCopy);
        if (javaChars)
            {
            AutoReleaseStringChars cleaner(aJniEnv, aName, javaChars);
            jsize length = aJniEnv->GetStringLength(aName);
            
            TRAPD(err, buffer = HBufC16::NewL(length));
            if (err == KErrNone)
                {
                 TText16* ptr =const_cast<TText16*> (buffer->Des().Ptr());
                 memcpy(ptr, javaChars, length * sizeof(jchar));
                 buffer->Des().SetLength(length);
                }
            else
                {
                throw std::bad_alloc();
                }
            }
        else
            {
            throw std::bad_alloc();
            }
        }

    // Set the name to the window group
    CCoeEnv* coe = CCoeEnv::Static();
    CApaWindowGroupName* wgn = NULL;
    TRAPD( err, wgn = CApaWindowGroupName::NewL(coe->WsSession()));
    if(err == KErrNone)
        {
        SwtTlsData* data = reinterpret_cast<SwtTlsData*>(Dll::Tls());
        wgn->SetAppUid(TUid::Uid(static_cast<TInt>(data->uid)));
        TRAP_IGNORE(wgn->SetCaptionL(*buffer));
        wgn->SetWindowGroupName(coe->RootWin());
        delete wgn;
        }

    delete buffer;
    }

bool SymbianUtils::eventFilter(QObject* object, const TWsEvent* aEvent)
{
    int swtEventType = -1;
    switch (aEvent->Type())
    {
    case KAknShutOrHideApp: //The event is received when exit from task list, 
                            //which terminates application straight away
        swtEventType = org_eclipse_swt_internal_qt_OS_QSWTEVENT_SYSTEMSHUTDOWN;
        break;
     case EEventUser:
        if ( ( *reinterpret_cast<TApaSystemEvent*>( aEvent->EventData() ) ) == EApaSystemEventShutdown )
            {
            // other system exit (e.g. when out of memory). 
            if( !CEikonEnv::Static()->IsSystem() )
                {
                swtEventType = org_eclipse_swt_internal_qt_OS_QSWTEVENT_SYSTEMSHUTDOWN;
                }
            }
        break;
    default:
        break;
    }
    return eventFilter( object, -1, swtEventType);
}

bool SymbianUtils::eventFilter(QObject* object, const TInt aSymbianType, TInt aSwtType)
    {
    if(aSymbianType > -1)
        {
        switch (aSymbianType)
           {
           case KEikDynamicLayoutVariantSwitch:
               aSwtType = org_eclipse_swt_internal_qt_OS_QSWTEVENT_RESOURCECHANGE;
               break;
           default:
               break;
           }
        }
    
    if (aSwtType > -1)
        {
        QVariant data = object->property(EVENT_FILTER);
        if (data.isValid())
            {
            EventCallback* filter = reinterpret_cast<EventCallback*> (data.toInt());
            if (filter)
                {
                return filter->eventFilter(object, 0, aSwtType);
                }
            }
        }
    return false;
    }

TInt SymbianUtils::GetScreenDeviceNumber()
    {
    return CCoeEnv::Static()->ScreenDevice()->GetScreenNumber();
    }

TInt SymbianUtils::GetColorDepth()
    {
    return TDisplayModeUtils::NumDisplayModeBitsPerPixel(
            CCoeEnv::Static()->ScreenDevice()->DisplayMode());
    }

TInt SymbianUtils::GetHwInputs()
    {
    TInt mask;
    HAL::Get(HALData::EKeyboard, mask);
    return mask;
    }
	
CApaApplication* SymbianUtils::NewApplication()
    {
    return new SwtQtS60MainApplication;
    }

void SymbianUtils::notifyThreadInitStatus(const TInt& aStatus, 
        TThreadId aInitThreadId, TRequestStatus* aStatusPtr)
    {
    RThread initThread;
    TInt openStatus = initThread.Open(aInitThreadId);
    __ASSERT_DEBUG(openStatus == KErrNone, User::Panic(KSwtDllName, 0));
    if(openStatus == KErrNone)
        {
        initThread.RequestComplete(aStatusPtr, aStatus);
        initThread.Close();
        }
    }

TInt SymbianUtils::uiThreadEntryPoint(TAny* aTlsData)
    {
    // Take ownership the the TLS data
    SwtTlsData* data = reinterpret_cast<SwtTlsData*>(aTlsData);
    Dll::SetTls(aTlsData);
    data->doFreeTLSInCleanupUIThread = EFalse;

    // Set the thread as process critical so that the entire process will die 
    // if the UI thread panics. 
    User::SetCritical(User::EProcessCritical);
    
    // Create a CleanupStack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(!cleanup)
        {
        notifyThreadInitStatus(KErrNoMemory, data->initThreadId, &data->initStatus);
        freeTLSData(data);
        return KErrNoMemory;
        }
    
    // Attach this thread to the VM to get the JNIEnv pointer. 
    JNIEnv* env = NULL;
    void* args = NULL;
    jint attachStatus = data->vm->AttachCurrentThread((void**)&env, args);    
    if(attachStatus < 0)
        {
        notifyThreadInitStatus(KErrNoMemory, data->initThreadId, &data->initStatus);
        freeTLSData(data);
        delete cleanup;
        cleanup = NULL;
        return KErrNoMemory;
        }
    
    // Obtain methodID of run() of the Java callback object
    jclass runnerClass = env->GetObjectClass(data->runner);
    jmethodID mid = NULL;
    if(runnerClass) mid = env->GetMethodID(runnerClass, "run", "()V");
    
    // Check if something failed
    if(!mid)
        {
        notifyThreadInitStatus(KErrNoMemory, data->initThreadId, &data->initStatus);
        
        TRAP_IGNORE(data->vm->DetachCurrentThread());
        freeTLSData(data);
        delete cleanup;
        cleanup = NULL;
        
        return KErrNoMemory;
        }
    
    // Notify the waiting thread that initialization has completed successfully
    notifyThreadInitStatus(KErrNone, data->initThreadId, &data->initStatus);

    // Call run() of the Java callback object. Inside this call the UI event
    // loop will be executed. 
    TRAPD(err, env->CallVoidMethod(data->runner, mid));
    if(err != KErrNone)
        {
        // Something did leave. All Qt APIs are trapped so it might be a Java 
        // class library that has failed. This is a fatal error and the process
        // should die.
        User::Panic(KSwtUiThreadName, 0);
        }
    
    // The application allowed the UI thread to exit. Clean-up and die. 
        
    // Remove the reference to the runner Java object
    TRAP_IGNORE(env->DeleteGlobalRef(data->runner));
    data->runner = NULL;
    
    // Detach the UI thread from the VM
    TRAP_IGNORE(data->vm->DetachCurrentThread());
    freeTLSData(data);
    delete cleanup;
    cleanup = NULL;
    
    return KErrNone;
    }

/*
 * Don't trust the JNI implementation to trap everything properly but let's 
 * always have a top-level trap also in this thread to avoid panics such as
 * EUSER-CBase 66/69.  
 */
TInt SymbianUtils::supportThreadEntryPoint(TAny* aParams)
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    int retVal = 0;
    TRAP_IGNORE(retVal = trappedSupportThreadEntryPoint(aParams));
    delete cleanup;
    cleanup = NULL;
    return retVal;
    }

TInt SymbianUtils::trappedSupportThreadEntryPoint(TAny* aParams)
    {
    // Prevent the library from getting detached when the VM closes its handle. 
    // That would lead in the destruction of the Qt's global statics in a different
    // thread than they were created in causing problems. Keep a handle to the 
    // library in a thread until the process terminates. 
    RLibrary libRef;
#ifdef _DEBUG
    TInt addLibRef = 
#endif
    libRef.Load(KSwtDllName);
    __ASSERT_DEBUG(addLibRef == KErrNone, User::Panic(KSwtDllName, 0));

    // Store JavaVM pointer and UI thread id from the thread params. 
    SwtTlsData* data = reinterpret_cast<SwtTlsData*>(aParams);
    TThreadId uiThreadId = data->uiThreadId; 
    JavaVM* vm = data->vm;
    
    // Try attach using the JavaVM pointer. At this point there's a Java thread
    // waiting ensuring that the VM has not closed down and the JavaVM pointer
    // must be valid. 
    JNIEnv* env = NULL;
    void* args = NULL;
    jint attachStatus = vm->AttachCurrentThread((void**)&env, args);
    __ASSERT_DEBUG(attachStatus == 0, User::Panic(KSwtDllName, 0));

    // Notify the waiting Java thread that we have attached and it can continue
    notifyThreadInitStatus(attachStatus, data->initThreadId, &data->initStatus);
    
    // Create a rendezvous request to detect if the UI thread dies. 
    RThread uiThread;
    TInt openStatus = uiThread.Open(uiThreadId);
    __ASSERT_DEBUG(openStatus == KErrNone, User::Panic(KSwtDllName, 0));
    TRequestStatus uiThreadRendezvous;
    if(openStatus == KErrNone)
        {
        uiThread.Rendezvous(uiThreadRendezvous);
        }

    // Wait until the process dies. If the UI thread dies notify MIDP 
    // application management software that the MIDlet should die. 
    // This is a workaround until QTBUG-5284 is resolved.
    while(ETrue)
        {
        User::WaitForAnyRequest();
        if(vm && openStatus == KErrNone) {
            if(uiThread.ExitType() != EExitPending)
                {
                // Notify once and detach the thread. 
                notifyExitCmd(env);
                vm->DetachCurrentThread();
                env = NULL;
                vm = NULL;
                }
            }
        }
    return KErrNone;
    }

void SymbianUtils::startSupportThread(TAny* aParams)
    {
    // Set the thread id that the support thread will notify when it has 
    // attached to the VM. 
    SwtTlsData* data = reinterpret_cast<SwtTlsData*>(aParams);
    RThread initThread;
    data->initThreadId = initThread.Id();
    data->initStatus = KRequestPending;

    // Launch the support thread
    TName supportThreadName(KSwtSupportThreadName);
    RThread supportThread;
    TInt createStatus = supportThread.Create(supportThreadName, SymbianUtils::supportThreadEntryPoint,
            KSwtSupportThreadStackSize, NULL, aParams);
    
    // If the application disposed the Display and recreated it in the same 
    // thread then support thread already exists. 
    if(createStatus == KErrAlreadyExists) return;
    
    if(createStatus == KErrNone) supportThread.Resume();
    __ASSERT_DEBUG(createStatus == KErrNone, User::Panic(KSwtSupportThreadName, 0));
    
    // Wait until the support thread is attached to the VM to ensure that 
    // the VM doesn't have a chance to unload and invalidate the VM pointer. 
    User::WaitForRequest(data->initStatus);
    }

void SymbianUtils::notifyExitCmd(JNIEnv* aEnv)
    {
    if(!aEnv) return;
    
    jclass clazz = aEnv->FindClass("org/eclipse/swt/internal/ExitNotificationWrapper");
    __ASSERT_DEBUG(clazz, User::Panic(KSwtUiThreadName, 0));
    if(clazz)
        {
        jmethodID id = aEnv->GetStaticMethodID(clazz, "notifyExit", "()V");
        __ASSERT_DEBUG(id, User::Panic(KSwtUiThreadName, 0));
        if(id)
            {
            aEnv->CallStaticVoidMethod(clazz, id);
            }
        aEnv->DeleteLocalRef(clazz);
        }
    }
