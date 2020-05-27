#include <gtkmm.h>
#include "MaynardLauncher.h"

class ExampleWindow : public Gtk::Window
{
public:
  ExampleWindow();
  virtual ~ExampleWindow();

protected:
  //Signal handlers:
  void on_button_quit();

  //Child widgets:
  MaynardLauncher m_widget;
};


ExampleWindow::ExampleWindow()
{
  set_title("Custom Container example");
  set_border_width(6);
  set_default_size(400, 200);

  add(m_widget);

  show_all_children();
}

ExampleWindow::~ExampleWindow()
{
}

void ExampleWindow::on_button_quit()
{
  hide();
}

#include <gtkmm/application.h>

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  ExampleWindow window;

  //Shows the window and returns when it is closed.
  return app->run(window);
}
