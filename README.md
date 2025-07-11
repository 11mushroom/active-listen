# Description
------
listen TCP port and when the client connects to it, it connects to server(ip and port) and relays traffic between them.

------
# How to compile/install
------
1. clone repo if you haven't done it yet
```
git clone https://github.com/11mushroom/active-listen.git
```

2. go to the `<directory of cloned repo>/src`
```
cd active-listen/src
```

3. compile it
```
make
```
if you want to install
```
make install
```
you can specify directory where you want to install it by specifying INSTDIR variable:
```
make install INSTDIR=/path/where/to/install
```
or specify PREFIX
```
make install PREFIX=/path/for/prefix
```

4. to delete/uninstall it
```
make uninstall
```
if it's installed in custom directory
```
make uninstall INSTDIR=/path/to/directory/where/you/installed/it
```
of if custom prefix
```
make uninstall PREFIX=/path/to/prefix/using/which/you/installed/it
```

