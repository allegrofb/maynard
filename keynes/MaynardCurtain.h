#ifndef MAYNARD_CURTAIN_H
#define MAYNARD_CURTAIN_H

#include <gtkmm.h>

#ifdef TEST_APP
class MaynardCurtain : public Gtk::Window
#else
#include "WlWindow.h"
class MaynardCurtain : public WlWindow
#endif
{
public:
    MaynardCurtain();
    virtual ~MaynardCurtain();

};


#endif //MAYNARD_CURTAIN_H
