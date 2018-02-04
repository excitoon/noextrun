Tool for running executables without extensions on Windows.

### Dependencies

```
scoop install gcc
scoop install make
scoop install cmake
```

### Build

```
git clone git@github.com:excitoon/noextrun
mkdir noextrun-bin
cd noextrun-bin
cmake ../noextrun -G "Unix Makefiles"
make
```

### Install

```
scoop bucket add user https://github.com/excitoon/scoop-user.git
scoop install user/noextrun
```
