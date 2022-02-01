  // This file is purposely left blank. LibUI uses the C++ STL on Windows, and
 // static librares require that all it's dependencies get linked to the exe.
// It is used to force CMake to use G++, thus implicitly linking with the STL DLL.