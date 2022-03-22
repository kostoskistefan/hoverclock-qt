<p align="center">
    <img src="images/logo.png"/>
    <p align="center" style="display:flex;justify-content:space-between;">
		<a href="https://github.com/kostoskistefan/hoverclock/releases">
            <img src="https://img.shields.io/github/release/kostoskistefan/hoverclock?style=for-the-badge"/>
        </a>
        <a href="https://aur.archlinux.org/packages/hoverclock-bin/">
            <img alt="AUR version" src="https://shields.api-test.nl/aur/version/hoverclock-bin?style=for-the-badge">
        </a>
	</p>
</p>

# Hoverclock 

Hoverclock is a simple, yet customizable floating clock for Linux created with QT. 

![Clock Screenshot](images/clock.jpg)

## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Quick note](#quick-note)
* [Installation](#installation)
  * [Arch based distro users](#arch-based-distro-users)
  * [Other distros](#other-distros)
* [Optional dependencies](#optional-dependencies)
  * [X Server](#x-server)
  * [Font](#font)
* [Configuration](#configuration)
* [Building and deploying](#building-and-deploying)
* [Known issues](#known-issues)
* [Future work](#future-work)
* [Getting help](#getting-help)
* [Getting involved](#getting-involved)
  * [Using the issue tracker](#using-the-issue-tracker)
  * [Changing the code-base](#changing-the-code-base)
    * [Code conventions](#code-conventions)

## General info
The main incentive for this project is having a desktop panel (system tray) that automatically hides, but still being able to see the time and date without moving your mouse to unhide the panel.

## Technologies
 * C++
 * QT 5.15 with QT Creator
 * [CQtDeployer](https://github.com/QuasarApp/CQtDeployer)

## Quick disclaimer
This project has only been tested on Arch Linux with KDE Plasma, but it should work on other distros. 

## Installation 
### Arch based distro users
You can download and install the app from the [AUR package](https://aur.archlinux.org/packages/hoverclock-bin/).

### Other distros
Head on over to the [releases section](https://github.com/kostoskistefan/hoverclock/releases) of this repository and download the latest release. 
 * If you are running on a Debian based distribution, use the .deb package. 
 * If you want to use a GUI installer which will configure everything for you, use the .run file. **(Recommended for non-debian distributions)**
 * If you wish to manually install everything, extract the .zip file to a location of your choosing. Then you can run the hoverclock.sh file to launch the clock. (You'll also need to copy the icons and desktop file manually, as well as creating a symlink of the hoverclock.sh in /usr/bin/hoverclock)

## Optional dependencies

### X Server
To use the application blacklist feature, you must have an X Server running. If you are using Wayland, make sure you have XWayland installed.

To support different systems, the blacklist feature is disabled by default and you need to enable it in the Visibility tab of the settings dialog to use it.

### Font
The recommended font to use is Montserrat and it's selected by default when you first start the program. 

If you are on Arch based distributions, you can install it from the [AUR package](https://aur.archlinux.org/packages/montserrat-ttf/).

Of course, if you wish to use a different font, you can. Change it in the settings dialog in the Appearance tab.

## Configuration
As stated previously, Hoverclock is designed to be customizable. 

Left clicking the system tray icon will toggle the visibility of the clock, while right clicking brings up a menu from which you can access the application settings.
If you do not have a system tray, the application settings can be accessed by launching hoverclock from a terminal and passing the `-c/--configure` flag. There's also a `-h` flag which will bring up a help menu.

![Time and Date settings screenshot](images/settings_timedate.png)
![Appearance settings screenshot](images/settings_appearance.png)
![Visibility settings screenshot](images/settings_misc.png)

## Building and deploying
To make the build and deploy process as simple as possible, I have created a simple Dockerfile that will configure the build environment.

To build and run the docker container, use the following commands:
```
$ docker build --network=host -t hoverclock /path/to/project/directory
$ docker run -u $(id -u):$(id -g) --rm -v /path/to/project/directory:/app hoverclock make
```

When it finishes building, you should have a directory called DistributionKit containing a .deb, .zip and a .run files. 

## Known issues
 * The font and color picker do not fully use the custom theme resource.

## Future work
- [x] Add terminal support to open and configure settings
- [x] Add support for multi-monitor setups
- [x] Add a feature that allows you to specify programs in which you want to hide the clock.
- [ ] Add Light and Dark mode icon support

## Getting help
If you have questions, concerns, bug reports, etc, please file an issue in this repository's [Issue Tracker](https://github.com/kostoskistefan/hoverclock/issues).

## Getting involved
There are two primary ways to help:

* Using the issue tracker, and
* Changing the code-base.

### Using the issue tracker
Use the [issue tracker](https://github.com/kostoskistefan/hoverclock/issues) to suggest feature requests, report bugs, and ask questions. Any feedback is greatly appreciated.

### Changing the code-base
If you want to directly affect the code, you should fork this repository, make changes on your own fork, then submit a pull request. 

#### Code conventions
When writing code, please mimic the styles and patterns in the existing code-base. That means, using camelCase variable names, self-explanatory variable names, all new files should use the underscore notation, use a #define guard for header files, etc.

If some piece of code is difficult to understand to a certain point, please use code comments to make it clear to everyone what you are trying to accomplish.
