@echo off
for %%f IN (*) DO (
	if "%%~xf"==".vert" (
		echo glslc.exe "%%~nxf" -o "%%~nf.spv"
		"%~dp0..\..\..\extern\CompWolf.Graphics.Core\vulkan\Tools\glslc.exe" "%%~dpnxf" -o "%%~dpnf.spv"
		echo.
	)
	if "%%~xf"==".frag" (
		echo glslc.exe "%%~nxf" -o "%%~nf.spv"
		"%~dp0..\..\..\extern\CompWolf.Graphics.Core\vulkan\Tools\glslc.exe" "%%~dpnxf" -o "%%~dpnf.spv"
		echo.
	)
)
