
# foo_quicksearch

[foo_quicksearch](https://github.com/stuerp/foo_quicksearch/releases) is a [foobar2000](https://www.foobar2000.org/) component that creates playlists based on quick search patterns.

## Features

* Supports foobar2000 2.0 (32-bit and 64-bit version)
* Supports dark mode.

## Requirements

* Microsoft Windows 10 or later
* [foobar2000](https://www.foobar2000.org/download) v2.0 or later (32 or 64-bit). ![foobar2000](https://www.foobar2000.org/button-small.png)


## Getting started

* Double-click `foo_quicksearch.fbk2-component`.

or

* Import `foo_quicksearch.fbk2-component` into foobar2000 using "File / Preferences / Components / Install...".

## Developing

The code builds out-of-the box with Visual Studio 2022.

### Requirements

To build the code you need:

* [Microsoft Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/downloads/) or later.
* [foobar2000 SDK](https://www.foobar2000.org/SDK) 2023-05-10.
* [Windows Template Library (WTL)](https://github.com/Win32-WTL/WTL) 10.0.10320.

To create the deployment package:

* [PowerShell 7.2](https://github.com/PowerShell/PowerShell) or later.

### Setup

Create the following directory structure:

    3rdParty
        WTL10_10320
    bin
        x86
    foo_quicksearch
    out
    sdk

* `3rdParty/WTL10_10320` contains WTL 10.0.10320.
* `bin` contains a portable version of foobar2000 64-bit for debugging purposes.
* `bin/x86` contains a portable version of foobar2000 32-bit for debugging purposes.
* `foo_quicksearch` contains the [Git](https://github.com/stuerp/foo_quicksearch) repository.
* `out` receives a deployable version of the component.
* `sdk` contains the foobar2000 SDK.

### Building

Open `foo_quicksearch.sln` with Visual Studio and build the solution.

### Packaging

To create the component first build the x86 configuration and next the x64 configuration.

## Contributing

If you'd like to contribute, please fork the repository and use a feature
branch. Pull requests are warmly welcome.

## Change Log

v1.0.0, 2023-05-14, *"Scratchin' the itch"*

* Initial release.

## Acknowledgements / Credits

* Peter Pawlowski for the [foobar2000](https://www.foobar2000.org/) audio player. ![foobar2000](https://www.foobar2000.org/button-small.png)
* [marc2k3](https://github.com/marc2k3) for [foo_qsearch](https://github.com/marc2003/foo_qsearch).

## Reference Material

## Links

* Home page: https://github.com/stuerp/foo_quicksearch
* Repository: https://github.com/stuerp/foo_quicksearch.git
* Issue tracker: https://github.com/stuerp/foo_quicksearch/issues

## License

![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)
