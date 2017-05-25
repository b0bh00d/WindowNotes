# WindowsNotes
A C++ Qt-based desktop application that attaches notes to windows

## Summary
WindowsNotes provides "sticky notes" that are tied to a window context.  It is
a Qt-based application designed for the Microsoft Windows desktop.

Notes are associated with a window by the context of its title, and can be
associated by full match, or it can match the context via a regular expression.

Notes can be "stacked" along various edges of a window.

Note contents can be automatically placed on the system Clipboard by single-
clicking on a note, or you can drag-and-drop a note into an edit-field target
to paste its contents.

## Qt versions
This project was developed with, and tested under, Qt versions 5.6.2.

### Current status
The system is quite functional, and has been in "production" use on my desktop
since 2013.

This system is designed for use with the Microsoft Windows desktop, and has been
tested under Windows 7.  Since I probably won't ever be upgrading to a version
of Windows higher than seven, I cannot vouch for its functionality under newer
versions, but expect it would function properly.

The window manager-specific code is isolated into its own module, so converting
it to other desktops (e.g., Linux) should not be an insurmountable task.

## Usage
WindowsNotes is a task-tray application.  Run the main application, and then
use the global key combination (default is "Windows Key + A") to open the
add-note window.

![Add a Note](https://cloud.githubusercontent.com/assets/4536448/26467150/9b910488-414e-11e7-9dc2-706e5a136601.png)

A note is completed when you click outside of it (i.e., it loses focus).  If
the note is empty when it loses focus, no note will be created.

![Complete Note](https://cloud.githubusercontent.com/assets/4536448/26467153/9ea7fa50-414e-11e7-8ed8-a250a2fe0c33.png)

When a note is attached to a window context, you can hover over it to see its
contents.

![Note Tooltip](https://cloud.githubusercontent.com/assets/4536448/26467156/a0b4e862-414e-11e7-9359-0aad04f6411a.png)

Right-click on the note to get a context menu that allows you to edit, copy
(duplicate), and delete it.  In addition, you can clear all notes associated
with the context.

![Context Menu](https://cloud.githubusercontent.com/assets/4536448/26467159/a2a5dece-414e-11e7-89d3-54dfeb7f0267.png)

Right-clicking on the task tray icon will allow you to edit the application's
settings, including positioning notes and setting the default global key trigger
combination.

![Application Context Menu](https://cloud.githubusercontent.com/assets/4536448/26467162/a4b0faaa-414e-11e7-847a-bfef3e5974e9.png)

## Documentation
You're reading it now.
