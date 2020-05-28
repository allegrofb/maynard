#include <iostream>
#include <algorithm> // std::max
#include "MaynardCurtain.h"

MaynardCurtain::MaynardCurtain()
{
    /* window properties */
    set_title("maynard");
    set_decorated(FALSE);
    set_size_request (8192, 8192);
    realize(); 

    #ifndef TEST_APP
    Wayland::getInstance()->curtain.window = this->window;
    Wayland::getInstance()->curtain.surface = this->surface;
    Wayland::getInstance()->curtain.pixbuf = this->pixbuf;
    show_all_children();
    #endif
}

MaynardCurtain::~MaynardCurtain()
{
    /*
  // These calls to Gtk::Widget::unparent() are necessary if MaynardCurtain is
  // deleted before its children. But if you use a version of gtkmm where bug
  // https://bugzilla.gnome.org/show_bug.cgi?id=605728
  // has not been fixed (gtkmm 3.7.10 or earlier) and the children are deleted
  // before the container, these calls can make the program crash.
  // That's because on_remove() is not called, when the children are deleted.
  if (m_child_one)
    m_child_one->unparent();

  if (m_child_two)
    m_child_two->unparent();
*/
}
