cl /nologo /TC /W4 /WX /Zi testbed.c /Fo:testbed_modern.obj /Fd:testbed_modern.pdb /Fe:testbed_modern.exe /link /DEBUG
cl /nologo /TC /W4 /WX /Zi testbed_compat.c /Fo:testbed_compat.obj /Fd:testbed_compat.pdb /Fe:testbed_compat.exe /link /DEBUG
