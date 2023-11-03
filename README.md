# rito library - overview
![linux build](https://github.com/bartekprtc/rito/actions/workflows/linux_gcc_13.yml/badge.svg)
![windows build](https://github.com/bartekprtc/rito/actions/workflows/windows_cl.yml/badge.svg)

Rito is a simple, crossplatform (Windows and Linux) C++20 library interfacing with Riot services (i.e. Riot REST API and Riot Client). It is currently in development - API changes might occur at some point. Rito uses Poco library in the backend. It offers a few classes which handle communication with Riot REST API, as well as with League Client (LCU). For League Client, Rito supports both REST API and Websocket communication.

# building
Rito uses cmake as a build system. It can be built with gcc-13 on Linux and with msvc on Windows. It depends on Poco library (with SSL). For details, see "Building Rito" wiki page.

# usage
Rito can be either installed on the system, or integrated within cmake project. See "Using Rito" wiki page for more information on how to use the library and "Docs" wiki page for some brief overview of classes.

# development/contributing
Rito was meant to be a simple League stat collection application, which turned into a library. Even though it's very simple, I decided to make it into proper github project (with CI, coverage, tests and other fancy stuff) for learning purposes. I am happy to accept suggestions, reviews and pull requests. If you wish to contribute (or you're just interested in running tests/static analysis locally), see "Contributing" wiki page.
