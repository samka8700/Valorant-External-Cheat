// dear py, v1.85
// (demo code)

// Help:
// - Read FAQ at http://dearpy.org/faq
// - Newcomers, read 'Programmer guide' in py.cpp for notes on how to setup Dear py in your codebase.
// - Call and read py::ShowDemoWindow() in py_demo.cpp. All applications in examples/ are doing that.
// Read py.cpp for more details, documentation and comments.
// Get the latest version at https://github.com/ocornut/py

// Message to the person tempted to delete this file when integrating Dear py into their codebase:
// Do NOT remove this file from your project! Think again! It is the most useful reference code that you and other
// coders will want to refer to and call. Have the py::ShowDemoWindow() function wired in an always-available
// debug menu of your game/app! Removing this file from your project is hindering access to documentation for everyone
// in your team, likely leading you to poorer usage of the library.
// Everything in this file will be stripped out by the linker if you don't call py::ShowDemoWindow().
// If you want to link core Dear py in your shipped builds but want a thorough guarantee that the demo will not be
// linked, you can setup your imconfig.h with #define py_DISABLE_DEMO_WINDOWS and those functions will be empty.
// In another situation, whenever you have Dear py available you probably want this to be available for reference.
// Thank you,
// -Your beloved friend, py_demo.cpp (which you won't delete)

// Message to beginner C/C++ programmers about the meaning of the 'static' keyword:
// In this demo code, we frequently use 'static' variables inside functions. A static variable persists across calls,
// so it is essentially like a global variable but declared inside the scope of the function. We do this as a way to
// gather code and data in the same place, to make the demo source code faster to read, faster to write, and smaller
// in size. It also happens to be a convenient way of storing simple UI related information as long as your function
// doesn't need to be reentrant or used in multiple threads. This might be a pattern you will want to use in your code,
// but most of the real data you would be editing is likely going to be stored outside your functions.

// The Demo code in this file is designed to be easy to copy-and-paste into your application!
// Because of this:
// - We never omit the py:: prefix when calling functions, even though most code here is in the same namespace.
// - We try to declare static variables in the local scope, as close as possible to the code using them.
// - We never use any of the helpers/facilities used internally by Dear py, unless available in the public API.
// - We never use maths operators on ImVec2/ImVec4. For our other sources files we use them, and they are provided
//   by py_internal.h using the py_DEFINE_MATH_OPERATORS define. For your own sources file they are optional
//   and require you either enable those, either provide your own via IM_VEC2_CLASS_EXTRA in imconfig.h.
//   Because we can't assume anything about your support of maths operators, we cannot use them in py_demo.cpp.

// Navigating this file:
// - In Visual Studio IDE: CTRL+comma ("Edit.NavigateTo") can follow symbols in comments, whereas CTRL+F12 ("Edit.GoToImplementation") cannot.
// - With Visual Assist installed: ALT+G ("VAssistX.GoToImplementation") can also follow symbols in comments.

