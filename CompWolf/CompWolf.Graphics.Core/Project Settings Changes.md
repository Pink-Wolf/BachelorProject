Base project: Static Library (C++)

Configuration Properties:
- C/C++
- - General
- - - Additional Include Directories
- - - - Added $(SolutionDir)..\extern\$(ProjectName)\glfw\include\GLFW
- - - - Added $(SolutionDir)..\extern\$(ProjectName)\vulkan\Include
- - - - Added $(ProjectDir)include\
- - - - Added $(ProjectDir)header\
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
