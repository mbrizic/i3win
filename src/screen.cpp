﻿#include <iostream>
#include <windows.h>
#include <set>
#include "window.h"
#include "screen.h"
#include "logger.h"

void Screen::initialize(LayoutType layoutType, int screenWidth, int screenHeight) {
    this->layoutType = layoutType;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->config = Config();
}

int counter = 0;

void Screen::addWindow(Window window) {
    bool doesWindowAlreadyExist = windowToPositionMap.find(window.hwnd) != windowToPositionMap.end();

    if (doesWindowAlreadyExist) {
        /*std::cout << "Exists:" << std::endl;*/
    } else {
        int currentPosition = windowToPositionMap.size();
        windowToPositionMap[window.hwnd] = currentPosition;
        positionToWindowMap[currentPosition] = window.hwnd;
    }

    windows[window.hwnd] = window;

    counter++;
}

// After a window is removed, few things happen:
// - focused window might be removed, so we need to set it to another one
// - leftover indexes remain, so we need to remove them
void Screen::normalizeScreenState() {
    bool isFocusedWindowKnown = windows.find(focusedWindow.hwnd) != windows.end();

    if (!isFocusedWindowKnown) {
        logInfo("Focused window unknown, setting it to the first available one.");
        int previouslyFocusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];
        focusedWindow = getWindowAtPosition(previouslyFocusedWindowIndex - 1);
        setFocusedWindow(focusedWindow);
    }

    std::set<std::pair<int, HWND>> itemsToRemove;

    for (auto item : positionToWindowMap) {
        int position = item.first;
        HWND hwnd = item.second;

        bool doesWindowStillExist = windows.find(hwnd) != windows.end();

        if (!doesWindowStillExist) {
            itemsToRemove.insert(item);
        }
    }

    for (auto item : itemsToRemove) {
        logInfo("Screen cleanup, removing " + item.first);

        positionToWindowMap.erase(item.first);
        windowToPositionMap.erase(item.second);
    }
}

void Screen::reset() {
    windows.clear();
}

void Screen::setFocusedWindow(Window window) {
    if (window.hwnd != NULL) {
        focusedWindow = window;
        SetForegroundWindow(focusedWindow.hwnd); // TODO: Maybe move this out of this struct?
    }
    else {
        logError("Can't set focused window as it's null");
    }
}

void Screen::setActiveLayout(LayoutType layout) {
    layoutType = layout;
}

void Screen::moveFocusLeft(){
    normalizeScreenState();

    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindow(
            getWindowAtPosition(0)
        );
    }

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex > 0) {
        setFocusedWindow(
            getWindowAtPosition(focusedWindowIndex - 1)
        );
    } else {
        logInfo("Can't go further left.");
    }
}

void Screen::moveFocusRight(){
    normalizeScreenState();

    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindow(
            getWindowAtPosition(0)
        );
    }

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex < windows.size() - 1) {
        setFocusedWindow(
            getWindowAtPosition(focusedWindowIndex + 1)
        );
    } else {
        logInfo("Can't go further right.");
    }
}

Window Screen::getWindowAtPosition(int position) {
    HWND hwnd = positionToWindowMap[position];

    return windows[hwnd];
}

void Screen::moveFocusedWindowRight() {
    normalizeScreenState();

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex == windows.size() - 1) {
        logInfo("Can't move to right, already at the edge");
        return;
    } else {
        positionToWindowMap[focusedWindowIndex] = positionToWindowMap[focusedWindowIndex + 1];
        positionToWindowMap[focusedWindowIndex + 1] = focusedWindow.hwnd;

        windowToPositionMap[focusedWindow.hwnd] = focusedWindowIndex + 1;
        windowToPositionMap[positionToWindowMap[focusedWindowIndex]] = focusedWindowIndex;
    }
}

void Screen::moveFocusedWindowLeft() {
    normalizeScreenState();

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex < 1) {
        logInfo("Can't move to left, already at the edge");
        return;
    } else {
        positionToWindowMap[focusedWindowIndex] = positionToWindowMap[focusedWindowIndex - 1];
        positionToWindowMap[focusedWindowIndex - 1] = focusedWindow.hwnd;

        windowToPositionMap[focusedWindow.hwnd] = focusedWindowIndex - 1;
        windowToPositionMap[positionToWindowMap[focusedWindowIndex]] = focusedWindowIndex;
    } 
}