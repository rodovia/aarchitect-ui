#define _ITERATOR_DEBUG_LEVEL 0
#include <aarchitect.h>
#include <aarchitect/chat.h>
#include <aarchitect/pluginhlpapi.hxx>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <ui_windows.h>
#endif

uiWindow* mainWindow;
uiMenu* editMenuBar;
// mbi - menu bar item
uiMenuItem* mbiSettings;
uiTab* gTab;
uv_thread_t connectionThread;

uiWindow* GetMainWindow()
{
    return mainWindow;
}

static void ConnectWrapper(uiButton* button, void* data);

void AAR_CALLCONV aarMessageBox(const wchar_t* message, const wchar_t* caption, int flags) {
#ifdef _WIN32
    int wFlags = 0;
    switch(flags) {
    case aarMessageBoxSeverity_Error:
        wFlags = MB_ICONEXCLAMATION | MB_OK;
        break;
    case aarMessageBoxSeverity_Warning:
        wFlags = MB_ICONWARNING | MB_OK;
        break;
    case aarMessageBoxSeverity_Ok:
        wFlags = MB_ICONASTERISK| MB_OK;
    }
    MessageBoxW(NULL, message, caption, wFlags);
#else
    printf("%s: %s", caption, message);
#endif
}

static void SetupMenuBar() {
    editMenuBar = uiNewMenu("Editar");
    mbiSettings = uiMenuAppendItem(editMenuBar, "Definições");
    uiMenuItemOnClicked(mbiSettings, aarSettingsWindowLauncher, NULL);
    uiMenuItemEnable(mbiSettings);
}

static uiControl* MakeLoginPage() {
    uiForm* entryForm;
    uiButton* button;

    uiBox* vbox = uiNewVerticalBox();

    entryForm = uiNewForm();
    uiFormSetPadded(entryForm, 1);
    uiBoxAppend(vbox, uiControl(entryForm), 1);

    uiFormAppend(entryForm, "Nome de utilizador", 
        uiControl(uiNewEntry()), 0);
    uiFormAppend(entryForm, "Servidor de host",
        uiControl(uiNewEntry()), 0);
    
    button = uiNewButton("Iniciar Sessão");
    uiButtonOnClicked(button, ConnectWrapper, NULL);
    uiBoxAppend(vbox, uiControl(button), 0);

    return uiControl(vbox);
}

int ShouldClose(uiWindow* window, void *data) {
    uiControlDestroy(uiControl(mainWindow));
    return 0;
}

int ShouldQuit(void *data) {
    uiQuit();
    return 0;
}

static void ConnectWrapper(uiButton* button, void* data)
{
    aarPrettyLog("Establishing connection..");

    aarUserConfiguration* cfg = static_cast<aarUserConfiguration*>(malloc(sizeof(aarUserConfiguration)));
    aarGetSettings(cfg);
    connectionThread = aarConnectClient(cfg);
    aarAboutToConnectWindowSetup(gTab);
}

int main() {
    uiInitOptions opt;
    const char *err;
    uiBox *hbox, *vbox;
    uiTab* tab;

    memset(&opt, 0, sizeof(uiInitOptions));
    err = uiInit(&opt);
    if (err != NULL) {
#ifndef _WIN32
        fprintf(stderr, "uiInit: %s", err);
#else
        const wchar_t* werr = EnwidenString(err);
        aarMessageBox(werr, L"Erro no arranque do aarchitect", aarMessageBoxSeverity_Error);
        /* for some reason free() signature is wrong. */
        free((wchar_t*) werr);
#endif
        uiFreeInitError(err);
        return 1;
    }

    SetupMenuBar();
    uiOnShouldQuit(ShouldQuit, NULL);

    mainWindow = uiNewWindow("aarchitect", 640, 480, 1);
    uiWindowSetMargined(mainWindow, 1);
    uiWindowOnClosing(mainWindow, ShouldClose, NULL);

#   ifdef _WIN32
    HINSTANCE lpModule;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, NULL, &lpModule);
    HICON hIcon = LoadIcon(lpModule, "IDI_ICON1");
    uiWindowsSetIcon(mainWindow, hIcon);
#   endif
    
    aarLoadAllPlugins("_clientplugins");

    gTab = uiNewTab();
    uiTabAppend(gTab, "Início de sessão", MakeLoginPage());
    uiTabSetMargined(gTab, 0, 1);

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    uiBoxAppend(vbox, uiControl(gTab), 1);
    uiWindowSetChild(mainWindow, uiControl(vbox));
    
    uiControlShow(uiControl(mainWindow));
    uiMain();
    return 0;
}
