import os
import oldgtk3/[gobject, gtk, gdk, gio, glib, gtksource, gdk_pixbuf, pango]

proc procMain() =
    setAllowedBackends("wayland")

    var nargv =  newSeq[string](paramCount())
    var x:cint = 0
    while x < paramCount():
       nargv[x] = paramStr(x+1)  # first is program name
       x += 1
    var argv: cStringArray = nargv.allocCStringArray()

    init(x, argv)

    argv.deallocCStringArray()


procMain()