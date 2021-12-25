#include <ui.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

uiWindow* mainWindow;

static uiControl* MakeLoginPage() {
    uiForm* entryForm;
    uiBox* vbox = uiNewVerticalBox();
    
    entryForm = uiNewForm();
    uiFormSetPadded(entryForm, 1);
    uiBoxAppend(vbox, uiControl(entryForm), 1);

    uiFormAppend(entryForm, "Nome de utilizador", 
        uiControl(uiNewEntry()), 0);
    uiFormAppend(entryForm, "Palavra-passe", 
        uiControl(uiNewSearchEntry()), 0);

    return uiControl(vbox);
}

static int ShouldClose(uiWindow* window, void *data) {
    uiControlDestroy(uiControl(window));
    uiQuit();
    return 1;
}

static int ShouldQuit(void *data) {
    uiControlDestroy(uiControl(mainWindow));
    return 1;
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
        fprintf("uiInit: %s", err);
#else
        MessageBoxA(NULL, err, "aarchitect startup failure", MB_OK | MB_ICONEXCLAMATION);
#endif
        uiFreeInitError(err);
        return 1;
    }

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
    // uiUninit();
    return 0;
}
