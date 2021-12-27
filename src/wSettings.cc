#include <aarchitect.h>
#include <cstdio>

int SettingsShouldClose(uiWindow* window, void *data) {
    uiControlDestroy(uiControl(window));
    return 0;
}

void aarSettingsWindowLauncher(uiMenuItem* sender, uiWindow* senderWindow, void* data) {
    uiWindow* window = uiNewWindow("Definições", 640, 480, 0);
    uiWindowOnClosing(window, SettingsShouldClose, NULL);
    aarMakeSettingsWindow(window);
    uiControlShow(uiControl(window));
}

void aarMakeSettingsWindow(uiWindow* window) {
    uiGroup* connectionGroup;
    uiBox* vbox;
    uiForm* form;

    form = uiNewForm();
    uiFormSetPadded(form, 1);
    uiFormAppend(form, "Servidor de destino", uiControl(uiNewEntry()), 1);

    vbox = uiNewVerticalBox();
    uiBoxAppend(vbox, uiControl(form), 0);

    connectionGroup = uiNewGroup("Conexão");
    uiGroupSetMargined(connectionGroup, 1);
    uiGroupSetChild(connectionGroup, uiControl(vbox));

    uiWindowSetChild(window, uiControl(connectionGroup));
}
