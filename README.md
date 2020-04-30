# simplemsptorch~
 basic Max/MSP external with LibTorch support

 2 flavours: classic C (as used in the SDK), and C++ (with some ugly workarounds but does make your life easier)

 tested on Windows 10 x64 only, no CUDA

 (OSX version coming soon)

 ## build (Win)

 clone repo into max-sdk-8.0.3\source\

 download LibTorch into (Project_Dir)/libtorch

 [ optional: change your project name. This means find-replacing all occurences of simplemsptorch, in every file ]

 copy the libtorch *.dll files to c:\Program Files\Cycling '74\Max 8\resources\support\ (or the /support directory in your package)

 open the VS2017 project, set the Release / x64 configuration

 build
