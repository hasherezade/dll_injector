# DLL Injector

[![Build status](https://ci.appveyor.com/api/projects/status/62n5oral9idn4jg5?svg=true)](https://ci.appveyor.com/project/hasherezade/dll-injector)

A simple commandline DLL injector.

Supported actions:
+ Load DLL
+ Unload DLL
+ Check if the DLL is loaded

Allows to inject into:
+ Existing process
+ Newly created process

Arguments:
```
Required:
/dll <wstring>
	 : DLL path
/target <wstring>
	 : Exe to be run with the DLL injected, or target PID (where to inject)

Optional:
/action <*action_id>
	 : Action to be executed
*action_id:
	0 (C) - check if the DLL is loaded
	1 (L) - load the DLL [DEFAULT]
	2 (U) - unload the DLL
```

Clone:
-
Use recursive clone to get the repo together with all the submodules:
```console
git clone --recursive https://github.com/hasherezade/dll_injector.git
```
