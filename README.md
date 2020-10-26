# QQuickBgfx

This repository implements [BGFX](https://github.com/bkaradzic/bgfx) renderering abstraction library to work under QML/QQuick for Qt6.

A QQuickBgfxItem can be instantiated in QML with a given [bgfx::viewId](https://bkaradzic.github.io/bgfx/bgfx.html#views).
The content of the framebuffer associated to the viewId will be visualized inside the QML BgfxItem.

---

Why having Qt/QML with an external renderer?

Despite the new technology trends and approaches, Qt/QML is still largely used. Over the last years, by using this framework, I noticed that the 3d rendering engine made in qt was under documented, difficult to use, and with poor performances due to the high level abstraction. And still, you might have to write some platform specific rendering code, which makes it even harder to use for crossplatform 3d application.

Probably Qt6 will improve all that, but I want to offer [BGFX](https://github.com/bkaradzic/bgfx) as an easy alternative that works great out of the box.
The BGFX [examples](https://github.com/bkaradzic/bgfx/tree/master/examples) show how to use the library and the different features. If you are familiar with the general GPU pipeline, it'll be fairly easy to start pushing some pixels :D

---

Metal and D3D11 backend are currently implemented.


TO DO:

- [ ] OpenGL backend
- [ ] Vulkan backend

DONE:

- [x] Metal backend
- [x] D3D11 backend

Clone:

The usual suspects
  
  * $ `git clone git@github.com:dashandslash/BgfxQQuickItem.git`
  * $ `cd BgfxQQuickItem`
  * $ `git submodule update --init --recursive`
  
Build:

Add Qt to the `PATH`

* $ `export PATH=$PATH:/path/to/Qt/6.0.0/clang_64/lib/cmake`

From the cloned folder

* $ `mkdir .build && cd .build`

Mac:

* $ `cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Debug` (Release for release mode)
Win:

* $ `cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Debug` (Release for release mode)


I suggest to use XCode to build and debug on Mac (the XCode GPU capture feature is very handy for debugging the Metal rendering pipeline).
