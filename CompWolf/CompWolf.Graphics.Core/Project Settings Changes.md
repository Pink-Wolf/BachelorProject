Base project: Static Library (C++)

Added project reference to:
- CompWolf.Core

Configuration Properties:
- C/C++
- - General
- - - Additional Include Directories
- - - - Added $(ProjectDir)header\
- - - - Added $(ProjectDir)include\
- - - - Added $(SolutionDir)CompWolf.Core/include/
- - - - Added $(SolutionDir)..\extern\$(ProjectName)\vulkan\Include
- - - - Added $(SolutionDir)..\extern\$(ProjectName)\glfw\include\GLFW
- - Language
- - - C++ Language Standard
- - - - Set to ISO C++20 Standard
- Librarian
- - General
- - - Additional Dependencies
- - - - Added vulkan/Lib/vulkan-1.lib
- - - - Added glfw/lib-vc2022/glfw3.lib
- - - Additional Library Directories
- - - - Added $(SolutionDir)..\extern\$(ProjectName)
