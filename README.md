# rito

![linux build](https://github.com/bartekprtc/rito/actions/workflows/linux_gcc_13.yml/badge.svg)
![windows build](https://github.com/bartekprtc/rito/actions/workflows/windows_cl.yml/badge.svg)

## Overview

Rito is a simple, crossplatform (Windows and Linux) C++20 library interfacing with Riot services (i.e. Riot REST API and League of Legends client). It is currently in development - API changes might occur at some point. Rito uses [POCO](https://github.com/pocoproject/poco) library in the backend. It offers a few classes which handle communication with Riot REST API, as well as with LoL client (LCU). For League client, Rito supports both REST API and Websocket communication.

## Features

You want to:
- Use Riot REST API easily? Use `Riot_rest_handler` class
- Get League client (LCU) params (port & remoting auth token) easily? Use `Lcu_process_handler` class
- Use League client REST API? Use `Lcu_rest_handler` class
- Connect to League client using Websocket? Use `Lcu_websocket_handler` class
- Use LCU events (with WAMP protocol over Websocket)? Use `Lcu_wamp_handler` class

Details in [Docs](https://github.com/bartekprtc/rito/wiki/Docs) wiki page.

## Build

### Quickstart

You'll need `gcc` 13, `cmake`, `ninja` and `poco` library. Install dependencies:

Ubuntu/Debian:

``` bash
sudo apt install gcc cmake ninja-build libpoco-dev
```

Clone repo and build Rito:

``` bash
git clone git@github.com:bartekprtc/rito.git
cd rito
cmake -S . -B build
cmake --build build
```

Install library:

``` bash
sudo cmake --build build -t install
```

### Detailed build guide

Rito uses cmake as a build system. It can be built with gcc-13 on Linux and with msvc on Windows. It depends on Poco library (with SSL). For details, see [Building Rito](https://github.com/bartekprtc/rito/wiki/Building-Rito) wiki page.

## Usage
Rito can be either installed on the system, or integrated within cmake project. See [Using Rito](https://github.com/bartekprtc/rito/wiki/Using-Rito) wiki page for more information on how to use the library and [Docs](https://github.com/bartekprtc/rito/wiki/Docs) wiki page for some brief overview of classes.

## Development/Contributing
Rito was meant to be a simple League stat collection application, which turned into a library. Even though it's very simple, I decided to make it into proper github project (with CI, coverage, tests and other fancy stuff) for learning purposes. I am happy to accept suggestions, reviews and pull requests. If you wish to contribute (or you're just interested in running tests/static analysis locally), see [Contributing](https://github.com/bartekprtc/rito/wiki/Contributing) wiki page.
