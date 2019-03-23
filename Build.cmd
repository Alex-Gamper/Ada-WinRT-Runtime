gprbuild -f -p -P winrt_runtime.gpr
link /lib /machine:x64 /nodefaultlib /out:.\lib\libother.a .\lib\*.o
gprinstall -f -p -P winrt_runtime.gpr
