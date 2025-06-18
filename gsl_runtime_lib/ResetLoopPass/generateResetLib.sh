clang `llvm-config --cxxflags` -Wl,-znodelete -fno-rtti -fPIC -shared ResetLoopPass.cpp -o libResetLoop.so `llvm-config --ldflags`
