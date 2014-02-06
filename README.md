qt5-ora-plugin
==============

Qt5 plug-in for reading OpenRaster images in Qt's image framework

### Features

- Reading OpenRaster images
- If mergedimage.png is not provided, the image is rendered using Qt's own renderer

### Known issues

- Text element is not implemented
- The non-separable compositing operations are not supported by QPainter and therefore not implemented.
- The whole file is loaded into memory, instead of being streamed to libarchive. This potentially requires more memory during loading.

### Building

**Dependencies**

- Qt5
- qmake
- libarchive
- pugixml (included)

**Building**

1. qmake
2. make

**Installation**

After successful compilation you should have a ora.* file (extension dependent on OS). You can install this globally or application specific. If the file is placed in Qt's plug-in folder under *imageformats*, it will be available to all Qt5 applications. To only use it with a specific application, create a folder called *imageformats* in the executables folder and place the plug-in there.