/*

Index of this file:

// [SECTION] Forward Declarations, Helpers
// [SECTION] Demo Window / ShowDemoWindow()
// - sub section: ShowDemoWindowWidgets()
// - sub section: ShowDemoWindowLayout()
// - sub section: ShowDemoWindowPopups()
// - sub section: ShowDemoWindowTables()
// - sub section: ShowDemoWindowMisc()
// [SECTION] About Window / ShowAboutWindow()
// [SECTION] Style Editor / ShowStyleEditor()
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
// [SECTION] Example App: Simple overlay / ShowExampleAppSimpleOverlay()
// [SECTION] Example App: Fullscreen window / ShowExampleAppFullscreen()
// [SECTION] Example App: Manipulating window titles / ShowExampleAppWindowTitles()
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "py.h"
#ifndef py_DISABLE

// System includes
#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow : Using operator 'xxx' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator 'xxx' to avoid overflow(io.2).
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'                     // not all warnings are known by all Clang versions and they tend to be rename-happy.. so ignoring warnings triggers new warnings on some configuration. Great!
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast                           // yes, they are more terse.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"        // warning: 'xx' is deprecated: The POSIX name for this..   // for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"       // warning: cast to 'void *' from smaller integer type
#pragma clang diagnostic ignored "-Wformat-security"                // warning: format string is not a string literal
#pragma clang diagnostic ignored "-Wexit-time-destructors"          // warning: declaration requires an exit-time destructor    // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. py coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wunused-macros"                  // warning: macro is not used                               // we define snprintf/vsnprintf on Windows so they are available, but not always used.
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                   // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wdouble-promotion"               // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#pragma clang diagnostic ignored "-Wreserved-id-macro"              // warning: macro name is a reserved identifier
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"          // warning: format string is not a string literal (potentially insecure)
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wmisleading-indentation"   // [__GNUC__ >= 6] warning: this 'if' clause does not guard this statement      // GCC 6.0+ only. See #883 on GitHub.
#endif

// Play it nice with Windows users (Update: May 2018, Notepad now supports Unix-style carriage returns!)
#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

// Helpers
#if defined(_MSC_VER) && !defined(snprintf)
#define snprintf    _snprintf
#endif
#if defined(_MSC_VER) && !defined(vsnprintf)
#define vsnprintf   _vsnprintf
#endif

// Format specifiers, printing 64-bit hasn't been decently standardized...
// In a real application you should be using PRId64 and PRIu64 from <inttypes.h> (non-windows) and on Windows define them yourself.
#ifdef _MSC_VER
#define IM_PRId64   "I64d"
#define IM_PRIu64   "I64u"
#else
#define IM_PRId64   "lld"
#define IM_PRIu64   "llu"
#endif

// Helpers macros
// We normally try to not use many helpers in py_demo.cpp in order to make code easier to copy and paste,
// but making an exception here as those are largely simplifying code...
// In other py sources we can use nicer internal functions from py_internal.h (ImMin/ImMax) but not in the demo.
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifndef py_CDECL
#ifdef _MSC_VER
#define py_CDECL __cdecl
#else
#define py_CDECL
#endif
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations, Helpers
//-----------------------------------------------------------------------------

#if !defined(py_DISABLE_DEMO_WINDOWS)

// Forward Declarations
static void ShowExampleAppDocuments(bool* p_open);
static void ShowExampleAppMainMenuBar();
static void ShowExampleAppConsole(bool* p_open);
static void ShowExampleAppLog(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);
static void ShowExampleAppPropertyEditor(bool* p_open);
static void ShowExampleAppLongText(bool* p_open);
static void ShowExampleAppAutoResize(bool* p_open);
static void ShowExampleAppConstrainedResize(bool* p_open);
static void ShowExampleAppSimpleOverlay(bool* p_open);
static void ShowExampleAppFullscreen(bool* p_open);
static void ShowExampleAppWindowTitles(bool* p_open);
static void ShowExampleAppCustomRendering(bool* p_open);
static void ShowExampleMenuFile();

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    py::TextDisabled("(?)");
    if (py::IsItemHovered())
    {
        py::BeginTooltip();
        py::PushTextWrapPos(py::GetFontSize() * 35.0f);
        py::TextUnformatted(desc);
        py::PopTextWrapPos();
        py::EndTooltip();
    }
}

// Helper to display basic user controls.
void py::ShowUserGuide()
{
    pyIO& io = py::GetIO();
    py::BulletText("Double-click on title bar to collapse window.");
    py::BulletText(
        "Click and drag on lower corner to resize window\n"
        "(double-click to auto fit window to its contents).");
    py::BulletText("CTRL+Click on a slider or drag box to input value as text.");
    py::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    if (io.FontAllowUserScaling)
        py::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    py::BulletText("While inputing text:\n");
    py::Indent();
    py::BulletText("CTRL+Left/Right to word jump.");
    py::BulletText("CTRL+A or double-click to select all.");
    py::BulletText("CTRL+X/C/V to use clipboard cut/copy/paste.");
    py::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
    py::BulletText("ESCAPE to revert.");
    py::BulletText("You can apply arithmetic operators +,*,/ on numerical values.\nUse +- to subtract.");
    py::Unindent();
    py::BulletText("With keyboard navigation enabled:");
    py::Indent();
    py::BulletText("Arrow keys to navigate.");
    py::BulletText("Space to activate a widget.");
    py::BulletText("Return to input text into a widget.");
    py::BulletText("Escape to deactivate a widget, close popup, exit child window.");
    py::BulletText("Alt to jump to the menu layer of a window.");
    py::BulletText("CTRL+Tab to select a window.");
    py::Unindent();
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window / ShowDemoWindow()
//-----------------------------------------------------------------------------
// - ShowDemoWindowWidgets()
// - ShowDemoWindowLayout()
// - ShowDemoWindowPopups()
// - ShowDemoWindowTables()
// - ShowDemoWindowColumns()
// - ShowDemoWindowMisc()
//-----------------------------------------------------------------------------

// We split the contents of the big ShowDemoWindow() function into smaller functions
// (because the link time of very large functions grow non-linearly)
static void ShowDemoWindowWidgets();
static void ShowDemoWindowLayout();
static void ShowDemoWindowPopups();
static void ShowDemoWindowTables();
static void ShowDemoWindowColumns();
static void ShowDemoWindowMisc();

// Demonstrate most Dear py features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does.
// You may then search for keywords in the code when you are interested by a specific feature.
void py::ShowDemoWindow(bool* p_open)
{
    // Exceptionally add an extra assert here for people confused about initial Dear py setup
    // Most py functions would normally just crash if the context is missing.
    IM_ASSERT(py::GetCurrentContext() != NULL && "Missing dear py context. Refer to examples app!");

    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_main_menu_bar = false;
    static bool show_app_documents = false;

    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_fullscreen = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;

    if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    if (show_app_documents)           ShowExampleAppDocuments(&show_app_documents);

    if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    if (show_app_fullscreen)          ShowExampleAppFullscreen(&show_app_fullscreen);
    if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    // Dear py Apps (accessible from the "Tools" menu)
    static bool show_app_metrics = false;
    static bool show_app_stack_tool = false;
    static bool show_app_style_editor = false;
    static bool show_app_about = false;

    if (show_app_metrics)       { py::ShowMetricsWindow(&show_app_metrics); }
    if (show_app_stack_tool)    { py::ShowStackToolWindow(&show_app_stack_tool); }
    if (show_app_about)         { py::ShowAboutWindow(&show_app_about); }
    if (show_app_style_editor)
    {
        py::Begin("Dear py Style Editor", &show_app_style_editor);
        py::ShowStyleEditor();
        py::End();
    }

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;
    static bool unsaved_document = false;

    pyWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= pyWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= pyWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= pyWindowFlags_MenuBar;
    if (no_move)            window_flags |= pyWindowFlags_NoMove;
    if (no_resize)          window_flags |= pyWindowFlags_NoResize;
    if (no_collapse)        window_flags |= pyWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= pyWindowFlags_NoNav;
    if (no_background)      window_flags |= pyWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= pyWindowFlags_NoBringToFrontOnFocus;
    if (unsaved_document)   window_flags |= pyWindowFlags_UnsavedDocument;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const pyViewport* main_viewport = py::GetMainViewport();
    py::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), pyCond_FirstUseEver);
    py::SetNextWindowSize(ImVec2(550, 680), pyCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!py::Begin("Dear py Demo", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        py::End();
        return;
    }

    // Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.

    // e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
    //py::PushItemWidth(-py::GetWindowWidth() * 0.35f);

    // e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
    py::PushItemWidth(py::GetFontSize() * -12);

    // Menu Bar
    if (py::BeginMenuBar())
    {
        if (py::BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            py::EndMenu();
        }
        if (py::BeginMenu("Examples"))
        {
            py::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            py::MenuItem("Console", NULL, &show_app_console);
            py::MenuItem("Log", NULL, &show_app_log);
            py::MenuItem("Simple layout", NULL, &show_app_layout);
            py::MenuItem("Property editor", NULL, &show_app_property_editor);
            py::MenuItem("Long text display", NULL, &show_app_long_text);
            py::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            py::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            py::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            py::MenuItem("Fullscreen window", NULL, &show_app_fullscreen);
            py::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            py::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            py::MenuItem("Documents", NULL, &show_app_documents);
            py::EndMenu();
        }
        //if (py::MenuItem("MenuItem")) {} // You can also use MenuItem() inside a menu bar!
        if (py::BeginMenu("Tools"))
        {
#ifndef py_DISABLE_METRICS_WINDOW
            py::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
            py::MenuItem("Stack Tool", NULL, &show_app_stack_tool);
#endif
            py::MenuItem("Style Editor", NULL, &show_app_style_editor);
            py::MenuItem("About Dear py", NULL, &show_app_about);
            py::EndMenu();
        }
        py::EndMenuBar();
    }

    py::Text("dear py says hello. (%s)", py_VERSION);
    py::Spacing();

    if (py::CollapsingHeader("Help"))
    {
        py::Text("ABOUT THIS DEMO:");
        py::BulletText("Sections below are demonstrating many aspects of the library.");
        py::BulletText("The \"Examples\" menu above leads to more demo contents.");
        py::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
                          "and Metrics/Debugger (general purpose Dear py debugging tool).");
        py::Separator();

        py::Text("PROGRAMMER GUIDE:");
        py::BulletText("See the ShowDemoWindow() code in py_demo.cpp. <- you are here!");
        py::BulletText("See comments in py.cpp.");
        py::BulletText("See example applications in the examples/ folder.");
        py::BulletText("Read the FAQ at http://www.dearpy.org/faq/");
        py::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        py::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        py::Separator();

        py::Text("USER GUIDE:");
        py::ShowUserGuide();
    }

    if (py::CollapsingHeader("Configuration"))
    {
        pyIO& io = py::GetIO();

        if (py::TreeNode("Configuration##2"))
        {
            py::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard",    &io.ConfigFlags, pyConfigFlags_NavEnableKeyboard);
            py::SameLine(); HelpMarker("Enable keyboard controls.");
            py::CheckboxFlags("io.ConfigFlags: NavEnableGamepad",     &io.ConfigFlags, pyConfigFlags_NavEnableGamepad);
            py::SameLine(); HelpMarker("Enable gamepad controls. Require backend to set io.BackendFlags |= pyBackendFlags_HasGamepad.\n\nRead instructions in py.cpp for details.");
            py::CheckboxFlags("io.ConfigFlags: NavEnableSetMousePos", &io.ConfigFlags, pyConfigFlags_NavEnableSetMousePos);
            py::SameLine(); HelpMarker("Instruct navigation to move the mouse cursor. See comment for pyConfigFlags_NavEnableSetMousePos.");
            py::CheckboxFlags("io.ConfigFlags: NoMouse",              &io.ConfigFlags, pyConfigFlags_NoMouse);
            if (io.ConfigFlags & pyConfigFlags_NoMouse)
            {
                // The "NoMouse" option can get us stuck with a disabled mouse! Let's provide an alternative way to fix it:
                if (fmodf((float)py::GetTime(), 0.40f) < 0.20f)
                {
                    py::SameLine();
                    py::Text("<<PRESS SPACE TO DISABLE>>");
                }
                if (py::IsKeyPressed(py::GetKeyIndex(pyKey_Space)))
                    io.ConfigFlags &= ~pyConfigFlags_NoMouse;
            }
            py::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange", &io.ConfigFlags, pyConfigFlags_NoMouseCursorChange);
            py::SameLine(); HelpMarker("Instruct backend to not alter mouse cursor shape and visibility.");
            py::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
            py::SameLine(); HelpMarker("Enable blinking cursor (optional as some users consider it to be distracting)");
            py::Checkbox("io.ConfigDragClickToInputText", &io.ConfigDragClickToInputText);
            py::SameLine(); HelpMarker("Enable turning DragXXX widgets into text input with a simple mouse click-release (without moving).");
            py::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
            py::SameLine(); HelpMarker("Enable resizing of windows from their edges and from the lower-left corner.\nThis requires (io.BackendFlags & pyBackendFlags_HasMouseCursors) because it needs mouse cursor feedback.");
            py::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
            py::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);
            py::SameLine(); HelpMarker("Instruct Dear py to render a mouse cursor itself. Note that a mouse cursor rendered via your application GPU rendering path will feel more laggy than hardware cursor, but will be more in sync with your other visuals.\n\nSome desktop applications may use both kinds of cursors (e.g. enable software cursor only when resizing/dragging something).");
            py::Text("Also see Style->Rendering for rendering options.");
            py::TreePop();
            py::Separator();
        }

        if (py::TreeNode("Backend Flags"))
        {
            HelpMarker(
                "Those flags are set by the backends (py_impl_xxx files) to specify their capabilities.\n"
                "Here we expose them as read-only fields to avoid breaking interactions with your backend.");

            // Make a local copy to avoid modifying actual backend flags.
            pyBackendFlags backend_flags = io.BackendFlags;
            py::CheckboxFlags("io.BackendFlags: HasGamepad",           &backend_flags, pyBackendFlags_HasGamepad);
            py::CheckboxFlags("io.BackendFlags: HasMouseCursors",      &backend_flags, pyBackendFlags_HasMouseCursors);
            py::CheckboxFlags("io.BackendFlags: HasSetMousePos",       &backend_flags, pyBackendFlags_HasSetMousePos);
            py::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", &backend_flags, pyBackendFlags_RendererHasVtxOffset);
            py::TreePop();
            py::Separator();
        }

        if (py::TreeNode("Style"))
        {
            HelpMarker("The same contents can be accessed in 'Tools->Style Editor' or by calling the ShowStyleEditor() function.");
            py::ShowStyleEditor();
            py::TreePop();
            py::Separator();
        }

        if (py::TreeNode("Capture/Logging"))
        {
            HelpMarker(
                "The logging API redirects all text output so you can easily capture the content of "
                "a window or a block. Tree nodes can be automatically expanded.\n"
                "Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            py::LogButtons();

            HelpMarker("You can also call py::LogText() to output directly to the log without a visual output.");
            if (py::Button("Copy \"Hello, world!\" to clipboard"))
            {
                py::LogToClipboard();
                py::LogText("Hello, world!");
                py::LogFinish();
            }
            py::TreePop();
        }
    }

    if (py::CollapsingHeader("Window options"))
    {
        if (py::BeginTable("split", 3))
        {
            py::TableNextColumn(); py::Checkbox("No titlebar", &no_titlebar);
            py::TableNextColumn(); py::Checkbox("No scrollbar", &no_scrollbar);
            py::TableNextColumn(); py::Checkbox("No menu", &no_menu);
            py::TableNextColumn(); py::Checkbox("No move", &no_move);
            py::TableNextColumn(); py::Checkbox("No resize", &no_resize);
            py::TableNextColumn(); py::Checkbox("No collapse", &no_collapse);
            py::TableNextColumn(); py::Checkbox("No close", &no_close);
            py::TableNextColumn(); py::Checkbox("No nav", &no_nav);
            py::TableNextColumn(); py::Checkbox("No background", &no_background);
            py::TableNextColumn(); py::Checkbox("No bring to front", &no_bring_to_front);
            py::TableNextColumn(); py::Checkbox("Unsaved document", &unsaved_document);
            py::EndTable();
        }
    }

    // All demo contents
    ShowDemoWindowWidgets();
    ShowDemoWindowLayout();
    ShowDemoWindowPopups();
    ShowDemoWindowTables();
    ShowDemoWindowMisc();

    // End of ShowDemoWindow()
    py::PopItemWidth();
    py::End();
}

static void ShowDemoWindowWidgets()
{
    if (!py::CollapsingHeader("Widgets"))
        return;

    static bool disable_all = false; // The Checkbox for that is inside the "Disabled" section at the bottom
    if (disable_all)
        py::BeginDisabled();

    if (py::TreeNode("Basic"))
    {
        static int clicked = 0;
        if (py::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            py::SameLine();
            py::Text("Thanks for clicking me!");
        }

        static bool check = true;
        py::Checkbox("checkbox", &check);

        static int e = 0;
        py::RadioButton("radio a", &e, 0); py::SameLine();
        py::RadioButton("radio b", &e, 1); py::SameLine();
        py::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                py::SameLine();
            py::PushID(i);
            py::PushStyleColor(pyCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            py::PushStyleColor(pyCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            py::PushStyleColor(pyCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            py::Button("Click");
            py::PopStyleColor(3);
            py::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
        // See 'Demo->Layout->Text Baseline Alignment' for details.
        py::AlignTextToFramePadding();
        py::Text("Hold to repeat:");
        py::SameLine();

        // Arrow buttons with Repeater
        static int counter = 0;
        float spacing = py::GetStyle().ItemInnerSpacing.x;
        py::PushButtonRepeat(true);
        if (py::ArrowButton("##left", pyDir_Left)) { counter--; }
        py::SameLine(0.0f, spacing);
        if (py::ArrowButton("##right", pyDir_Right)) { counter++; }
        py::PopButtonRepeat();
        py::SameLine();
        py::Text("%d", counter);

        py::Text("Hover over me");
        if (py::IsItemHovered())
            py::SetTooltip("I am a tooltip");

        py::SameLine();
        py::Text("- or me");
        if (py::IsItemHovered())
        {
            py::BeginTooltip();
            py::Text("I am a fancy tooltip");
            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            py::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
            py::EndTooltip();
        }

        py::Separator();

        py::LabelText("label", "Value");

        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more flexible BeginCombo()/EndCombo() api.
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
            static int item_current = 0;
            py::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            py::SameLine(); HelpMarker(
                "Using the simplified one-liner Combo API here.\nRefer to the \"Combo\" section below for an explanation of how to use the more flexible and general BeginCombo/EndCombo API.");
        }

        {
            // To wire InputText() with std::string or any other custom string type,
            // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/py_stdlib.h file.
            static char str0[128] = "Hello, world!";
            py::InputText("input text", str0, IM_ARRAYSIZE(str0));
            py::SameLine(); HelpMarker(
                "USER:\n"
                "Hold SHIFT or use mouse to select text.\n"
                "CTRL+Left/Right to word jump.\n"
                "CTRL+A or double-click to select all.\n"
                "CTRL+X,CTRL+C,CTRL+V clipboard.\n"
                "CTRL+Z,CTRL+Y undo/redo.\n"
                "ESCAPE to revert.\n\n"
                "PROGRAMMER:\n"
                "You can use the pyInputTextFlags_CallbackResize facility if you need to wire InputText() "
                "to a dynamic string type. See misc/cpp/py_stdlib.h for an example (this is not demonstrated "
                "in py_demo.cpp).");

            static char str1[128] = "";
            py::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            static int i0 = 123;
            py::InputInt("input int", &i0);
            py::SameLine(); HelpMarker(
                "You can apply arithmetic operators +,*,/ on numerical values.\n"
                "  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\n"
                "Use +- to subtract.");

            static float f0 = 0.001f;
            py::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            py::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            py::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            py::SameLine(); HelpMarker(
                "You can input value using the scientific notation,\n"
                "  e.g. \"1e+8\" becomes \"100000000\".");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            py::InputFloat3("input float3", vec4a);
        }

        {
            static int i1 = 50, i2 = 42;
            py::DragInt("drag int", &i1, 1);
            py::SameLine(); HelpMarker(
                "Click and drag to edit value.\n"
                "Hold SHIFT/ALT for faster/slower edit.\n"
                "Double-click or CTRL+click to input value.");

            py::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%", pySliderFlags_AlwaysClamp);

            static float f1 = 1.00f, f2 = 0.0067f;
            py::DragFloat("drag float", &f1, 0.005f);
            py::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1 = 0;
            py::SliderInt("slider int", &i1, -1, 3);
            py::SameLine(); HelpMarker("CTRL+click to input value.");

            static float f1 = 0.123f, f2 = 0.0f;
            py::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            py::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", pySliderFlags_Logarithmic);

            static float angle = 0.0f;
            py::SliderAngle("slider angle", &angle);

            // Using the format string to display a name instead of an integer.
            // Here we completely omit '%d' from the format string, so it'll only display a name.
            // This technique can also be used with DragInt().
            enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
            static int elem = Element_Fire;
            const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
            const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
            py::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name);
            py::SameLine(); HelpMarker("Using the format string parameter to display a name instead of the underlying integer.");
        }

        {
            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            py::ColorEdit3("color 1", col1);
            py::SameLine(); HelpMarker(
                "Click on the color square to open a color picker.\n"
                "Click and hold to use drag and drop.\n"
                "Right-click on the color square to show options.\n"
                "CTRL+click on individual component to input value.\n");

            py::ColorEdit4("color 2", col2);
        }

        {
            // Using the _simplified_ one-liner ListBox() api here
            // See "List boxes" section for examples of how to use the more flexible BeginListBox()/EndListBox() api.
            const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int item_current = 1;
            py::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 4);
            py::SameLine(); HelpMarker(
                "Using the simplified one-liner ListBox API here.\nRefer to the \"List boxes\" section below for an explanation of how to use the more flexible and general BeginListBox/EndListBox API.");
        }

        py::TreePop();
    }

    // Testing pyOnceUponAFrame helper.
    //static pyOnceUponAFrame once;
    //for (int i = 0; i < 5; i++)
    //    if (once)
    //        py::Text("This will be displayed only once.");

    if (py::TreeNode("Trees"))
    {
        if (py::TreeNode("Basic trees"))
        {
            for (int i = 0; i < 5; i++)
            {
                // Use SetNextItemOpen() so set the default state of a node to be open. We could
                // also use TreeNodeEx() with the pyTreeNodeFlags_DefaultOpen flag to achieve the same thing!
                if (i == 0)
                    py::SetNextItemOpen(true, pyCond_Once);

                if (py::TreeNode((void*)(intptr_t)i, "Child %d", i))
                {
                    py::Text("blah blah");
                    py::SameLine();
                    if (py::SmallButton("button")) {}
                    py::TreePop();
                }
            }
            py::TreePop();
        }

        if (py::TreeNode("Advanced, with Selectable nodes"))
        {
            HelpMarker(
                "This is a more typical looking tree with selectable nodes.\n"
                "Click to select, CTRL+Click to toggle, click on arrows or double-click to open.");
            static pyTreeNodeFlags base_flags = pyTreeNodeFlags_OpenOnArrow | pyTreeNodeFlags_OpenOnDoubleClick | pyTreeNodeFlags_SpanAvailWidth;
            static bool align_label_with_current_x_position = false;
            static bool test_drag_and_drop = false;
            py::CheckboxFlags("pyTreeNodeFlags_OpenOnArrow",       &base_flags, pyTreeNodeFlags_OpenOnArrow);
            py::CheckboxFlags("pyTreeNodeFlags_OpenOnDoubleClick", &base_flags, pyTreeNodeFlags_OpenOnDoubleClick);
            py::CheckboxFlags("pyTreeNodeFlags_SpanAvailWidth",    &base_flags, pyTreeNodeFlags_SpanAvailWidth); py::SameLine(); HelpMarker("Extend hit area to all available width instead of allowing more items to be laid out after the node.");
            py::CheckboxFlags("pyTreeNodeFlags_SpanFullWidth",     &base_flags, pyTreeNodeFlags_SpanFullWidth);
            py::Checkbox("Align label with current X position", &align_label_with_current_x_position);
            py::Checkbox("Test tree node as drag source", &test_drag_and_drop);
            py::Text("Hello!");
            if (align_label_with_current_x_position)
                py::Unindent(py::GetTreeNodeToLabelSpacing());

            // 'selection_mask' is dumb representation of what may be user-side selection state.
            //  You may retain selection state inside or outside your objects in whatever format you see fit.
            // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
            /// of the loop. May be a pointer to your own node type, etc.
            static int selection_mask = (1 << 2);
            int node_clicked = -1;
            for (int i = 0; i < 6; i++)
            {
                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                pyTreeNodeFlags node_flags = base_flags;
                const bool is_selected = (selection_mask & (1 << i)) != 0;
                if (is_selected)
                    node_flags |= pyTreeNodeFlags_Selected;
                if (i < 3)
                {
                    // Items 0..2 are Tree Node
                    bool node_open = py::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    if (py::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && py::BeginDragDropSource())
                    {
                        py::SetDragDropPayload("_TREENODE", NULL, 0);
                        py::Text("This is a drag and drop source");
                        py::EndDragDropSource();
                    }
                    if (node_open)
                    {
                        py::BulletText("Blah blah\nBlah Blah");
                        py::TreePop();
                    }
                }
                else
                {
                    // Items 3..5 are Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                    node_flags |= pyTreeNodeFlags_Leaf | pyTreeNodeFlags_NoTreePushOnOpen; // pyTreeNodeFlags_Bullet
                    py::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (py::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && py::BeginDragDropSource())
                    {
                        py::SetDragDropPayload("_TREENODE", NULL, 0);
                        py::Text("This is a drag and drop source");
                        py::EndDragDropSource();
                    }
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if (py::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            if (align_label_with_current_x_position)
                py::Indent(py::GetTreeNodeToLabelSpacing());
            py::TreePop();
        }
        py::TreePop();
    }

    if (py::TreeNode("Collapsing Headers"))
    {
        static bool closable_group = true;
        py::Checkbox("Show 2nd header", &closable_group);
        if (py::CollapsingHeader("Header", pyTreeNodeFlags_None))
        {
            py::Text("IsItemHovered: %d", py::IsItemHovered());
            for (int i = 0; i < 5; i++)
                py::Text("Some content %d", i);
        }
        if (py::CollapsingHeader("Header with a close button", &closable_group))
        {
            py::Text("IsItemHovered: %d", py::IsItemHovered());
            for (int i = 0; i < 5; i++)
                py::Text("More content %d", i);
        }
        /*
        if (py::CollapsingHeader("Header with a bullet", pyTreeNodeFlags_Bullet))
            py::Text("IsItemHovered: %d", py::IsItemHovered());
        */
        py::TreePop();
    }

    if (py::TreeNode("Bullets"))
    {
        py::BulletText("Bullet point 1");
        py::BulletText("Bullet point 2\nOn multiple lines");
        if (py::TreeNode("Tree node"))
        {
            py::BulletText("Another bullet point");
            py::TreePop();
        }
        py::Bullet(); py::Text("Bullet point 3 (two calls)");
        py::Bullet(); py::SmallButton("Button");
        py::TreePop();
    }

    if (py::TreeNode("Text"))
    {
        if (py::TreeNode("Colorful Text"))
        {
            // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
            py::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
            py::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yellow");
            py::TextDisabled("Disabled");
            py::SameLine(); HelpMarker("The TextDisabled color is stored in pyStyle.");
            py::TreePop();
        }

        if (py::TreeNode("Word Wrapping"))
        {
            // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
            py::TextWrapped(
                "This text should automatically wrap on the edge of the window. The current implementation "
                "for text wrapping follows simple rules suitable for English and possibly other languages.");
            py::Spacing();

            static float wrap_width = 200.0f;
            py::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

            ImDrawList* draw_list = py::GetWindowDrawList();
            for (int n = 0; n < 2; n++)
            {
                py::Text("Test paragraph %d:", n);
                ImVec2 pos = py::GetCursorScreenPos();
                ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
                ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + py::GetTextLineHeight());
                py::PushTextWrapPos(py::GetCursorPos().x + wrap_width);
                if (n == 0)
                    py::Text("The lazy dog is a good dog. This paragraph should fit within %.0f pixels. Testing a 1 character word. The quick brown fox jumps over the lazy dog.", wrap_width);
                else
                    py::Text("aaaaaaaa bbbbbbbb, c cccccccc,dddddddd. d eeeeeeee   ffffffff. gggggggg!hhhhhhhh");

                // Draw actual text bounding box, following by marker of our expected limit (should not overlap!)
                draw_list->AddRect(py::GetItemRectMin(), py::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
                draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(255, 0, 255, 255));
                py::PopTextWrapPos();
            }

            py::TreePop();
        }

        if (py::TreeNode("UTF-8 Text"))
        {
            // UTF-8 test with Japanese characters
            // (Needs a suitable font? Try "Google Noto" or "Arial Unicode". See docs/FONTS.md for details.)
            // - From C++11 you can use the u8"my text" syntax to encode literal strings as UTF-8
            // - For earlier compiler, you may be able to encode your sources as UTF-8 (e.g. in Visual Studio, you
            //   can save your source files as 'UTF-8 without signature').
            // - FOR THIS DEMO FILE ONLY, BECAUSE WE WANT TO SUPPORT OLD COMPILERS, WE ARE *NOT* INCLUDING RAW UTF-8
            //   CHARACTERS IN THIS SOURCE FILE. Instead we are encoding a few strings with hexadecimal constants.
            //   Don't do this in your application! Please use u8"text in any language" in your application!
            // Note that characters values are preserved even by InputText() if the font cannot be displayed,
            // so you can safely copy & paste garbled characters into another application.
            py::TextWrapped(
                "CJK text will only appears if the font was loaded with the appropriate CJK character ranges. "
                "Call io.Fonts->AddFontFromFileTTF() manually to load extra character ranges. "
                "Read docs/FONTS.md for details.");
            py::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)"); // Normally we would use u8"blah blah" with the proper characters directly in the string.
            py::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
            static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
            //static char buf[32] = u8"NIHONGO"; // <- this is how you would write it with C++11, using real kanjis
            py::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
            py::TreePop();
        }
        py::TreePop();
    }

    if (py::TreeNode("Images"))
    {
        pyIO& io = py::GetIO();
        py::TextWrapped(
            "Below we are displaying the font texture (which is the only texture we have access to in this demo). "
            "Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. "
            "Hover the texture for a zoomed view!");

        // Below we are displaying the font texture because it is the only texture we have access to inside the demo!
        // Remember that ImTextureID is just storage for whatever you want it to be. It is essentially a value that
        // will be passed to the rendering backend via the ImDrawCmd structure.
        // If you use one of the default py_impl_XXXX.cpp rendering backend, they all have comments at the top
        // of their respective source file to specify what they expect to be stored in ImTextureID, for example:
        // - The py_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer
        // - The py_impl_opengl3.cpp renderer expect a GLuint OpenGL texture identifier, etc.
        // More:
        // - If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers
        //   to py::Image(), and gather width/height through your own functions, etc.
        // - You can use ShowMetricsWindow() to inspect the draw data that are being passed to your renderer,
        //   it will help you debug issues if you are confused about it.
        // - Consider using the lower-level ImDrawList::AddImage() API, via py::GetWindowDrawList()->AddImage().
        // - Read https://github.com/ocornut/py/blob/master/docs/FAQ.md
        // - Read https://github.com/ocornut/py/wiki/Image-Loading-and-Displaying-Examples
        ImTextureID my_tex_id = io.Fonts->TexID;
        float my_tex_w = (float)io.Fonts->TexWidth;
        float my_tex_h = (float)io.Fonts->TexHeight;
        {
            py::Text("%.0fx%.0f", my_tex_w, my_tex_h);
            ImVec2 pos = py::GetCursorScreenPos();
            ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
            ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
            ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
            py::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            if (py::IsItemHovered())
            {
                py::BeginTooltip();
                float region_sz = 32.0f;
                float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
                float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                float zoom = 4.0f;
                if (region_x < 0.0f) { region_x = 0.0f; }
                else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
                if (region_y < 0.0f) { region_y = 0.0f; }
                else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
                py::Text("Min: (%.2f, %.2f)", region_x, region_y);
                py::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
                ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
                ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
                py::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
                py::EndTooltip();
            }
        }
        py::TextWrapped("And now some textured buttons..");
        static int pressed_count = 0;
        for (int i = 0; i < 8; i++)
        {
            py::PushID(i);
            int frame_padding = -1 + i;                             // -1 == uses default padding (style.FramePadding)
            ImVec2 size = ImVec2(32.0f, 32.0f);                     // Size of the image we want to make visible
            ImVec2 uv0 = ImVec2(0.0f, 0.0f);                        // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32.0f / my_tex_h);// UV coordinates for (32,32) in our texture
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint
            if (py::ImageButton(my_tex_id, size, uv0, uv1, frame_padding, bg_col, tint_col))
                pressed_count += 1;
            py::PopID();
            py::SameLine();
        }
        py::NewLine();
        py::Text("Pressed %d times.", pressed_count);
        py::TreePop();
    }

    if (py::TreeNode("Combo"))
    {
        // Expose flags as checkbox for the demo
        static pyComboFlags flags = 0;
        py::CheckboxFlags("pyComboFlags_PopupAlignLeft", &flags, pyComboFlags_PopupAlignLeft);
        py::SameLine(); HelpMarker("Only makes a difference if the popup is larger than the combo");
        if (py::CheckboxFlags("pyComboFlags_NoArrowButton", &flags, pyComboFlags_NoArrowButton))
            flags &= ~pyComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
        if (py::CheckboxFlags("pyComboFlags_NoPreview", &flags, pyComboFlags_NoPreview))
            flags &= ~pyComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

        // Using the generic BeginCombo() API, you have full control over how to display the combo contents.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
        // stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static int item_current_idx = 0; // Here we store our selection data as an index.
        const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
        if (py::BeginCombo("combo 1", combo_preview_value, flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (py::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    py::SetItemDefaultFocus();
            }
            py::EndCombo();
        }

        // Simplified one-liner Combo() API, using values packed in a single constant string
        // This is a convenience for when the selection set is small and known at compile-time.
        static int item_current_2 = 0;
        py::Combo("combo 2 (one-liner)", &item_current_2, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

        // Simplified one-liner Combo() using an array of const char*
        // This is not very useful (may obsolete): prefer using BeginCombo()/EndCombo() for full control.
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        py::Combo("combo 3 (array)", &item_current_3, items, IM_ARRAYSIZE(items));

        // Simplified one-liner Combo() using an accessor function
        struct Funcs { static bool ItemGetter(void* data, int n, const char** out_str) { *out_str = ((const char**)data)[n]; return true; } };
        static int item_current_4 = 0;
        py::Combo("combo 4 (function)", &item_current_4, &Funcs::ItemGetter, items, IM_ARRAYSIZE(items));

        py::TreePop();
    }

    if (py::TreeNode("List boxes"))
    {
        // Using the generic BeginListBox() API, you have full control over how to display the combo contents.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
        // stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static int item_current_idx = 0; // Here we store our selection data as an index.
        if (py::BeginListBox("listbox 1"))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (py::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    py::SetItemDefaultFocus();
            }
            py::EndListBox();
        }

        // Custom size: use all width, 5 items tall
        py::Text("Full-width:");
        if (py::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * py::GetTextLineHeightWithSpacing())))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (py::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    py::SetItemDefaultFocus();
            }
            py::EndListBox();
        }

        py::TreePop();
    }

    if (py::TreeNode("Selectables"))
    {
        // Selectable() has 2 overloads:
        // - The one taking "bool selected" as a read-only selection information.
        //   When Selectable() has been clicked it returns true and you can alter selection state accordingly.
        // - The one taking "bool* p_selected" as a read-write selection information (convenient in some cases)
        // The earlier is more flexible, as in real application your selection may be stored in many different ways
        // and not necessarily inside a bool value (e.g. in flags within objects, as an external list, etc).
        if (py::TreeNode("Basic"))
        {
            static bool selection[5] = { false, true, false, false, false };
            py::Selectable("1. I am selectable", &selection[0]);
            py::Selectable("2. I am selectable", &selection[1]);
            py::Text("(I am not selectable)");
            py::Selectable("4. I am selectable", &selection[3]);
            if (py::Selectable("5. I am double clickable", selection[4], pySelectableFlags_AllowDoubleClick))
                if (py::IsMouseDoubleClicked(0))
                    selection[4] = !selection[4];
            py::TreePop();
        }
        if (py::TreeNode("Selection State: Single Selection"))
        {
            static int selected = -1;
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (py::Selectable(buf, selected == n))
                    selected = n;
            }
            py::TreePop();
        }
        if (py::TreeNode("Selection State: Multiple Selection"))
        {
            HelpMarker("Hold CTRL and click to select multiple items.");
            static bool selection[5] = { false, false, false, false, false };
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (py::Selectable(buf, selection[n]))
                {
                    if (!py::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                        memset(selection, 0, sizeof(selection));
                    selection[n] ^= 1;
                }
            }
            py::TreePop();
        }
        if (py::TreeNode("Rendering more text into the same line"))
        {
            // Using the Selectable() override that takes "bool* p_selected" parameter,
            // this function toggle your bool value automatically.
            static bool selected[3] = { false, false, false };
            py::Selectable("main.c",    &selected[0]); py::SameLine(300); py::Text(" 2,345 bytes");
            py::Selectable("Hello.cpp", &selected[1]); py::SameLine(300); py::Text("12,345 bytes");
            py::Selectable("Hello.h",   &selected[2]); py::SameLine(300); py::Text(" 2,345 bytes");
            py::TreePop();
        }
        if (py::TreeNode("In columns"))
        {
            static bool selected[10] = {};

            if (py::BeginTable("split1", 3, pyTableFlags_Resizable | pyTableFlags_NoSavedSettings | pyTableFlags_Borders))
            {
                for (int i = 0; i < 10; i++)
                {
                    char label[32];
                    sprintf(label, "Item %d", i);
                    py::TableNextColumn();
                    py::Selectable(label, &selected[i]); // FIXME-TABLE: Selection overlap
                }
                py::EndTable();
            }
            py::Spacing();
            if (py::BeginTable("split2", 3, pyTableFlags_Resizable | pyTableFlags_NoSavedSettings | pyTableFlags_Borders))
            {
                for (int i = 0; i < 10; i++)
                {
                    char label[32];
                    sprintf(label, "Item %d", i);
                    py::TableNextRow();
                    py::TableNextColumn();
                    py::Selectable(label, &selected[i], pySelectableFlags_SpanAllColumns);
                    py::TableNextColumn();
                    py::Text("Some other contents");
                    py::TableNextColumn();
                    py::Text("123456");
                }
                py::EndTable();
            }
            py::TreePop();
        }
        if (py::TreeNode("Grid"))
        {
            static char selected[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

            // Add in a bit of silly fun...
            const float time = (float)py::GetTime();
            const bool winning_state = memchr(selected, 0, sizeof(selected)) == NULL; // If all cells are selected...
            if (winning_state)
                py::PushStyleVar(pyStyleVar_SelectableTextAlign, ImVec2(0.5f + 0.5f * cosf(time * 2.0f), 0.5f + 0.5f * sinf(time * 3.0f)));

            for (int y = 0; y < 4; y++)
                for (int x = 0; x < 4; x++)
                {
                    if (x > 0)
                        py::SameLine();
                    py::PushID(y * 4 + x);
                    if (py::Selectable("Sailor", selected[y][x] != 0, 0, ImVec2(50, 50)))
                    {
                        // Toggle clicked cell + toggle neighbors
                        selected[y][x] ^= 1;
                        if (x > 0) { selected[y][x - 1] ^= 1; }
                        if (x < 3) { selected[y][x + 1] ^= 1; }
                        if (y > 0) { selected[y - 1][x] ^= 1; }
                        if (y < 3) { selected[y + 1][x] ^= 1; }
                    }
                    py::PopID();
                }

            if (winning_state)
                py::PopStyleVar();
            py::TreePop();
        }
        if (py::TreeNode("Alignment"))
        {
            HelpMarker(
                "By default, Selectables uses style.SelectableTextAlign but it can be overridden on a per-item "
                "basis using PushStyleVar(). You'll probably want to always keep your default situation to "
                "left-align otherwise it becomes difficult to layout multiple items on a same line");
            static bool selected[3 * 3] = { true, false, true, false, true, false, true, false, true };
            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 3; x++)
                {
                    ImVec2 alignment = ImVec2((float)x / 2.0f, (float)y / 2.0f);
                    char name[32];
                    sprintf(name, "(%.1f,%.1f)", alignment.x, alignment.y);
                    if (x > 0) py::SameLine();
                    py::PushStyleVar(pyStyleVar_SelectableTextAlign, alignment);
                    py::Selectable(name, &selected[3 * y + x], pySelectableFlags_None, ImVec2(80, 80));
                    py::PopStyleVar();
                }
            }
            py::TreePop();
        }
        py::TreePop();
    }

    // To wire InputText() with std::string or any other custom string type,
    // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/py_stdlib.h file.
    if (py::TreeNode("Text Input"))
    {
        if (py::TreeNode("Multi-line Text Input"))
        {
            // Note: we are using a fixed-sized buffer for simplicity here. See pyInputTextFlags_CallbackResize
            // and the code in misc/cpp/py_stdlib.h for how to setup InputText() for dynamically resizing strings.
            static char text[1024 * 16] =
                "/*\n"
                " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                " the hexadecimal encoding of one offending instruction,\n"
                " more formally, the invalid operand with locked CMPXCHG8B\n"
                " instruction bug, is a design flaw in the majority of\n"
                " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                " processors (all in the P5 microarchitecture).\n"
                "*/\n\n"
                "label:\n"
                "\tlock cmpxchg8b eax\n";

            static pyInputTextFlags flags = pyInputTextFlags_AllowTabInput;
            HelpMarker("You can use the pyInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/py_stdlib.h for an example. (This is not demonstrated in py_demo.cpp because we don't want to include <string> in here)");
            py::CheckboxFlags("pyInputTextFlags_ReadOnly", &flags, pyInputTextFlags_ReadOnly);
            py::CheckboxFlags("pyInputTextFlags_AllowTabInput", &flags, pyInputTextFlags_AllowTabInput);
            py::CheckboxFlags("pyInputTextFlags_CtrlEnterForNewLine", &flags, pyInputTextFlags_CtrlEnterForNewLine);
            py::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, py::GetTextLineHeight() * 16), flags);
            py::TreePop();
        }

        if (py::TreeNode("Filtered Text Input"))
        {
            struct TextFilters
            {
                // Return 0 (pass) if the character is 'i' or 'm' or 'g' or 'u' or 'i'
                static int FilterpyLetters(pyInputTextCallbackData* data)
                {
                    if (data->EventChar < 256 && strchr("py", (char)data->EventChar))
                        return 0;
                    return 1;
                }
            };

            static char buf1[64] = ""; py::InputText("default",     buf1, 64);
            static char buf2[64] = ""; py::InputText("decimal",     buf2, 64, pyInputTextFlags_CharsDecimal);
            static char buf3[64] = ""; py::InputText("hexadecimal", buf3, 64, pyInputTextFlags_CharsHexadecimal | pyInputTextFlags_CharsUppercase);
            static char buf4[64] = ""; py::InputText("uppercase",   buf4, 64, pyInputTextFlags_CharsUppercase);
            static char buf5[64] = ""; py::InputText("no blank",    buf5, 64, pyInputTextFlags_CharsNoBlank);
            static char buf6[64] = ""; py::InputText("\"py\" letters", buf6, 64, pyInputTextFlags_CallbackCharFilter, TextFilters::FilterpyLetters);
            py::TreePop();
        }

        if (py::TreeNode("Password Input"))
        {
            static char password[64] = "password123";
            py::InputText("password", password, IM_ARRAYSIZE(password), pyInputTextFlags_Password);
            py::SameLine(); HelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");
            py::InputTextWithHint("password (w/ hint)", "<password>", password, IM_ARRAYSIZE(password), pyInputTextFlags_Password);
            py::InputText("password (clear)", password, IM_ARRAYSIZE(password));
            py::TreePop();
        }

        if (py::TreeNode("Completion, History, Edit Callbacks"))
        {
            struct Funcs
            {
                static int MyCallback(pyInputTextCallbackData* data)
                {
                    if (data->EventFlag == pyInputTextFlags_CallbackCompletion)
                    {
                        data->InsertChars(data->CursorPos, "..");
                    }
                    else if (data->EventFlag == pyInputTextFlags_CallbackHistory)
                    {
                        if (data->EventKey == pyKey_UpArrow)
                        {
                            data->DeleteChars(0, data->BufTextLen);
                            data->InsertChars(0, "Pressed Up!");
                            data->SelectAll();
                        }
                        else if (data->EventKey == pyKey_DownArrow)
                        {
                            data->DeleteChars(0, data->BufTextLen);
                            data->InsertChars(0, "Pressed Down!");
                            data->SelectAll();
                        }
                    }
                    else if (data->EventFlag == pyInputTextFlags_CallbackEdit)
                    {
                        // Toggle casing of first character
                        char c = data->Buf[0];
                        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) data->Buf[0] ^= 32;
                        data->BufDirty = true;

                        // Increment a counter
                        int* p_int = (int*)data->UserData;
                        *p_int = *p_int + 1;
                    }
                    return 0;
                }
            };
            static char buf1[64];
            py::InputText("Completion", buf1, 64, pyInputTextFlags_CallbackCompletion, Funcs::MyCallback);
            py::SameLine(); HelpMarker("Here we append \"..\" each time Tab is pressed. See 'Examples>Console' for a more meaningful demonstration of using this callback.");

            static char buf2[64];
            py::InputText("History", buf2, 64, pyInputTextFlags_CallbackHistory, Funcs::MyCallback);
            py::SameLine(); HelpMarker("Here we replace and select text each time Up/Down are pressed. See 'Examples>Console' for a more meaningful demonstration of using this callback.");

            static char buf3[64];
            static int edit_count = 0;
            py::InputText("Edit", buf3, 64, pyInputTextFlags_CallbackEdit, Funcs::MyCallback, (void*)&edit_count);
            py::SameLine(); HelpMarker("Here we toggle the casing of the first character on every edits + count edits.");
            py::SameLine(); py::Text("(%d)", edit_count);

            py::TreePop();
        }

        if (py::TreeNode("Resize Callback"))
        {
            // To wire InputText() with std::string or any other custom string type,
            // you can use the pyInputTextFlags_CallbackResize flag + create a custom py::InputText() wrapper
            // using your preferred type. See misc/cpp/py_stdlib.h for an implementation of this using std::string.
            HelpMarker(
                "Using pyInputTextFlags_CallbackResize to wire your custom string type to InputText().\n\n"
                "See misc/cpp/py_stdlib.h for an implementation of this for std::string.");
            struct Funcs
            {
                static int MyResizeCallback(pyInputTextCallbackData* data)
                {
                    if (data->EventFlag == pyInputTextFlags_CallbackResize)
                    {
                        ImVector<char>* my_str = (ImVector<char>*)data->UserData;
                        IM_ASSERT(my_str->begin() == data->Buf);
                        my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
                        data->Buf = my_str->begin();
                    }
                    return 0;
                }

                // Note: Because py:: is a namespace you would typically add your own function into the namespace.
                // For example, you code may declare a function 'py::InputText(const char* label, MyString* my_str)'
                static bool MyInputTextMultiline(const char* label, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), pyInputTextFlags flags = 0)
                {
                    IM_ASSERT((flags & pyInputTextFlags_CallbackResize) == 0);
                    return py::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | pyInputTextFlags_CallbackResize, Funcs::MyResizeCallback, (void*)my_str);
                }
            };

            // For this demo we are using ImVector as a string container.
            // Note that because we need to store a terminating zero character, our size/capacity are 1 more
            // than usually reported by a typical string class.
            static ImVector<char> my_str;
            if (my_str.empty())
                my_str.push_back(0);
            Funcs::MyInputTextMultiline("##MyStr", &my_str, ImVec2(-FLT_MIN, py::GetTextLineHeight() * 16));
            py::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)my_str.begin(), my_str.size(), my_str.capacity());
            py::TreePop();
        }

        py::TreePop();
    }

    // Tabs
    if (py::TreeNode("Tabs"))
    {
        if (py::TreeNode("Basic"))
        {
            pyTabBarFlags tab_bar_flags = pyTabBarFlags_None;
            if (py::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (py::BeginTabItem("Avocado"))
                {
                    py::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    py::EndTabItem();
                }
                if (py::BeginTabItem("Broccoli"))
                {
                    py::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    py::EndTabItem();
                }
                if (py::BeginTabItem("Cucumber"))
                {
                    py::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    py::EndTabItem();
                }
                py::EndTabBar();
            }
            py::Separator();
            py::TreePop();
        }

        if (py::TreeNode("Advanced & Close Button"))
        {
            // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
            static pyTabBarFlags tab_bar_flags = pyTabBarFlags_Reorderable;
            py::CheckboxFlags("pyTabBarFlags_Reorderable", &tab_bar_flags, pyTabBarFlags_Reorderable);
            py::CheckboxFlags("pyTabBarFlags_AutoSelectNewTabs", &tab_bar_flags, pyTabBarFlags_AutoSelectNewTabs);
            py::CheckboxFlags("pyTabBarFlags_TabListPopupButton", &tab_bar_flags, pyTabBarFlags_TabListPopupButton);
            py::CheckboxFlags("pyTabBarFlags_NoCloseWithMiddleMouseButton", &tab_bar_flags, pyTabBarFlags_NoCloseWithMiddleMouseButton);
            if ((tab_bar_flags & pyTabBarFlags_FittingPolicyMask_) == 0)
                tab_bar_flags |= pyTabBarFlags_FittingPolicyDefault_;
            if (py::CheckboxFlags("pyTabBarFlags_FittingPolicyResizeDown", &tab_bar_flags, pyTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(pyTabBarFlags_FittingPolicyMask_ ^ pyTabBarFlags_FittingPolicyResizeDown);
            if (py::CheckboxFlags("pyTabBarFlags_FittingPolicyScroll", &tab_bar_flags, pyTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(pyTabBarFlags_FittingPolicyMask_ ^ pyTabBarFlags_FittingPolicyScroll);

            // Tab Bar
            const char* names[4] = { "Artichoke", "Beetroot", "Celery", "Daikon" };
            static bool opened[4] = { true, true, true, true }; // Persistent user state
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
            {
                if (n > 0) { py::SameLine(); }
                py::Checkbox(names[n], &opened[n]);
            }

            // Passing a bool* to BeginTabItem() is similar to passing one to Begin():
            // the underlying bool will be set to false when the tab is closed.
            if (py::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                    if (opened[n] && py::BeginTabItem(names[n], &opened[n], pyTabItemFlags_None))
                    {
                        py::Text("This is the %s tab!", names[n]);
                        if (n & 1)
                            py::Text("I am an odd tab.");
                        py::EndTabItem();
                    }
                py::EndTabBar();
            }
            py::Separator();
            py::TreePop();
        }

        if (py::TreeNode("TabItemButton & Leading/Trailing flags"))
        {
            static ImVector<int> active_tabs;
            static int next_tab_id = 0;
            if (next_tab_id == 0) // Initialize with some default tabs
                for (int i = 0; i < 3; i++)
                    active_tabs.push_back(next_tab_id++);

            // TabItemButton() and Leading/Trailing flags are distinct features which we will demo together.
            // (It is possible to submit regular tabs with Leading/Trailing flags, or TabItemButton tabs without Leading/Trailing flags...
            // but they tend to make more sense together)
            static bool show_leading_button = true;
            static bool show_trailing_button = true;
            py::Checkbox("Show Leading TabItemButton()", &show_leading_button);
            py::Checkbox("Show Trailing TabItemButton()", &show_trailing_button);

            // Expose some other flags which are useful to showcase how they interact with Leading/Trailing tabs
            static pyTabBarFlags tab_bar_flags = pyTabBarFlags_AutoSelectNewTabs | pyTabBarFlags_Reorderable | pyTabBarFlags_FittingPolicyResizeDown;
            py::CheckboxFlags("pyTabBarFlags_TabListPopupButton", &tab_bar_flags, pyTabBarFlags_TabListPopupButton);
            if (py::CheckboxFlags("pyTabBarFlags_FittingPolicyResizeDown", &tab_bar_flags, pyTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(pyTabBarFlags_FittingPolicyMask_ ^ pyTabBarFlags_FittingPolicyResizeDown);
            if (py::CheckboxFlags("pyTabBarFlags_FittingPolicyScroll", &tab_bar_flags, pyTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(pyTabBarFlags_FittingPolicyMask_ ^ pyTabBarFlags_FittingPolicyScroll);

            if (py::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                // Demo a Leading TabItemButton(): click the "?" button to open a menu
                if (show_leading_button)
                    if (py::TabItemButton("?", pyTabItemFlags_Leading | pyTabItemFlags_NoTooltip))
                        py::OpenPopup("MyHelpMenu");
                if (py::BeginPopup("MyHelpMenu"))
                {
                    py::Selectable("Hello!");
                    py::EndPopup();
                }

                // Demo Trailing Tabs: click the "+" button to add a new tab (in your app you may want to use a font icon instead of the "+")
                // Note that we submit it before the regular tabs, but because of the pyTabItemFlags_Trailing flag it will always appear at the end.
                if (show_trailing_button)
                    if (py::TabItemButton("+", pyTabItemFlags_Trailing | pyTabItemFlags_NoTooltip))
                        active_tabs.push_back(next_tab_id++); // Add new tab

                // Submit our regular tabs
                for (int n = 0; n < active_tabs.Size; )
                {
                    bool open = true;
                    char name[16];
                    snprintf(name, IM_ARRAYSIZE(name), "%04d", active_tabs[n]);
                    if (py::BeginTabItem(name, &open, pyTabItemFlags_None))
                    {
                        py::Text("This is the %s tab!", name);
                        py::EndTabItem();
                    }

                    if (!open)
                        active_tabs.erase(active_tabs.Data + n);
                    else
                        n++;
                }

                py::EndTabBar();
            }
            py::Separator();
            py::TreePop();
        }
        py::TreePop();
    }

    // Plot/Graph widgets are not very good.
    // Consider using a third-party library such as ImPlot: https://github.com/epezent/implot
    // (see others https://github.com/ocornut/py/wiki/Useful-Extensions)
    if (py::TreeNode("Plots Widgets"))
    {
        static bool animate = true;
        py::Checkbox("Animate", &animate);

        // Plot as lines and plot as histogram
        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        py::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));
        py::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

        // Fill an array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float
        // and the sizeof() of your structure in the "stride" parameter.
        static float values[90] = {};
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0)
            refresh_time = py::GetTime();
        while (refresh_time < py::GetTime()) // Create data at fixed 60 Hz rate for the demo
        {
            static float phase = 0.0f;
            values[values_offset] = cosf(phase);
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            phase += 0.10f * values_offset;
            refresh_time += 1.0f / 60.0f;
        }

        // Plots can display overlay texts
        // (in this example, we will display an average value)
        {
            float average = 0.0f;
            for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                average += values[n];
            average /= (float)IM_ARRAYSIZE(values);
            char overlay[32];
            sprintf(overlay, "avg %f", average);
            py::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        }

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices.
        // We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        py::Separator();
        py::SetNextItemWidth(py::GetFontSize() * 8);
        py::Combo("func", &func_type, "Sin\0Saw\0");
        py::SameLine();
        py::SliderInt("Sample count", &display_count, 1, 400);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        py::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        py::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        py::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * py::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
        // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        py::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        py::SameLine(0.0f, py::GetStyle().ItemInnerSpacing.x);
        py::Text("Progress Bar");

        float progress_saturated = IM_CLAMP(progress, 0.0f, 1.0f);
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        py::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
        py::TreePop();
    }

    if (py::TreeNode("Color/Picker Widgets"))
    {
        static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        py::Checkbox("With Alpha Preview", &alpha_preview);
        py::Checkbox("With Half Alpha Preview", &alpha_half_preview);
        py::Checkbox("With Drag and Drop", &drag_and_drop);
        py::Checkbox("With Options Menu", &options_menu); py::SameLine(); HelpMarker("Right-click on the individual color widget to show options.");
        py::Checkbox("With HDR", &hdr); py::SameLine(); HelpMarker("Currently all this does is to lift the 0..1 limits on dragging widgets.");
        pyColorEditFlags misc_flags = (hdr ? pyColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : pyColorEditFlags_NoDragDrop) | (alpha_half_preview ? pyColorEditFlags_AlphaPreviewHalf : (alpha_preview ? pyColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : pyColorEditFlags_NoOptions);

        py::Text("Color widget:");
        py::SameLine(); HelpMarker(
            "Click on the color square to open a color picker.\n"
            "CTRL+click on individual component to input value.\n");
        py::ColorEdit3("MyColor##1", (float*)&color, misc_flags);

        py::Text("Color widget HSV with Alpha:");
        py::ColorEdit4("MyColor##2", (float*)&color, pyColorEditFlags_DisplayHSV | misc_flags);

        py::Text("Color widget with Float Display:");
        py::ColorEdit4("MyColor##2f", (float*)&color, pyColorEditFlags_Float | misc_flags);

        py::Text("Color button with Picker:");
        py::SameLine(); HelpMarker(
            "With the pyColorEditFlags_NoInputs flag you can hide all the slider/text inputs.\n"
            "With the pyColorEditFlags_NoLabel flag you can pass a non-empty label which will only "
            "be used for the tooltip and picker popup.");
        py::ColorEdit4("MyColor##3", (float*)&color, pyColorEditFlags_NoInputs | pyColorEditFlags_NoLabel | misc_flags);

        py::Text("Color button with Custom Picker Popup:");

        // Generate a default palette. The palette will persist and can be edited.
        static bool saved_palette_init = true;
        static ImVec4 saved_palette[32] = {};
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                py::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                    saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }

        static ImVec4 backup_color;
        bool open_popup = py::ColorButton("MyColor##3b", color, misc_flags);
        py::SameLine(0, py::GetStyle().ItemInnerSpacing.x);
        open_popup |= py::Button("Palette");
        if (open_popup)
        {
            py::OpenPopup("mypicker");
            backup_color = color;
        }
        if (py::BeginPopup("mypicker"))
        {
            py::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
            py::Separator();
            py::ColorPicker4("##picker", (float*)&color, misc_flags | pyColorEditFlags_NoSidePreview | pyColorEditFlags_NoSmallPreview);
            py::SameLine();

            py::BeginGroup(); // Lock X position
            py::Text("Current");
            py::ColorButton("##current", color, pyColorEditFlags_NoPicker | pyColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            py::Text("Previous");
            if (py::ColorButton("##previous", backup_color, pyColorEditFlags_NoPicker | pyColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                color = backup_color;
            py::Separator();
            py::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                py::PushID(n);
                if ((n % 8) != 0)
                    py::SameLine(0.0f, py::GetStyle().ItemSpacing.y);

                pyColorEditFlags palette_button_flags = pyColorEditFlags_NoAlpha | pyColorEditFlags_NoPicker | pyColorEditFlags_NoTooltip;
                if (py::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                    color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
                // drag source by default, unless specifying the pyColorEditFlags_NoDragDrop flag.
                if (py::BeginDragDropTarget())
                {
                    if (const pyPayload* payload = py::AcceptDragDropPayload(py_PAYLOAD_TYPE_COLOR_3F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                    if (const pyPayload* payload = py::AcceptDragDropPayload(py_PAYLOAD_TYPE_COLOR_4F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                    py::EndDragDropTarget();
                }

                py::PopID();
            }
            py::EndGroup();
            py::EndPopup();
        }

        py::Text("Color button only:");
        static bool no_border = false;
        py::Checkbox("pyColorEditFlags_NoBorder", &no_border);
        py::ColorButton("MyColor##3c", *(ImVec4*)&color, misc_flags | (no_border ? pyColorEditFlags_NoBorder : 0), ImVec2(80, 80));

        py::Text("Color picker:");
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static bool ref_color = false;
        static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
        static int display_mode = 0;
        static int picker_mode = 0;
        py::Checkbox("With Alpha", &alpha);
        py::Checkbox("With Alpha Bar", &alpha_bar);
        py::Checkbox("With Side Preview", &side_preview);
        if (side_preview)
        {
            py::SameLine();
            py::Checkbox("With Ref Color", &ref_color);
            if (ref_color)
            {
                py::SameLine();
                py::ColorEdit4("##RefColor", &ref_color_v.x, pyColorEditFlags_NoInputs | misc_flags);
            }
        }
        py::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
        py::SameLine(); HelpMarker(
            "ColorEdit defaults to displaying RGB inputs if you don't specify a display mode, "
            "but the user can change it with a right-click.\n\nColorPicker defaults to displaying RGB+HSV+Hex "
            "if you don't specify a display mode.\n\nYou can change the defaults using SetColorEditOptions().");
        py::Combo("Picker Mode", &picker_mode, "Auto/Current\0Hue bar + SV rect\0Hue wheel + SV triangle\0");
        py::SameLine(); HelpMarker("User can right-click the picker to change mode.");
        pyColorEditFlags flags = misc_flags;
        if (!alpha)            flags |= pyColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
        if (alpha_bar)         flags |= pyColorEditFlags_AlphaBar;
        if (!side_preview)     flags |= pyColorEditFlags_NoSidePreview;
        if (picker_mode == 1)  flags |= pyColorEditFlags_PickerHueBar;
        if (picker_mode == 2)  flags |= pyColorEditFlags_PickerHueWheel;
        if (display_mode == 1) flags |= pyColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
        if (display_mode == 2) flags |= pyColorEditFlags_DisplayRGB;     // Override display mode
        if (display_mode == 3) flags |= pyColorEditFlags_DisplayHSV;
        if (display_mode == 4) flags |= pyColorEditFlags_DisplayHex;
        py::ColorPicker4("MyColor##4", (float*)&color, flags, ref_color ? &ref_color_v.x : NULL);

        py::Text("Set defaults in code:");
        py::SameLine(); HelpMarker(
            "SetColorEditOptions() is designed to allow you to set boot-time default.\n"
            "We don't have Push/Pop functions because you can force options on a per-widget basis if needed,"
            "and the user can change non-forced ones with the options menu.\nWe don't have a getter to avoid"
            "encouraging you to persistently save values that aren't forward-compatible.");
        if (py::Button("Default: Uint8 + HSV + Hue Bar"))
            py::SetColorEditOptions(pyColorEditFlags_Uint8 | pyColorEditFlags_DisplayHSV | pyColorEditFlags_PickerHueBar);
        if (py::Button("Default: Float + HDR + Hue Wheel"))
            py::SetColorEditOptions(pyColorEditFlags_Float | pyColorEditFlags_HDR | pyColorEditFlags_PickerHueWheel);

        // HSV encoded support (to avoid RGB<>HSV round trips and singularities when S==0 or V==0)
        static ImVec4 color_hsv(0.23f, 1.0f, 1.0f, 1.0f); // Stored as HSV!
        py::Spacing();
        py::Text("HSV encoded colors");
        py::SameLine(); HelpMarker(
            "By default, colors are given to ColorEdit and ColorPicker in RGB, but pyColorEditFlags_InputHSV"
            "allows you to store colors as HSV and pass them to ColorEdit and ColorPicker as HSV. This comes with the"
            "added benefit that you can manipulate hue values with the picker even when saturation or value are zero.");
        py::Text("Color widget with InputHSV:");
        py::ColorEdit4("HSV shown as RGB##1", (float*)&color_hsv, pyColorEditFlags_DisplayRGB | pyColorEditFlags_InputHSV | pyColorEditFlags_Float);
        py::ColorEdit4("HSV shown as HSV##1", (float*)&color_hsv, pyColorEditFlags_DisplayHSV | pyColorEditFlags_InputHSV | pyColorEditFlags_Float);
        py::DragFloat4("Raw HSV values", (float*)&color_hsv, 0.01f, 0.0f, 1.0f);

        py::TreePop();
    }

    if (py::TreeNode("Drag/Slider Flags"))
    {
        // Demonstrate using advanced flags for DragXXX and SliderXXX functions. Note that the flags are the same!
        static pySliderFlags flags = pySliderFlags_None;
        py::CheckboxFlags("pySliderFlags_AlwaysClamp", &flags, pySliderFlags_AlwaysClamp);
        py::SameLine(); HelpMarker("Always clamp value to min/max bounds (if any) when input manually with CTRL+Click.");
        py::CheckboxFlags("pySliderFlags_Logarithmic", &flags, pySliderFlags_Logarithmic);
        py::SameLine(); HelpMarker("Enable logarithmic editing (more precision for small values).");
        py::CheckboxFlags("pySliderFlags_NoRoundToFormat", &flags, pySliderFlags_NoRoundToFormat);
        py::SameLine(); HelpMarker("Disable rounding underlying value to match precision of the format string (e.g. %.3f values are rounded to those 3 digits).");
        py::CheckboxFlags("pySliderFlags_NoInput", &flags, pySliderFlags_NoInput);
        py::SameLine(); HelpMarker("Disable CTRL+Click or Enter key allowing to input text directly into the widget.");

        // Drags
        static float drag_f = 0.5f;
        static int drag_i = 50;
        py::Text("Underlying float value: %f", drag_f);
        py::DragFloat("DragFloat (0 -> 1)", &drag_f, 0.005f, 0.0f, 1.0f, "%.3f", flags);
        py::DragFloat("DragFloat (0 -> +inf)", &drag_f, 0.005f, 0.0f, FLT_MAX, "%.3f", flags);
        py::DragFloat("DragFloat (-inf -> 1)", &drag_f, 0.005f, -FLT_MAX, 1.0f, "%.3f", flags);
        py::DragFloat("DragFloat (-inf -> +inf)", &drag_f, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
        py::DragInt("DragInt (0 -> 100)", &drag_i, 0.5f, 0, 100, "%d", flags);

        // Sliders
        static float slider_f = 0.5f;
        static int slider_i = 50;
        py::Text("Underlying float value: %f", slider_f);
        py::SliderFloat("SliderFloat (0 -> 1)", &slider_f, 0.0f, 1.0f, "%.3f", flags);
        py::SliderInt("SliderInt (0 -> 100)", &slider_i, 0, 100, "%d", flags);

        py::TreePop();
    }

    if (py::TreeNode("Range Widgets"))
    {
        static float begin = 10, end = 90;
        static int begin_i = 100, end_i = 1000;
        py::DragFloatRange2("range float", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%", pySliderFlags_AlwaysClamp);
        py::DragIntRange2("range int", &begin_i, &end_i, 5, 0, 1000, "Min: %d units", "Max: %d units");
        py::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %d units", "Max: %d units");
        py::TreePop();
    }

    if (py::TreeNode("Data Types"))
    {
        // DragScalar/InputScalar/SliderScalar functions allow various data types
        // - signed/unsigned
        // - 8/16/32/64-bits
        // - integer/float/double
        // To avoid polluting the public API with all possible combinations, we use the pyDataType enum
        // to pass the type, and passing all arguments by pointer.
        // This is the reason the test code below creates local variables to hold "zero" "one" etc. for each types.
        // In practice, if you frequently use a given type that is not covered by the normal API entry points,
        // you can wrap it yourself inside a 1 line function which can take typed argument as value instead of void*,
        // and then pass their address to the generic function. For example:
        //   bool MySliderU64(const char *label, u64* value, u64 min = 0, u64 max = 0, const char* format = "%lld")
        //   {
        //      return SliderScalar(label, pyDataType_U64, value, &min, &max, format);
        //   }

        // Setup limits (as helper variables so we can take their address, as explained above)
        // Note: SliderScalar() functions have a maximum usable range of half the natural type maximum, hence the /2.
        #ifndef LLONG_MIN
        ImS64 LLONG_MIN = -9223372036854775807LL - 1;
        ImS64 LLONG_MAX = 9223372036854775807LL;
        ImU64 ULLONG_MAX = (2ULL * 9223372036854775807LL + 1);
        #endif
        const char    s8_zero  = 0,   s8_one  = 1,   s8_fifty  = 50, s8_min  = -128,        s8_max = 127;
        const ImU8    u8_zero  = 0,   u8_one  = 1,   u8_fifty  = 50, u8_min  = 0,           u8_max = 255;
        const short   s16_zero = 0,   s16_one = 1,   s16_fifty = 50, s16_min = -32768,      s16_max = 32767;
        const ImU16   u16_zero = 0,   u16_one = 1,   u16_fifty = 50, u16_min = 0,           u16_max = 65535;
        const ImS32   s32_zero = 0,   s32_one = 1,   s32_fifty = 50, s32_min = INT_MIN/2,   s32_max = INT_MAX/2,    s32_hi_a = INT_MAX/2 - 100,    s32_hi_b = INT_MAX/2;
        const ImU32   u32_zero = 0,   u32_one = 1,   u32_fifty = 50, u32_min = 0,           u32_max = UINT_MAX/2,   u32_hi_a = UINT_MAX/2 - 100,   u32_hi_b = UINT_MAX/2;
        const ImS64   s64_zero = 0,   s64_one = 1,   s64_fifty = 50, s64_min = LLONG_MIN/2, s64_max = LLONG_MAX/2,  s64_hi_a = LLONG_MAX/2 - 100,  s64_hi_b = LLONG_MAX/2;
        const ImU64   u64_zero = 0,   u64_one = 1,   u64_fifty = 50, u64_min = 0,           u64_max = ULLONG_MAX/2, u64_hi_a = ULLONG_MAX/2 - 100, u64_hi_b = ULLONG_MAX/2;
        const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
        const double  f64_zero = 0.,  f64_one = 1.,  f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

        // State
        static char   s8_v  = 127;
        static ImU8   u8_v  = 255;
        static short  s16_v = 32767;
        static ImU16  u16_v = 65535;
        static ImS32  s32_v = -1;
        static ImU32  u32_v = (ImU32)-1;
        static ImS64  s64_v = -1;
        static ImU64  u64_v = (ImU64)-1;
        static float  f32_v = 0.123f;
        static double f64_v = 90000.01234567890123456789;

        const float drag_speed = 0.2f;
        static bool drag_clamp = false;
        py::Text("Drags:");
        py::Checkbox("Clamp integers to 0..50", &drag_clamp);
        py::SameLine(); HelpMarker(
            "As with every widgets in dear py, we never modify values unless there is a user interaction.\n"
            "You can override the clamping limits by using CTRL+Click to input a value.");
        py::DragScalar("drag s8",        pyDataType_S8,     &s8_v,  drag_speed, drag_clamp ? &s8_zero  : NULL, drag_clamp ? &s8_fifty  : NULL);
        py::DragScalar("drag u8",        pyDataType_U8,     &u8_v,  drag_speed, drag_clamp ? &u8_zero  : NULL, drag_clamp ? &u8_fifty  : NULL, "%u ms");
        py::DragScalar("drag s16",       pyDataType_S16,    &s16_v, drag_speed, drag_clamp ? &s16_zero : NULL, drag_clamp ? &s16_fifty : NULL);
        py::DragScalar("drag u16",       pyDataType_U16,    &u16_v, drag_speed, drag_clamp ? &u16_zero : NULL, drag_clamp ? &u16_fifty : NULL, "%u ms");
        py::DragScalar("drag s32",       pyDataType_S32,    &s32_v, drag_speed, drag_clamp ? &s32_zero : NULL, drag_clamp ? &s32_fifty : NULL);
        py::DragScalar("drag u32",       pyDataType_U32,    &u32_v, drag_speed, drag_clamp ? &u32_zero : NULL, drag_clamp ? &u32_fifty : NULL, "%u ms");
        py::DragScalar("drag s64",       pyDataType_S64,    &s64_v, drag_speed, drag_clamp ? &s64_zero : NULL, drag_clamp ? &s64_fifty : NULL);
        py::DragScalar("drag u64",       pyDataType_U64,    &u64_v, drag_speed, drag_clamp ? &u64_zero : NULL, drag_clamp ? &u64_fifty : NULL);
        py::DragScalar("drag float",     pyDataType_Float,  &f32_v, 0.005f,  &f32_zero, &f32_one, "%f");
        py::DragScalar("drag float log", pyDataType_Float,  &f32_v, 0.005f,  &f32_zero, &f32_one, "%f", pySliderFlags_Logarithmic);
        py::DragScalar("drag double",    pyDataType_Double, &f64_v, 0.0005f, &f64_zero, NULL,     "%.10f grams");
        py::DragScalar("drag double log",pyDataType_Double, &f64_v, 0.0005f, &f64_zero, &f64_one, "0 < %.10f < 1", pySliderFlags_Logarithmic);

        py::Text("Sliders");
        py::SliderScalar("slider s8 full",       pyDataType_S8,     &s8_v,  &s8_min,   &s8_max,   "%d");
        py::SliderScalar("slider u8 full",       pyDataType_U8,     &u8_v,  &u8_min,   &u8_max,   "%u");
        py::SliderScalar("slider s16 full",      pyDataType_S16,    &s16_v, &s16_min,  &s16_max,  "%d");
        py::SliderScalar("slider u16 full",      pyDataType_U16,    &u16_v, &u16_min,  &u16_max,  "%u");
        py::SliderScalar("slider s32 low",       pyDataType_S32,    &s32_v, &s32_zero, &s32_fifty,"%d");
        py::SliderScalar("slider s32 high",      pyDataType_S32,    &s32_v, &s32_hi_a, &s32_hi_b, "%d");
        py::SliderScalar("slider s32 full",      pyDataType_S32,    &s32_v, &s32_min,  &s32_max,  "%d");
        py::SliderScalar("slider u32 low",       pyDataType_U32,    &u32_v, &u32_zero, &u32_fifty,"%u");
        py::SliderScalar("slider u32 high",      pyDataType_U32,    &u32_v, &u32_hi_a, &u32_hi_b, "%u");
        py::SliderScalar("slider u32 full",      pyDataType_U32,    &u32_v, &u32_min,  &u32_max,  "%u");
        py::SliderScalar("slider s64 low",       pyDataType_S64,    &s64_v, &s64_zero, &s64_fifty,"%" IM_PRId64);
        py::SliderScalar("slider s64 high",      pyDataType_S64,    &s64_v, &s64_hi_a, &s64_hi_b, "%" IM_PRId64);
        py::SliderScalar("slider s64 full",      pyDataType_S64,    &s64_v, &s64_min,  &s64_max,  "%" IM_PRId64);
        py::SliderScalar("slider u64 low",       pyDataType_U64,    &u64_v, &u64_zero, &u64_fifty,"%" IM_PRIu64 " ms");
        py::SliderScalar("slider u64 high",      pyDataType_U64,    &u64_v, &u64_hi_a, &u64_hi_b, "%" IM_PRIu64 " ms");
        py::SliderScalar("slider u64 full",      pyDataType_U64,    &u64_v, &u64_min,  &u64_max,  "%" IM_PRIu64 " ms");
        py::SliderScalar("slider float low",     pyDataType_Float,  &f32_v, &f32_zero, &f32_one);
        py::SliderScalar("slider float low log", pyDataType_Float,  &f32_v, &f32_zero, &f32_one,  "%.10f", pySliderFlags_Logarithmic);
        py::SliderScalar("slider float high",    pyDataType_Float,  &f32_v, &f32_lo_a, &f32_hi_a, "%e");
        py::SliderScalar("slider double low",    pyDataType_Double, &f64_v, &f64_zero, &f64_one,  "%.10f grams");
        py::SliderScalar("slider double low log",pyDataType_Double, &f64_v, &f64_zero, &f64_one,  "%.10f", pySliderFlags_Logarithmic);
        py::SliderScalar("slider double high",   pyDataType_Double, &f64_v, &f64_lo_a, &f64_hi_a, "%e grams");

        py::Text("Sliders (reverse)");
        py::SliderScalar("slider s8 reverse",    pyDataType_S8,   &s8_v,  &s8_max,    &s8_min,   "%d");
        py::SliderScalar("slider u8 reverse",    pyDataType_U8,   &u8_v,  &u8_max,    &u8_min,   "%u");
        py::SliderScalar("slider s32 reverse",   pyDataType_S32,  &s32_v, &s32_fifty, &s32_zero, "%d");
        py::SliderScalar("slider u32 reverse",   pyDataType_U32,  &u32_v, &u32_fifty, &u32_zero, "%u");
        py::SliderScalar("slider s64 reverse",   pyDataType_S64,  &s64_v, &s64_fifty, &s64_zero, "%" IM_PRId64);
        py::SliderScalar("slider u64 reverse",   pyDataType_U64,  &u64_v, &u64_fifty, &u64_zero, "%" IM_PRIu64 " ms");

        static bool inputs_step = true;
        py::Text("Inputs");
        py::Checkbox("Show step buttons", &inputs_step);
        py::InputScalar("input s8",      pyDataType_S8,     &s8_v,  inputs_step ? &s8_one  : NULL, NULL, "%d");
        py::InputScalar("input u8",      pyDataType_U8,     &u8_v,  inputs_step ? &u8_one  : NULL, NULL, "%u");
        py::InputScalar("input s16",     pyDataType_S16,    &s16_v, inputs_step ? &s16_one : NULL, NULL, "%d");
        py::InputScalar("input u16",     pyDataType_U16,    &u16_v, inputs_step ? &u16_one : NULL, NULL, "%u");
        py::InputScalar("input s32",     pyDataType_S32,    &s32_v, inputs_step ? &s32_one : NULL, NULL, "%d");
        py::InputScalar("input s32 hex", pyDataType_S32,    &s32_v, inputs_step ? &s32_one : NULL, NULL, "%08X", pyInputTextFlags_CharsHexadecimal);
        py::InputScalar("input u32",     pyDataType_U32,    &u32_v, inputs_step ? &u32_one : NULL, NULL, "%u");
        py::InputScalar("input u32 hex", pyDataType_U32,    &u32_v, inputs_step ? &u32_one : NULL, NULL, "%08X", pyInputTextFlags_CharsHexadecimal);
        py::InputScalar("input s64",     pyDataType_S64,    &s64_v, inputs_step ? &s64_one : NULL);
        py::InputScalar("input u64",     pyDataType_U64,    &u64_v, inputs_step ? &u64_one : NULL);
        py::InputScalar("input float",   pyDataType_Float,  &f32_v, inputs_step ? &f32_one : NULL);
        py::InputScalar("input double",  pyDataType_Double, &f64_v, inputs_step ? &f64_one : NULL);

        py::TreePop();
    }

    if (py::TreeNode("Multi-component Widgets"))
    {
        static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
        static int vec4i[4] = { 1, 5, 100, 255 };

        py::InputFloat2("input float2", vec4f);
        py::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
        py::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
        py::InputInt2("input int2", vec4i);
        py::DragInt2("drag int2", vec4i, 1, 0, 255);
        py::SliderInt2("slider int2", vec4i, 0, 255);
        py::Spacing();

        py::InputFloat3("input float3", vec4f);
        py::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
        py::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
        py::InputInt3("input int3", vec4i);
        py::DragInt3("drag int3", vec4i, 1, 0, 255);
        py::SliderInt3("slider int3", vec4i, 0, 255);
        py::Spacing();

        py::InputFloat4("input float4", vec4f);
        py::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
        py::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
        py::InputInt4("input int4", vec4i);
        py::DragInt4("drag int4", vec4i, 1, 0, 255);
        py::SliderInt4("slider int4", vec4i, 0, 255);

        py::TreePop();
    }

    if (py::TreeNode("Vertical Sliders"))
    {
        const float spacing = 4;
        py::PushStyleVar(pyStyleVar_ItemSpacing, ImVec2(spacing, spacing));

        static int int_value = 0;
        py::VSliderInt("##int", ImVec2(18, 160), &int_value, 0, 5);
        py::SameLine();

        static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
        py::PushID("set1");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) py::SameLine();
            py::PushID(i);
            py::PushStyleColor(pyCol_FrameBg, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
            py::PushStyleColor(pyCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
            py::PushStyleColor(pyCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
            py::PushStyleColor(pyCol_SliderGrab, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
            py::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.0f, 1.0f, "");
            if (py::IsItemActive() || py::IsItemHovered())
                py::SetTooltip("%.3f", values[i]);
            py::PopStyleColor(4);
            py::PopID();
        }
        py::PopID();

        py::SameLine();
        py::PushID("set2");
        static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
        const int rows = 3;
        const ImVec2 small_slider_size(18, (float)(int)((160.0f - (rows - 1) * spacing) / rows));
        for (int nx = 0; nx < 4; nx++)
        {
            if (nx > 0) py::SameLine();
            py::BeginGroup();
            for (int ny = 0; ny < rows; ny++)
            {
                py::PushID(nx * rows + ny);
                py::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
                if (py::IsItemActive() || py::IsItemHovered())
                    py::SetTooltip("%.3f", values2[nx]);
                py::PopID();
            }
            py::EndGroup();
        }
        py::PopID();

        py::SameLine();
        py::PushID("set3");
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) py::SameLine();
            py::PushID(i);
            py::PushStyleVar(pyStyleVar_GrabMinSize, 40);
            py::VSliderFloat("##v", ImVec2(40, 160), &values[i], 0.0f, 1.0f, "%.2f\nsec");
            py::PopStyleVar();
            py::PopID();
        }
        py::PopID();
        py::PopStyleVar();
        py::TreePop();
    }

    if (py::TreeNode("Drag and Drop"))
    {
        if (py::TreeNode("Drag and drop in standard widgets"))
        {
            // ColorEdit widgets automatically act as drag source and drag target.
            // They are using standardized payload strings py_PAYLOAD_TYPE_COLOR_3F and py_PAYLOAD_TYPE_COLOR_4F
            // to allow your own widgets to use colors in their drag and drop interaction.
            // Also see 'Demo->Widgets->Color/Picker Widgets->Palette' demo.
            HelpMarker("You can drag from the color squares.");
            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            py::ColorEdit3("color 1", col1);
            py::ColorEdit4("color 2", col2);
            py::TreePop();
        }

        if (py::TreeNode("Drag and drop to copy/swap items"))
        {
            enum Mode
            {
                Mode_Copy,
                Mode_Move,
                Mode_Swap
            };
            static int mode = 0;
            if (py::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } py::SameLine();
            if (py::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } py::SameLine();
            if (py::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }
            static const char* names[9] =
            {
                "Bobby", "Beatrice", "Betty",
                "Brianna", "Barry", "Bernard",
                "Bibi", "Blaine", "Bryn"
            };
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                py::PushID(n);
                if ((n % 3) != 0)
                    py::SameLine();
                py::Button(names[n], ImVec2(60, 60));

                // Our buttons are both drag sources and drag targets here!
                if (py::BeginDragDropSource(pyDragDropFlags_None))
                {
                    // Set payload to carry the index of our item (could be anything)
                    py::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

                    // Display preview (could be anything, e.g. when dragging an image we could decide to display
                    // the filename and a small preview of the image, etc.)
                    if (mode == Mode_Copy) { py::Text("Copy %s", names[n]); }
                    if (mode == Mode_Move) { py::Text("Move %s", names[n]); }
                    if (mode == Mode_Swap) { py::Text("Swap %s", names[n]); }
                    py::EndDragDropSource();
                }
                if (py::BeginDragDropTarget())
                {
                    if (const pyPayload* payload = py::AcceptDragDropPayload("DND_DEMO_CELL"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        if (mode == Mode_Copy)
                        {
                            names[n] = names[payload_n];
                        }
                        if (mode == Mode_Move)
                        {
                            names[n] = names[payload_n];
                            names[payload_n] = "";
                        }
                        if (mode == Mode_Swap)
                        {
                            const char* tmp = names[n];
                            names[n] = names[payload_n];
                            names[payload_n] = tmp;
                        }
                    }
                    py::EndDragDropTarget();
                }
                py::PopID();
            }
            py::TreePop();
        }

        if (py::TreeNode("Drag to reorder items (simple)"))
        {
            // Simple reordering
            HelpMarker(
                "We don't use the drag and drop api at all here! "
                "Instead we query when the item is held but not hovered, and order items accordingly.");
            static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
            for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
            {
                const char* item = item_names[n];
                py::Selectable(item);

                if (py::IsItemActive() && !py::IsItemHovered())
                {
                    int n_next = n + (py::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
                    {
                        item_names[n] = item_names[n_next];
                        item_names[n_next] = item;
                        py::ResetMouseDragDelta();
                    }
                }
            }
            py::TreePop();
        }

        py::TreePop();
    }

    if (py::TreeNode("Querying Item Status (Edited/Active/Hovered etc.)"))
    {
        // Select an item type
        const char* item_names[] =
        {
            "Text", "Button", "Button (w/ repeat)", "Checkbox", "SliderFloat", "InputText", "InputFloat",
            "InputFloat3", "ColorEdit4", "Selectable", "MenuItem", "TreeNode", "TreeNode (w/ double-click)", "Combo", "ListBox"
        };
        static int item_type = 4;
        static bool item_disabled = false;
        py::Combo("Item Type", &item_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));
        py::SameLine();
        HelpMarker("Testing how various types of items are interacting with the IsItemXXX functions. Note that the bool return value of most py function is generally equivalent to calling py::IsItemHovered().");
        py::Checkbox("Item Disabled",  &item_disabled);

        // Submit selected item item so we can query their status in the code following it.
        bool ret = false;
        static bool b = false;
        static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
        static char str[16] = {};
        if (item_disabled)
            py::BeginDisabled(true);
        if (item_type == 0) { py::Text("ITEM: Text"); }                                              // Testing text items with no identifier/interaction
        if (item_type == 1) { ret = py::Button("ITEM: Button"); }                                    // Testing button
        if (item_type == 2) { py::PushButtonRepeat(true); ret = py::Button("ITEM: Button"); py::PopButtonRepeat(); } // Testing button (with repeater)
        if (item_type == 3) { ret = py::Checkbox("ITEM: Checkbox", &b); }                            // Testing checkbox
        if (item_type == 4) { ret = py::SliderFloat("ITEM: SliderFloat", &col4f[0], 0.0f, 1.0f); }   // Testing basic item
        if (item_type == 5) { ret = py::InputText("ITEM: InputText", &str[0], IM_ARRAYSIZE(str)); }  // Testing input text (which handles tabbing)
        if (item_type == 6) { ret = py::InputFloat("ITEM: InputFloat", col4f, 1.0f); }               // Testing +/- buttons on scalar input
        if (item_type == 7) { ret = py::InputFloat3("ITEM: InputFloat3", col4f); }                   // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 8) { ret = py::ColorEdit4("ITEM: ColorEdit4", col4f); }                     // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 9) { ret = py::Selectable("ITEM: Selectable"); }                            // Testing selectable item
        if (item_type == 10){ ret = py::MenuItem("ITEM: MenuItem"); }                                // Testing menu item (they use pyButtonFlags_PressedOnRelease button policy)
        if (item_type == 11){ ret = py::TreeNode("ITEM: TreeNode"); if (ret) py::TreePop(); }     // Testing tree node
        if (item_type == 12){ ret = py::TreeNodeEx("ITEM: TreeNode w/ pyTreeNodeFlags_OpenOnDoubleClick", pyTreeNodeFlags_OpenOnDoubleClick | pyTreeNodeFlags_NoTreePushOnOpen); } // Testing tree node with pyButtonFlags_PressedOnDoubleClick button policy.
        if (item_type == 13){ const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = py::Combo("ITEM: Combo", &current, items, IM_ARRAYSIZE(items)); }
        if (item_type == 14){ const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = py::ListBox("ITEM: ListBox", &current, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)); }

        // Display the values of IsItemHovered() and other common item state functions.
        // Note that the pyHoveredFlags_XXX flags can be combined.
        // Because BulletText is an item itself and that would affect the output of IsItemXXX functions,
        // we query every state in a single call to avoid storing them and to simplify the code.
        py::BulletText(
            "Return value = %d\n"
            "IsItemFocused() = %d\n"
            "IsItemHovered() = %d\n"
            "IsItemHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsItemHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsItemHovered(_AllowWhenOverlapped) = %d\n"
            "IsItemHovered(_AllowWhenDisabled) = %d\n"
            "IsItemHovered(_RectOnly) = %d\n"
            "IsItemActive() = %d\n"
            "IsItemEdited() = %d\n"
            "IsItemActivated() = %d\n"
            "IsItemDeactivated() = %d\n"
            "IsItemDeactivatedAfterEdit() = %d\n"
            "IsItemVisible() = %d\n"
            "IsItemClicked() = %d\n"
            "IsItemToggledOpen() = %d\n"
            "GetItemRectMin() = (%.1f, %.1f)\n"
            "GetItemRectMax() = (%.1f, %.1f)\n"
            "GetItemRectSize() = (%.1f, %.1f)",
            ret,
            py::IsItemFocused(),
            py::IsItemHovered(),
            py::IsItemHovered(pyHoveredFlags_AllowWhenBlockedByPopup),
            py::IsItemHovered(pyHoveredFlags_AllowWhenBlockedByActiveItem),
            py::IsItemHovered(pyHoveredFlags_AllowWhenOverlapped),
            py::IsItemHovered(pyHoveredFlags_AllowWhenDisabled),
            py::IsItemHovered(pyHoveredFlags_RectOnly),
            py::IsItemActive(),
            py::IsItemEdited(),
            py::IsItemActivated(),
            py::IsItemDeactivated(),
            py::IsItemDeactivatedAfterEdit(),
            py::IsItemVisible(),
            py::IsItemClicked(),
            py::IsItemToggledOpen(),
            py::GetItemRectMin().x, py::GetItemRectMin().y,
            py::GetItemRectMax().x, py::GetItemRectMax().y,
            py::GetItemRectSize().x, py::GetItemRectSize().y
        );

        if (item_disabled)
            py::EndDisabled();

        char buf[1] = "";
        py::InputText("unused", buf, IM_ARRAYSIZE(buf), pyInputTextFlags_ReadOnly);
        py::SameLine();
        HelpMarker("This widget is only here to be able to tab-out of the widgets above and see e.g. Deactivated() status.");

        py::TreePop();
    }

    if (py::TreeNode("Querying Window Status (Focused/Hovered etc.)"))
    {
        static bool embed_all_inside_a_child_window = false;
        py::Checkbox("Embed everything inside a child window for testing _RootWindow flag.", &embed_all_inside_a_child_window);
        if (embed_all_inside_a_child_window)
            py::BeginChild("outer_child", ImVec2(0, py::GetFontSize() * 20.0f), true);

        // Testing IsWindowFocused() function with its various flags.
        py::BulletText(
            "IsWindowFocused() = %d\n"
            "IsWindowFocused(_ChildWindows) = %d\n"
            "IsWindowFocused(_ChildWindows|_NoPopupHierarchy) = %d\n"
            "IsWindowFocused(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowFocused(_ChildWindows|_RootWindow|_NoPopupHierarchy) = %d\n"
            "IsWindowFocused(_RootWindow) = %d\n"
            "IsWindowFocused(_RootWindow|_NoPopupHierarchy) = %d\n"
            "IsWindowFocused(_AnyWindow) = %d\n",
            py::IsWindowFocused(),
            py::IsWindowFocused(pyFocusedFlags_ChildWindows),
            py::IsWindowFocused(pyFocusedFlags_ChildWindows | pyFocusedFlags_NoPopupHierarchy),
            py::IsWindowFocused(pyFocusedFlags_ChildWindows | pyFocusedFlags_RootWindow),
            py::IsWindowFocused(pyFocusedFlags_ChildWindows | pyFocusedFlags_RootWindow | pyFocusedFlags_NoPopupHierarchy),
            py::IsWindowFocused(pyFocusedFlags_RootWindow),
            py::IsWindowFocused(pyFocusedFlags_RootWindow | pyFocusedFlags_NoPopupHierarchy),
            py::IsWindowFocused(pyFocusedFlags_AnyWindow));

        // Testing IsWindowHovered() function with its various flags.
        py::BulletText(
            "IsWindowHovered() = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsWindowHovered(_ChildWindows) = %d\n"
            "IsWindowHovered(_ChildWindows|_NoPopupHierarchy) = %d\n"
            "IsWindowHovered(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowHovered(_ChildWindows|_RootWindow|_NoPopupHierarchy) = %d\n"
            "IsWindowHovered(_RootWindow) = %d\n"
            "IsWindowHovered(_RootWindow|_NoPopupHierarchy) = %d\n"
            "IsWindowHovered(_ChildWindows|_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_AnyWindow) = %d\n",
            py::IsWindowHovered(),
            py::IsWindowHovered(pyHoveredFlags_AllowWhenBlockedByPopup),
            py::IsWindowHovered(pyHoveredFlags_AllowWhenBlockedByActiveItem),
            py::IsWindowHovered(pyHoveredFlags_ChildWindows),
            py::IsWindowHovered(pyHoveredFlags_ChildWindows | pyHoveredFlags_NoPopupHierarchy),
            py::IsWindowHovered(pyHoveredFlags_ChildWindows | pyHoveredFlags_RootWindow),
            py::IsWindowHovered(pyHoveredFlags_ChildWindows | pyHoveredFlags_RootWindow | pyHoveredFlags_NoPopupHierarchy),
            py::IsWindowHovered(pyHoveredFlags_RootWindow),
            py::IsWindowHovered(pyHoveredFlags_RootWindow | pyHoveredFlags_NoPopupHierarchy),
            py::IsWindowHovered(pyHoveredFlags_ChildWindows | pyHoveredFlags_AllowWhenBlockedByPopup),
            py::IsWindowHovered(pyHoveredFlags_AnyWindow));

        py::BeginChild("child", ImVec2(0, 50), true);
        py::Text("This is another child window for testing the _ChildWindows flag.");
        py::EndChild();
        if (embed_all_inside_a_child_window)
            py::EndChild();

        // Calling IsItemHovered() after begin returns the hovered status of the title bar.
        // This is useful in particular if you want to create a context menu associated to the title bar of a window.
        static bool test_window = false;
        py::Checkbox("Hovered/Active tests after Begin() for title bar testing", &test_window);
        if (test_window)
        {
            py::Begin("Title bar Hovered/Active tests", &test_window);
            if (py::BeginPopupContextItem()) // <-- This is using IsItemHovered()
            {
                if (py::MenuItem("Close")) { test_window = false; }
                py::EndPopup();
            }
            py::Text(
                "IsItemHovered() after begin = %d (== is title bar hovered)\n"
                "IsItemActive() after begin = %d (== is window being clicked/moved)\n",
                py::IsItemHovered(), py::IsItemActive());
            py::End();
        }

        py::TreePop();
    }

    // Demonstrate BeginDisabled/EndDisabled using a checkbox located at the bottom of the section (which is a bit odd:
    // logically we'd have this checkbox at the top of the section, but we don't want this feature to steal that space)
    if (disable_all)
        py::EndDisabled();

    if (py::TreeNode("Disable block"))
    {
        py::Checkbox("Disable entire section above", &disable_all);
        py::SameLine(); HelpMarker("Demonstrate using BeginDisabled()/EndDisabled() across this section.");
        py::TreePop();
    }
}

static void ShowDemoWindowLayout()
{
    if (!py::CollapsingHeader("Layout & Scrolling"))
        return;

    if (py::TreeNode("Child windows"))
    {
        HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        py::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        py::Checkbox("Disable Menu", &disable_menu);

        // Child 1: no border, enable horizontal scrollbar
        {
            pyWindowFlags window_flags = pyWindowFlags_HorizontalScrollbar;
            if (disable_mouse_wheel)
                window_flags |= pyWindowFlags_NoScrollWithMouse;
            py::BeginChild("ChildL", ImVec2(py::GetContentRegionAvail().x * 0.5f, 260), false, window_flags);
            for (int i = 0; i < 100; i++)
                py::Text("%04d: scrollable region", i);
            py::EndChild();
        }

        py::SameLine();

        // Child 2: rounded border
        {
            pyWindowFlags window_flags = pyWindowFlags_None;
            if (disable_mouse_wheel)
                window_flags |= pyWindowFlags_NoScrollWithMouse;
            if (!disable_menu)
                window_flags |= pyWindowFlags_MenuBar;
            py::PushStyleVar(pyStyleVar_ChildRounding, 5.0f);
            py::BeginChild("ChildR", ImVec2(0, 260), true, window_flags);
            if (!disable_menu && py::BeginMenuBar())
            {
                if (py::BeginMenu("Menu"))
                {
                    ShowExampleMenuFile();
                    py::EndMenu();
                }
                py::EndMenuBar();
            }
            if (py::BeginTable("split", 2, pyTableFlags_Resizable | pyTableFlags_NoSavedSettings))
            {
                for (int i = 0; i < 100; i++)
                {
                    char buf[32];
                    sprintf(buf, "%03d", i);
                    py::TableNextColumn();
                    py::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                }
                py::EndTable();
            }
            py::EndChild();
            py::PopStyleVar();
        }

        py::Separator();

        // Demonstrate a few extra things
        // - Changing pyCol_ChildBg (which is transparent black in default styles)
        // - Using SetCursorPos() to position child window (the child window is an item from the POV of parent window)
        //   You can also call SetNextWindowPos() to position the child window. The parent window will effectively
        //   layout from this position.
        // - Using py::GetItemRectMin/Max() to query the "item" state (because the child window is an item from
        //   the POV of the parent window). See 'Demo->Querying Status (Edited/Active/Hovered etc.)' for details.
        {
            static int offset_x = 0;
            py::SetNextItemWidth(py::GetFontSize() * 8);
            py::DragInt("Offset X", &offset_x, 1.0f, -1000, 1000);

            py::SetCursorPosX(py::GetCursorPosX() + (float)offset_x);
            py::PushStyleColor(pyCol_ChildBg, IM_COL32(255, 0, 0, 100));
            py::BeginChild("Red", ImVec2(200, 100), true, pyWindowFlags_None);
            for (int n = 0; n < 50; n++)
                py::Text("Some test %d", n);
            py::EndChild();
            bool child_is_hovered = py::IsItemHovered();
            ImVec2 child_rect_min = py::GetItemRectMin();
            ImVec2 child_rect_max = py::GetItemRectMax();
            py::PopStyleColor();
            py::Text("Hovered: %d", child_is_hovered);
            py::Text("Rect of child window is: (%.0f,%.0f) (%.0f,%.0f)", child_rect_min.x, child_rect_min.y, child_rect_max.x, child_rect_max.y);
        }

        py::TreePop();
    }

    if (py::TreeNode("Widgets Width"))
    {
        static float f = 0.0f;
        static bool show_indented_items = true;
        py::Checkbox("Show indented items", &show_indented_items);

        // Use SetNextItemWidth() to set the width of a single upcoming item.
        // Use PushItemWidth()/PopItemWidth() to set the width of a group of items.
        // In real code use you'll probably want to choose width values that are proportional to your font size
        // e.g. Using '20.0f * GetFontSize()' as width instead of '200.0f', etc.

        py::Text("SetNextItemWidth/PushItemWidth(100)");
        py::SameLine(); HelpMarker("Fixed width.");
        py::PushItemWidth(100);
        py::DragFloat("float##1b", &f);
        if (show_indented_items)
        {
            py::Indent();
            py::DragFloat("float (indented)##1b", &f);
            py::Unindent();
        }
        py::PopItemWidth();

        py::Text("SetNextItemWidth/PushItemWidth(-100)");
        py::SameLine(); HelpMarker("Align to right edge minus 100");
        py::PushItemWidth(-100);
        py::DragFloat("float##2a", &f);
        if (show_indented_items)
        {
            py::Indent();
            py::DragFloat("float (indented)##2b", &f);
            py::Unindent();
        }
        py::PopItemWidth();

        py::Text("SetNextItemWidth/PushItemWidth(GetContentRegionAvail().x * 0.5f)");
        py::SameLine(); HelpMarker("Half of available width.\n(~ right-cursor_pos)\n(works within a column set)");
        py::PushItemWidth(py::GetContentRegionAvail().x * 0.5f);
        py::DragFloat("float##3a", &f);
        if (show_indented_items)
        {
            py::Indent();
            py::DragFloat("float (indented)##3b", &f);
            py::Unindent();
        }
        py::PopItemWidth();

        py::Text("SetNextItemWidth/PushItemWidth(-GetContentRegionAvail().x * 0.5f)");
        py::SameLine(); HelpMarker("Align to right edge minus half");
        py::PushItemWidth(-py::GetContentRegionAvail().x * 0.5f);
        py::DragFloat("float##4a", &f);
        if (show_indented_items)
        {
            py::Indent();
            py::DragFloat("float (indented)##4b", &f);
            py::Unindent();
        }
        py::PopItemWidth();

        // Demonstrate using PushItemWidth to surround three items.
        // Calling SetNextItemWidth() before each of them would have the same effect.
        py::Text("SetNextItemWidth/PushItemWidth(-FLT_MIN)");
        py::SameLine(); HelpMarker("Align to right edge");
        py::PushItemWidth(-FLT_MIN);
        py::DragFloat("##float5a", &f);
        if (show_indented_items)
        {
            py::Indent();
            py::DragFloat("float (indented)##5b", &f);
            py::Unindent();
        }
        py::PopItemWidth();

        py::TreePop();
    }

    if (py::TreeNode("Basic Horizontal Layout"))
    {
        py::TextWrapped("(Use py::SameLine() to keep adding items to the right of the preceding item)");

        // Text
        py::Text("Two items: Hello"); py::SameLine();
        py::TextColored(ImVec4(1,1,0,1), "Sailor");

        // Adjust spacing
        py::Text("More spacing: Hello"); py::SameLine(0, 20);
        py::TextColored(ImVec4(1,1,0,1), "Sailor");

        // Button
        py::AlignTextToFramePadding();
        py::Text("Normal buttons"); py::SameLine();
        py::Button("Banana"); py::SameLine();
        py::Button("Apple"); py::SameLine();
        py::Button("Corniflower");

        // Button
        py::Text("Small buttons"); py::SameLine();
        py::SmallButton("Like this one"); py::SameLine();
        py::Text("can fit within a text block.");

        // Aligned to arbitrary position. Easy/cheap column.
        py::Text("Aligned");
        py::SameLine(150); py::Text("x=150");
        py::SameLine(300); py::Text("x=300");
        py::Text("Aligned");
        py::SameLine(150); py::SmallButton("x=150");
        py::SameLine(300); py::SmallButton("x=300");

        // Checkbox
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        py::Checkbox("My", &c1); py::SameLine();
        py::Checkbox("Tailor", &c2); py::SameLine();
        py::Checkbox("Is", &c3); py::SameLine();
        py::Checkbox("Rich", &c4);

        // Various
        static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
        py::PushItemWidth(80);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
        static int item = -1;
        py::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); py::SameLine();
        py::SliderFloat("X", &f0, 0.0f, 5.0f); py::SameLine();
        py::SliderFloat("Y", &f1, 0.0f, 5.0f); py::SameLine();
        py::SliderFloat("Z", &f2, 0.0f, 5.0f);
        py::PopItemWidth();

        py::PushItemWidth(80);
        py::Text("Lists:");
        static int selection[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) py::SameLine();
            py::PushID(i);
            py::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
            py::PopID();
            //if (py::IsItemHovered()) py::SetTooltip("ListBox %d hovered", i);
        }
        py::PopItemWidth();

        // Dummy
        ImVec2 button_sz(40, 40);
        py::Button("A", button_sz); py::SameLine();
        py::Dummy(button_sz); py::SameLine();
        py::Button("B", button_sz);

        // Manually wrapping
        // (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
        py::Text("Manually wrapping:");
        pyStyle& style = py::GetStyle();
        int buttons_count = 20;
        float window_visible_x2 = py::GetWindowPos().x + py::GetWindowContentRegionMax().x;
        for (int n = 0; n < buttons_count; n++)
        {
            py::PushID(n);
            py::Button("Box", button_sz);
            float last_button_x2 = py::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
            if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                py::SameLine();
            py::PopID();
        }

        py::TreePop();
    }

    if (py::TreeNode("Groups"))
    {
        HelpMarker(
            "BeginGroup() basically locks the horizontal position for new line. "
            "EndGroup() bundles the whole group so that you can use \"item\" functions such as "
            "IsItemHovered()/IsItemActive() or SameLine() etc. on the whole group.");
        py::BeginGroup();
        {
            py::BeginGroup();
            py::Button("AAA");
            py::SameLine();
            py::Button("BBB");
            py::SameLine();
            py::BeginGroup();
            py::Button("CCC");
            py::Button("DDD");
            py::EndGroup();
            py::SameLine();
            py::Button("EEE");
            py::EndGroup();
            if (py::IsItemHovered())
                py::SetTooltip("First group hovered");
        }
        // Capture the group size and create widgets using the same size
        ImVec2 size = py::GetItemRectSize();
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        py::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        py::Button("ACTION", ImVec2((size.x - py::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        py::SameLine();
        py::Button("REACTION", ImVec2((size.x - py::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        py::EndGroup();
        py::SameLine();

        py::Button("LEVERAGE\nBUZZWORD", size);
        py::SameLine();

        if (py::BeginListBox("List", size))
        {
            py::Selectable("Selected", true);
            py::Selectable("Not Selected", false);
            py::EndListBox();
        }

        py::TreePop();
    }

    if (py::TreeNode("Text Baseline Alignment"))
    {
        {
            py::BulletText("Text baseline:");
            py::SameLine(); HelpMarker(
                "This is testing the vertical alignment that gets applied on text to keep it aligned with widgets. "
                "Lines only composed of text or \"small\" widgets use less vertical space than lines with framed widgets.");
            py::Indent();

            py::Text("KO Blahblah"); py::SameLine();
            py::Button("Some framed item"); py::SameLine();
            HelpMarker("Baseline of button will look misaligned with text..");

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            // (because we don't know what's coming after the Text() statement, we need to move the text baseline
            // down by FramePadding.y ahead of time)
            py::AlignTextToFramePadding();
            py::Text("OK Blahblah"); py::SameLine();
            py::Button("Some framed item"); py::SameLine();
            HelpMarker("We call AlignTextToFramePadding() to vertically align the text baseline by +FramePadding.y");

            // SmallButton() uses the same vertical padding as Text
            py::Button("TEST##1"); py::SameLine();
            py::Text("TEST"); py::SameLine();
            py::SmallButton("TEST##2");

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            py::AlignTextToFramePadding();
            py::Text("Text aligned to framed item"); py::SameLine();
            py::Button("Item##1"); py::SameLine();
            py::Text("Item"); py::SameLine();
            py::SmallButton("Item##2"); py::SameLine();
            py::Button("Item##3");

            py::Unindent();
        }

        py::Spacing();

        {
            py::BulletText("Multi-line text:");
            py::Indent();
            py::Text("One\nTwo\nThree"); py::SameLine();
            py::Text("Hello\nWorld"); py::SameLine();
            py::Text("Banana");

            py::Text("Banana"); py::SameLine();
            py::Text("Hello\nWorld"); py::SameLine();
            py::Text("One\nTwo\nThree");

            py::Button("HOP##1"); py::SameLine();
            py::Text("Banana"); py::SameLine();
            py::Text("Hello\nWorld"); py::SameLine();
            py::Text("Banana");

            py::Button("HOP##2"); py::SameLine();
            py::Text("Hello\nWorld"); py::SameLine();
            py::Text("Banana");
            py::Unindent();
        }

        py::Spacing();

        {
            py::BulletText("Misc items:");
            py::Indent();

            // SmallButton() sets FramePadding to zero. Text baseline is aligned to match baseline of previous Button.
            py::Button("80x80", ImVec2(80, 80));
            py::SameLine();
            py::Button("50x50", ImVec2(50, 50));
            py::SameLine();
            py::Button("Button()");
            py::SameLine();
            py::SmallButton("SmallButton()");

            // Tree
            const float spacing = py::GetStyle().ItemInnerSpacing.x;
            py::Button("Button##1");
            py::SameLine(0.0f, spacing);
            if (py::TreeNode("Node##1"))
            {
                // Placeholder tree data
                for (int i = 0; i < 6; i++)
                    py::BulletText("Item %d..", i);
                py::TreePop();
            }

            // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget.
            // Otherwise you can use SmallButton() (smaller fit).
            py::AlignTextToFramePadding();

            // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add
            // other contents below the node.
            bool node_open = py::TreeNode("Node##2");
            py::SameLine(0.0f, spacing); py::Button("Button##2");
            if (node_open)
            {
                // Placeholder tree data
                for (int i = 0; i < 6; i++)
                    py::BulletText("Item %d..", i);
                py::TreePop();
            }

            // Bullet
            py::Button("Button##3");
            py::SameLine(0.0f, spacing);
            py::BulletText("Bullet text");

            py::AlignTextToFramePadding();
            py::BulletText("Node");
            py::SameLine(0.0f, spacing); py::Button("Button##4");
            py::Unindent();
        }

        py::TreePop();
    }

    if (py::TreeNode("Scrolling"))
    {
        // Vertical scroll functions
        HelpMarker("Use SetScrollHereY() or SetScrollFromPosY() to scroll to a given vertical position.");

        static int track_item = 50;
        static bool enable_track = true;
        static bool enable_extra_decorations = false;
        static float scroll_to_off_px = 0.0f;
        static float scroll_to_pos_px = 200.0f;

        py::Checkbox("Decoration", &enable_extra_decorations);

        py::Checkbox("Track", &enable_track);
        py::PushItemWidth(100);
        py::SameLine(140); enable_track |= py::DragInt("##item", &track_item, 0.25f, 0, 99, "Item = %d");

        bool scroll_to_off = py::Button("Scroll Offset");
        py::SameLine(140); scroll_to_off |= py::DragFloat("##off", &scroll_to_off_px, 1.00f, 0, FLT_MAX, "+%.0f px");

        bool scroll_to_pos = py::Button("Scroll To Pos");
        py::SameLine(140); scroll_to_pos |= py::DragFloat("##pos", &scroll_to_pos_px, 1.00f, -10, FLT_MAX, "X/Y = %.0f px");
        py::PopItemWidth();

        if (scroll_to_off || scroll_to_pos)
            enable_track = false;

        pyStyle& style = py::GetStyle();
        float child_w = (py::GetContentRegionAvail().x - 4 * style.ItemSpacing.x) / 5;
        if (child_w < 1.0f)
            child_w = 1.0f;
        py::PushID("##VerticalScrolling");
        for (int i = 0; i < 5; i++)
        {
            if (i > 0) py::SameLine();
            py::BeginGroup();
            const char* names[] = { "Top", "25%", "Center", "75%", "Bottom" };
            py::TextUnformatted(names[i]);

            const pyWindowFlags child_flags = enable_extra_decorations ? pyWindowFlags_MenuBar : 0;
            const pyID child_id = py::GetID((void*)(intptr_t)i);
            const bool child_is_visible = py::BeginChild(child_id, ImVec2(child_w, 200.0f), true, child_flags);
            if (py::BeginMenuBar())
            {
                py::TextUnformatted("abc");
                py::EndMenuBar();
            }
            if (scroll_to_off)
                py::SetScrollY(scroll_to_off_px);
            if (scroll_to_pos)
                py::SetScrollFromPosY(py::GetCursorStartPos().y + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (enable_track && item == track_item)
                    {
                        py::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        py::SetScrollHereY(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                    }
                    else
                    {
                        py::Text("Item %d", item);
                    }
                }
            }
            float scroll_y = py::GetScrollY();
            float scroll_max_y = py::GetScrollMaxY();
            py::EndChild();
            py::Text("%.0f/%.0f", scroll_y, scroll_max_y);
            py::EndGroup();
        }
        py::PopID();

        // Horizontal scroll functions
        py::Spacing();
        HelpMarker(
            "Use SetScrollHereX() or SetScrollFromPosX() to scroll to a given horizontal position.\n\n"
            "Because the clipping rectangle of most window hides half worth of WindowPadding on the "
            "left/right, using SetScrollFromPosX(+1) will usually result in clipped text whereas the "
            "equivalent SetScrollFromPosY(+1) wouldn't.");
        py::PushID("##HorizontalScrolling");
        for (int i = 0; i < 5; i++)
        {
            float child_height = py::GetTextLineHeight() + style.ScrollbarSize + style.WindowPadding.y * 2.0f;
            pyWindowFlags child_flags = pyWindowFlags_HorizontalScrollbar | (enable_extra_decorations ? pyWindowFlags_AlwaysVerticalScrollbar : 0);
            pyID child_id = py::GetID((void*)(intptr_t)i);
            bool child_is_visible = py::BeginChild(child_id, ImVec2(-100, child_height), true, child_flags);
            if (scroll_to_off)
                py::SetScrollX(scroll_to_off_px);
            if (scroll_to_pos)
                py::SetScrollFromPosX(py::GetCursorStartPos().x + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (item > 0)
                        py::SameLine();
                    if (enable_track && item == track_item)
                    {
                        py::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        py::SetScrollHereX(i * 0.25f); // 0.0f:left, 0.5f:center, 1.0f:right
                    }
                    else
                    {
                        py::Text("Item %d", item);
                    }
                }
            }
            float scroll_x = py::GetScrollX();
            float scroll_max_x = py::GetScrollMaxX();
            py::EndChild();
            py::SameLine();
            const char* names[] = { "Left", "25%", "Center", "75%", "Right" };
            py::Text("%s\n%.0f/%.0f", names[i], scroll_x, scroll_max_x);
            py::Spacing();
        }
        py::PopID();

        // Miscellaneous Horizontal Scrolling Demo
        HelpMarker(
            "Horizontal scrolling for a window is enabled via the pyWindowFlags_HorizontalScrollbar flag.\n\n"
            "You may want to also explicitly specify content width by using SetNextWindowContentWidth() before Begin().");
        static int lines = 7;
        py::SliderInt("Lines", &lines, 1, 15);
        py::PushStyleVar(pyStyleVar_FrameRounding, 3.0f);
        py::PushStyleVar(pyStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImVec2 scrolling_child_size = ImVec2(0, py::GetFrameHeightWithSpacing() * 7 + 30);
        py::BeginChild("scrolling", scrolling_child_size, true, pyWindowFlags_HorizontalScrollbar);
        for (int line = 0; line < lines; line++)
        {
            // Display random stuff. For the sake of this trivial demo we are using basic Button() + SameLine()
            // If you want to create your own time line for a real application you may be better off manipulating
            // the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets
            // yourself. You may also want to use the lower-level ImDrawList API.
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) py::SameLine();
                py::PushID(n + line * 1000);
                char num_buf[16];
                sprintf(num_buf, "%d", n);
                const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;
                float hue = n * 0.05f;
                py::PushStyleColor(pyCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                py::PushStyleColor(pyCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                py::PushStyleColor(pyCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                py::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                py::PopStyleColor(3);
                py::PopID();
            }
        }
        float scroll_x = py::GetScrollX();
        float scroll_max_x = py::GetScrollMaxX();
        py::EndChild();
        py::PopStyleVar(2);
        float scroll_x_delta = 0.0f;
        py::SmallButton("<<");
        if (py::IsItemActive())
            scroll_x_delta = -py::GetIO().DeltaTime * 1000.0f;
        py::SameLine();
        py::Text("Scroll from code"); py::SameLine();
        py::SmallButton(">>");
        if (py::IsItemActive())
            scroll_x_delta = +py::GetIO().DeltaTime * 1000.0f;
        py::SameLine();
        py::Text("%.0f/%.0f", scroll_x, scroll_max_x);
        if (scroll_x_delta != 0.0f)
        {
            // Demonstrate a trick: you can use Begin to set yourself in the context of another window
            // (here we are already out of your child window)
            py::BeginChild("scrolling");
            py::SetScrollX(py::GetScrollX() + scroll_x_delta);
            py::EndChild();
        }
        py::Spacing();

        static bool show_horizontal_contents_size_demo_window = false;
        py::Checkbox("Show Horizontal contents size demo window", &show_horizontal_contents_size_demo_window);

        if (show_horizontal_contents_size_demo_window)
        {
            static bool show_h_scrollbar = true;
            static bool show_button = true;
            static bool show_tree_nodes = true;
            static bool show_text_wrapped = false;
            static bool show_columns = true;
            static bool show_tab_bar = true;
            static bool show_child = false;
            static bool explicit_content_size = false;
            static float contents_size_x = 300.0f;
            if (explicit_content_size)
                py::SetNextWindowContentSize(ImVec2(contents_size_x, 0.0f));
            py::Begin("Horizontal contents size demo window", &show_horizontal_contents_size_demo_window, show_h_scrollbar ? pyWindowFlags_HorizontalScrollbar : 0);
            py::PushStyleVar(pyStyleVar_ItemSpacing, ImVec2(2, 0));
            py::PushStyleVar(pyStyleVar_FramePadding, ImVec2(2, 0));
            HelpMarker("Test of different widgets react and impact the work rectangle growing when horizontal scrolling is enabled.\n\nUse 'Metrics->Tools->Show windows rectangles' to visualize rectangles.");
            py::Checkbox("H-scrollbar", &show_h_scrollbar);
            py::Checkbox("Button", &show_button);            // Will grow contents size (unless explicitly overwritten)
            py::Checkbox("Tree nodes", &show_tree_nodes);    // Will grow contents size and display highlight over full width
            py::Checkbox("Text wrapped", &show_text_wrapped);// Will grow and use contents size
            py::Checkbox("Columns", &show_columns);          // Will use contents size
            py::Checkbox("Tab bar", &show_tab_bar);          // Will use contents size
            py::Checkbox("Child", &show_child);              // Will grow and use contents size
            py::Checkbox("Explicit content size", &explicit_content_size);
            py::Text("Scroll %.1f/%.1f %.1f/%.1f", py::GetScrollX(), py::GetScrollMaxX(), py::GetScrollY(), py::GetScrollMaxY());
            if (explicit_content_size)
            {
                py::SameLine();
                py::SetNextItemWidth(100);
                py::DragFloat("##csx", &contents_size_x);
                ImVec2 p = py::GetCursorScreenPos();
                py::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 10, p.y + 10), IM_COL32_WHITE);
                py::GetWindowDrawList()->AddRectFilled(ImVec2(p.x + contents_size_x - 10, p.y), ImVec2(p.x + contents_size_x, p.y + 10), IM_COL32_WHITE);
                py::Dummy(ImVec2(0, 10));
            }
            py::PopStyleVar(2);
            py::Separator();
            if (show_button)
            {
                py::Button("this is a 300-wide button", ImVec2(300, 0));
            }
            if (show_tree_nodes)
            {
                bool open = true;
                if (py::TreeNode("this is a tree node"))
                {
                    if (py::TreeNode("another one of those tree node..."))
                    {
                        py::Text("Some tree contents");
                        py::TreePop();
                    }
                    py::TreePop();
                }
                py::CollapsingHeader("CollapsingHeader", &open);
            }
            if (show_text_wrapped)
            {
                py::TextWrapped("This text should automatically wrap on the edge of the work rectangle.");
            }
            if (show_columns)
            {
                py::Text("Tables:");
                if (py::BeginTable("table", 4, pyTableFlags_Borders))
                {
                    for (int n = 0; n < 4; n++)
                    {
                        py::TableNextColumn();
                        py::Text("Width %.2f", py::GetContentRegionAvail().x);
                    }
                    py::EndTable();
                }
                py::Text("Columns:");
                py::Columns(4);
                for (int n = 0; n < 4; n++)
                {
                    py::Text("Width %.2f", py::GetColumnWidth());
                    py::NextColumn();
                }
                py::Columns(1);
            }
            if (show_tab_bar && py::BeginTabBar("Hello"))
            {
                if (py::BeginTabItem("OneOneOne")) { py::EndTabItem(); }
                if (py::BeginTabItem("TwoTwoTwo")) { py::EndTabItem(); }
                if (py::BeginTabItem("ThreeThreeThree")) { py::EndTabItem(); }
                if (py::BeginTabItem("FourFourFour")) { py::EndTabItem(); }
                py::EndTabBar();
            }
            if (show_child)
            {
                py::BeginChild("child", ImVec2(0, 0), true);
                py::EndChild();
            }
            py::End();
        }

        py::TreePop();
    }

    if (py::TreeNode("Clipping"))
    {
        static ImVec2 size(100.0f, 100.0f);
        static ImVec2 offset(30.0f, 30.0f);
        py::DragFloat2("size", (float*)&size, 0.5f, 1.0f, 200.0f, "%.0f");
        py::TextWrapped("(Click and drag to scroll)");

        for (int n = 0; n < 3; n++)
        {
            if (n > 0)
                py::SameLine();
            py::PushID(n);
            py::BeginGroup(); // Lock X position

            py::InvisibleButton("##empty", size);
            if (py::IsItemActive() && py::IsMouseDragging(pyMouseButton_Left))
            {
                offset.x += py::GetIO().MouseDelta.x;
                offset.y += py::GetIO().MouseDelta.y;
            }
            const ImVec2 p0 = py::GetItemRectMin();
            const ImVec2 p1 = py::GetItemRectMax();
            const char* text_str = "Line 1 hello\nLine 2 clip me!";
            const ImVec2 text_pos = ImVec2(p0.x + offset.x, p0.y + offset.y);
            ImDrawList* draw_list = py::GetWindowDrawList();

            switch (n)
            {
            case 0:
                HelpMarker(
                    "Using py::PushClipRect():\n"
                    "Will alter py hit-testing logic + ImDrawList rendering.\n"
                    "(use this if you want your clipping rectangle to affect interactions)");
                py::PushClipRect(p0, p1, true);
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
                py::PopClipRect();
                break;
            case 1:
                HelpMarker(
                    "Using ImDrawList::PushClipRect():\n"
                    "Will alter ImDrawList rendering only.\n"
                    "(use this as a shortcut if you are only using ImDrawList calls)");
                draw_list->PushClipRect(p0, p1, true);
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
                draw_list->PopClipRect();
                break;
            case 2:
                HelpMarker(
                    "Using ImDrawList::AddText() with a fine ClipRect:\n"
                    "Will alter only this specific ImDrawList::AddText() rendering.\n"
                    "(this is often used internally to avoid altering the clipping rectangle and minimize draw calls)");
                ImVec4 clip_rect(p0.x, p0.y, p1.x, p1.y); // AddText() takes a ImVec4* here so let's convert.
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(py::GetFont(), py::GetFontSize(), text_pos, IM_COL32_WHITE, text_str, NULL, 0.0f, &clip_rect);
                break;
            }
            py::EndGroup();
            py::PopID();
        }

        py::TreePop();
    }
}

static void ShowDemoWindowPopups()
{
    if (!py::CollapsingHeader("Popups & Modal windows"))
        return;

    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by Dear py instead of being held by the programmer as
    //   we are used to with regular Begin() calls. User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(pyHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even
    //     when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state BECAUSE it can close
    // popups at any time.

    // Typical use for regular windows:
    //   bool my_tool_is_active = false; if (py::Button("Open")) my_tool_is_active = true; [...] if (my_tool_is_active) Begin("My Tool", &my_tool_is_active) { [...] } End();
    // Typical use for popups:
    //   if (py::Button("Open")) py::OpenPopup("MyPopup"); if (py::BeginPopup("MyPopup") { [...] EndPopup(); }

    // With popups we have to go through a library call (here OpenPopup) to manipulate the visibility state.
    // This may be a bit confusing at first but it should quickly make sense. Follow on the examples below.

    if (py::TreeNode("Popups"))
    {
        py::TextWrapped(
            "When a popup is active, it inhibits interacting with windows that are behind the popup. "
            "Clicking outside the popup closes it.");

        static int selected_fish = -1;
        const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
        static bool toggles[] = { true, false, false, false, false };

        // Simple selection popup (if you want to show the current selection inside the Button itself,
        // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
        if (py::Button("Select.."))
            py::OpenPopup("my_select_popup");
        py::SameLine();
        py::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
        if (py::BeginPopup("my_select_popup"))
        {
            py::Text("Aquarium");
            py::Separator();
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                if (py::Selectable(names[i]))
                    selected_fish = i;
            py::EndPopup();
        }

        // Showing a menu with toggles
        if (py::Button("Toggle.."))
            py::OpenPopup("my_toggle_popup");
        if (py::BeginPopup("my_toggle_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                py::MenuItem(names[i], "", &toggles[i]);
            if (py::BeginMenu("Sub-menu"))
            {
                py::MenuItem("Click me");
                py::EndMenu();
            }

            py::Separator();
            py::Text("Tooltip here");
            if (py::IsItemHovered())
                py::SetTooltip("I am a tooltip over a popup");

            if (py::Button("Stacked Popup"))
                py::OpenPopup("another popup");
            if (py::BeginPopup("another popup"))
            {
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    py::MenuItem(names[i], "", &toggles[i]);
                if (py::BeginMenu("Sub-menu"))
                {
                    py::MenuItem("Click me");
                    if (py::Button("Stacked Popup"))
                        py::OpenPopup("another popup");
                    if (py::BeginPopup("another popup"))
                    {
                        py::Text("I am the last one here.");
                        py::EndPopup();
                    }
                    py::EndMenu();
                }
                py::EndPopup();
            }
            py::EndPopup();
        }

        // Call the more complete ShowExampleMenuFile which we use in various places of this demo
        if (py::Button("File Menu.."))
            py::OpenPopup("my_file_popup");
        if (py::BeginPopup("my_file_popup"))
        {
            ShowExampleMenuFile();
            py::EndPopup();
        }

        py::TreePop();
    }

    if (py::TreeNode("Context menus"))
    {
        HelpMarker("\"Context\" functions are simple helpers to associate a Popup to a given Item or Window identifier.");

        // BeginPopupContextItem() is a helper to provide common/simple popup behavior of essentially doing:
        //     if (id == 0)
        //         id = GetItemID(); // Use last item id
        //     if (IsItemHovered() && IsMouseReleased(pyMouseButton_Right))
        //         OpenPopup(id);
        //     return BeginPopup(id);
        // For advanced advanced uses you may want to replicate and customize this code.
        // See more details in BeginPopupContextItem().

        // Example 1
        // When used after an item that has an ID (e.g. Button), we can skip providing an ID to BeginPopupContextItem(),
        // and BeginPopupContextItem() will use the last item ID as the popup ID.
        {
            const char* names[5] = { "Label1", "Label2", "Label3", "Label4", "Label5" };
            for (int n = 0; n < 5; n++)
            {
                py::Selectable(names[n]);
                if (py::BeginPopupContextItem()) // <-- use last item id as popup id
                {
                    py::Text("This a popup for \"%s\"!", names[n]);
                    if (py::Button("Close"))
                        py::CloseCurrentPopup();
                    py::EndPopup();
                }
                if (py::IsItemHovered())
                    py::SetTooltip("Right-click to open popup");
            }
        }

        // Example 2
        // Popup on a Text() element which doesn't have an identifier: we need to provide an identifier to BeginPopupContextItem().
        // Using an explicit identifier is also convenient if you want to activate the popups from different locations.
        {
            HelpMarker("Text() elements don't have stable identifiers so we need to provide one.");
            static float value = 0.5f;
            py::Text("Value = %.3f <-- (1) right-click this value", value);
            if (py::BeginPopupContextItem("my popup"))
            {
                if (py::Selectable("Set to zero")) value = 0.0f;
                if (py::Selectable("Set to PI")) value = 3.1415f;
                py::SetNextItemWidth(-FLT_MIN);
                py::DragFloat("##Value", &value, 0.1f, 0.0f, 0.0f);
                py::EndPopup();
            }

            // We can also use OpenPopupOnItemClick() to toggle the visibility of a given popup.
            // Here we make it that right-clicking this other text element opens the same popup as above.
            // The popup itself will be submitted by the code above.
            py::Text("(2) Or right-click this text");
            py::OpenPopupOnItemClick("my popup", pyPopupFlags_MouseButtonRight);

            // Back to square one: manually open the same popup.
            if (py::Button("(3) Or click this button"))
                py::OpenPopup("my popup");
        }

        // Example 3
        // When using BeginPopupContextItem() with an implicit identifier (NULL == use last item ID),
        // we need to make sure your item identifier is stable.
        // In this example we showcase altering the item label while preserving its identifier, using the ### operator (see FAQ).
        {
            HelpMarker("Showcase using a popup ID linked to item ID, with the item having a changing label + stable ID using the ### operator.");
            static char name[32] = "Label1";
            char buf[64];
            sprintf(buf, "Button: %s###Button", name); // ### operator override ID ignoring the preceding label
            py::Button(buf);
            if (py::BeginPopupContextItem())
            {
                py::Text("Edit name:");
                py::InputText("##edit", name, IM_ARRAYSIZE(name));
                if (py::Button("Close"))
                    py::CloseCurrentPopup();
                py::EndPopup();
            }
            py::SameLine(); py::Text("(<-- right-click here)");
        }

        py::TreePop();
    }

    if (py::TreeNode("Modals"))
    {
        py::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside.");

        if (py::Button("Delete.."))
            py::OpenPopup("Delete?");

        // Always center this window when appearing
        ImVec2 center = py::GetMainViewport()->GetCenter();
        py::SetNextWindowPos(center, pyCond_Appearing, ImVec2(0.5f, 0.5f));

        if (py::BeginPopupModal("Delete?", NULL, pyWindowFlags_AlwaysAutoResize))
        {
            py::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
            py::Separator();

            //static int unused_i = 0;
            //py::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            py::PushStyleVar(pyStyleVar_FramePadding, ImVec2(0, 0));
            py::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            py::PopStyleVar();

            if (py::Button("OK", ImVec2(120, 0))) { py::CloseCurrentPopup(); }
            py::SetItemDefaultFocus();
            py::SameLine();
            if (py::Button("Cancel", ImVec2(120, 0))) { py::CloseCurrentPopup(); }
            py::EndPopup();
        }

        if (py::Button("Stacked modals.."))
            py::OpenPopup("Stacked 1");
        if (py::BeginPopupModal("Stacked 1", NULL, pyWindowFlags_MenuBar))
        {
            if (py::BeginMenuBar())
            {
                if (py::BeginMenu("File"))
                {
                    if (py::MenuItem("Some menu item")) {}
                    py::EndMenu();
                }
                py::EndMenuBar();
            }
            py::Text("Hello from Stacked The First\nUsing style.Colors[pyCol_ModalWindowDimBg] behind it.");

            // Testing behavior of widgets stacking their own regular popups over the modal.
            static int item = 1;
            static float color[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            py::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
            py::ColorEdit4("color", color);

            if (py::Button("Add another modal.."))
                py::OpenPopup("Stacked 2");

            // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which
            // will close the popup. Note that the visibility state of popups is owned by py, so the input value
            // of the bool actually doesn't matter here.
            bool unused_open = true;
            if (py::BeginPopupModal("Stacked 2", &unused_open))
            {
                py::Text("Hello from Stacked The Second!");
                if (py::Button("Close"))
                    py::CloseCurrentPopup();
                py::EndPopup();
            }

            if (py::Button("Close"))
                py::CloseCurrentPopup();
            py::EndPopup();
        }

        py::TreePop();
    }

    if (py::TreeNode("Menus inside a regular window"))
    {
        py::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
        py::Separator();

        // Note: As a quirk in this very specific example, we want to differentiate the parent of this menu from the
        // parent of the various popup menus above. To do so we are encloding the items in a PushID()/PopID() block
        // to make them two different menusets. If we don't, opening any popup above and hovering our menu here would
        // open it. This is because once a menu is active, we allow to switch to a sibling menu by just hovering on it,
        // which is the desired behavior for regular menus.
        py::PushID("foo");
        py::MenuItem("Menu item", "CTRL+M");
        if (py::BeginMenu("Menu inside a regular window"))
        {
            ShowExampleMenuFile();
            py::EndMenu();
        }
        py::PopID();
        py::Separator();
        py::TreePop();
    }
}

// Dummy data structure that we use for the Table demo.
// (pre-C++11 doesn't allow us to instantiate ImVector<MyItem> template if this structure if defined inside the demo function)
namespace
{
// We are passing our own identifier to TableSetupColumn() to facilitate identifying columns in the sorting code.
// This identifier will be passed down into pyTableSortSpec::ColumnUserID.
// But it is possible to omit the user id parameter of TableSetupColumn() and just use the column index instead! (pyTableSortSpec::ColumnIndex)
// If you don't use sorting, you will generally never care about giving column an ID!
enum MyItemColumnID
{
    MyItemColumnID_ID,
    MyItemColumnID_Name,
    MyItemColumnID_Action,
    MyItemColumnID_Quantity,
    MyItemColumnID_Description
};

struct MyItem
{
    int         ID;
    const char* Name;
    int         Quantity;

    // We have a problem which is affecting _only this demo_ and should not affect your code:
    // As we don't rely on std:: or other third-party library to compile dear py, we only have reliable access to qsort(),
    // however qsort doesn't allow passing user data to comparing function.
    // As a workaround, we are storing the sort specs in a static/global for the comparing function to access.
    // In your own use case you would probably pass the sort specs to your sorting/comparing functions directly and not use a global.
    // We could technically call py::TableGetSortSpecs() in CompareWithSortSpecs(), but considering that this function is called
    // very often by the sorting algorithm it would be a little wasteful.
    static const pyTableSortSpecs* s_current_sort_specs;

    // Compare function to be used by qsort()
    static int py_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
    {
        const MyItem* a = (const MyItem*)lhs;
        const MyItem* b = (const MyItem*)rhs;
        for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
        {
            // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
            // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
            const pyTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
            int delta = 0;
            switch (sort_spec->ColumnUserID)
            {
            case MyItemColumnID_ID:             delta = (a->ID - b->ID);                break;
            case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));     break;
            case MyItemColumnID_Quantity:       delta = (a->Quantity - b->Quantity);    break;
            case MyItemColumnID_Description:    delta = (strcmp(a->Name, b->Name));     break;
            default: IM_ASSERT(0); break;
            }
            if (delta > 0)
                return (sort_spec->SortDirection == pySortDirection_Ascending) ? +1 : -1;
            if (delta < 0)
                return (sort_spec->SortDirection == pySortDirection_Ascending) ? -1 : +1;
        }

        // qsort() is instable so always return a way to differenciate items.
        // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
        return (a->ID - b->ID);
    }
};
const pyTableSortSpecs* MyItem::s_current_sort_specs = NULL;
}

// Make the UI compact because there are so many fields
static void PushStyleCompact()
{
    pyStyle& style = py::GetStyle();
    py::PushStyleVar(pyStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
    py::PushStyleVar(pyStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
    py::PopStyleVar(2);
}

// Show a combo box with a choice of sizing policies
static void EditTableSizingFlags(pyTableFlags* p_flags)
{
    struct EnumDesc { pyTableFlags Value; const char* Name; const char* Tooltip; };
    static const EnumDesc policies[] =
    {
        { pyTableFlags_None,               "Default",                            "Use default sizing policy:\n- pyTableFlags_SizingFixedFit if ScrollX is on or if host window has pyWindowFlags_AlwaysAutoResize.\n- pyTableFlags_SizingStretchSame otherwise." },
        { pyTableFlags_SizingFixedFit,     "pyTableFlags_SizingFixedFit",     "Columns default to _WidthFixed (if resizable) or _WidthAuto (if not resizable), matching contents width." },
        { pyTableFlags_SizingFixedSame,    "pyTableFlags_SizingFixedSame",    "Columns are all the same width, matching the maximum contents width.\nImplicitly disable pyTableFlags_Resizable and enable pyTableFlags_NoKeepColumnsVisible." },
        { pyTableFlags_SizingStretchProp,  "pyTableFlags_SizingStretchProp",  "Columns default to _WidthStretch with weights proportional to their widths." },
        { pyTableFlags_SizingStretchSame,  "pyTableFlags_SizingStretchSame",  "Columns default to _WidthStretch with same weights." }
    };
    int idx;
    for (idx = 0; idx < IM_ARRAYSIZE(policies); idx++)
        if (policies[idx].Value == (*p_flags & pyTableFlags_SizingMask_))
            break;
    const char* preview_text = (idx < IM_ARRAYSIZE(policies)) ? policies[idx].Name + (idx > 0 ? strlen("pyTableFlags") : 0) : "";
    if (py::BeginCombo("Sizing Policy", preview_text))
    {
        for (int n = 0; n < IM_ARRAYSIZE(policies); n++)
            if (py::Selectable(policies[n].Name, idx == n))
                *p_flags = (*p_flags & ~pyTableFlags_SizingMask_) | policies[n].Value;
        py::EndCombo();
    }
    py::SameLine();
    py::TextDisabled("(?)");
    if (py::IsItemHovered())
    {
        py::BeginTooltip();
        py::PushTextWrapPos(py::GetFontSize() * 50.0f);
        for (int m = 0; m < IM_ARRAYSIZE(policies); m++)
        {
            py::Separator();
            py::Text("%s:", policies[m].Name);
            py::Separator();
            py::SetCursorPosX(py::GetCursorPosX() + py::GetStyle().IndentSpacing * 0.5f);
            py::TextUnformatted(policies[m].Tooltip);
        }
        py::PopTextWrapPos();
        py::EndTooltip();
    }
}

static void EditTableColumnsFlags(pyTableColumnFlags* p_flags)
{
    py::CheckboxFlags("_Disabled", p_flags, pyTableColumnFlags_Disabled); py::SameLine(); HelpMarker("Master disable flag (also hide from context menu)");
    py::CheckboxFlags("_DefaultHide", p_flags, pyTableColumnFlags_DefaultHide);
    py::CheckboxFlags("_DefaultSort", p_flags, pyTableColumnFlags_DefaultSort);
    if (py::CheckboxFlags("_WidthStretch", p_flags, pyTableColumnFlags_WidthStretch))
        *p_flags &= ~(pyTableColumnFlags_WidthMask_ ^ pyTableColumnFlags_WidthStretch);
    if (py::CheckboxFlags("_WidthFixed", p_flags, pyTableColumnFlags_WidthFixed))
        *p_flags &= ~(pyTableColumnFlags_WidthMask_ ^ pyTableColumnFlags_WidthFixed);
    py::CheckboxFlags("_NoResize", p_flags, pyTableColumnFlags_NoResize);
    py::CheckboxFlags("_NoReorder", p_flags, pyTableColumnFlags_NoReorder);
    py::CheckboxFlags("_NoHide", p_flags, pyTableColumnFlags_NoHide);
    py::CheckboxFlags("_NoClip", p_flags, pyTableColumnFlags_NoClip);
    py::CheckboxFlags("_NoSort", p_flags, pyTableColumnFlags_NoSort);
    py::CheckboxFlags("_NoSortAscending", p_flags, pyTableColumnFlags_NoSortAscending);
    py::CheckboxFlags("_NoSortDescending", p_flags, pyTableColumnFlags_NoSortDescending);
    py::CheckboxFlags("_NoHeaderLabel", p_flags, pyTableColumnFlags_NoHeaderLabel);
    py::CheckboxFlags("_NoHeaderWidth", p_flags, pyTableColumnFlags_NoHeaderWidth);
    py::CheckboxFlags("_PreferSortAscending", p_flags, pyTableColumnFlags_PreferSortAscending);
    py::CheckboxFlags("_PreferSortDescending", p_flags, pyTableColumnFlags_PreferSortDescending);
    py::CheckboxFlags("_IndentEnable", p_flags, pyTableColumnFlags_IndentEnable); py::SameLine(); HelpMarker("Default for column 0");
    py::CheckboxFlags("_IndentDisable", p_flags, pyTableColumnFlags_IndentDisable); py::SameLine(); HelpMarker("Default for column >0");
}

static void ShowTableColumnsStatusFlags(pyTableColumnFlags flags)
{
    py::CheckboxFlags("_IsEnabled", &flags, pyTableColumnFlags_IsEnabled);
    py::CheckboxFlags("_IsVisible", &flags, pyTableColumnFlags_IsVisible);
    py::CheckboxFlags("_IsSorted", &flags, pyTableColumnFlags_IsSorted);
    py::CheckboxFlags("_IsHovered", &flags, pyTableColumnFlags_IsHovered);
}

static void ShowDemoWindowTables()
{
    //py::SetNextItemOpen(true, pyCond_Once);
    if (!py::CollapsingHeader("Tables & Columns"))
        return;

    // Using those as a base value to create width/height that are factor of the size of our font
    const float TEXT_BASE_WIDTH = py::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = py::GetTextLineHeightWithSpacing();

    py::PushID("Tables");

    int open_action = -1;
    if (py::Button("Open all"))
        open_action = 1;
    py::SameLine();
    if (py::Button("Close all"))
        open_action = 0;
    py::SameLine();

    // Options
    static bool disable_indent = false;
    py::Checkbox("Disable tree indentation", &disable_indent);
    py::SameLine();
    HelpMarker("Disable the indenting of tree nodes so demo tables can use the full window width.");
    py::Separator();
    if (disable_indent)
        py::PushStyleVar(pyStyleVar_IndentSpacing, 0.0f);

    // About Styling of tables
    // Most settings are configured on a per-table basis via the flags passed to BeginTable() and TableSetupColumns APIs.
    // There are however a few settings that a shared and part of the pyStyle structure:
    //   style.CellPadding                          // Padding within each cell
    //   style.Colors[pyCol_TableHeaderBg]       // Table header background
    //   style.Colors[pyCol_TableBorderStrong]   // Table outer and header borders
    //   style.Colors[pyCol_TableBorderLight]    // Table inner borders
    //   style.Colors[pyCol_TableRowBg]          // Table row background when pyTableFlags_RowBg is enabled (even rows)
    //   style.Colors[pyCol_TableRowBgAlt]       // Table row background when pyTableFlags_RowBg is enabled (odds rows)

    // Demos
    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Basic"))
    {
        // Here we will showcase three different ways to output a table.
        // They are very simple variations of a same thing!

        // [Method 1] Using TableNextRow() to create a new row, and TableSetColumnIndex() to select the column.
        // In many situations, this is the most flexible and easy to use pattern.
        HelpMarker("Using TableNextRow() + calling TableSetColumnIndex() _before_ each cell, in a loop.");
        if (py::BeginTable("table1", 3))
        {
            for (int row = 0; row < 4; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("Row %d Column %d", row, column);
                }
            }
            py::EndTable();
        }

        // [Method 2] Using TableNextColumn() called multiple times, instead of using a for loop + TableSetColumnIndex().
        // This is generally more convenient when you have code manually submitting the contents of each columns.
        HelpMarker("Using TableNextRow() + calling TableNextColumn() _before_ each cell, manually.");
        if (py::BeginTable("table2", 3))
        {
            for (int row = 0; row < 4; row++)
            {
                py::TableNextRow();
                py::TableNextColumn();
                py::Text("Row %d", row);
                py::TableNextColumn();
                py::Text("Some contents");
                py::TableNextColumn();
                py::Text("123.456");
            }
            py::EndTable();
        }

        // [Method 3] We call TableNextColumn() _before_ each cell. We never call TableNextRow(),
        // as TableNextColumn() will automatically wrap around and create new roes as needed.
        // This is generally more convenient when your cells all contains the same type of data.
        HelpMarker(
            "Only using TableNextColumn(), which tends to be convenient for tables where every cells contains the same type of contents.\n"
            "This is also more similar to the old NextColumn() function of the Columns API, and provided to facilitate the Columns->Tables API transition.");
        if (py::BeginTable("table3", 3))
        {
            for (int item = 0; item < 14; item++)
            {
                py::TableNextColumn();
                py::Text("Item %d", item);
            }
            py::EndTable();
        }

        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Borders, background"))
    {
        // Expose a few Borders related flags interactively
        enum ContentsType { CT_Text, CT_FillButton };
        static pyTableFlags flags = pyTableFlags_Borders | pyTableFlags_RowBg;
        static bool display_headers = false;
        static int contents_type = CT_Text;

        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_RowBg", &flags, pyTableFlags_RowBg);
        py::CheckboxFlags("pyTableFlags_Borders", &flags, pyTableFlags_Borders);
        py::SameLine(); HelpMarker("pyTableFlags_Borders\n = pyTableFlags_BordersInnerV\n | pyTableFlags_BordersOuterV\n | pyTableFlags_BordersInnerV\n | pyTableFlags_BordersOuterH");
        py::Indent();

        py::CheckboxFlags("pyTableFlags_BordersH", &flags, pyTableFlags_BordersH);
        py::Indent();
        py::CheckboxFlags("pyTableFlags_BordersOuterH", &flags, pyTableFlags_BordersOuterH);
        py::CheckboxFlags("pyTableFlags_BordersInnerH", &flags, pyTableFlags_BordersInnerH);
        py::Unindent();

        py::CheckboxFlags("pyTableFlags_BordersV", &flags, pyTableFlags_BordersV);
        py::Indent();
        py::CheckboxFlags("pyTableFlags_BordersOuterV", &flags, pyTableFlags_BordersOuterV);
        py::CheckboxFlags("pyTableFlags_BordersInnerV", &flags, pyTableFlags_BordersInnerV);
        py::Unindent();

        py::CheckboxFlags("pyTableFlags_BordersOuter", &flags, pyTableFlags_BordersOuter);
        py::CheckboxFlags("pyTableFlags_BordersInner", &flags, pyTableFlags_BordersInner);
        py::Unindent();

        py::AlignTextToFramePadding(); py::Text("Cell contents:");
        py::SameLine(); py::RadioButton("Text", &contents_type, CT_Text);
        py::SameLine(); py::RadioButton("FillButton", &contents_type, CT_FillButton);
        py::Checkbox("Display headers", &display_headers);
        py::CheckboxFlags("pyTableFlags_NoBordersInBody", &flags, pyTableFlags_NoBordersInBody); py::SameLine(); HelpMarker("Disable vertical borders in columns Body (borders will always appears in Headers");
        PopStyleCompact();

        if (py::BeginTable("table1", 3, flags))
        {
            // Display headers so we can inspect their interaction with borders.
            // (Headers are not the main purpose of this section of the demo, so we are not elaborating on them too much. See other sections for details)
            if (display_headers)
            {
                py::TableSetupColumn("One");
                py::TableSetupColumn("Two");
                py::TableSetupColumn("Three");
                py::TableHeadersRow();
            }

            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    char buf[32];
                    sprintf(buf, "Hello %d,%d", column, row);
                    if (contents_type == CT_Text)
                        py::TextUnformatted(buf);
                    else if (contents_type)
                        py::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Resizable, stretch"))
    {
        // By default, if we don't enable ScrollX the sizing policy for each columns is "Stretch"
        // Each columns maintain a sizing weight, and they will occupy all available width.
        static pyTableFlags flags = pyTableFlags_SizingStretchSame | pyTableFlags_Resizable | pyTableFlags_BordersOuter | pyTableFlags_BordersV | pyTableFlags_ContextMenuInBody;
        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_Resizable", &flags, pyTableFlags_Resizable);
        py::CheckboxFlags("pyTableFlags_BordersV", &flags, pyTableFlags_BordersV);
        py::SameLine(); HelpMarker("Using the _Resizable flag automatically enables the _BordersInnerV flag as well, this is why the resize borders are still showing when unchecking this.");
        PopStyleCompact();

        if (py::BeginTable("table1", 3, flags))
        {
            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("Hello %d,%d", column, row);
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Resizable, fixed"))
    {
        // Here we use pyTableFlags_SizingFixedFit (even though _ScrollX is not set)
        // So columns will adopt the "Fixed" policy and will maintain a fixed width regardless of the whole available width (unless table is small)
        // If there is not enough available width to fit all columns, they will however be resized down.
        // FIXME-TABLE: Providing a stretch-on-init would make sense especially for tables which don't have saved settings
        HelpMarker(
            "Using _Resizable + _SizingFixedFit flags.\n"
            "Fixed-width columns generally makes more sense if you want to use horizontal scrolling.\n\n"
            "Double-click a column border to auto-fit the column to its contents.");
        PushStyleCompact();
        static pyTableFlags flags = pyTableFlags_SizingFixedFit | pyTableFlags_Resizable | pyTableFlags_BordersOuter | pyTableFlags_BordersV | pyTableFlags_ContextMenuInBody;
        py::CheckboxFlags("pyTableFlags_NoHostExtendX", &flags, pyTableFlags_NoHostExtendX);
        PopStyleCompact();

        if (py::BeginTable("table1", 3, flags))
        {
            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("Hello %d,%d", column, row);
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Resizable, mixed"))
    {
        HelpMarker(
            "Using TableSetupColumn() to alter resizing policy on a per-column basis.\n\n"
            "When combining Fixed and Stretch columns, generally you only want one, maybe two trailing columns to use _WidthStretch.");
        static pyTableFlags flags = pyTableFlags_SizingFixedFit | pyTableFlags_RowBg | pyTableFlags_Borders | pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable;

        if (py::BeginTable("table1", 3, flags))
        {
            py::TableSetupColumn("AAA", pyTableColumnFlags_WidthFixed);
            py::TableSetupColumn("BBB", pyTableColumnFlags_WidthFixed);
            py::TableSetupColumn("CCC", pyTableColumnFlags_WidthStretch);
            py::TableHeadersRow();
            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("%s %d,%d", (column == 2) ? "Stretch" : "Fixed", column, row);
                }
            }
            py::EndTable();
        }
        if (py::BeginTable("table2", 6, flags))
        {
            py::TableSetupColumn("AAA", pyTableColumnFlags_WidthFixed);
            py::TableSetupColumn("BBB", pyTableColumnFlags_WidthFixed);
            py::TableSetupColumn("CCC", pyTableColumnFlags_WidthFixed | pyTableColumnFlags_DefaultHide);
            py::TableSetupColumn("DDD", pyTableColumnFlags_WidthStretch);
            py::TableSetupColumn("EEE", pyTableColumnFlags_WidthStretch);
            py::TableSetupColumn("FFF", pyTableColumnFlags_WidthStretch | pyTableColumnFlags_DefaultHide);
            py::TableHeadersRow();
            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 6; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("%s %d,%d", (column >= 3) ? "Stretch" : "Fixed", column, row);
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Reorderable, hideable, with headers"))
    {
        HelpMarker(
            "Click and drag column headers to reorder columns.\n\n"
            "Right-click on a header to open a context menu.");
        static pyTableFlags flags = pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable | pyTableFlags_BordersOuter | pyTableFlags_BordersV;
        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_Resizable", &flags, pyTableFlags_Resizable);
        py::CheckboxFlags("pyTableFlags_Reorderable", &flags, pyTableFlags_Reorderable);
        py::CheckboxFlags("pyTableFlags_Hideable", &flags, pyTableFlags_Hideable);
        py::CheckboxFlags("pyTableFlags_NoBordersInBody", &flags, pyTableFlags_NoBordersInBody);
        py::CheckboxFlags("pyTableFlags_NoBordersInBodyUntilResize", &flags, pyTableFlags_NoBordersInBodyUntilResize); py::SameLine(); HelpMarker("Disable vertical borders in columns Body until hovered for resize (borders will always appears in Headers)");
        PopStyleCompact();

        if (py::BeginTable("table1", 3, flags))
        {
            // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
            // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
            py::TableSetupColumn("One");
            py::TableSetupColumn("Two");
            py::TableSetupColumn("Three");
            py::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("Hello %d,%d", column, row);
                }
            }
            py::EndTable();
        }

        // Use outer_size.x == 0.0f instead of default to make the table as tight as possible (only valid when no scrolling and no stretch column)
        if (py::BeginTable("table2", 3, flags | pyTableFlags_SizingFixedFit, ImVec2(0.0f, 0.0f)))
        {
            py::TableSetupColumn("One");
            py::TableSetupColumn("Two");
            py::TableSetupColumn("Three");
            py::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("Fixed %d,%d", column, row);
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Padding"))
    {
        // First example: showcase use of padding flags and effect of BorderOuterV/BorderInnerV on X padding.
        // We don't expose BorderOuterH/BorderInnerH here because they have no effect on X padding.
        HelpMarker(
            "We often want outer padding activated when any using features which makes the edges of a column visible:\n"
            "e.g.:\n"
            "- BorderOuterV\n"
            "- any form of row selection\n"
            "Because of this, activating BorderOuterV sets the default to PadOuterX. Using PadOuterX or NoPadOuterX you can override the default.\n\n"
            "Actual padding values are using style.CellPadding.\n\n"
            "In this demo we don't show horizontal borders to emphasis how they don't affect default horizontal padding.");

        static pyTableFlags flags1 = pyTableFlags_BordersV;
        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_PadOuterX", &flags1, pyTableFlags_PadOuterX);
        py::SameLine(); HelpMarker("Enable outer-most padding (default if pyTableFlags_BordersOuterV is set)");
        py::CheckboxFlags("pyTableFlags_NoPadOuterX", &flags1, pyTableFlags_NoPadOuterX);
        py::SameLine(); HelpMarker("Disable outer-most padding (default if pyTableFlags_BordersOuterV is not set)");
        py::CheckboxFlags("pyTableFlags_NoPadInnerX", &flags1, pyTableFlags_NoPadInnerX);
        py::SameLine(); HelpMarker("Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off)");
        py::CheckboxFlags("pyTableFlags_BordersOuterV", &flags1, pyTableFlags_BordersOuterV);
        py::CheckboxFlags("pyTableFlags_BordersInnerV", &flags1, pyTableFlags_BordersInnerV);
        static bool show_headers = false;
        py::Checkbox("show_headers", &show_headers);
        PopStyleCompact();

        if (py::BeginTable("table_padding", 3, flags1))
        {
            if (show_headers)
            {
                py::TableSetupColumn("One");
                py::TableSetupColumn("Two");
                py::TableSetupColumn("Three");
                py::TableHeadersRow();
            }

            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    if (row == 0)
                    {
                        py::Text("Avail %.2f", py::GetContentRegionAvail().x);
                    }
                    else
                    {
                        char buf[32];
                        sprintf(buf, "Hello %d,%d", column, row);
                        py::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                    }
                    //if (py::TableGetColumnFlags() & pyTableColumnFlags_IsHovered)
                    //    py::TableSetBgColor(pyTableBgTarget_CellBg, IM_COL32(0, 100, 0, 255));
                }
            }
            py::EndTable();
        }

        // Second example: set style.CellPadding to (0.0) or a custom value.
        // FIXME-TABLE: Vertical border effectively not displayed the same way as horizontal one...
        HelpMarker("Setting style.CellPadding to (0,0) or a custom value.");
        static pyTableFlags flags2 = pyTableFlags_Borders | pyTableFlags_RowBg;
        static ImVec2 cell_padding(0.0f, 0.0f);
        static bool show_widget_frame_bg = true;

        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_Borders", &flags2, pyTableFlags_Borders);
        py::CheckboxFlags("pyTableFlags_BordersH", &flags2, pyTableFlags_BordersH);
        py::CheckboxFlags("pyTableFlags_BordersV", &flags2, pyTableFlags_BordersV);
        py::CheckboxFlags("pyTableFlags_BordersInner", &flags2, pyTableFlags_BordersInner);
        py::CheckboxFlags("pyTableFlags_BordersOuter", &flags2, pyTableFlags_BordersOuter);
        py::CheckboxFlags("pyTableFlags_RowBg", &flags2, pyTableFlags_RowBg);
        py::CheckboxFlags("pyTableFlags_Resizable", &flags2, pyTableFlags_Resizable);
        py::Checkbox("show_widget_frame_bg", &show_widget_frame_bg);
        py::SliderFloat2("CellPadding", &cell_padding.x, 0.0f, 10.0f, "%.0f");
        PopStyleCompact();

        py::PushStyleVar(pyStyleVar_CellPadding, cell_padding);
        if (py::BeginTable("table_padding_2", 3, flags2))
        {
            static char text_bufs[3 * 5][16]; // Mini text storage for 3x5 cells
            static bool init = true;
            if (!show_widget_frame_bg)
                py::PushStyleColor(pyCol_FrameBg, 0);
            for (int cell = 0; cell < 3 * 5; cell++)
            {
                py::TableNextColumn();
                if (init)
                    strcpy(text_bufs[cell], "edit me");
                py::SetNextItemWidth(-FLT_MIN);
                py::PushID(cell);
                py::InputText("##cell", text_bufs[cell], IM_ARRAYSIZE(text_bufs[cell]));
                py::PopID();
            }
            if (!show_widget_frame_bg)
                py::PopStyleColor();
            init = false;
            py::EndTable();
        }
        py::PopStyleVar();

        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Sizing policies"))
    {
        static pyTableFlags flags1 = pyTableFlags_BordersV | pyTableFlags_BordersOuterH | pyTableFlags_RowBg | pyTableFlags_ContextMenuInBody;
        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_Resizable", &flags1, pyTableFlags_Resizable);
        py::CheckboxFlags("pyTableFlags_NoHostExtendX", &flags1, pyTableFlags_NoHostExtendX);
        PopStyleCompact();

        static pyTableFlags sizing_policy_flags[4] = { pyTableFlags_SizingFixedFit, pyTableFlags_SizingFixedSame, pyTableFlags_SizingStretchProp, pyTableFlags_SizingStretchSame };
        for (int table_n = 0; table_n < 4; table_n++)
        {
            py::PushID(table_n);
            py::SetNextItemWidth(TEXT_BASE_WIDTH * 30);
            EditTableSizingFlags(&sizing_policy_flags[table_n]);

            // To make it easier to understand the different sizing policy,
            // For each policy: we display one table where the columns have equal contents width, and one where the columns have different contents width.
            if (py::BeginTable("table1", 3, sizing_policy_flags[table_n] | flags1))
            {
                for (int row = 0; row < 3; row++)
                {
                    py::TableNextRow();
                    py::TableNextColumn(); py::Text("Oh dear");
                    py::TableNextColumn(); py::Text("Oh dear");
                    py::TableNextColumn(); py::Text("Oh dear");
                }
                py::EndTable();
            }
            if (py::BeginTable("table2", 3, sizing_policy_flags[table_n] | flags1))
            {
                for (int row = 0; row < 3; row++)
                {
                    py::TableNextRow();
                    py::TableNextColumn(); py::Text("AAAA");
                    py::TableNextColumn(); py::Text("BBBBBBBB");
                    py::TableNextColumn(); py::Text("CCCCCCCCCCCC");
                }
                py::EndTable();
            }
            py::PopID();
        }

        py::Spacing();
        py::TextUnformatted("Advanced");
        py::SameLine();
        HelpMarker("This section allows you to interact and see the effect of various sizing policies depending on whether Scroll is enabled and the contents of your columns.");

        enum ContentsType { CT_ShowWidth, CT_ShortText, CT_LongText, CT_Button, CT_FillButton, CT_InputText };
        static pyTableFlags flags = pyTableFlags_ScrollY | pyTableFlags_Borders | pyTableFlags_RowBg | pyTableFlags_Resizable;
        static int contents_type = CT_ShowWidth;
        static int column_count = 3;

        PushStyleCompact();
        py::PushID("Advanced");
        py::PushItemWidth(TEXT_BASE_WIDTH * 30);
        EditTableSizingFlags(&flags);
        py::Combo("Contents", &contents_type, "Show width\0Short Text\0Long Text\0Button\0Fill Button\0InputText\0");
        if (contents_type == CT_FillButton)
        {
            py::SameLine();
            HelpMarker("Be mindful that using right-alignment (e.g. size.x = -FLT_MIN) creates a feedback loop where contents width can feed into auto-column width can feed into contents width.");
        }
        py::DragInt("Columns", &column_count, 0.1f, 1, 64, "%d", pySliderFlags_AlwaysClamp);
        py::CheckboxFlags("pyTableFlags_Resizable", &flags, pyTableFlags_Resizable);
        py::CheckboxFlags("pyTableFlags_PreciseWidths", &flags, pyTableFlags_PreciseWidths);
        py::SameLine(); HelpMarker("Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.");
        py::CheckboxFlags("pyTableFlags_ScrollX", &flags, pyTableFlags_ScrollX);
        py::CheckboxFlags("pyTableFlags_ScrollY", &flags, pyTableFlags_ScrollY);
        py::CheckboxFlags("pyTableFlags_NoClip", &flags, pyTableFlags_NoClip);
        py::PopItemWidth();
        py::PopID();
        PopStyleCompact();

        if (py::BeginTable("table2", column_count, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 7)))
        {
            for (int cell = 0; cell < 10 * column_count; cell++)
            {
                py::TableNextColumn();
                int column = py::TableGetColumnIndex();
                int row = py::TableGetRowIndex();

                py::PushID(cell);
                char label[32];
                static char text_buf[32] = "";
                sprintf(label, "Hello %d,%d", column, row);
                switch (contents_type)
                {
                case CT_ShortText:  py::TextUnformatted(label); break;
                case CT_LongText:   py::Text("Some %s text %d,%d\nOver two lines..", column == 0 ? "long" : "longeeer", column, row); break;
                case CT_ShowWidth:  py::Text("W: %.1f", py::GetContentRegionAvail().x); break;
                case CT_Button:     py::Button(label); break;
                case CT_FillButton: py::Button(label, ImVec2(-FLT_MIN, 0.0f)); break;
                case CT_InputText:  py::SetNextItemWidth(-FLT_MIN); py::InputText("##", text_buf, IM_ARRAYSIZE(text_buf)); break;
                }
                py::PopID();
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Vertical scrolling, with clipping"))
    {
        HelpMarker("Here we activate ScrollY, which will create a child window container to allow hosting scrollable contents.\n\nWe also demonstrate using pyListClipper to virtualize the submission of many items.");
        static pyTableFlags flags = pyTableFlags_ScrollY | pyTableFlags_RowBg | pyTableFlags_BordersOuter | pyTableFlags_BordersV | pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable;

        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_ScrollY", &flags, pyTableFlags_ScrollY);
        PopStyleCompact();

        // When using ScrollX or ScrollY we need to specify a size for our table container!
        // Otherwise by default the table will fit all available space, like a BeginChild() call.
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
        if (py::BeginTable("table_scrolly", 3, flags, outer_size))
        {
            py::TableSetupScrollFreeze(0, 1); // Make top row always visible
            py::TableSetupColumn("One", pyTableColumnFlags_None);
            py::TableSetupColumn("Two", pyTableColumnFlags_None);
            py::TableSetupColumn("Three", pyTableColumnFlags_None);
            py::TableHeadersRow();

            // Demonstrate using clipper for large vertical lists
            pyListClipper clipper;
            clipper.Begin(1000);
            while (clipper.Step())
            {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    py::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        py::TableSetColumnIndex(column);
                        py::Text("Hello %d,%d", column, row);
                    }
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Horizontal scrolling"))
    {
        HelpMarker(
            "When ScrollX is enabled, the default sizing policy becomes pyTableFlags_SizingFixedFit, "
            "as automatically stretching columns doesn't make much sense with horizontal scrolling.\n\n"
            "Also note that as of the current version, you will almost always want to enable ScrollY along with ScrollX,"
            "because the container window won't automatically extend vertically to fix contents (this may be improved in future versions).");
        static pyTableFlags flags = pyTableFlags_ScrollX | pyTableFlags_ScrollY | pyTableFlags_RowBg | pyTableFlags_BordersOuter | pyTableFlags_BordersV | pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable;
        static int freeze_cols = 1;
        static int freeze_rows = 1;

        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_Resizable", &flags, pyTableFlags_Resizable);
        py::CheckboxFlags("pyTableFlags_ScrollX", &flags, pyTableFlags_ScrollX);
        py::CheckboxFlags("pyTableFlags_ScrollY", &flags, pyTableFlags_ScrollY);
        py::SetNextItemWidth(py::GetFrameHeight());
        py::DragInt("freeze_cols", &freeze_cols, 0.2f, 0, 9, NULL, pySliderFlags_NoInput);
        py::SetNextItemWidth(py::GetFrameHeight());
        py::DragInt("freeze_rows", &freeze_rows, 0.2f, 0, 9, NULL, pySliderFlags_NoInput);
        PopStyleCompact();

        // When using ScrollX or ScrollY we need to specify a size for our table container!
        // Otherwise by default the table will fit all available space, like a BeginChild() call.
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
        if (py::BeginTable("table_scrollx", 7, flags, outer_size))
        {
            py::TableSetupScrollFreeze(freeze_cols, freeze_rows);
            py::TableSetupColumn("Line #", pyTableColumnFlags_NoHide); // Make the first column not hideable to match our use of TableSetupScrollFreeze()
            py::TableSetupColumn("One");
            py::TableSetupColumn("Two");
            py::TableSetupColumn("Three");
            py::TableSetupColumn("Four");
            py::TableSetupColumn("Five");
            py::TableSetupColumn("Six");
            py::TableHeadersRow();
            for (int row = 0; row < 20; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 7; column++)
                {
                    // Both TableNextColumn() and TableSetColumnIndex() return true when a column is visible or performing width measurement.
                    // Because here we know that:
                    // - A) all our columns are contributing the same to row height
                    // - B) column 0 is always visible,
                    // We only always submit this one column and can skip others.
                    // More advanced per-column clipping behaviors may benefit from polling the status flags via TableGetColumnFlags().
                    if (!py::TableSetColumnIndex(column) && column > 0)
                        continue;
                    if (column == 0)
                        py::Text("Line %d", row);
                    else
                        py::Text("Hello world %d,%d", column, row);
                }
            }
            py::EndTable();
        }

        py::Spacing();
        py::TextUnformatted("Stretch + ScrollX");
        py::SameLine();
        HelpMarker(
            "Showcase using Stretch columns + ScrollX together: "
            "this is rather unusual and only makes sense when specifying an 'inner_width' for the table!\n"
            "Without an explicit value, inner_width is == outer_size.x and therefore using Stretch columns + ScrollX together doesn't make sense.");
        static pyTableFlags flags2 = pyTableFlags_SizingStretchSame | pyTableFlags_ScrollX | pyTableFlags_ScrollY | pyTableFlags_BordersOuter | pyTableFlags_RowBg | pyTableFlags_ContextMenuInBody;
        static float inner_width = 1000.0f;
        PushStyleCompact();
        py::PushID("flags3");
        py::PushItemWidth(TEXT_BASE_WIDTH * 30);
        py::CheckboxFlags("pyTableFlags_ScrollX", &flags2, pyTableFlags_ScrollX);
        py::DragFloat("inner_width", &inner_width, 1.0f, 0.0f, FLT_MAX, "%.1f");
        py::PopItemWidth();
        py::PopID();
        PopStyleCompact();
        if (py::BeginTable("table2", 7, flags2, outer_size, inner_width))
        {
            for (int cell = 0; cell < 20 * 7; cell++)
            {
                py::TableNextColumn();
                py::Text("Hello world %d,%d", py::TableGetColumnIndex(), py::TableGetRowIndex());
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Columns flags"))
    {
        // Create a first table just to show all the options/flags we want to make visible in our example!
        const int column_count = 3;
        const char* column_names[column_count] = { "One", "Two", "Three" };
        static pyTableColumnFlags column_flags[column_count] = { pyTableColumnFlags_DefaultSort, pyTableColumnFlags_None, pyTableColumnFlags_DefaultHide };
        static pyTableColumnFlags column_flags_out[column_count] = { 0, 0, 0 }; // Output from TableGetColumnFlags()

        if (py::BeginTable("table_columns_flags_checkboxes", column_count, pyTableFlags_None))
        {
            PushStyleCompact();
            for (int column = 0; column < column_count; column++)
            {
                py::TableNextColumn();
                py::PushID(column);
                py::AlignTextToFramePadding(); // FIXME-TABLE: Workaround for wrong text baseline propagation
                py::Text("'%s'", column_names[column]);
                py::Spacing();
                py::Text("Input flags:");
                EditTableColumnsFlags(&column_flags[column]);
                py::Spacing();
                py::Text("Output flags:");
                ShowTableColumnsStatusFlags(column_flags_out[column]);
                py::PopID();
            }
            PopStyleCompact();
            py::EndTable();
        }

        // Create the real table we care about for the example!
        // We use a scrolling table to be able to showcase the difference between the _IsEnabled and _IsVisible flags above, otherwise in
        // a non-scrolling table columns are always visible (unless using pyTableFlags_NoKeepColumnsVisible + resizing the parent window down)
        const pyTableFlags flags
            = pyTableFlags_SizingFixedFit | pyTableFlags_ScrollX | pyTableFlags_ScrollY
            | pyTableFlags_RowBg | pyTableFlags_BordersOuter | pyTableFlags_BordersV
            | pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable | pyTableFlags_Sortable;
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 9);
        if (py::BeginTable("table_columns_flags", column_count, flags, outer_size))
        {
            for (int column = 0; column < column_count; column++)
                py::TableSetupColumn(column_names[column], column_flags[column]);
            py::TableHeadersRow();
            for (int column = 0; column < column_count; column++)
                column_flags_out[column] = py::TableGetColumnFlags(column);
            float indent_step = (float)((int)TEXT_BASE_WIDTH / 2);
            for (int row = 0; row < 8; row++)
            {
                py::Indent(indent_step); // Add some indentation to demonstrate usage of per-column IndentEnable/IndentDisable flags.
                py::TableNextRow();
                for (int column = 0; column < column_count; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("%s %s", (column == 0) ? "Indented" : "Hello", py::TableGetColumnName(column));
                }
            }
            py::Unindent(indent_step * 8.0f);

            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Columns widths"))
    {
        HelpMarker("Using TableSetupColumn() to setup default width.");

        static pyTableFlags flags1 = pyTableFlags_Borders | pyTableFlags_NoBordersInBodyUntilResize;
        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_Resizable", &flags1, pyTableFlags_Resizable);
        py::CheckboxFlags("pyTableFlags_NoBordersInBodyUntilResize", &flags1, pyTableFlags_NoBordersInBodyUntilResize);
        PopStyleCompact();
        if (py::BeginTable("table1", 3, flags1))
        {
            // We could also set pyTableFlags_SizingFixedFit on the table and all columns will default to pyTableColumnFlags_WidthFixed.
            py::TableSetupColumn("one", pyTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
            py::TableSetupColumn("two", pyTableColumnFlags_WidthFixed, 200.0f); // Default to 200.0f
            py::TableSetupColumn("three", pyTableColumnFlags_WidthFixed);       // Default to auto
            py::TableHeadersRow();
            for (int row = 0; row < 4; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableSetColumnIndex(column);
                    if (row == 0)
                        py::Text("(w: %5.1f)", py::GetContentRegionAvail().x);
                    else
                        py::Text("Hello %d,%d", column, row);
                }
            }
            py::EndTable();
        }

        HelpMarker("Using TableSetupColumn() to setup explicit width.\n\nUnless _NoKeepColumnsVisible is set, fixed columns with set width may still be shrunk down if there's not enough space in the host.");

        static pyTableFlags flags2 = pyTableFlags_None;
        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_NoKeepColumnsVisible", &flags2, pyTableFlags_NoKeepColumnsVisible);
        py::CheckboxFlags("pyTableFlags_BordersInnerV", &flags2, pyTableFlags_BordersInnerV);
        py::CheckboxFlags("pyTableFlags_BordersOuterV", &flags2, pyTableFlags_BordersOuterV);
        PopStyleCompact();
        if (py::BeginTable("table2", 4, flags2))
        {
            // We could also set pyTableFlags_SizingFixedFit on the table and all columns will default to pyTableColumnFlags_WidthFixed.
            py::TableSetupColumn("", pyTableColumnFlags_WidthFixed, 100.0f);
            py::TableSetupColumn("", pyTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
            py::TableSetupColumn("", pyTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 30.0f);
            py::TableSetupColumn("", pyTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 4; column++)
                {
                    py::TableSetColumnIndex(column);
                    if (row == 0)
                        py::Text("(w: %5.1f)", py::GetContentRegionAvail().x);
                    else
                        py::Text("Hello %d,%d", column, row);
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Nested tables"))
    {
        HelpMarker("This demonstrate embedding a table into another table cell.");

        if (py::BeginTable("table_nested1", 2, pyTableFlags_Borders | pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable))
        {
            py::TableSetupColumn("A0");
            py::TableSetupColumn("A1");
            py::TableHeadersRow();

            py::TableNextColumn();
            py::Text("A0 Row 0");
            {
                float rows_height = TEXT_BASE_HEIGHT * 2;
                if (py::BeginTable("table_nested2", 2, pyTableFlags_Borders | pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable))
                {
                    py::TableSetupColumn("B0");
                    py::TableSetupColumn("B1");
                    py::TableHeadersRow();

                    py::TableNextRow(pyTableRowFlags_None, rows_height);
                    py::TableNextColumn();
                    py::Text("B0 Row 0");
                    py::TableNextColumn();
                    py::Text("B1 Row 0");
                    py::TableNextRow(pyTableRowFlags_None, rows_height);
                    py::TableNextColumn();
                    py::Text("B0 Row 1");
                    py::TableNextColumn();
                    py::Text("B1 Row 1");

                    py::EndTable();
                }
            }
            py::TableNextColumn(); py::Text("A1 Row 0");
            py::TableNextColumn(); py::Text("A0 Row 1");
            py::TableNextColumn(); py::Text("A1 Row 1");
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Row height"))
    {
        HelpMarker("You can pass a 'min_row_height' to TableNextRow().\n\nRows are padded with 'style.CellPadding.y' on top and bottom, so effectively the minimum row height will always be >= 'style.CellPadding.y * 2.0f'.\n\nWe cannot honor a _maximum_ row height as that would requires a unique clipping rectangle per row.");
        if (py::BeginTable("table_row_height", 1, pyTableFlags_BordersOuter | pyTableFlags_BordersInnerV))
        {
            for (int row = 0; row < 10; row++)
            {
                float min_row_height = (float)(int)(TEXT_BASE_HEIGHT * 0.30f * row);
                py::TableNextRow(pyTableRowFlags_None, min_row_height);
                py::TableNextColumn();
                py::Text("min_row_height = %.2f", min_row_height);
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Outer size"))
    {
        // Showcasing use of pyTableFlags_NoHostExtendX and pyTableFlags_NoHostExtendY
        // Important to that note how the two flags have slightly different behaviors!
        py::Text("Using NoHostExtendX and NoHostExtendY:");
        PushStyleCompact();
        static pyTableFlags flags = pyTableFlags_Borders | pyTableFlags_Resizable | pyTableFlags_ContextMenuInBody | pyTableFlags_RowBg | pyTableFlags_SizingFixedFit | pyTableFlags_NoHostExtendX;
        py::CheckboxFlags("pyTableFlags_NoHostExtendX", &flags, pyTableFlags_NoHostExtendX);
        py::SameLine(); HelpMarker("Make outer width auto-fit to columns, overriding outer_size.x value.\n\nOnly available when ScrollX/ScrollY are disabled and Stretch columns are not used.");
        py::CheckboxFlags("pyTableFlags_NoHostExtendY", &flags, pyTableFlags_NoHostExtendY);
        py::SameLine(); HelpMarker("Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit).\n\nOnly available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.");
        PopStyleCompact();

        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5.5f);
        if (py::BeginTable("table1", 3, flags, outer_size))
        {
            for (int row = 0; row < 10; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableNextColumn();
                    py::Text("Cell %d,%d", column, row);
                }
            }
            py::EndTable();
        }
        py::SameLine();
        py::Text("Hello!");

        py::Spacing();

        py::Text("Using explicit size:");
        if (py::BeginTable("table2", 3, pyTableFlags_Borders | pyTableFlags_RowBg, ImVec2(TEXT_BASE_WIDTH * 30, 0.0f)))
        {
            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    py::TableNextColumn();
                    py::Text("Cell %d,%d", column, row);
                }
            }
            py::EndTable();
        }
        py::SameLine();
        if (py::BeginTable("table3", 3, pyTableFlags_Borders | pyTableFlags_RowBg, ImVec2(TEXT_BASE_WIDTH * 30, 0.0f)))
        {
            for (int row = 0; row < 3; row++)
            {
                py::TableNextRow(0, TEXT_BASE_HEIGHT * 1.5f);
                for (int column = 0; column < 3; column++)
                {
                    py::TableNextColumn();
                    py::Text("Cell %d,%d", column, row);
                }
            }
            py::EndTable();
        }

        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Background color"))
    {
        static pyTableFlags flags = pyTableFlags_RowBg;
        static int row_bg_type = 1;
        static int row_bg_target = 1;
        static int cell_bg_type = 1;

        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_Borders", &flags, pyTableFlags_Borders);
        py::CheckboxFlags("pyTableFlags_RowBg", &flags, pyTableFlags_RowBg);
        py::SameLine(); HelpMarker("pyTableFlags_RowBg automatically sets RowBg0 to alternative colors pulled from the Style.");
        py::Combo("row bg type", (int*)&row_bg_type, "None\0Red\0Gradient\0");
        py::Combo("row bg target", (int*)&row_bg_target, "RowBg0\0RowBg1\0"); py::SameLine(); HelpMarker("Target RowBg0 to override the alternating odd/even colors,\nTarget RowBg1 to blend with them.");
        py::Combo("cell bg type", (int*)&cell_bg_type, "None\0Blue\0"); py::SameLine(); HelpMarker("We are colorizing cells to B1->C2 here.");
        IM_ASSERT(row_bg_type >= 0 && row_bg_type <= 2);
        IM_ASSERT(row_bg_target >= 0 && row_bg_target <= 1);
        IM_ASSERT(cell_bg_type >= 0 && cell_bg_type <= 1);
        PopStyleCompact();

        if (py::BeginTable("table1", 5, flags))
        {
            for (int row = 0; row < 6; row++)
            {
                py::TableNextRow();

                // Demonstrate setting a row background color with 'py::TableSetBgColor(pyTableBgTarget_RowBgX, ...)'
                // We use a transparent color so we can see the one behind in case our target is RowBg1 and RowBg0 was already targeted by the pyTableFlags_RowBg flag.
                if (row_bg_type != 0)
                {
                    ImU32 row_bg_color = py::GetColorU32(row_bg_type == 1 ? ImVec4(0.7f, 0.3f, 0.3f, 0.65f) : ImVec4(0.2f + row * 0.1f, 0.2f, 0.2f, 0.65f)); // Flat or Gradient?
                    py::TableSetBgColor(pyTableBgTarget_RowBg0 + row_bg_target, row_bg_color);
                }

                // Fill cells
                for (int column = 0; column < 5; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("%c%c", 'A' + row, '0' + column);

                    // Change background of Cells B1->C2
                    // Demonstrate setting a cell background color with 'py::TableSetBgColor(pyTableBgTarget_CellBg, ...)'
                    // (the CellBg color will be blended over the RowBg and ColumnBg colors)
                    // We can also pass a column number as a third parameter to TableSetBgColor() and do this outside the column loop.
                    if (row >= 1 && row <= 2 && column >= 1 && column <= 2 && cell_bg_type == 1)
                    {
                        ImU32 cell_bg_color = py::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                        py::TableSetBgColor(pyTableBgTarget_CellBg, cell_bg_color);
                    }
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Tree view"))
    {
        static pyTableFlags flags = pyTableFlags_BordersV | pyTableFlags_BordersOuterH | pyTableFlags_Resizable | pyTableFlags_RowBg | pyTableFlags_NoBordersInBody;

        if (py::BeginTable("3ways", 3, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            py::TableSetupColumn("Name", pyTableColumnFlags_NoHide);
            py::TableSetupColumn("Size", pyTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            py::TableSetupColumn("Type", pyTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            py::TableHeadersRow();

            // Simple storage to output a dummy file-system.
            struct MyTreeNode
            {
                const char*     Name;
                const char*     Type;
                int             Size;
                int             ChildIdx;
                int             ChildCount;
                static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
                {
                    py::TableNextRow();
                    py::TableNextColumn();
                    const bool is_folder = (node->ChildCount > 0);
                    if (is_folder)
                    {
                        bool open = py::TreeNodeEx(node->Name, pyTreeNodeFlags_SpanFullWidth);
                        py::TableNextColumn();
                        py::TextDisabled("--");
                        py::TableNextColumn();
                        py::TextUnformatted(node->Type);
                        if (open)
                        {
                            for (int child_n = 0; child_n < node->ChildCount; child_n++)
                                DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
                            py::TreePop();
                        }
                    }
                    else
                    {
                        py::TreeNodeEx(node->Name, pyTreeNodeFlags_Leaf | pyTreeNodeFlags_Bullet | pyTreeNodeFlags_NoTreePushOnOpen | pyTreeNodeFlags_SpanFullWidth);
                        py::TableNextColumn();
                        py::Text("%d", node->Size);
                        py::TableNextColumn();
                        py::TextUnformatted(node->Type);
                    }
                }
            };
            static const MyTreeNode nodes[] =
            {
                { "Root",                         "Folder",       -1,       1, 3    }, // 0
                { "Music",                        "Folder",       -1,       4, 2    }, // 1
                { "Textures",                     "Folder",       -1,       6, 3    }, // 2
                { "desktop.ini",                  "System file",  1024,    -1,-1    }, // 3
                { "File1_a.wav",                  "Audio file",   123000,  -1,-1    }, // 4
                { "File1_b.wav",                  "Audio file",   456000,  -1,-1    }, // 5
                { "Image001.png",                 "Image file",   203128,  -1,-1    }, // 6
                { "Copy of Image001.png",         "Image file",   203256,  -1,-1    }, // 7
                { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
            };

            MyTreeNode::DisplayNode(&nodes[0], nodes);

            py::EndTable();
        }
        py::TreePop();
    }

    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Item width"))
    {
        HelpMarker(
            "Showcase using PushItemWidth() and how it is preserved on a per-column basis.\n\n"
            "Note that on auto-resizing non-resizable fixed columns, querying the content width for e.g. right-alignment doesn't make sense.");
        if (py::BeginTable("table_item_width", 3, pyTableFlags_Borders))
        {
            py::TableSetupColumn("small");
            py::TableSetupColumn("half");
            py::TableSetupColumn("right-align");
            py::TableHeadersRow();

            for (int row = 0; row < 3; row++)
            {
                py::TableNextRow();
                if (row == 0)
                {
                    // Setup ItemWidth once (instead of setting up every time, which is also possible but less efficient)
                    py::TableSetColumnIndex(0);
                    py::PushItemWidth(TEXT_BASE_WIDTH * 3.0f); // Small
                    py::TableSetColumnIndex(1);
                    py::PushItemWidth(-py::GetContentRegionAvail().x * 0.5f);
                    py::TableSetColumnIndex(2);
                    py::PushItemWidth(-FLT_MIN); // Right-aligned
                }

                // Draw our contents
                static float dummy_f = 0.0f;
                py::PushID(row);
                py::TableSetColumnIndex(0);
                py::SliderFloat("float0", &dummy_f, 0.0f, 1.0f);
                py::TableSetColumnIndex(1);
                py::SliderFloat("float1", &dummy_f, 0.0f, 1.0f);
                py::TableSetColumnIndex(2);
                py::SliderFloat("float2", &dummy_f, 0.0f, 1.0f);
                py::PopID();
            }
            py::EndTable();
        }
        py::TreePop();
    }

    // Demonstrate using TableHeader() calls instead of TableHeadersRow()
    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Custom headers"))
    {
        const int COLUMNS_COUNT = 3;
        if (py::BeginTable("table_custom_headers", COLUMNS_COUNT, pyTableFlags_Borders | pyTableFlags_Reorderable | pyTableFlags_Hideable))
        {
            py::TableSetupColumn("Apricot");
            py::TableSetupColumn("Banana");
            py::TableSetupColumn("Cherry");

            // Dummy entire-column selection storage
            // FIXME: It would be nice to actually demonstrate full-featured selection using those checkbox.
            static bool column_selected[3] = {};

            // Instead of calling TableHeadersRow() we'll submit custom headers ourselves
            py::TableNextRow(pyTableRowFlags_Headers);
            for (int column = 0; column < COLUMNS_COUNT; column++)
            {
                py::TableSetColumnIndex(column);
                const char* column_name = py::TableGetColumnName(column); // Retrieve name passed to TableSetupColumn()
                py::PushID(column);
                py::PushStyleVar(pyStyleVar_FramePadding, ImVec2(0, 0));
                py::Checkbox("##checkall", &column_selected[column]);
                py::PopStyleVar();
                py::SameLine(0.0f, py::GetStyle().ItemInnerSpacing.x);
                py::TableHeader(column_name);
                py::PopID();
            }

            for (int row = 0; row < 5; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    char buf[32];
                    sprintf(buf, "Cell %d,%d", column, row);
                    py::TableSetColumnIndex(column);
                    py::Selectable(buf, column_selected[column]);
                }
            }
            py::EndTable();
        }
        py::TreePop();
    }

    // Demonstrate creating custom context menus inside columns, while playing it nice with context menus provided by TableHeadersRow()/TableHeader()
    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Context menus"))
    {
        HelpMarker("By default, right-clicking over a TableHeadersRow()/TableHeader() line will open the default context-menu.\nUsing pyTableFlags_ContextMenuInBody we also allow right-clicking over columns body.");
        static pyTableFlags flags1 = pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable | pyTableFlags_Borders | pyTableFlags_ContextMenuInBody;

        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_ContextMenuInBody", &flags1, pyTableFlags_ContextMenuInBody);
        PopStyleCompact();

        // Context Menus: first example
        // [1.1] Right-click on the TableHeadersRow() line to open the default table context menu.
        // [1.2] Right-click in columns also open the default table context menu (if pyTableFlags_ContextMenuInBody is set)
        const int COLUMNS_COUNT = 3;
        if (py::BeginTable("table_context_menu", COLUMNS_COUNT, flags1))
        {
            py::TableSetupColumn("One");
            py::TableSetupColumn("Two");
            py::TableSetupColumn("Three");

            // [1.1]] Right-click on the TableHeadersRow() line to open the default table context menu.
            py::TableHeadersRow();

            // Submit dummy contents
            for (int row = 0; row < 4; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < COLUMNS_COUNT; column++)
                {
                    py::TableSetColumnIndex(column);
                    py::Text("Cell %d,%d", column, row);
                }
            }
            py::EndTable();
        }

        // Context Menus: second example
        // [2.1] Right-click on the TableHeadersRow() line to open the default table context menu.
        // [2.2] Right-click on the ".." to open a custom popup
        // [2.3] Right-click in columns to open another custom popup
        HelpMarker("Demonstrate mixing table context menu (over header), item context button (over button) and custom per-colum context menu (over column body).");
        pyTableFlags flags2 = pyTableFlags_Resizable | pyTableFlags_SizingFixedFit | pyTableFlags_Reorderable | pyTableFlags_Hideable | pyTableFlags_Borders;
        if (py::BeginTable("table_context_menu_2", COLUMNS_COUNT, flags2))
        {
            py::TableSetupColumn("One");
            py::TableSetupColumn("Two");
            py::TableSetupColumn("Three");

            // [2.1] Right-click on the TableHeadersRow() line to open the default table context menu.
            py::TableHeadersRow();
            for (int row = 0; row < 4; row++)
            {
                py::TableNextRow();
                for (int column = 0; column < COLUMNS_COUNT; column++)
                {
                    // Submit dummy contents
                    py::TableSetColumnIndex(column);
                    py::Text("Cell %d,%d", column, row);
                    py::SameLine();

                    // [2.2] Right-click on the ".." to open a custom popup
                    py::PushID(row * COLUMNS_COUNT + column);
                    py::SmallButton("..");
                    if (py::BeginPopupContextItem())
                    {
                        py::Text("This is the popup for Button(\"..\") in Cell %d,%d", column, row);
                        if (py::Button("Close"))
                            py::CloseCurrentPopup();
                        py::EndPopup();
                    }
                    py::PopID();
                }
            }

            // [2.3] Right-click anywhere in columns to open another custom popup
            // (instead of testing for !IsAnyItemHovered() we could also call OpenPopup() with pyPopupFlags_NoOpenOverExistingPopup
            // to manage popup priority as the popups triggers, here "are we hovering a column" are overlapping)
            int hovered_column = -1;
            for (int column = 0; column < COLUMNS_COUNT + 1; column++)
            {
                py::PushID(column);
                if (py::TableGetColumnFlags(column) & pyTableColumnFlags_IsHovered)
                    hovered_column = column;
                if (hovered_column == column && !py::IsAnyItemHovered() && py::IsMouseReleased(1))
                    py::OpenPopup("MyPopup");
                if (py::BeginPopup("MyPopup"))
                {
                    if (column == COLUMNS_COUNT)
                        py::Text("This is a custom popup for unused space after the last column.");
                    else
                        py::Text("This is a custom popup for Column %d", column);
                    if (py::Button("Close"))
                        py::CloseCurrentPopup();
                    py::EndPopup();
                }
                py::PopID();
            }

            py::EndTable();
            py::Text("Hovered column: %d", hovered_column);
        }
        py::TreePop();
    }

    // Demonstrate creating multiple tables with the same ID
    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Synced instances"))
    {
        HelpMarker("Multiple tables with the same identifier will share their settings, width, visibility, order etc.");
        for (int n = 0; n < 3; n++)
        {
            char buf[32];
            sprintf(buf, "Synced Table %d", n);
            bool open = py::CollapsingHeader(buf, pyTreeNodeFlags_DefaultOpen);
            if (open && py::BeginTable("Table", 3, pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable | pyTableFlags_Borders | pyTableFlags_SizingFixedFit | pyTableFlags_NoSavedSettings))
            {
                py::TableSetupColumn("One");
                py::TableSetupColumn("Two");
                py::TableSetupColumn("Three");
                py::TableHeadersRow();
                for (int cell = 0; cell < 9; cell++)
                {
                    py::TableNextColumn();
                    py::Text("this cell %d", cell);
                }
                py::EndTable();
            }
        }
        py::TreePop();
    }

    // Demonstrate using Sorting facilities
    // This is a simplified version of the "Advanced" example, where we mostly focus on the code necessary to handle sorting.
    // Note that the "Advanced" example also showcase manually triggering a sort (e.g. if item quantities have been modified)
    static const char* template_items_names[] =
    {
        "Banana", "Apple", "Cherry", "Watermelon", "Grapefruit", "Strawberry", "Mango",
        "Kiwi", "Orange", "Pineapple", "Blueberry", "Plum", "Coconut", "Pear", "Apricot"
    };
    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Sorting"))
    {
        // Create item list
        static ImVector<MyItem> items;
        if (items.Size == 0)
        {
            items.resize(50, MyItem());
            for (int n = 0; n < items.Size; n++)
            {
                const int template_n = n % IM_ARRAYSIZE(template_items_names);
                MyItem& item = items[n];
                item.ID = n;
                item.Name = template_items_names[template_n];
                item.Quantity = (n * n - n) % 20; // Assign default quantities
            }
        }

        // Options
        static pyTableFlags flags =
            pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable | pyTableFlags_Sortable | pyTableFlags_SortMulti
            | pyTableFlags_RowBg | pyTableFlags_BordersOuter | pyTableFlags_BordersV | pyTableFlags_NoBordersInBody
            | pyTableFlags_ScrollY;
        PushStyleCompact();
        py::CheckboxFlags("pyTableFlags_SortMulti", &flags, pyTableFlags_SortMulti);
        py::SameLine(); HelpMarker("When sorting is enabled: hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).");
        py::CheckboxFlags("pyTableFlags_SortTristate", &flags, pyTableFlags_SortTristate);
        py::SameLine(); HelpMarker("When sorting is enabled: allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).");
        PopStyleCompact();

        if (py::BeginTable("table_sorting", 4, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            // Demonstrate using a mixture of flags among available sort-related flags:
            // - pyTableColumnFlags_DefaultSort
            // - pyTableColumnFlags_NoSort / pyTableColumnFlags_NoSortAscending / pyTableColumnFlags_NoSortDescending
            // - pyTableColumnFlags_PreferSortAscending / pyTableColumnFlags_PreferSortDescending
            py::TableSetupColumn("ID",       pyTableColumnFlags_DefaultSort          | pyTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_ID);
            py::TableSetupColumn("Name",                                                  pyTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_Name);
            py::TableSetupColumn("Action",   pyTableColumnFlags_NoSort               | pyTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_Action);
            py::TableSetupColumn("Quantity", pyTableColumnFlags_PreferSortDescending | pyTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Quantity);
            py::TableSetupScrollFreeze(0, 1); // Make row always visible
            py::TableHeadersRow();

            // Sort our data if sort specs have been changed!
            if (pyTableSortSpecs* sorts_specs = py::TableGetSortSpecs())
                if (sorts_specs->SpecsDirty)
                {
                    MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                    if (items.Size > 1)
                        qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                    MyItem::s_current_sort_specs = NULL;
                    sorts_specs->SpecsDirty = false;
                }

            // Demonstrate using clipper for large vertical lists
            pyListClipper clipper;
            clipper.Begin(items.Size);
            while (clipper.Step())
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    // Display a data item
                    MyItem* item = &items[row_n];
                    py::PushID(item->ID);
                    py::TableNextRow();
                    py::TableNextColumn();
                    py::Text("%04d", item->ID);
                    py::TableNextColumn();
                    py::TextUnformatted(item->Name);
                    py::TableNextColumn();
                    py::SmallButton("None");
                    py::TableNextColumn();
                    py::Text("%d", item->Quantity);
                    py::PopID();
                }
            py::EndTable();
        }
        py::TreePop();
    }

    // In this example we'll expose most table flags and settings.
    // For specific flags and settings refer to the corresponding section for more detailed explanation.
    // This section is mostly useful to experiment with combining certain flags or settings with each others.
    //py::SetNextItemOpen(true, pyCond_Once); // [DEBUG]
    if (open_action != -1)
        py::SetNextItemOpen(open_action != 0);
    if (py::TreeNode("Advanced"))
    {
        static pyTableFlags flags =
            pyTableFlags_Resizable | pyTableFlags_Reorderable | pyTableFlags_Hideable
            | pyTableFlags_Sortable | pyTableFlags_SortMulti
            | pyTableFlags_RowBg | pyTableFlags_Borders | pyTableFlags_NoBordersInBody
            | pyTableFlags_ScrollX | pyTableFlags_ScrollY
            | pyTableFlags_SizingFixedFit;

        enum ContentsType { CT_Text, CT_Button, CT_SmallButton, CT_FillButton, CT_Selectable, CT_SelectableSpanRow };
        static int contents_type = CT_SelectableSpanRow;
        const char* contents_type_names[] = { "Text", "Button", "SmallButton", "FillButton", "Selectable", "Selectable (span row)" };
        static int freeze_cols = 1;
        static int freeze_rows = 1;
        static int items_count = IM_ARRAYSIZE(template_items_names) * 2;
        static ImVec2 outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 12);
        static float row_min_height = 0.0f; // Auto
        static float inner_width_with_scroll = 0.0f; // Auto-extend
        static bool outer_size_enabled = true;
        static bool show_headers = true;
        static bool show_wrapped_text = false;
        //static pyTextFilter filter;
        //py::SetNextItemOpen(true, pyCond_Once); // FIXME-TABLE: Enabling this results in initial clipped first pass on table which tend to affects column sizing
        if (py::TreeNode("Options"))
        {
            // Make the UI compact because there are so many fields
            PushStyleCompact();
            py::PushItemWidth(TEXT_BASE_WIDTH * 28.0f);

            if (py::TreeNodeEx("Features:", pyTreeNodeFlags_DefaultOpen))
            {
                py::CheckboxFlags("pyTableFlags_Resizable", &flags, pyTableFlags_Resizable);
                py::CheckboxFlags("pyTableFlags_Reorderable", &flags, pyTableFlags_Reorderable);
                py::CheckboxFlags("pyTableFlags_Hideable", &flags, pyTableFlags_Hideable);
                py::CheckboxFlags("pyTableFlags_Sortable", &flags, pyTableFlags_Sortable);
                py::CheckboxFlags("pyTableFlags_NoSavedSettings", &flags, pyTableFlags_NoSavedSettings);
                py::CheckboxFlags("pyTableFlags_ContextMenuInBody", &flags, pyTableFlags_ContextMenuInBody);
                py::TreePop();
            }

            if (py::TreeNodeEx("Decorations:", pyTreeNodeFlags_DefaultOpen))
            {
                py::CheckboxFlags("pyTableFlags_RowBg", &flags, pyTableFlags_RowBg);
                py::CheckboxFlags("pyTableFlags_BordersV", &flags, pyTableFlags_BordersV);
                py::CheckboxFlags("pyTableFlags_BordersOuterV", &flags, pyTableFlags_BordersOuterV);
                py::CheckboxFlags("pyTableFlags_BordersInnerV", &flags, pyTableFlags_BordersInnerV);
                py::CheckboxFlags("pyTableFlags_BordersH", &flags, pyTableFlags_BordersH);
                py::CheckboxFlags("pyTableFlags_BordersOuterH", &flags, pyTableFlags_BordersOuterH);
                py::CheckboxFlags("pyTableFlags_BordersInnerH", &flags, pyTableFlags_BordersInnerH);
                py::CheckboxFlags("pyTableFlags_NoBordersInBody", &flags, pyTableFlags_NoBordersInBody); py::SameLine(); HelpMarker("Disable vertical borders in columns Body (borders will always appears in Headers");
                py::CheckboxFlags("pyTableFlags_NoBordersInBodyUntilResize", &flags, pyTableFlags_NoBordersInBodyUntilResize); py::SameLine(); HelpMarker("Disable vertical borders in columns Body until hovered for resize (borders will always appears in Headers)");
                py::TreePop();
            }

            if (py::TreeNodeEx("Sizing:", pyTreeNodeFlags_DefaultOpen))
            {
                EditTableSizingFlags(&flags);
                py::SameLine(); HelpMarker("In the Advanced demo we override the policy of each column so those table-wide settings have less effect that typical.");
                py::CheckboxFlags("pyTableFlags_NoHostExtendX", &flags, pyTableFlags_NoHostExtendX);
                py::SameLine(); HelpMarker("Make outer width auto-fit to columns, overriding outer_size.x value.\n\nOnly available when ScrollX/ScrollY are disabled and Stretch columns are not used.");
                py::CheckboxFlags("pyTableFlags_NoHostExtendY", &flags, pyTableFlags_NoHostExtendY);
                py::SameLine(); HelpMarker("Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit).\n\nOnly available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.");
                py::CheckboxFlags("pyTableFlags_NoKeepColumnsVisible", &flags, pyTableFlags_NoKeepColumnsVisible);
                py::SameLine(); HelpMarker("Only available if ScrollX is disabled.");
                py::CheckboxFlags("pyTableFlags_PreciseWidths", &flags, pyTableFlags_PreciseWidths);
                py::SameLine(); HelpMarker("Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.");
                py::CheckboxFlags("pyTableFlags_NoClip", &flags, pyTableFlags_NoClip);
                py::SameLine(); HelpMarker("Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with ScrollFreeze options.");
                py::TreePop();
            }

            if (py::TreeNodeEx("Padding:", pyTreeNodeFlags_DefaultOpen))
            {
                py::CheckboxFlags("pyTableFlags_PadOuterX", &flags, pyTableFlags_PadOuterX);
                py::CheckboxFlags("pyTableFlags_NoPadOuterX", &flags, pyTableFlags_NoPadOuterX);
                py::CheckboxFlags("pyTableFlags_NoPadInnerX", &flags, pyTableFlags_NoPadInnerX);
                py::TreePop();
            }

            if (py::TreeNodeEx("Scrolling:", pyTreeNodeFlags_DefaultOpen))
            {
                py::CheckboxFlags("pyTableFlags_ScrollX", &flags, pyTableFlags_ScrollX);
                py::SameLine();
                py::SetNextItemWidth(py::GetFrameHeight());
                py::DragInt("freeze_cols", &freeze_cols, 0.2f, 0, 9, NULL, pySliderFlags_NoInput);
                py::CheckboxFlags("pyTableFlags_ScrollY", &flags, pyTableFlags_ScrollY);
                py::SameLine();
                py::SetNextItemWidth(py::GetFrameHeight());
                py::DragInt("freeze_rows", &freeze_rows, 0.2f, 0, 9, NULL, pySliderFlags_NoInput);
                py::TreePop();
            }

            if (py::TreeNodeEx("Sorting:", pyTreeNodeFlags_DefaultOpen))
            {
                py::CheckboxFlags("pyTableFlags_SortMulti", &flags, pyTableFlags_SortMulti);
                py::SameLine(); HelpMarker("When sorting is enabled: hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).");
                py::CheckboxFlags("pyTableFlags_SortTristate", &flags, pyTableFlags_SortTristate);
                py::SameLine(); HelpMarker("When sorting is enabled: allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).");
                py::TreePop();
            }

            if (py::TreeNodeEx("Other:", pyTreeNodeFlags_DefaultOpen))
            {
                py::Checkbox("show_headers", &show_headers);
                py::Checkbox("show_wrapped_text", &show_wrapped_text);

                py::DragFloat2("##OuterSize", &outer_size_value.x);
                py::SameLine(0.0f, py::GetStyle().ItemInnerSpacing.x);
                py::Checkbox("outer_size", &outer_size_enabled);
                py::SameLine();
                HelpMarker("If scrolling is disabled (ScrollX and ScrollY not set):\n"
                    "- The table is output directly in the parent window.\n"
                    "- OuterSize.x < 0.0f will right-align the table.\n"
                    "- OuterSize.x = 0.0f will narrow fit the table unless there are any Stretch column.\n"
                    "- OuterSize.y then becomes the minimum size for the table, which will extend vertically if there are more rows (unless NoHostExtendY is set).");

                // From a user point of view we will tend to use 'inner_width' differently depending on whether our table is embedding scrolling.
                // To facilitate toying with this demo we will actually pass 0.0f to the BeginTable() when ScrollX is disabled.
                py::DragFloat("inner_width (when ScrollX active)", &inner_width_with_scroll, 1.0f, 0.0f, FLT_MAX);

                py::DragFloat("row_min_height", &row_min_height, 1.0f, 0.0f, FLT_MAX);
                py::SameLine(); HelpMarker("Specify height of the Selectable item.");

                py::DragInt("items_count", &items_count, 0.1f, 0, 9999);
                py::Combo("items_type (first column)", &contents_type, contents_type_names, IM_ARRAYSIZE(contents_type_names));
                //filter.Draw("filter");
                py::TreePop();
            }

            py::PopItemWidth();
            PopStyleCompact();
            py::Spacing();
            py::TreePop();
        }

        // Update item list if we changed the number of items
        static ImVector<MyItem> items;
        static ImVector<int> selection;
        static bool items_need_sort = false;
        if (items.Size != items_count)
        {
            items.resize(items_count, MyItem());
            for (int n = 0; n < items_count; n++)
            {
                const int template_n = n % IM_ARRAYSIZE(template_items_names);
                MyItem& item = items[n];
                item.ID = n;
                item.Name = template_items_names[template_n];
                item.Quantity = (template_n == 3) ? 10 : (template_n == 4) ? 20 : 0; // Assign default quantities
            }
        }

        const ImDrawList* parent_draw_list = py::GetWindowDrawList();
        const int parent_draw_list_draw_cmd_count = parent_draw_list->CmdBuffer.Size;
        ImVec2 table_scroll_cur, table_scroll_max; // For debug display
        const ImDrawList* table_draw_list = NULL;  // "

        // Submit table
        const float inner_width_to_use = (flags & pyTableFlags_ScrollX) ? inner_width_with_scroll : 0.0f;
        if (py::BeginTable("table_advanced", 6, flags, outer_size_enabled ? outer_size_value : ImVec2(0, 0), inner_width_to_use))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            py::TableSetupColumn("ID",           pyTableColumnFlags_DefaultSort | pyTableColumnFlags_WidthFixed | pyTableColumnFlags_NoHide, 0.0f, MyItemColumnID_ID);
            py::TableSetupColumn("Name",         pyTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
            py::TableSetupColumn("Action",       pyTableColumnFlags_NoSort | pyTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
            py::TableSetupColumn("Quantity",     pyTableColumnFlags_PreferSortDescending, 0.0f, MyItemColumnID_Quantity);
            py::TableSetupColumn("Description",  (flags & pyTableFlags_NoHostExtendX) ? 0 : pyTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Description);
            py::TableSetupColumn("Hidden",       pyTableColumnFlags_DefaultHide | pyTableColumnFlags_NoSort);
            py::TableSetupScrollFreeze(freeze_cols, freeze_rows);

            // Sort our data if sort specs have been changed!
            pyTableSortSpecs* sorts_specs = py::TableGetSortSpecs();
            if (sorts_specs && sorts_specs->SpecsDirty)
                items_need_sort = true;
            if (sorts_specs && items_need_sort && items.Size > 1)
            {
                MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                MyItem::s_current_sort_specs = NULL;
                sorts_specs->SpecsDirty = false;
            }
            items_need_sort = false;

            // Take note of whether we are currently sorting based on the Quantity field,
            // we will use this to trigger sorting when we know the data of this column has been modified.
            const bool sorts_specs_using_quantity = (py::TableGetColumnFlags(3) & pyTableColumnFlags_IsSorted) != 0;

            // Show headers
            if (show_headers)
                py::TableHeadersRow();

            // Show data
            // FIXME-TABLE FIXME-NAV: How we can get decent up/down even though we have the buttons here?
            py::PushButtonRepeat(true);
#if 1
            // Demonstrate using clipper for large vertical lists
            pyListClipper clipper;
            clipper.Begin(items.Size);
            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
#else
            // Without clipper
            {
                for (int row_n = 0; row_n < items.Size; row_n++)
#endif
                {
                    MyItem* item = &items[row_n];
                    //if (!filter.PassFilter(item->Name))
                    //    continue;

                    const bool item_is_selected = selection.contains(item->ID);
                    py::PushID(item->ID);
                    py::TableNextRow(pyTableRowFlags_None, row_min_height);

                    // For the demo purpose we can select among different type of items submitted in the first column
                    py::TableSetColumnIndex(0);
                    char label[32];
                    sprintf(label, "%04d", item->ID);
                    if (contents_type == CT_Text)
                        py::TextUnformatted(label);
                    else if (contents_type == CT_Button)
                        py::Button(label);
                    else if (contents_type == CT_SmallButton)
                        py::SmallButton(label);
                    else if (contents_type == CT_FillButton)
                        py::Button(label, ImVec2(-FLT_MIN, 0.0f));
                    else if (contents_type == CT_Selectable || contents_type == CT_SelectableSpanRow)
                    {
                        pySelectableFlags selectable_flags = (contents_type == CT_SelectableSpanRow) ? pySelectableFlags_SpanAllColumns | pySelectableFlags_AllowItemOverlap : pySelectableFlags_None;
                        if (py::Selectable(label, item_is_selected, selectable_flags, ImVec2(0, row_min_height)))
                        {
                            if (py::GetIO().KeyCtrl)
                            {
                                if (item_is_selected)
                                    selection.find_erase_unsorted(item->ID);
                                else
                                    selection.push_back(item->ID);
                            }
                            else
                            {
                                selection.clear();
                                selection.push_back(item->ID);
                            }
                        }
                    }

                    if (py::TableSetColumnIndex(1))
                        py::TextUnformatted(item->Name);

                    // Here we demonstrate marking our data set as needing to be sorted again if we modified a quantity,
                    // and we are currently sorting on the column showing the Quantity.
                    // To avoid triggering a sort while holding the button, we only trigger it when the button has been released.
                    // You will probably need a more advanced system in your code if you want to automatically sort when a specific entry changes.
                    if (py::TableSetColumnIndex(2))
                    {
                        if (py::SmallButton("Chop")) { item->Quantity += 1; }
                        if (sorts_specs_using_quantity && py::IsItemDeactivated()) { items_need_sort = true; }
                        py::SameLine();
                        if (py::SmallButton("Eat")) { item->Quantity -= 1; }
                        if (sorts_specs_using_quantity && py::IsItemDeactivated()) { items_need_sort = true; }
                    }

                    if (py::TableSetColumnIndex(3))
                        py::Text("%d", item->Quantity);

                    py::TableSetColumnIndex(4);
                    if (show_wrapped_text)
                        py::TextWrapped("Lorem ipsum dolor sit amet");
                    else
                        py::Text("Lorem ipsum dolor sit amet");

                    if (py::TableSetColumnIndex(5))
                        py::Text("1234");

                    py::PopID();
                }
            }
            py::PopButtonRepeat();

            // Store some info to display debug details below
            table_scroll_cur = ImVec2(py::GetScrollX(), py::GetScrollY());
            table_scroll_max = ImVec2(py::GetScrollMaxX(), py::GetScrollMaxY());
            table_draw_list = py::GetWindowDrawList();
            py::EndTable();
        }
        static bool show_debug_details = false;
        py::Checkbox("Debug details", &show_debug_details);
        if (show_debug_details && table_draw_list)
        {
            py::SameLine(0.0f, 0.0f);
            const int table_draw_list_draw_cmd_count = table_draw_list->CmdBuffer.Size;
            if (table_draw_list == parent_draw_list)
                py::Text(": DrawCmd: +%d (in same window)",
                    table_draw_list_draw_cmd_count - parent_draw_list_draw_cmd_count);
            else
                py::Text(": DrawCmd: +%d (in child window), Scroll: (%.f/%.f) (%.f/%.f)",
                    table_draw_list_draw_cmd_count - 1, table_scroll_cur.x, table_scroll_max.x, table_scroll_cur.y, table_scroll_max.y);
        }
        py::TreePop();
    }

    py::PopID();

    ShowDemoWindowColumns();

    if (disable_indent)
        py::PopStyleVar();
}

// Demonstrate old/legacy Columns API!
// [2020: Columns are under-featured and not maintained. Prefer using the more flexible and powerful BeginTable() API!]
static void ShowDemoWindowColumns()
{
    bool open = py::TreeNode("Legacy Columns API");
    py::SameLine();
    HelpMarker("Columns() is an old API! Prefer using the more flexible and powerful BeginTable() API!");
    if (!open)
        return;

    // Basic columns
    if (py::TreeNode("Basic"))
    {
        py::Text("Without border:");
        py::Columns(3, "mycolumns3", false);  // 3-ways, no border
        py::Separator();
        for (int n = 0; n < 14; n++)
        {
            char label[32];
            sprintf(label, "Item %d", n);
            if (py::Selectable(label)) {}
            //if (py::Button(label, ImVec2(-FLT_MIN,0.0f))) {}
            py::NextColumn();
        }
        py::Columns(1);
        py::Separator();

        py::Text("With border:");
        py::Columns(4, "mycolumns"); // 4-ways, with border
        py::Separator();
        py::Text("ID"); py::NextColumn();
        py::Text("Name"); py::NextColumn();
        py::Text("Path"); py::NextColumn();
        py::Text("Hovered"); py::NextColumn();
        py::Separator();
        const char* names[3] = { "One", "Two", "Three" };
        const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (py::Selectable(label, selected == i, pySelectableFlags_SpanAllColumns))
                selected = i;
            bool hovered = py::IsItemHovered();
            py::NextColumn();
            py::Text(names[i]); py::NextColumn();
            py::Text(paths[i]); py::NextColumn();
            py::Text("%d", hovered); py::NextColumn();
        }
        py::Columns(1);
        py::Separator();
        py::TreePop();
    }

    if (py::TreeNode("Borders"))
    {
        // NB: Future columns API should allow automatic horizontal borders.
        static bool h_borders = true;
        static bool v_borders = true;
        static int columns_count = 4;
        const int lines_count = 3;
        py::SetNextItemWidth(py::GetFontSize() * 8);
        py::DragInt("##columns_count", &columns_count, 0.1f, 2, 10, "%d columns");
        if (columns_count < 2)
            columns_count = 2;
        py::SameLine();
        py::Checkbox("horizontal", &h_borders);
        py::SameLine();
        py::Checkbox("vertical", &v_borders);
        py::Columns(columns_count, NULL, v_borders);
        for (int i = 0; i < columns_count * lines_count; i++)
        {
            if (h_borders && py::GetColumnIndex() == 0)
                py::Separator();
            py::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
            py::Text("Width %.2f", py::GetColumnWidth());
            py::Text("Avail %.2f", py::GetContentRegionAvail().x);
            py::Text("Offset %.2f", py::GetColumnOffset());
            py::Text("Long text that is likely to clip");
            py::Button("Button", ImVec2(-FLT_MIN, 0.0f));
            py::NextColumn();
        }
        py::Columns(1);
        if (h_borders)
            py::Separator();
        py::TreePop();
    }

    // Create multiple items in a same cell before switching to next column
    if (py::TreeNode("Mixed items"))
    {
        py::Columns(3, "mixed");
        py::Separator();

        py::Text("Hello");
        py::Button("Banana");
        py::NextColumn();

        py::Text("py");
        py::Button("Apple");
        static float foo = 1.0f;
        py::InputFloat("red", &foo, 0.05f, 0, "%.3f");
        py::Text("An extra line here.");
        py::NextColumn();

        py::Text("Sailor");
        py::Button("Corniflower");
        static float bar = 1.0f;
        py::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
        py::NextColumn();

        if (py::CollapsingHeader("Category A")) { py::Text("Blah blah blah"); } py::NextColumn();
        if (py::CollapsingHeader("Category B")) { py::Text("Blah blah blah"); } py::NextColumn();
        if (py::CollapsingHeader("Category C")) { py::Text("Blah blah blah"); } py::NextColumn();
        py::Columns(1);
        py::Separator();
        py::TreePop();
    }

    // Word wrapping
    if (py::TreeNode("Word-wrapping"))
    {
        py::Columns(2, "word-wrapping");
        py::Separator();
        py::TextWrapped("The quick brown fox jumps over the lazy dog.");
        py::TextWrapped("Hello Left");
        py::NextColumn();
        py::TextWrapped("The quick brown fox jumps over the lazy dog.");
        py::TextWrapped("Hello Right");
        py::Columns(1);
        py::Separator();
        py::TreePop();
    }

    if (py::TreeNode("Horizontal Scrolling"))
    {
        py::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
        ImVec2 child_size = ImVec2(0, py::GetFontSize() * 20.0f);
        py::BeginChild("##ScrollingRegion", child_size, false, pyWindowFlags_HorizontalScrollbar);
        py::Columns(10);

        // Also demonstrate using clipper for large vertical lists
        int ITEMS_COUNT = 2000;
        pyListClipper clipper;
        clipper.Begin(ITEMS_COUNT);
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                for (int j = 0; j < 10; j++)
                {
                    py::Text("Line %d Column %d...", i, j);
                    py::NextColumn();
                }
        }
        py::Columns(1);
        py::EndChild();
        py::TreePop();
    }

    if (py::TreeNode("Tree"))
    {
        py::Columns(2, "tree", true);
        for (int x = 0; x < 3; x++)
        {
            bool open1 = py::TreeNode((void*)(intptr_t)x, "Node%d", x);
            py::NextColumn();
            py::Text("Node contents");
            py::NextColumn();
            if (open1)
            {
                for (int y = 0; y < 3; y++)
                {
                    bool open2 = py::TreeNode((void*)(intptr_t)y, "Node%d.%d", x, y);
                    py::NextColumn();
                    py::Text("Node contents");
                    if (open2)
                    {
                        py::Text("Even more contents");
                        if (py::TreeNode("Tree in column"))
                        {
                            py::Text("The quick brown fox jumps over the lazy dog");
                            py::TreePop();
                        }
                    }
                    py::NextColumn();
                    if (open2)
                        py::TreePop();
                }
                py::TreePop();
            }
        }
        py::Columns(1);
        py::TreePop();
    }

    py::TreePop();
}

static void ShowDemoWindowMisc()
{
    if (py::CollapsingHeader("Filtering"))
    {
        // Helper class to easy setup a text filter.
        // You may want to implement a more feature-full filtering scheme in your own application.
        static pyTextFilter filter;
        py::Text("Filter usage:\n"
                    "  \"\"         display all lines\n"
                    "  \"xxx\"      display lines containing \"xxx\"\n"
                    "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                    "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                py::BulletText("%s", lines[i]);
    }

    if (py::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        pyIO& io = py::GetIO();

        // Display pyIO output flags
        py::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
        py::Text("WantCaptureMouseUnlessPopupClose: %d", io.WantCaptureMouseUnlessPopupClose);
        py::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
        py::Text("WantTextInput: %d", io.WantTextInput);
        py::Text("WantSetMousePos: %d", io.WantSetMousePos);
        py::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

        // Display Mouse state
        if (py::TreeNode("Mouse State"))
        {
            if (py::IsMousePosValid())
                py::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                py::Text("Mouse pos: <INVALID>");
            py::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            py::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (py::IsMouseDown(i))         { py::SameLine(); py::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            py::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (py::IsMouseClicked(i))      { py::SameLine(); py::Text("b%d", i); }
            py::Text("Mouse dblclick:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (py::IsMouseDoubleClicked(i)){ py::SameLine(); py::Text("b%d", i); }
            py::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (py::IsMouseReleased(i))     { py::SameLine(); py::Text("b%d", i); }
            py::Text("Mouse wheel: %.1f", io.MouseWheel);
            py::Text("Pen Pressure: %.1f", io.PenPressure); // Note: currently unused
            py::TreePop();
        }

        // Display Keyboard/Mouse state
        if (py::TreeNode("Keyboard & Navigation State"))
        {
            py::Text("Keys down:");          for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (py::IsKeyDown(i))        { py::SameLine(); py::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
            py::Text("Keys pressed:");       for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (py::IsKeyPressed(i))     { py::SameLine(); py::Text("%d (0x%X)", i, i); }
            py::Text("Keys release:");       for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (py::IsKeyReleased(i))    { py::SameLine(); py::Text("%d (0x%X)", i, i); }
            py::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            py::Text("Chars queue:");        for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; py::SameLine();  py::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

            py::Text("NavInputs down:");     for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f)              { py::SameLine(); py::Text("[%d] %.2f (%.02f secs)", i, io.NavInputs[i], io.NavInputsDownDuration[i]); }
            py::Text("NavInputs pressed:");  for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { py::SameLine(); py::Text("[%d]", i); }

            py::Button("Hovering me sets the\nkeyboard capture flag");
            if (py::IsItemHovered())
                py::CaptureKeyboardFromApp(true);
            py::SameLine();
            py::Button("Holding me clears the\nthe keyboard capture flag");
            if (py::IsItemActive())
                py::CaptureKeyboardFromApp(false);
            py::TreePop();
        }

        if (py::TreeNode("Tabbing"))
        {
            py::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "hello";
            py::InputText("1", buf, IM_ARRAYSIZE(buf));
            py::InputText("2", buf, IM_ARRAYSIZE(buf));
            py::InputText("3", buf, IM_ARRAYSIZE(buf));
            py::PushAllowKeyboardFocus(false);
            py::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            py::SameLine(); HelpMarker("Item won't be cycled through when using TAB or Shift+Tab.");
            py::PopAllowKeyboardFocus();
            py::InputText("5", buf, IM_ARRAYSIZE(buf));
            py::TreePop();
        }

        if (py::TreeNode("Focus from code"))
        {
            bool focus_1 = py::Button("Focus on 1"); py::SameLine();
            bool focus_2 = py::Button("Focus on 2"); py::SameLine();
            bool focus_3 = py::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus_1) py::SetKeyboardFocusHere();
            py::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (py::IsItemActive()) has_focus = 1;

            if (focus_2) py::SetKeyboardFocusHere();
            py::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (py::IsItemActive()) has_focus = 2;

            py::PushAllowKeyboardFocus(false);
            if (focus_3) py::SetKeyboardFocusHere();
            py::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (py::IsItemActive()) has_focus = 3;
            py::SameLine(); HelpMarker("Item won't be cycled through when using TAB or Shift+Tab.");
            py::PopAllowKeyboardFocus();

            if (has_focus)
                py::Text("Item with focus: %d", has_focus);
            else
                py::Text("Item with focus: <none>");

            // Use >= 0 parameter to SetKeyboardFocusHere() to focus an upcoming item
            static float f3[3] = { 0.0f, 0.0f, 0.0f };
            int focus_ahead = -1;
            if (py::Button("Focus on X")) { focus_ahead = 0; } py::SameLine();
            if (py::Button("Focus on Y")) { focus_ahead = 1; } py::SameLine();
            if (py::Button("Focus on Z")) { focus_ahead = 2; }
            if (focus_ahead != -1) py::SetKeyboardFocusHere(focus_ahead);
            py::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

            py::TextWrapped("NB: Cursor & selection are preserved when refocusing last used item in code.");
            py::TreePop();
        }

        if (py::TreeNode("Dragging"))
        {
            py::TextWrapped("You can use py::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            for (int button = 0; button < 3; button++)
            {
                py::Text("IsMouseDragging(%d):", button);
                py::Text("  w/ default threshold: %d,", py::IsMouseDragging(button));
                py::Text("  w/ zero threshold: %d,", py::IsMouseDragging(button, 0.0f));
                py::Text("  w/ large threshold: %d,", py::IsMouseDragging(button, 20.0f));
            }

            py::Button("Drag Me");
            if (py::IsItemActive())
                py::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, py::GetColorU32(pyCol_Button), 4.0f); // Draw a line between the button and the mouse cursor

            // Drag operations gets "unlocked" when the mouse has moved past a certain threshold
            // (the default threshold is stored in io.MouseDragThreshold). You can request a lower or higher
            // threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta().
            ImVec2 value_raw = py::GetMouseDragDelta(0, 0.0f);
            ImVec2 value_with_lock_threshold = py::GetMouseDragDelta(0);
            ImVec2 mouse_delta = io.MouseDelta;
            py::Text("GetMouseDragDelta(0):");
            py::Text("  w/ default threshold: (%.1f, %.1f)", value_with_lock_threshold.x, value_with_lock_threshold.y);
            py::Text("  w/ zero threshold: (%.1f, %.1f)", value_raw.x, value_raw.y);
            py::Text("io.MouseDelta: (%.1f, %.1f)", mouse_delta.x, mouse_delta.y);
            py::TreePop();
        }

        if (py::TreeNode("Mouse cursors"))
        {
            const char* mouse_cursors_names[] = { "Arrow", "TextInput", "ResizeAll", "ResizeNS", "ResizeEW", "ResizeNESW", "ResizeNWSE", "Hand", "NotAllowed" };
            IM_ASSERT(IM_ARRAYSIZE(mouse_cursors_names) == pyMouseCursor_COUNT);

            pyMouseCursor current = py::GetMouseCursor();
            py::Text("Current mouse cursor = %d: %s", current, mouse_cursors_names[current]);
            py::Text("Hover to see mouse cursors:");
            py::SameLine(); HelpMarker(
                "Your application can render a different mouse cursor based on what py::GetMouseCursor() returns. "
                "If software cursor rendering (io.MouseDrawCursor) is set py will draw the right cursor for you, "
                "otherwise your backend needs to handle it.");
            for (int i = 0; i < pyMouseCursor_COUNT; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d: %s", i, mouse_cursors_names[i]);
                py::Bullet(); py::Selectable(label, false);
                if (py::IsItemHovered())
                    py::SetMouseCursor(i);
            }
            py::TreePop();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] About Window / ShowAboutWindow()
// Access from Dear py Demo -> Tools -> About
//-----------------------------------------------------------------------------

void py::ShowAboutWindow(bool* p_open)
{
    if (!py::Begin("About Dear py", p_open, pyWindowFlags_AlwaysAutoResize))
    {
        py::End();
        return;
    }
    py::Text("Dear py %s", py::GetVersion());
    py::Separator();
    py::Text("By Omar Cornut and all Dear py contributors.");
    py::Text("Dear py is licensed under the MIT License, see LICENSE for more information.");

    static bool show_config_info = false;
    py::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        pyIO& io = py::GetIO();
        pyStyle& style = py::GetStyle();

        bool copy_to_clipboard = py::Button("Copy to clipboard");
        ImVec2 child_size = ImVec2(0, py::GetTextLineHeightWithSpacing() * 18);
        py::BeginChildFrame(py::GetID("cfg_infos"), child_size, pyWindowFlags_NoMove);
        if (copy_to_clipboard)
        {
            py::LogToClipboard();
            py::LogText("```\n"); // Back quotes will make text appears without formatting when pasting on GitHub
        }

        py::Text("Dear py %s (%d)", py_VERSION, py_VERSION_NUM);
        py::Separator();
        py::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        py::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef py_DISABLE_OBSOLETE_FUNCTIONS
        py::Text("define: py_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef py_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        py::Text("define: py_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef py_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        py::Text("define: py_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef py_DISABLE_WIN32_FUNCTIONS
        py::Text("define: py_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef py_DISABLE_DEFAULT_FORMAT_FUNCTIONS
        py::Text("define: py_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef py_DISABLE_DEFAULT_MATH_FUNCTIONS
        py::Text("define: py_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef py_DISABLE_DEFAULT_FILE_FUNCTIONS
        py::Text("define: py_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef py_DISABLE_FILE_FUNCTIONS
        py::Text("define: py_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef py_DISABLE_DEFAULT_ALLOCATORS
        py::Text("define: py_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef py_USE_BGRA_PACKED_COLOR
        py::Text("define: py_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        py::Text("define: _WIN32");
#endif
#ifdef _WIN64
        py::Text("define: _WIN64");
#endif
#ifdef __linux__
        py::Text("define: __linux__");
#endif
#ifdef __APPLE__
        py::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        py::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
        py::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
        py::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        py::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        py::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        py::Text("define: __clang_version__=%s", __clang_version__);
#endif
        py::Separator();
        py::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        py::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        py::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & pyConfigFlags_NavEnableKeyboard)        py::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & pyConfigFlags_NavEnableGamepad)         py::Text(" NavEnableGamepad");
        if (io.ConfigFlags & pyConfigFlags_NavEnableSetMousePos)     py::Text(" NavEnableSetMousePos");
        if (io.ConfigFlags & pyConfigFlags_NavNoCaptureKeyboard)     py::Text(" NavNoCaptureKeyboard");
        if (io.ConfigFlags & pyConfigFlags_NoMouse)                  py::Text(" NoMouse");
        if (io.ConfigFlags & pyConfigFlags_NoMouseCursorChange)      py::Text(" NoMouseCursorChange");
        if (io.MouseDrawCursor)                                         py::Text("io.MouseDrawCursor");
        if (io.ConfigMacOSXBehaviors)                                   py::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigInputTextCursorBlink)                              py::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            py::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       py::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        if (io.ConfigMemoryCompactTimer >= 0.0f)                        py::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
        py::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & pyBackendFlags_HasGamepad)             py::Text(" HasGamepad");
        if (io.BackendFlags & pyBackendFlags_HasMouseCursors)        py::Text(" HasMouseCursors");
        if (io.BackendFlags & pyBackendFlags_HasSetMousePos)         py::Text(" HasSetMousePos");
        if (io.BackendFlags & pyBackendFlags_RendererHasVtxOffset)   py::Text(" RendererHasVtxOffset");
        py::Separator();
        py::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        py::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        py::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        py::Separator();
        py::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        py::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        py::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        py::Text("style.FrameRounding: %.2f", style.FrameRounding);
        py::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        py::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        py::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
        {
            py::LogText("\n```\n");
            py::LogFinish();
        }
        py::EndChildFrame();
    }
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Style Editor / ShowStyleEditor()
//-----------------------------------------------------------------------------
// - ShowFontSelector()
// - ShowStyleSelector()
// - ShowStyleEditor()
//-----------------------------------------------------------------------------

// Forward declare ShowFontAtlas() which isn't worth putting in public API yet
namespace py { py_API void ShowFontAtlas(ImFontAtlas* atlas); }

// Demo helper function to select among loaded fonts.
// Here we use the regular BeginCombo()/EndCombo() api which is more the more flexible one.
void py::ShowFontSelector(const char* label)
{
    pyIO& io = py::GetIO();
    ImFont* font_current = py::GetFont();
    if (py::BeginCombo(label, font_current->GetDebugName()))
    {
        for (int n = 0; n < io.Fonts->Fonts.Size; n++)
        {
            ImFont* font = io.Fonts->Fonts[n];
            py::PushID((void*)font);
            if (py::Selectable(font->GetDebugName(), font == font_current))
                io.FontDefault = font;
            py::PopID();
        }
        py::EndCombo();
    }
    py::SameLine();
    HelpMarker(
        "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
        "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
        "- Read FAQ and docs/FONTS.md for more details.\n"
        "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string.
// Useful for quick combo boxes where the choices are known locally.
bool py::ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (py::Combo(label, &style_idx, "Dark\0Light\0Classic\0"))
    {
        switch (style_idx)
        {
        case 0: py::StyleColorsDark(); break;
        case 1: py::StyleColorsLight(); break;
        case 2: py::StyleColorsClassic(); break;
        }
        return true;
    }
    return false;
}

void py::ShowStyleEditor(pyStyle* ref)
{
    // You can pass in a reference pyStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    pyStyle& style = py::GetStyle();
    static pyStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    py::PushItemWidth(py::GetWindowWidth() * 0.50f);

    if (py::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    py::ShowFontSelector("Fonts##Selector");

    // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
    if (py::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool border = (style.WindowBorderSize > 0.0f); if (py::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
    py::SameLine();
    { bool border = (style.FrameBorderSize > 0.0f);  if (py::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
    py::SameLine();
    { bool border = (style.PopupBorderSize > 0.0f);  if (py::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

    // Save/Revert button
    if (py::Button("Save Ref"))
        *ref = ref_saved_style = style;
    py::SameLine();
    if (py::Button("Revert Ref"))
        style = *ref;
    py::SameLine();
    HelpMarker(
        "Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
        "Use \"Export\" below to save them somewhere.");

    py::Separator();

    if (py::BeginTabBar("##tabs", pyTabBarFlags_None))
    {
        if (py::BeginTabItem("Sizes"))
        {
            py::Text("Main");
            py::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            py::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            py::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            py::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            py::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            py::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            py::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            py::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            py::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            py::Text("Borders");
            py::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            py::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            py::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            py::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            py::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            py::Text("Rounding");
            py::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            py::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            py::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            py::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            py::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            py::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            py::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            py::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            py::Text("Alignment");
            py::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (py::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            py::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            py::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            py::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            py::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            py::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            py::Text("Safe Area Padding");
            py::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            py::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            py::EndTabItem();
        }

        if (py::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (py::Button("Export"))
            {
                if (output_dest == 0)
                    py::LogToClipboard();
                else
                    py::LogToTTY();
                py::LogText("ImVec4* colors = py::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < pyCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = py::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        py::LogText("colors[pyCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                py::LogFinish();
            }
            py::SameLine(); py::SetNextItemWidth(120); py::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            py::SameLine(); py::Checkbox("Only Modified Colors", &output_only_modified);

            static pyTextFilter filter;
            filter.Draw("Filter colors", py::GetFontSize() * 16);

            static pyColorEditFlags alpha_flags = 0;
            if (py::RadioButton("Opaque", alpha_flags == pyColorEditFlags_None))             { alpha_flags = pyColorEditFlags_None; } py::SameLine();
            if (py::RadioButton("Alpha",  alpha_flags == pyColorEditFlags_AlphaPreview))     { alpha_flags = pyColorEditFlags_AlphaPreview; } py::SameLine();
            if (py::RadioButton("Both",   alpha_flags == pyColorEditFlags_AlphaPreviewHalf)) { alpha_flags = pyColorEditFlags_AlphaPreviewHalf; } py::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            py::BeginChild("##colors", ImVec2(0, 0), true, pyWindowFlags_AlwaysVerticalScrollbar | pyWindowFlags_AlwaysHorizontalScrollbar | pyWindowFlags_NavFlattened);
            py::PushItemWidth(-160);
            for (int i = 0; i < pyCol_COUNT; i++)
            {
                const char* name = py::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                py::PushID(i);
                py::ColorEdit4("##color", (float*)&style.Colors[i], pyColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    py::SameLine(0.0f, style.ItemInnerSpacing.x); if (py::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    py::SameLine(0.0f, style.ItemInnerSpacing.x); if (py::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                }
                py::SameLine(0.0f, style.ItemInnerSpacing.x);
                py::TextUnformatted(name);
                py::PopID();
            }
            py::PopItemWidth();
            py::EndChild();

            py::EndTabItem();
        }

        if (py::BeginTabItem("Fonts"))
        {
            pyIO& io = py::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
            py::ShowFontAtlas(atlas);

            // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
            // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
            HelpMarker(
                "Those are old settings provided for convenience.\n"
                "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
                "rebuild the font atlas, and call style.ScaleAllSizes() on a reference pyStyle structure.\n"
                "Using those settings here will give you poor quality results.");
            static float window_scale = 1.0f;
            py::PushItemWidth(py::GetFontSize() * 8);
            if (py::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", pySliderFlags_AlwaysClamp)) // Scale only this window
                py::SetWindowFontScale(window_scale);
            py::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", pySliderFlags_AlwaysClamp); // Scale everything
            py::PopItemWidth();

            py::EndTabItem();
        }

        if (py::BeginTabItem("Rendering"))
        {
            py::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            py::SameLine();
            HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

            py::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            py::SameLine();
            HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

            py::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            py::PushItemWidth(py::GetFontSize() * 8);
            py::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

            // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
            py::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", pySliderFlags_AlwaysClamp);
            if (py::IsItemActive())
            {
                py::SetNextWindowPos(py::GetCursorScreenPos());
                py::BeginTooltip();
                py::TextUnformatted("(R = radius, N = number of segments)");
                py::Spacing();
                ImDrawList* draw_list = py::GetWindowDrawList();
                const float min_widget_width = py::CalcTextSize("N: MMM\nR: MMM").x;
                for (int n = 0; n < 8; n++)
                {
                    const float RAD_MIN = 5.0f;
                    const float RAD_MAX = 70.0f;
                    const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

                    py::BeginGroup();

                    py::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

                    const float canvas_width = IM_MAX(min_widget_width, rad * 2.0f);
                    const float offset_x     = floorf(canvas_width * 0.5f);
                    const float offset_y     = floorf(RAD_MAX);

                    const ImVec2 p1 = py::GetCursorScreenPos();
                    draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, py::GetColorU32(pyCol_Text));
                    py::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

                    /*
                    const ImVec2 p2 = py::GetCursorScreenPos();
                    draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, py::GetColorU32(pyCol_Text));
                    py::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
                    */

                    py::EndGroup();
                    py::SameLine();
                }
                py::EndTooltip();
            }
            py::SameLine();
            HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

            py::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            py::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); py::SameLine(); HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
            py::PopItemWidth();

            py::EndTabItem();
        }

        py::EndTabBar();
    }

    py::PopItemWidth();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
//-----------------------------------------------------------------------------
// - ShowExampleAppMainMenuBar()
// - ShowExampleMenuFile()
//-----------------------------------------------------------------------------

// Demonstrate creating a "main" fullscreen menu bar and populating it.
// Note the difference between BeginMainMenuBar() and BeginMenuBar():
// - BeginMenuBar() = menu-bar inside current window (which needs the pyWindowFlags_MenuBar flag!)
// - BeginMainMenuBar() = helper to create menu-bar-sized window at the top of the main viewport + call BeginMenuBar() into it.
static void ShowExampleAppMainMenuBar()
{
    if (py::BeginMainMenuBar())
    {
        if (py::BeginMenu("File"))
        {
            ShowExampleMenuFile();
            py::EndMenu();
        }
        if (py::BeginMenu("Edit"))
        {
            if (py::MenuItem("Undo", "CTRL+Z")) {}
            if (py::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            py::Separator();
            if (py::MenuItem("Cut", "CTRL+X")) {}
            if (py::MenuItem("Copy", "CTRL+C")) {}
            if (py::MenuItem("Paste", "CTRL+V")) {}
            py::EndMenu();
        }
        py::EndMainMenuBar();
    }
}

// Note that shortcuts are currently provided for display only
// (future version will add explicit flags to BeginMenu() to request processing shortcuts)
static void ShowExampleMenuFile()
{
    py::MenuItem("(demo menu)", NULL, false, false);
    if (py::MenuItem("New")) {}
    if (py::MenuItem("Open", "Ctrl+O")) {}
    if (py::BeginMenu("Open Recent"))
    {
        py::MenuItem("fish_hat.c");
        py::MenuItem("fish_hat.inl");
        py::MenuItem("fish_hat.h");
        if (py::BeginMenu("More.."))
        {
            py::MenuItem("Hello");
            py::MenuItem("Sailor");
            if (py::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                py::EndMenu();
            }
            py::EndMenu();
        }
        py::EndMenu();
    }
    if (py::MenuItem("Save", "Ctrl+S")) {}
    if (py::MenuItem("Save As..")) {}

    py::Separator();
    if (py::BeginMenu("Options"))
    {
        static bool enabled = true;
        py::MenuItem("Enabled", "", &enabled);
        py::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            py::Text("Scrolling Text %d", i);
        py::EndChild();
        static float f = 0.5f;
        static int n = 0;
        py::SliderFloat("Value", &f, 0.0f, 1.0f);
        py::InputFloat("Input", &f, 0.1f);
        py::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        py::EndMenu();
    }

    if (py::BeginMenu("Colors"))
    {
        float sz = py::GetTextLineHeight();
        for (int i = 0; i < pyCol_COUNT; i++)
        {
            const char* name = py::GetStyleColorName((pyCol)i);
            ImVec2 p = py::GetCursorScreenPos();
            py::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), py::GetColorU32((pyCol)i));
            py::Dummy(ImVec2(sz, sz));
            py::SameLine();
            py::MenuItem(name);
        }
        py::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (py::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        py::Checkbox("SomeOption", &b);
        py::EndMenu();
    }

    if (py::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (py::MenuItem("Checked", NULL, true)) {}
    if (py::MenuItem("Quit", "Alt+F4")) {}
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    pyTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear py!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void    Draw(const char* title, bool* p_open)
    {
        py::SetNextWindowSize(ImVec2(520, 600), pyCond_FirstUseEver);
        if (!py::Begin(title, p_open))
        {
            py::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (py::BeginPopupContextItem())
        {
            if (py::MenuItem("Close Console"))
                *p_open = false;
            py::EndPopup();
        }

        py::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        py::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (py::SmallButton("Add Debug Text"))  { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
        py::SameLine();
        if (py::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
        py::SameLine();
        if (py::SmallButton("Clear"))           { ClearLog(); }
        py::SameLine();
        bool copy_to_clipboard = py::SmallButton("Copy");
        //static float t = 0.0f; if (py::GetTime() - t > 0.02f) { t = py::GetTime(); AddLog("Spam %f", t); }

        py::Separator();

        // Options menu
        if (py::BeginPopup("Options"))
        {
            py::Checkbox("Auto-scroll", &AutoScroll);
            py::EndPopup();
        }

        // Options, Filter
        if (py::Button("Options"))
            py::OpenPopup("Options");
        py::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        py::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = py::GetStyle().ItemSpacing.y + py::GetFrameHeightWithSpacing();
        py::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, pyWindowFlags_HorizontalScrollbar);
        if (py::BeginPopupContextWindow())
        {
            if (py::Selectable("Clear")) ClearLog();
            py::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use py::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      pyListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        py::PushStyleVar(pyStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            py::LogToClipboard();
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!Filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]"))          { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                py::PushStyleColor(pyCol_Text, color);
            py::TextUnformatted(item);
            if (has_color)
                py::PopStyleColor();
        }
        if (copy_to_clipboard)
            py::LogFinish();

        if (ScrollToBottom || (AutoScroll && py::GetScrollY() >= py::GetScrollMaxY()))
            py::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        py::PopStyleVar();
        py::EndChild();
        py::Separator();

        // Command-line
        bool reclaim_focus = false;
        pyInputTextFlags input_text_flags = pyInputTextFlags_EnterReturnsTrue | pyInputTextFlags_CallbackCompletion | pyInputTextFlags_CallbackHistory;
        if (py::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        py::SetItemDefaultFocus();
        if (reclaim_focus)
            py::SetKeyboardFocusHere(-1); // Auto focus previous widget

        py::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(pyInputTextCallbackData* data)
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(pyInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case pyInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for (int i = 0; i < Commands.Size; i++)
                    if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                        candidates.push_back(Commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can..
                    // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c == 0 || c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    AddLog("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        AddLog("- %s\n", candidates[i]);
                }

                break;
            }
        case pyInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == pyKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == pyKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    pyTextBuffer     Buf;
    pyTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!py::Begin(title, p_open))
        {
            py::End();
            return;
        }

        // Options menu
        if (py::BeginPopup("Options"))
        {
            py::Checkbox("Auto-scroll", &AutoScroll);
            py::EndPopup();
        }

        // Main window
        if (py::Button("Options"))
            py::OpenPopup("Options");
        py::SameLine();
        bool clear = py::Button("Clear");
        py::SameLine();
        bool copy = py::Button("Copy");
        py::SameLine();
        Filter.Draw("Filter", -100.0f);

        py::Separator();
        py::BeginChild("scrolling", ImVec2(0, 0), false, pyWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            py::LogToClipboard();

        py::PushStyleVar(pyStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    py::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   py::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using pyListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            pyListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    py::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        py::PopStyleVar();

        if (AutoScroll && py::GetScrollY() >= py::GetScrollMaxY())
            py::SetScrollHereY(1.0f);

        py::EndChild();
        py::End();
    }
};

// Demonstrate creating a simple log window with basic filtering.
static void ShowExampleAppLog(bool* p_open)
{
    static ExampleAppLog log;

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    py::SetNextWindowSize(ImVec2(500, 400), pyCond_FirstUseEver);
    py::Begin("Example: Log", p_open);
    if (py::SmallButton("[Debug] Add 5 entries"))
    {
        static int counter = 0;
        const char* categories[3] = { "info", "warn", "error" };
        const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
        for (int n = 0; n < 5; n++)
        {
            const char* category = categories[counter % IM_ARRAYSIZE(categories)];
            const char* word = words[counter % IM_ARRAYSIZE(words)];
            log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
                py::GetFrameCount(), category, py::GetTime(), word);
            counter++;
        }
    }
    py::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Example: Log", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
//-----------------------------------------------------------------------------

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open)
{
    py::SetNextWindowSize(ImVec2(500, 440), pyCond_FirstUseEver);
    if (py::Begin("Example: Simple layout", p_open, pyWindowFlags_MenuBar))
    {
        if (py::BeginMenuBar())
        {
            if (py::BeginMenu("File"))
            {
                if (py::MenuItem("Close")) *p_open = false;
                py::EndMenu();
            }
            py::EndMenuBar();
        }

        // Left
        static int selected = 0;
        {
            py::BeginChild("left pane", ImVec2(150, 0), true);
            for (int i = 0; i < 100; i++)
            {
                // FIXME: Good candidate to use pySelectableFlags_SelectOnNav
                char label[128];
                sprintf(label, "MyObject %d", i);
                if (py::Selectable(label, selected == i))
                    selected = i;
            }
            py::EndChild();
        }
        py::SameLine();

        // Right
        {
            py::BeginGroup();
            py::BeginChild("item view", ImVec2(0, -py::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            py::Text("MyObject: %d", selected);
            py::Separator();
            if (py::BeginTabBar("##Tabs", pyTabBarFlags_None))
            {
                if (py::BeginTabItem("Description"))
                {
                    py::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                    py::EndTabItem();
                }
                if (py::BeginTabItem("Details"))
                {
                    py::Text("ID: 0123456789");
                    py::EndTabItem();
                }
                py::EndTabBar();
            }
            py::EndChild();
            if (py::Button("Revert")) {}
            py::SameLine();
            if (py::Button("Save")) {}
            py::EndGroup();
        }
    }
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
//-----------------------------------------------------------------------------

static void ShowPlaceholderObject(const char* prefix, int uid)
{
    // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
    py::PushID(uid);

    // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
    py::TableNextRow();
    py::TableSetColumnIndex(0);
    py::AlignTextToFramePadding();
    bool node_open = py::TreeNode("Object", "%s_%u", prefix, uid);
    py::TableSetColumnIndex(1);
    py::Text("my sailor is rich");

    if (node_open)
    {
        static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
        for (int i = 0; i < 8; i++)
        {
            py::PushID(i); // Use field index as identifier.
            if (i < 2)
            {
                ShowPlaceholderObject("Child", 424242);
            }
            else
            {
                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                py::TableNextRow();
                py::TableSetColumnIndex(0);
                py::AlignTextToFramePadding();
                pyTreeNodeFlags flags = pyTreeNodeFlags_Leaf | pyTreeNodeFlags_NoTreePushOnOpen | pyTreeNodeFlags_Bullet;
                py::TreeNodeEx("Field", flags, "Field_%d", i);

                py::TableSetColumnIndex(1);
                py::SetNextItemWidth(-FLT_MIN);
                if (i >= 5)
                    py::InputFloat("##value", &placeholder_members[i], 1.0f);
                else
                    py::DragFloat("##value", &placeholder_members[i], 0.01f);
                py::NextColumn();
            }
            py::PopID();
        }
        py::TreePop();
    }
    py::PopID();
}

// Demonstrate create a simple property editor.
static void ShowExampleAppPropertyEditor(bool* p_open)
{
    py::SetNextWindowSize(ImVec2(430, 450), pyCond_FirstUseEver);
    if (!py::Begin("Example: Property editor", p_open))
    {
        py::End();
        return;
    }

    HelpMarker(
        "This example shows how you may implement a property editor using two columns.\n"
        "All objects/fields data are dummies here.\n"
        "Remember that in many simple cases, you can use py::SameLine(xxx) to position\n"
        "your cursor horizontally instead of using the Columns() API.");

    py::PushStyleVar(pyStyleVar_FramePadding, ImVec2(2, 2));
    if (py::BeginTable("split", 2, pyTableFlags_BordersOuter | pyTableFlags_Resizable))
    {
        // Iterate placeholder objects (all the same data)
        for (int obj_i = 0; obj_i < 4; obj_i++)
        {
            ShowPlaceholderObject("Object", obj_i);
            //py::Separator();
        }
        py::EndTable();
    }
    py::PopStyleVar();
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
//-----------------------------------------------------------------------------

// Demonstrate/test rendering huge amount of text, and the incidence of clipping.
static void ShowExampleAppLongText(bool* p_open)
{
    py::SetNextWindowSize(ImVec2(520, 600), pyCond_FirstUseEver);
    if (!py::Begin("Example: Long text display", p_open))
    {
        py::End();
        return;
    }

    static int test_type = 0;
    static pyTextBuffer log;
    static int lines = 0;
    py::Text("Printing unusually long amount of text.");
    py::Combo("Test type", &test_type,
        "Single call to TextUnformatted()\0"
        "Multiple calls to Text(), clipped\0"
        "Multiple calls to Text(), not clipped (slow)\0");
    py::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
    if (py::Button("Clear")) { log.clear(); lines = 0; }
    py::SameLine();
    if (py::Button("Add 1000 lines"))
    {
        for (int i = 0; i < 1000; i++)
            log.appendf("%i The quick brown fox jumps over the lazy dog\n", lines + i);
        lines += 1000;
    }
    py::BeginChild("Log");
    switch (test_type)
    {
    case 0:
        // Single call to TextUnformatted() with a big buffer
        py::TextUnformatted(log.begin(), log.end());
        break;
    case 1:
        {
            // Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the pyListClipper helper.
            py::PushStyleVar(pyStyleVar_ItemSpacing, ImVec2(0, 0));
            pyListClipper clipper;
            clipper.Begin(lines);
            while (clipper.Step())
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                    py::Text("%i The quick brown fox jumps over the lazy dog", i);
            py::PopStyleVar();
            break;
        }
    case 2:
        // Multiple calls to Text(), not clipped (slow)
        py::PushStyleVar(pyStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int i = 0; i < lines; i++)
            py::Text("%i The quick brown fox jumps over the lazy dog", i);
        py::PopStyleVar();
        break;
    }
    py::EndChild();
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window which gets auto-resized according to its content.
static void ShowExampleAppAutoResize(bool* p_open)
{
    if (!py::Begin("Example: Auto-resizing window", p_open, pyWindowFlags_AlwaysAutoResize))
    {
        py::End();
        return;
    }

    static int lines = 10;
    py::TextUnformatted(
        "Window will resize every-frame to the size of its content.\n"
        "Note that you probably don't want to query the window size to\n"
        "output your content because that would create a feedback loop.");
    py::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        py::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window with custom resize constraints.
static void ShowExampleAppConstrainedResize(bool* p_open)
{
    struct CustomConstraints
    {
        // Helper functions to demonstrate programmatic constraints
        static void Square(pySizeCallbackData* data) { data->DesiredSize.x = data->DesiredSize.y = IM_MAX(data->DesiredSize.x, data->DesiredSize.y); }
        static void Step(pySizeCallbackData* data)   { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = ImVec2((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
    };

    const char* test_desc[] =
    {
        "Resize vertical only",
        "Resize horizontal only",
        "Width > 100, Height > 100",
        "Width 400-500",
        "Height 400-500",
        "Custom: Always Square",
        "Custom: Fixed Steps (100)",
    };

    static bool auto_resize = false;
    static int type = 0;
    static int display_lines = 10;
    if (type == 0) py::SetNextWindowSizeConstraints(ImVec2(-1, 0),    ImVec2(-1, FLT_MAX));      // Vertical only
    if (type == 1) py::SetNextWindowSizeConstraints(ImVec2(0, -1),    ImVec2(FLT_MAX, -1));      // Horizontal only
    if (type == 2) py::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100
    if (type == 3) py::SetNextWindowSizeConstraints(ImVec2(400, -1),  ImVec2(500, -1));          // Width 400-500
    if (type == 4) py::SetNextWindowSizeConstraints(ImVec2(-1, 400),  ImVec2(-1, 500));          // Height 400-500
    if (type == 5) py::SetNextWindowSizeConstraints(ImVec2(0, 0),     ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Square);                     // Always Square
    if (type == 6) py::SetNextWindowSizeConstraints(ImVec2(0, 0),     ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Step, (void*)(intptr_t)100); // Fixed Step

    pyWindowFlags flags = auto_resize ? pyWindowFlags_AlwaysAutoResize : 0;
    if (py::Begin("Example: Constrained Resize", p_open, flags))
    {
        if (py::Button("200x200")) { py::SetWindowSize(ImVec2(200, 200)); } py::SameLine();
        if (py::Button("500x500")) { py::SetWindowSize(ImVec2(500, 500)); } py::SameLine();
        if (py::Button("800x200")) { py::SetWindowSize(ImVec2(800, 200)); }
        py::SetNextItemWidth(200);
        py::Combo("Constraint", &type, test_desc, IM_ARRAYSIZE(test_desc));
        py::SetNextItemWidth(200);
        py::DragInt("Lines", &display_lines, 0.2f, 1, 100);
        py::Checkbox("Auto-resize", &auto_resize);
        for (int i = 0; i < display_lines; i++)
            py::Text("%*sHello, sailor! Making this line long enough for the example.", i * 4, "");
    }
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration
// + a context-menu to choose which corner of the screen to use.
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    static int corner = 0;
    pyIO& io = py::GetIO();
    pyWindowFlags window_flags = pyWindowFlags_NoDecoration | pyWindowFlags_AlwaysAutoResize | pyWindowFlags_NoSavedSettings | pyWindowFlags_NoFocusOnAppearing | pyWindowFlags_NoNav;
    if (corner != -1)
    {
        const float PAD = 10.0f;
        const pyViewport* viewport = py::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        py::SetNextWindowPos(window_pos, pyCond_Always, window_pos_pivot);
        window_flags |= pyWindowFlags_NoMove;
    }
    py::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (py::Begin("Example: Simple overlay", p_open, window_flags))
    {
        py::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
        py::Separator();
        if (py::IsMousePosValid())
            py::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            py::Text("Mouse Position: <invalid>");
        if (py::BeginPopupContextWindow())
        {
            if (py::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (py::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
            if (py::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
            if (py::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
            if (py::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && py::MenuItem("Close")) *p_open = false;
            py::EndPopup();
        }
    }
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Fullscreen window / ShowExampleAppFullscreen()
//-----------------------------------------------------------------------------

// Demonstrate creating a window covering the entire screen/viewport
static void ShowExampleAppFullscreen(bool* p_open)
{
    static bool use_work_area = true;
    static pyWindowFlags flags = pyWindowFlags_NoDecoration | pyWindowFlags_NoMove | pyWindowFlags_NoResize | pyWindowFlags_NoSavedSettings;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one of the other.
    const pyViewport* viewport = py::GetMainViewport();
    py::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    py::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

    if (py::Begin("Example: Fullscreen window", p_open, flags))
    {
        py::Checkbox("Use work area instead of main area", &use_work_area);
        py::SameLine();
        HelpMarker("Main Area = entire viewport,\nWork Area = entire viewport minus sections used by the main menu bars, task bars etc.\n\nEnable the main-menu bar in Examples menu to see the difference.");

        py::CheckboxFlags("pyWindowFlags_NoBackground", &flags, pyWindowFlags_NoBackground);
        py::CheckboxFlags("pyWindowFlags_NoDecoration", &flags, pyWindowFlags_NoDecoration);
        py::Indent();
        py::CheckboxFlags("pyWindowFlags_NoTitleBar", &flags, pyWindowFlags_NoTitleBar);
        py::CheckboxFlags("pyWindowFlags_NoCollapse", &flags, pyWindowFlags_NoCollapse);
        py::CheckboxFlags("pyWindowFlags_NoScrollbar", &flags, pyWindowFlags_NoScrollbar);
        py::Unindent();

        if (p_open && py::Button("Close this window"))
            *p_open = false;
    }
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
//-----------------------------------------------------------------------------

// Demonstrate using "##" and "###" in identifiers to manipulate ID generation.
// This apply to all regular items as well.
// Read FAQ section "How can I have multiple widgets with the same label?" for details.
static void ShowExampleAppWindowTitles(bool*)
{
    const pyViewport* viewport = py::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;

    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display/ID.

    // Using "##" to display same title but have unique identifier.
    py::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 100), pyCond_FirstUseEver);
    py::Begin("Same title as another window##1");
    py::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    py::End();

    py::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 200), pyCond_FirstUseEver);
    py::Begin("Same title as another window##2");
    py::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    py::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(py::GetTime() / 0.25f) & 3], py::GetFrameCount());
    py::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 300), pyCond_FirstUseEver);
    py::Begin(buf);
    py::Text("This window has a changing title.");
    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void ShowExampleAppCustomRendering(bool* p_open)
{
    if (!py::Begin("Example: Custom rendering", p_open))
    {
        py::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
    // overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
    // types and ImVec2/ImVec4. Dear py defines overloaded operators but they are internal to py.cpp and not
    // exposed outside (to avoid messing with your types) In this example we are not using the maths operators!

    if (py::BeginTabBar("##TabBar"))
    {
        if (py::BeginTabItem("Primitives"))
        {
            py::PushItemWidth(-py::GetFontSize() * 15);
            ImDrawList* draw_list = py::GetWindowDrawList();

            // Draw gradients
            // (note that those are currently exacerbating our sRGB/Linear issues)
            // Calling py::GetColorU32() multiplies the given colors by the current Style Alpha, but you may pass the IM_COL32() directly as well..
            py::Text("Gradients");
            ImVec2 gradient_size = ImVec2(py::CalcItemWidth(), py::GetFrameHeight());
            {
                ImVec2 p0 = py::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = py::GetColorU32(IM_COL32(0, 0, 0, 255));
                ImU32 col_b = py::GetColorU32(IM_COL32(255, 255, 255, 255));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                py::InvisibleButton("##gradient1", gradient_size);
            }
            {
                ImVec2 p0 = py::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = py::GetColorU32(IM_COL32(0, 255, 0, 255));
                ImU32 col_b = py::GetColorU32(IM_COL32(255, 0, 0, 255));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                py::InvisibleButton("##gradient2", gradient_size);
            }

            // Draw a bunch of primitives
            py::Text("All primitives");
            static float sz = 36.0f;
            static float thickness = 3.0f;
            static int ngon_sides = 6;
            static bool circle_segments_override = false;
            static int circle_segments_override_v = 12;
            static bool curve_segments_override = false;
            static int curve_segments_override_v = 8;
            static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            py::DragFloat("Size", &sz, 0.2f, 2.0f, 100.0f, "%.0f");
            py::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
            py::SliderInt("N-gon sides", &ngon_sides, 3, 12);
            py::Checkbox("##circlesegmentoverride", &circle_segments_override);
            py::SameLine(0.0f, py::GetStyle().ItemInnerSpacing.x);
            circle_segments_override |= py::SliderInt("Circle segments override", &circle_segments_override_v, 3, 40);
            py::Checkbox("##curvessegmentoverride", &curve_segments_override);
            py::SameLine(0.0f, py::GetStyle().ItemInnerSpacing.x);
            curve_segments_override |= py::SliderInt("Curves segments override", &curve_segments_override_v, 3, 40);
            py::ColorEdit4("Color", &colf.x);

            const ImVec2 p = py::GetCursorScreenPos();
            const ImU32 col = ImColor(colf);
            const float spacing = 10.0f;
            const ImDrawFlags corners_tl_br = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomRight;
            const float rounding = sz / 5.0f;
            const int circle_segments = circle_segments_override ? circle_segments_override_v : 0;
            const int curve_segments = curve_segments_override ? curve_segments_override_v : 0;
            float x = p.x + 4.0f;
            float y = p.y + 4.0f;
            for (int n = 0; n < 2; n++)
            {
                // First line uses a thickness of 1.0f, second line uses the configurable thickness
                float th = (n == 0) ? 1.0f : thickness;
                draw_list->AddNgon(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, ngon_sides, th);                 x += sz + spacing;  // N-gon
                draw_list->AddCircle(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, circle_segments, th);          x += sz + spacing;  // Circle
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 0.0f, ImDrawFlags_None, th);          x += sz + spacing;  // Square
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, rounding, ImDrawFlags_None, th);      x += sz + spacing;  // Square with all rounded corners
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, rounding, corners_tl_br, th);         x += sz + spacing;  // Square with two rounded corners
                draw_list->AddTriangle(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col, th);x += sz + spacing;  // Triangle
                //draw_list->AddTriangle(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col, th);x+= sz*0.4f + spacing; // Thin triangle
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col, th);                                       x += sz + spacing;  // Horizontal line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col, th);                                       x += spacing;       // Vertical line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col, th);                                  x += sz + spacing;  // Diagonal line

                // Quadratic Bezier Curve (3 control points)
                ImVec2 cp3[3] = { ImVec2(x, y + sz * 0.6f), ImVec2(x + sz * 0.5f, y - sz * 0.4f), ImVec2(x + sz, y + sz) };
                draw_list->AddBezierQuadratic(cp3[0], cp3[1], cp3[2], col, th, curve_segments); x += sz + spacing;

                // Cubic Bezier Curve (4 control points)
                ImVec2 cp4[4] = { ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz) };
                draw_list->AddBezierCubic(cp4[0], cp4[1], cp4[2], cp4[3], col, th, curve_segments);

                x = p.x + 4;
                y += sz + spacing;
            }
            draw_list->AddNgonFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col, ngon_sides);               x += sz + spacing;  // N-gon
            draw_list->AddCircleFilled(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, circle_segments);            x += sz + spacing;  // Circle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col);                                    x += sz + spacing;  // Square
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f);                             x += sz + spacing;  // Square with all rounded corners
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br);              x += sz + spacing;  // Square with two rounded corners
            draw_list->AddTriangleFilled(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col);  x += sz + spacing;  // Triangle
            //draw_list->AddTriangleFilled(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col); x += sz*0.4f + spacing; // Thin triangle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col);                             x += sz + spacing;  // Horizontal line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col);                             x += spacing * 2.0f;// Vertical line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col);                                      x += sz;            // Pixel (faster than AddLine)
            draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));

            py::Dummy(ImVec2((sz + spacing) * 10.2f, (sz + spacing) * 3.0f));
            py::PopItemWidth();
            py::EndTabItem();
        }

        if (py::BeginTabItem("Canvas"))
        {
            static ImVector<ImVec2> points;
            static ImVec2 scrolling(0.0f, 0.0f);
            static bool opt_enable_grid = true;
            static bool opt_enable_context_menu = true;
            static bool adding_line = false;

            py::Checkbox("Enable grid", &opt_enable_grid);
            py::Checkbox("Enable context menu", &opt_enable_context_menu);
            py::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

            // Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
            // Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
            // To use a child window instead we could use, e.g:
            //      py::PushStyleVar(pyStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
            //      py::PushStyleColor(pyCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
            //      py::BeginChild("canvas", ImVec2(0.0f, 0.0f), true, pyWindowFlags_NoMove);
            //      py::PopStyleColor();
            //      py::PopStyleVar();
            //      [...]
            //      py::EndChild();

            // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
            ImVec2 canvas_p0 = py::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = py::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

            // Draw border and background color
            pyIO& io = py::GetIO();
            ImDrawList* draw_list = py::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

            // This will catch our interactions
            py::InvisibleButton("canvas", canvas_sz, pyButtonFlags_MouseButtonLeft | pyButtonFlags_MouseButtonRight);
            const bool is_hovered = py::IsItemHovered(); // Hovered
            const bool is_active = py::IsItemActive();   // Held
            const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
            const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

            // Add first and second point
            if (is_hovered && !adding_line && py::IsMouseClicked(pyMouseButton_Left))
            {
                points.push_back(mouse_pos_in_canvas);
                points.push_back(mouse_pos_in_canvas);
                adding_line = true;
            }
            if (adding_line)
            {
                points.back() = mouse_pos_in_canvas;
                if (!py::IsMouseDown(pyMouseButton_Left))
                    adding_line = false;
            }

            // Pan (we use a zero mouse threshold when there's no context menu)
            // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
            const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
            if (is_active && py::IsMouseDragging(pyMouseButton_Right, mouse_threshold_for_pan))
            {
                scrolling.x += io.MouseDelta.x;
                scrolling.y += io.MouseDelta.y;
            }

            // Context menu (under default mouse threshold)
            ImVec2 drag_delta = py::GetMouseDragDelta(pyMouseButton_Right);
            if (opt_enable_context_menu && py::IsMouseReleased(pyMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
                py::OpenPopupOnItemClick("context");
            if (py::BeginPopup("context"))
            {
                if (adding_line)
                    points.resize(points.size() - 2);
                adding_line = false;
                if (py::MenuItem("Remove one", NULL, false, points.Size > 0)) { points.resize(points.size() - 2); }
                if (py::MenuItem("Remove all", NULL, false, points.Size > 0)) { points.clear(); }
                py::EndPopup();
            }

            // Draw grid + all lines in the canvas
            draw_list->PushClipRect(canvas_p0, canvas_p1, true);
            if (opt_enable_grid)
            {
                const float GRID_STEP = 64.0f;
                for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
                for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
            }
            for (int n = 0; n < points.Size; n += 2)
                draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
            draw_list->PopClipRect();

            py::EndTabItem();
        }

        if (py::BeginTabItem("BG/FG draw lists"))
        {
            static bool draw_bg = true;
            static bool draw_fg = true;
            py::Checkbox("Draw in Background draw list", &draw_bg);
            py::SameLine(); HelpMarker("The Background draw list will be rendered below every Dear py windows.");
            py::Checkbox("Draw in Foreground draw list", &draw_fg);
            py::SameLine(); HelpMarker("The Foreground draw list will be rendered over every Dear py windows.");
            ImVec2 window_pos = py::GetWindowPos();
            ImVec2 window_size = py::GetWindowSize();
            ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
            if (draw_bg)
                py::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 0, 10 + 4);
            if (draw_fg)
                py::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
            py::EndTabItem();
        }

        py::EndTabBar();
    }

    py::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Simplified structure to mimic a Document model
struct MyDocument
{
    const char* Name;       // Document title
    bool        Open;       // Set when open (we keep an array of all available documents to simplify demo code!)
    bool        OpenPrev;   // Copy of Open from last update.
    bool        Dirty;      // Set when the document has been modified
    bool        WantClose;  // Set when the document
    ImVec4      Color;      // An arbitrary variable associated to the document

    MyDocument(const char* name, bool open = true, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
    {
        Name = name;
        Open = OpenPrev = open;
        Dirty = false;
        WantClose = false;
        Color = color;
    }
    void DoOpen()       { Open = true; }
    void DoQueueClose() { WantClose = true; }
    void DoForceClose() { Open = false; Dirty = false; }
    void DoSave()       { Dirty = false; }

    // Display placeholder contents for the Document
    static void DisplayContents(MyDocument* doc)
    {
        py::PushID(doc);
        py::Text("Document \"%s\"", doc->Name);
        py::PushStyleColor(pyCol_Text, doc->Color);
        py::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        py::PopStyleColor();
        if (py::Button("Modify", ImVec2(100, 0)))
            doc->Dirty = true;
        py::SameLine();
        if (py::Button("Save", ImVec2(100, 0)))
            doc->DoSave();
        py::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
        py::PopID();
    }

    // Display context menu for the Document
    static void DisplayContextMenu(MyDocument* doc)
    {
        if (!py::BeginPopupContextItem())
            return;

        char buf[256];
        sprintf(buf, "Save %s", doc->Name);
        if (py::MenuItem(buf, "CTRL+S", false, doc->Open))
            doc->DoSave();
        if (py::MenuItem("Close", "CTRL+W", false, doc->Open))
            doc->DoQueueClose();
        py::EndPopup();
    }
};

struct ExampleAppDocuments
{
    ImVector<MyDocument> Documents;

    ExampleAppDocuments()
    {
        Documents.push_back(MyDocument("Lettuce",             true,  ImVec4(0.4f, 0.8f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("Eggplant",            true,  ImVec4(0.8f, 0.5f, 1.0f, 1.0f)));
        Documents.push_back(MyDocument("Carrot",              true,  ImVec4(1.0f, 0.8f, 0.5f, 1.0f)));
        Documents.push_back(MyDocument("Tomato",              false, ImVec4(1.0f, 0.3f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("A Rather Long Title", false));
        Documents.push_back(MyDocument("Some Document",       false));
    }
};

// [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
// If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo,
// as opposed to clicking on the regular tab closing button) and stops being submitted, it will take a frame for
// the tab bar to notice its absence. During this frame there will be a gap in the tab bar, and if the tab that has
// disappeared was the selected one, the tab bar will report no selected tab during the frame. This will effectively
// give the impression of a flicker for one frame.
// We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
// Note that this completely optional, and only affect tab bars with the pyTabBarFlags_Reorderable flag.
static void NotifyOfDocumentsClosedElsewhere(ExampleAppDocuments& app)
{
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (!doc->Open && doc->OpenPrev)
            py::SetTabItemClosed(doc->Name);
        doc->OpenPrev = doc->Open;
    }
}

void ShowExampleAppDocuments(bool* p_open)
{
    static ExampleAppDocuments app;

    // Options
    static bool opt_reorderable = true;
    static pyTabBarFlags opt_fitting_flags = pyTabBarFlags_FittingPolicyDefault_;

    bool window_contents_visible = py::Begin("Example: Documents", p_open, pyWindowFlags_MenuBar);
    if (!window_contents_visible)
    {
        py::End();
        return;
    }

    // Menu
    if (py::BeginMenuBar())
    {
        if (py::BeginMenu("File"))
        {
            int open_count = 0;
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                open_count += app.Documents[doc_n].Open ? 1 : 0;

            if (py::BeginMenu("Open", open_count < app.Documents.Size))
            {
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                {
                    MyDocument* doc = &app.Documents[doc_n];
                    if (!doc->Open)
                        if (py::MenuItem(doc->Name))
                            doc->DoOpen();
                }
                py::EndMenu();
            }
            if (py::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                    app.Documents[doc_n].DoQueueClose();
            if (py::MenuItem("Exit", "Alt+F4")) {}
            py::EndMenu();
        }
        py::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (doc_n > 0)
            py::SameLine();
        py::PushID(doc);
        if (py::Checkbox(doc->Name, &doc->Open))
            if (!doc->Open)
                doc->DoForceClose();
        py::PopID();
    }

    py::Separator();

    // About the pyWindowFlags_UnsavedDocument / pyTabItemFlags_UnsavedDocument flags.
    // They have multiple effects:
    // - Display a dot next to the title.
    // - Tab is selected when clicking the X close button.
    // - Closure is not assumed (will wait for user to stop submitting the tab).
    //   Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    //   We need to assume closure by default otherwise waiting for "lack of submission" on the next frame would leave an empty
    //   hole for one-frame, both in the tab-bar and in tab-contents when closing a tab/window.
    //   The rarely used SetTabItemClosed() function is a way to notify of programmatic closure to avoid the one-frame hole.

    // Submit Tab Bar and Tabs
    {
        pyTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? pyTabBarFlags_Reorderable : 0);
        if (py::BeginTabBar("##tabs", tab_bar_flags))
        {
            if (opt_reorderable)
                NotifyOfDocumentsClosedElsewhere(app);

            // [DEBUG] Stress tests
            //if ((py::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (py::GetIO().KeyCtrl) py::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
            {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open)
                    continue;

                pyTabItemFlags tab_flags = (doc->Dirty ? pyTabItemFlags_UnsavedDocument : 0);
                bool visible = py::BeginTabItem(doc->Name, &doc->Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty)
                {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible)
                {
                    MyDocument::DisplayContents(doc);
                    py::EndTabItem();
                }
            }

            py::EndTabBar();
        }
    }

    // Update closing queue
    static ImVector<MyDocument*> close_queue;
    if (close_queue.empty())
    {
        // Close queue is locked once we started a popup
        for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
        {
            MyDocument* doc = &app.Documents[doc_n];
            if (doc->WantClose)
            {
                doc->WantClose = false;
                close_queue.push_back(doc);
            }
        }
    }

    // Display closing confirmation UI
    if (!close_queue.empty())
    {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < close_queue.Size; n++)
            if (close_queue[n]->Dirty)
                close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0)
        {
            // Close documents when all are unsaved
            for (int n = 0; n < close_queue.Size; n++)
                close_queue[n]->DoForceClose();
            close_queue.clear();
        }
        else
        {
            if (!py::IsPopupOpen("Save?"))
                py::OpenPopup("Save?");
            if (py::BeginPopupModal("Save?", NULL, pyWindowFlags_AlwaysAutoResize))
            {
                py::Text("Save change to the following items?");
                float item_height = py::GetTextLineHeightWithSpacing();
                if (py::BeginChildFrame(py::GetID("frame"), ImVec2(-FLT_MIN, 6.25f * item_height)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        if (close_queue[n]->Dirty)
                            py::Text("%s", close_queue[n]->Name);
                    py::EndChildFrame();
                }

                ImVec2 button_size(py::GetFontSize() * 7.0f, 0.0f);
                if (py::Button("Yes", button_size))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                    {
                        if (close_queue[n]->Dirty)
                            close_queue[n]->DoSave();
                        close_queue[n]->DoForceClose();
                    }
                    close_queue.clear();
                    py::CloseCurrentPopup();
                }
                py::SameLine();
                if (py::Button("No", button_size))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        close_queue[n]->DoForceClose();
                    close_queue.clear();
                    py::CloseCurrentPopup();
                }
                py::SameLine();
                if (py::Button("Cancel", button_size))
                {
                    close_queue.clear();
                    py::CloseCurrentPopup();
                }
                py::EndPopup();
            }
        }
    }

    py::End();
}

// End of Demo code
#else

void py::ShowAboutWindow(bool*) {}
void py::ShowDemoWindow(bool*) {}
void py::ShowUserGuide() {}
void py::ShowStyleEditor(pyStyle*) {}

#endif

#endif // #ifndef py_DISABLE
