
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "WlWindow.h"

class BackgroundWindow : public WlWindow
{
public:
  BackgroundWindow();
  virtual ~BackgroundWindow();

protected:
  //Signal handlers:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

};

#endif //BACKGROUND_H