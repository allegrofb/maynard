# maynard

This is a desktop shell client for Weston based on GTK. It was based
on weston-gtk-shell, a project by Tiago Vignatti.

License information can be found in the LICENSE file in the same
directory as this README.

To build maynard use:

     ./configure --prefix=<installdir>
     make
     make install

Before running maynard you need a ~/.config/weston.ini like

    [core]
    modules=<installdir>/lib/weston/shell-helper.so
   
    [shell]
    client=<installdir>/libexec/maynard
    focus-animation=dim-layer
    animation=zoom

You can then run maynard with

    <installdir>/bin/maynard

	
	
	
# log
Ubuntu 18.04

sudo apt-get install autoconf
sudo apt-get install libtool
sudo apt-get install intltool
sudo apt-get install libgtk-3-dev
sudo apt-get install libgnome-desktop-3-dev
sudo apt-get install libgnome-menu-3-dev
sudo apt-get install libasound2-dev
sudo apt-get install weston
sudo apt-get install libweston-3-dev


autoreconf --install
./configure
make
make install
weston -c ~/.config/weston.ini    

	
	