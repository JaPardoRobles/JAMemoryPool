-- A solution contains projects, and defines the available configurations
solution "JAPR"
location "build"
targetdir "build/bin"
configurations { "Debug", "Release" }
      configuration "Debug"
         defines { "DEBUG","MYENGINE_PLATFORM_WINDOWS" }
         flags { "Symbols" }

      configuration "Release"
         defines { "NDEBUG","MYENGINE_PLATFORM_WINDOWS" }
         flags { "Optimize" }

   -- A project defines one build target
   project "JAMemoryPool"
      kind "ConsoleApp"
      language "C++"
      files { "src/public/*","src/private/*", "src/*.c*" ,"src/*.cc","src/*.h","deps/SDL/include/*.h"}
    includedirs {"include","include/MyProject/JAPR","deps","src","src/public","src/private"}
