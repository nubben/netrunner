# Netrunner

is an implementation of a new web browser engine written in C++ utilizing OpenGL.

Status: You can give it a url and it'll download, parse, and render the text from the page

## Odilitime's vision
A modern web browser is a huge project. I'd like to see this broken into several composable pieces.
Network engine separate from the parsing engine, separate from the compositing engine separate from the renderer.
This way it could be used a document viewer or an editor as well.
A browser development kit for developers.
I'd like to develop text-based structures for communication between each piece.

**USE AT YOUR OWN RISK!**

## Compilation Requirements
- gcc or llvm
- opengl
- [glew (2.0 works)](https://github.com/nigels-com/glew/releases)
- [glfw (3.2.1 works)](https://github.com/glfw/glfw/releases)
- [freetype2 (2.8 works)](https://www.freetype.org/download.html)
- [harfbuzz (1.4.6 works)](https://github.com/behdad/harfbuzz/releases)
- [md5sum (for shader compilation)](https://spit.mixtape.moe/view/765a1dc1)

### Package shortucts
#### Debian (and derivates)
`sudo apt-get install libfreetype6-dev libharfbuzz-dev libglew-dev libglfw3-dev`

#### Arch (and derivates)
`sudo pacman -Suy glew glfw freetype2 harfbuzz`

#### Void
`sudo xbps-install -S glew glfw harfbuzz-devel`

#### Gentoo
`sudo emerge freetype harfbuzz glew glfw`

## Binaries
### Linux
[2017-07-31 binary package](https://my.mixtape.moe/imatcb.tar.gz)

GyroNinja.net is temporarily offline. Working with registrar to bring it back online.
[nightly binary only (no font files)](https://gyroninja.net:1615/job/NetRunner/lastSuccessfulBuild/artifact/netrunner)

### OSX
[2017-07-31 binary package](https://my.mixtape.moe/ywjanx.zip)

### Windows
[2017-07-31 binary package](https://my.mixtape.moe/hkpcyu.zip)

## Milestones
- Browse 4chan /g/ board

## FAQ
- Why don't you use x engine?
Because it didn't do what we wanted it to do.
- You realize this project is a massive undertaking
Yes.
- Why are you doing this?
odilitime: To watch people get triggered on /g/
- This code is awful?
Yes, we're planning to throw it away. It's just a proof-of-concept at the moment.
- How can I help?
Join IRC, compile the code, figure out something you want to improve
- What about security?
I don't think one on here could ever say they know what their doing without being laughed off the board. Security is a huge challenge to any project. We're going to do the best with the resources we have.

## Who

- odilitime - Lead Developer
  - OSX Dev, focused on clean up
  - Repo: https://gitgud.io/odilitime/netrunner/

- Nubben
  - contributed local file code
  - repo: https://github.com/nubben/netrunner

- Tomleb
  - contributed keyboard code
  - repo: https://github.com/tomleb/netrunner

- RetroTech - Web / Sys Admin
  - Created original website
  - Helps manage archive of logos and threads
  - set up etherpad for collaboration

- gyroninja - Jenkins
  - Was original Lead Developer (left to work on a servo-based project), Linux Dev, Did primary OOP planning and structure
  - Repo: https://git.teknik.io/gyroninja/netrunner/

- Gillman - archivist
  - works with RetroTech on archival

- Cygnus - Web
  - Created nicer website

- ETI
  - did an early links repo
  - contributed README clean up

- anons that are making the /g/ threads
  - Your threads keep us motivated
  - Drop by irc/discord and say hi to us
  - Repo? https://github.com/5yph3r/Netrunner/

We coordinate on [irc.rizon.net](https://www.rizon.net/chat) #/g/netrunner or [Odili's Discord](https://discord.gg/ffWabPn).

## Documentation

### Overview
- networking - downloads requested files
- html - HTML parser
- graphics - has the various interface interface
  - opengl - our opengl Renderer
  - text - our text rasterizer
  - component - our renderer classes
  - element - our DOM tree classes

### Class Types
- Nodes: DOM tree objects
- Elements: Individual tag types
- Components: (gameobjects) renderer entities
