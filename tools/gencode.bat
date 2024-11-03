
if not exist cpp mkdir cpp
if not exist java mkdir java
protoc.exe --cpp_out=.\cpp --java_out=.\java message.proto

xcopy /E /Y .\java  ..\src\NetAnalysis\src\
xcopy /E /Y .\cpp  ..\src\NetCap\jni\