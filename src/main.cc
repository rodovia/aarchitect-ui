#include <aarchitect.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

uiWindow* mainWindow;
uiMenu* editMenuBar;
// mbi - menu bar item
uiMenuItem* mbiSettings;

static void SetupMenuBar() {
    editMenuBar = uiNewMenu("Editar");
    mbiSettings = uiMenuAppendItem(editMenuBar, "Definições");
    uiMenuItemOnClicked(mbiSettings, aarSettingsWindowLauncher, NULL);
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
    uiFormAppend(entryForm, "Palavra-passe", 
        uiControl(uiNewSearchEntry()), 0);
    
    button = uiNewButton("Iniciar Sessão");
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
        MessageBoxA(NULL, err, "aarchitect startup failure", MB_OK | MB_ICONEXCLAMATION);
#endif
        uiFreeInitError(err);
        return 1;
    }

    SetupMenuBar();
    uiOnShouldQuit(ShouldQuit, NULL);

    mainWindow = uiNewWindow("aarchitect", 640, 480, 1);
    uiWindowSetMargined(mainWindow, 1);
    uiWindowOnClosing(mainWindow, ShouldClose, NULL);

    hbox = uiNewHorizontalBox();
    uiBoxSetPadded(hbox, 1);
    uiWindowSetChild(mainWindow, uiControl(hbox));

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    uiWindowSetChild(mainWindow, uiControl(vbox));

    tab = uiNewTab();
    uiWindowSetChild(mainWindow, uiControl(tab));
    uiTabAppend(tab, "Início de sessão", MakeLoginPage());
    uiTabSetMargined(tab, 0, 1);

    uiControlShow(uiControl(mainWindow));
    uiMain();
    return 0;
}
