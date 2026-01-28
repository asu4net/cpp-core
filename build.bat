call vs2022.bat

call MSBuild.exe ".project-files/cpp-core.sln" ^
/p:Configuration=Debug ^
/p:Platform=x64