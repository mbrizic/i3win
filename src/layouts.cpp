﻿#include "layouts.h"
#include "screen.h"
#include "window.h"
#include "logger.h"

#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <vector>
#include <iostream>

const int padding = 50;
const int taskbarSize = 50;

void buildStackedLayout(Screen screen) {
    int windowCount = 0;

    for (auto window : screen.windows) {
        int spacing = padding * windowCount;

        bool isWindowMovedSuccessfully = window.move(
            0,
            spacing,
            screen.screenWidth,
            screen.screenHeight - spacing);

        if (isWindowMovedSuccessfully) {
            windowCount++;
        }
    }
}

void buildSplitLayout(Screen screen) {
    int windowsCount = screen.windows.size();

    int windowWidth = screen.screenWidth / windowsCount;

    int windowCount = 0;

    for (auto window : screen.windows) {
        bool isWindowMovedSuccessfully = window.move(
            windowCount * windowWidth,
            0,
            windowWidth,
            screen.screenHeight - taskbarSize);

        if (isWindowMovedSuccessfully) {
            windowCount++;
        }
    }
}

void buildCenteredLayout(Screen screen) {
    int windowsCount = screen.windows.size();

    int centerWindowWidth = screen.screenWidth * screen.config.getNumericConfigValue("CENTER_LAYOUT_MAIN_WINDOW_WIDTH_PERCENTAGE") / 100;

    int sideWindowWidth = (screen.screenWidth - centerWindowWidth) / 2;
    int sideWindowHeight = screen.screenHeight / 2;

    int windowCount = 0;

    for (auto window : screen.windows) {

        int xPosition, yPosition, width, height;

        if (windowCount == 0) {
            window.move(
                sideWindowWidth,
                0,
                centerWindowWidth,
                screen.screenHeight - taskbarSize);
        } else if (windowCount < 5) {
            int yPosition = windowCount % 2 == 0 
                ? sideWindowHeight 
                : 0;

            bool isLeftSideWindow = windowCount < 3;

            int xPosition = isLeftSideWindow
                ? 0 
                : sideWindowWidth + centerWindowWidth;

            window.move(
                xPosition,
                yPosition,
                sideWindowWidth,
                sideWindowHeight);
        } else {
            logError("This layout only supports up to 5 windows!");
        }

        windowCount++;
    }
}

void buildLayout(Screen screen) {
    switch (screen.layoutType)
    {
    case LAYOUT_TYPE_NONE:
        break;
    case LAYOUT_TYPE_STACKED:
        buildStackedLayout(screen);
        break;
    case LAYOUT_TYPE_SPLIT:
        buildSplitLayout(screen);
        break;
    case LAYOUT_TYPE_CENTERED:
        buildCenteredLayout(screen);
        break;
    default:
        logError("Unknown layout type, doing nothing.");
        break;
    }
}