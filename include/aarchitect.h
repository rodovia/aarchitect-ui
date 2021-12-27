#pragma once

#include <ui.h>

// Settings window
void aarSettingsWindowLauncher(uiMenuItem* sender, uiWindow* window, void* data);
void aarMakeSettingsWindow(uiWindow* window);

// General window utilities
int ShouldClose(uiWindow* window, void *data);
int ShouldQuit(void *data);
