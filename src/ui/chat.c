#include <aarchitect.h>
#include <aarchitect/chat.h>
#include <ui.h>

static int EndsWith(const char* str, const char* suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuff = strlen(suffix);
    if (lensuff > lenstr)
        return 0;

    return strncmp(str + lenstr - lensuff, suffix, lensuff) == 0;
}

static void AutoSendCallback(uiEntry* entry, void* data)
{
    char* eText = uiEntryText(entry);
    if (EndsWith(eText, "\r"))
    {
        aarSendChatMessage(eText);
    }
}

static void SendChatMessageWrapper(uiButton* button, void* data)
{
    char* eText = uiEntryText((uiEntry*) data);
    aarSendChatMessage(eText);    
}

static uiControl* CreateChatWindow()
{
    uiMultilineEntry* chatHistory;
    uiEntry* messageEntry;
    uiButton* smButton;
    uiButton* quitButton;

    uiBox* vbox = uiNewVerticalBox();
    uiBox* subVbox = uiNewVerticalBox();

    uiBox* hbox = uiNewHorizontalBox();
    uiBoxSetPadded(vbox, 1);
    uiBoxSetPadded(hbox, 1);

    chatHistory = uiNewMultilineEntry();
    uiMultilineEntrySetReadOnly(chatHistory, 1);
    uiBoxAppend(vbox, uiControl(chatHistory), 1);

    quitButton = uiNewButton("Sair");
    uiBoxAppend(subVbox, uiControl(quitButton), 0);

    messageEntry = uiNewEntry();
    uiEntryOnChanged(messageEntry, AutoSendCallback, NULL);
    uiBoxAppend(hbox, uiControl(messageEntry), 0);

    smButton = uiNewButton("Enviar");
    uiButtonOnClicked(smButton, SendChatMessageWrapper, messageEntry);
    uiBoxAppend(hbox, uiControl(smButton), 0);
    uiBoxAppend(subVbox, uiControl(hbox), 1);
    uiBoxAppend(vbox, uiControl(subVbox), 0);
    
    return uiControl(vbox);
}

int aarAboutToConnectWindowSetup(uiTab* group)
{
    uiControl* chWnd = CreateChatWindow();
    uiTabAppend(group, "Servidor", uiControl(chWnd));
}