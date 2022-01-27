#include <aarchitect.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

uiEntry* serverHost;
uiEntry* serverPort;

int SettingsShouldClose(uiWindow* window, void *data) {
    uiControlDestroy(uiControl(window));
    return 0;
}

void SaveButtonClicked(uiButton* button, void* data) {
    char* serhost = uiEntryText(serverHost);
    char* serport = uiEntryText(serverPort);
    
    uiEntrySetReadOnly(serverHost, 1);
    uiEntrySetReadOnly(serverPort, 1);

    int port;
    int dig = 1;
    for (int i = 0; i <= strlen(serport) - 1; i++) {
        if (!isdigit(serport[i])) {
            aarPrettyLog("Port does not contains only numbers, using 9192");
            port = 9192;
            dig = 0;
            break;
        }
    }

    aarPrettyLog("%s", serport);

    if (dig != 0) {
        port = atoi(serport);
    }
    
    aarUserConfiguration* confu = malloc(sizeof(aarUserConfiguration));
    aarGetSettings(confu);

    confu->server.host = serhost;
    confu->server.port = (double) port;
    aarWriteSettings(confu);

    free(confu);
    uiEntrySetReadOnly(serverHost, 0);
    uiEntrySetReadOnly(serverPort, 0);

    aarMessageBox(L"Definições guardadas com êxito", L"Definições", aarMessageBoxSeverity_Ok);
}

void aarSettingsWindowLauncher(uiMenuItem* sender, uiWindow* senderWindow, void* data) {
    uiWindow* window = uiNewWindow("Definições", 640, 480, 0);
    uiWindowOnClosing(window, SettingsShouldClose, NULL);
    int status = aarMakeSettingsWindow(window);
    if (status == 0) {
        uiControlShow(uiControl(window));
        return;
    }

    uiControlDestroy(uiControl(window));
}

int aarMakeSettingsWindow(uiWindow* window) {
    uiGroup* connectionGroup;
    uiBox* vbox;
    uiBox* hbox;
    uiForm* form;
    uiButton* saveButton;
    int status;
    
    aarUserConfiguration* conf = malloc(sizeof(aarUserConfiguration));
    status = aarGetSettings(conf);
    if (status == aarError_FERROR) {
        aarMessageBox(L"O ficheiro de definições não existe ou não é válido.", L"Erro", aarMessageBoxSeverity_Warning);
        return aarRequest_DESTROY_WINDOW;
    }

    aarPrettyLog("Current host: %s", conf->server.host);
    aarPrettyLog("Current port: %zd", conf->server.port);
    serverHost = uiNewEntry();
    serverPort = uiNewEntry();

    saveButton = uiNewButton("Guardar");
    uiButtonOnClicked(saveButton, SaveButtonClicked, NULL);

    if (strlen(conf->server.host) > 0)
        uiEntrySetText(serverHost, conf->server.host);
    uiEntrySetText(serverPort, "0");

    form = uiNewForm();
    uiFormSetPadded(form, 1);
    
    uiFormAppend(form, "Servidor de destino", uiControl(serverHost), 1);
    uiFormAppend(form, "Porta do servidor de destino", uiControl(serverPort), 0);
    
    vbox = uiNewVerticalBox();
    uiBoxAppend(vbox, uiControl(form), 0);

    hbox = uiNewHorizontalBox();
    uiBoxSetPadded(hbox, 1);
    uiBoxAppend(hbox, uiControl(saveButton), 0);
    uiBoxAppend(vbox, uiControl(hbox), 0);

    connectionGroup = uiNewGroup("Ligação");
    uiGroupSetMargined(connectionGroup, 1);
    uiGroupSetChild(connectionGroup, uiControl(vbox));

    uiWindowSetChild(window, uiControl(connectionGroup));

    return 0;
}
