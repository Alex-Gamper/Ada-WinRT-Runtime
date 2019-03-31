gprbuild -p -P winrt_runtime.gpr
link /lib /machine:x64 /nodefaultlib /out:.\lib\libother.a .\lib\*.o
link /lib /machine:x64 /nodefaultlib /out:.\libgnarl\libgnarl.a .\libgnarl\*.o
gprinstall -f -p -P winrt_runtime.gpr
