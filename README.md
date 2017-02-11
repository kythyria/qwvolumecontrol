This is a volume mixer for Windows. It exists because sndvol.exe has the rather
annoying property of scaling the appearance of session volumes so that 1.0 maps
to the current endpoint volume, despite the API to all other appearances not in
fact working this way.

The UI takes a few cues from pavucontrol, though it can't take all the cues, as
Windows' sound system doesn't work quite the same way.

To build, you need Qt 5.7ish, the Windows SDK, and MSVC 2015. It uses the smart
pointer library that MSVC supplies as comsupw.lib, so I doubt MinGW will work.
