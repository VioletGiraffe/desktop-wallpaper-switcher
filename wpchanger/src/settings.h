#ifndef SETTINGS_H
#define SETTINGS_H

// Interval between switching wallpapers
#define SETTINGS_INTERVAL "Interval"
#define SETTINGS_DEFAULT_INTERVAL 3600 // Seconds

// Determines whether to switch WPs in order or randomly
#define SETTINGS_RANDOMIZE "Randomize"
#define SETTINGS_DEFAULT_RANDOMIZE true

// Start switching when the program starts
#define SETTINGS_START_SWITCHING_ON_STARTUP "AutostartSwitching"
#define SETTINGS_DEFAULT_AUTOSTART true

// Path to the active image list file
#define SETTINGS_IMAGE_LIST_FILE "ActiveImageList"

// Previous state
#define SETTINGS_WINDOW_GEOMETRY   "WindowGeometry"
#define SETTINGS_WINDOW_STATE      "WindowState"
#define SETTINGS_LIST_GEOMETRY     "ListGeometry"
#define SETTINGS_LIST_STATE        "ListState"
#define SETTINGS_CURRENT_WALLPAPER "CurrentWallpaperIndex"
#define SETTINGS_TIME_TO_SWITCH    "TimeToSwitch"

#endif // SETTINGS_H
