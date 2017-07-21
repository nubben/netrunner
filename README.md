# Netrunner

is an implementation of a new web browser written in C++ utilizing OpenGL. This is a brand new engine.

Status: You can give it a url and it'll download, parser, and render the text from the page

## Odilitime's vision
A modern web browser is a huge project. I'd like to see this broken into several composeable pieces.
Network engine separate from the parsing engine, separate from the compositing engine separate from the renderer.
This way it could be used a document viewer or an editor as well.
A browser development kit for developers.
I'd like to develop text-based structures for communication between each piece.

USE AT YOUR OWN RISK!

## Compliation Requirements
- gcc or llvm
- opengl
- glew (2.0 works) https://github.com/nigels-com/glew/releases
- glfw (3.2.1 works) https://github.com/glfw/glfw/releases
- freetype2 (2.8 works) https://www.freetype.org/download.html
- harfbuzz (1.4.6 works) https://github.com/behdad/harfbuzz/releases
- md5sum (for shader compilation) https://spit.mixtape.moe/view/765a1dc1

### Package shortucts
to install requirements

#### debian/ubuntu
`sudo apt-get install libfreetype6-dev libharfbuzz-dev libglew-dev libglfw3-dev`

#### arch
`sudo pacman -Suy glew glfw freetype2 harfbuzz`

## Binaries
### Linux
https://gyroninja.net:1615/job/NetRunner/lastSuccessfulBuild/artifact/netrunner
### OSX
20170721 binary https://my.mixtape.moe/lbsgid.zip

## Milestones
- Browse 4chan /g/ board

## FAQ
- Why don't you use x engine?
Because it didn't do what we wanted it.
- You realize this project is a massive undertaking
Yes
- Why are you doing this?
odilitime: To watch people get triggered on /g/
- This code is awful?
Yes, we're planning to throw it away. It's just a proof-of-concept at the moment.
- How can I help?
Join IRC, compile the code, figure out something you want to improve
- What about security
I don't think one on here could ever say they know what their doing without being laughed off the board. Security is a huge challenge to any project. We're going to do the best with the resources we have.

## Who
- gyroninja - Lead developer
Linux Dev, Doing primary OOP planning and structure
Repo: https://git.teknik.io/gyroninja/netrunner/

- odilitime - Developer
OSX Dev, focused on clean up
Repo: https://gitgud.io/odilitime/netrunner/
Discord: https://discord.gg/ffWabPn

- RetroTech - Web / Sys Admin
Created website

- Someone else that's making the /g/ threads?
Repo? https://github.com/5yph3r/Netrunner/
Repo? https://git.teknik.io/eti/netrunner/

We coordinate on irc.rizon.net #/g/netrunner or Odili's discord: https://discord.gg/ffWabPn

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
- Nodes DOM tree objects
- Elements Individual tag types
- Components (gameobjects) renderer entities

