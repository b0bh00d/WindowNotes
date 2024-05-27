<p align="center">
    <img width="20%" alt="WindowNotes" src="https://private-user-images.githubusercontent.com/4536448/334136299-7a6ac094-178f-4082-8aa3-bce9b073d40d.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MTY4MjA0NzUsIm5iZiI6MTcxNjgyMDE3NSwicGF0aCI6Ii80NTM2NDQ4LzMzNDEzNjI5OS03YTZhYzA5NC0xNzhmLTQwODItOGFhMy1iY2U5YjA3M2Q0MGQucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MDUyNyUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDA1MjdUMTQyOTM1WiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9NzhmODMxM2E1ZWNkMjIyODc3NDExY2M4YTg5MzE0MDc2ZDM5NzQ4MDA3MTlhMjJmYjM0M2UxYmUyNTkxOWFlMiZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QmYWN0b3JfaWQ9MCZrZXlfaWQ9MCZyZXBvX2lkPTAifQ.a6hgHztLfC1J-jcUgxpMhdHkGhkzcrHeey9s6_19gXU">
</p>

# WindowsNotes
A C++ Qt-based desktop application that attaches notes to windows

## Summary
WindowsNotes provides "sticky notes" that are tied to a window context.  It is
a Qt-based application designed for the Microsoft Windows desktop, and Linux
using the GNOME Window Manager.

Notes are associated with a window by the context of its title, and can be
associated by full match, or it can match the context via a regular expression.

Notes can be "stacked" along various edges of a window.

Note contents can be automatically placed on the system Clipboard by single-
clicking on a note, or you can drag-and-drop a note into an edit-field target
to paste its contents.

## Qt versions
This project was initially developed with, and tested under, Qt versions 5.6.2,
but compiles and functions up through 5.15.3.

### Current status
The system is quite functional, and has been in "production" use on my Windows desktop
since 2013.

This system is designed for use with the Microsoft Windows desktop, and has been
tested under Windows 10.  Having migrated away from Microsoft Windows to Linux at
the end of 2023, I cannot vouch for its functionality under Windows 11.

The window manager-specific code is largely isolated into its own module, so
converting it should not be an insurmountable task.

#### Linux
I recently updatged WindowsNotes to work under Linux using the GNOME Window Manager.
It functions by using a GNOME Shell extension to provide it with window-manager
information (active window, geometry, title, etc.) on a DBus channel, which would
otherwise come directly from the Win32 API under Windows.  It also relies on an "Add"
button on Linux, instead of using a global key hook, so every window on the desktop has
an "Add" decorator, even if it doesn't have notes assigned.

It is quite functional right now, but the main application is having to poll
the Shell extension at 100ms intervals in order to feel responsive (which I'm not happy
about).  I will continue to fiddle with this arrangement, however, to see if I can
figure out how to "push" the information from the extension, making WindowsNotes
more event-driven, instead of high-speed polling for it from the application.  There's
not a lot of hand-holding material out there on writing robust GNOME Shell extensions,
and even at v45, GNOME's API still seems very unstable.

## Usage
WindowsNotes is a task-tray application.  On Windows, Run the main application,
and then use the global key combination (default is "Windows Key + A") to open the
add-note window.  On Linux, use the "Add" button attached to each desktop window to
activate the add-note Window.

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
settings, including positioning notes and, on Microsoft Windows, setting the
default global key trigger combination.

![Application Context Menu](https://cloud.githubusercontent.com/assets/4536448/26467162/a4b0faaa-414e-11e7-847a-bfef3e5974e9.png)

## Documentation
You're reading it now.
