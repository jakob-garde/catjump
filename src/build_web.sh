em++ -o catjump.html main.cpp -Wall -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -O3 -I. -I../src -Iexternal -L. -L../src -L../src -sTOTAL_MEMORY=134217728 -sFORCE_FILESYSTEM=1 -sEXPORTED_RUNTIME_METHODS=ccall -sMINIFY_HTML=0 -sUSE_GLFW=3 -sASYNCIFY --shell-file shell.html libraylib.web.a -DPLATFORM_WEB --preload-file resources@resources

