#include <aarchitect.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __WINDOWS__
#include <windows.h>
#endif

typedef struct {
    uiEntry* serverHost;
    uiEntry* serverPort;
} ConfigurationEntries;

int SettingsShouldClose(uiWindow* window, void *data) {
    uiControlDestroy(uiControl(window));
    return 0;
}

void SaveButtonClicked(uiButton* button, void* confev) {
    ConfigurationEntries confe = *(ConfigurationEntries*)confev;
    aarPrettyLog("entries->serverPort = %p", confe.serverPort);
    char* serhost = uiEntryText(confe.serverHost);
    char* serport;

    // Crash handling
#ifdef __WINDOWS__
    __try {
        serport = uiEntryText(confe.serverPort);
    } __except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        serport = "9192";
    }
#else
    serport = uiEntryText(confe.serverPort);
#endif
    
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
}

void aarSettingsWindowLauncher(uiMenuItem* sender, uiWindow* senderWindow, void* data) {
    uiWindow* window = uiNewWindow("Definições", 640, 480, 0);
    uiWindowOnClosing(window, SettingsShouldClose, NULL);
    int status = aarMakeSettingsWindow(window);
    if (status == 0) {
        return uiControlShow(uiControl(window));
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
        aarMessageBox(L"O ficheiro de definicoes nao existe ou e inválido le-lo.", L"Definições", aarMessageBoxSeverity_Warning);
        return aarRequest_DESTROY_WINDOW;
    }

    aarPrettyLog("Current host: %s", conf->server.host);
    aarPrettyLog("Current port: %lf", conf->server.port);
    uiEntry* serverHostEntry = uiNewEntry();
    uiEntry* portHostEntry = uiNewEntry();
    ConfigurationEntries *entries = calloc(1, sizeof(ConfigurationEntries));
    
    entries->serverHost = serverHostEntry;
    entries->serverPort = portHostEntry;
    aarPrettyLog("entries->serverPort = %p", entries->serverPort);

    saveButton = uiNewButton("Guardar");
    uiButtonOnClicked(saveButton, SaveButtonClicked, (void *) &entries);

    if (strlen(conf->server.host) > 0)
        uiEntrySetText(serverHostEntry, conf->server.host);
    uiEntrySetText(portHostEntry, "0");

    form = uiNewForm();
    uiFormSetPadded(form, 1);
    
    uiFormAppend(form, "Servidor de destino", uiControl(serverHostEntry), 1);
    uiFormAppend(form, "Porta do servidor de destino", uiControl(portHostEntry), 0);
    
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
