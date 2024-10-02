# Custom micropython with callstack extraction to variable.

## 1. Usage

Prebuilt image: images/firmware.uf2

### 1. Import and set up:
```
try:
    from excprinter import get_callstack, get_current_callstack
except Exception as e:
    get_callstack = lambda e : []
    get_current_callstack = lambda: []

def get_exception_info(self, e):
    return "exception: " + str(type(e)) + "(" + str(e) + "), exception callstack: " + str(get_callstack(e)) + ", catch callstack: " + str(get_current_callstack())
```

### 2. Get the current call stack up to main.py.

```
print(get_current_callstack())
```

example output:
```
['logger.py:28 __init__', 'web_server.py:32 __init__', 'main.py:12 <module>']
```

### 3. Get the exception call stack up to try/catch block


```
        try:
            ntptime.settime()
        except Exception as e:
            print(get_exception_info(e))
```

example output:
```
exception: <class 'NameError'>(name 'fail' isn't defined), exception callstack: ['web_server.py:40 __init__', 'derived_logic.py:11 __init__'], catch callstack: ['logger.py:32 get_exception_info', 'logger.py:56 send', 'logger.py:62 send_status', 'web_server.py:45 __init__', '<stdin>:12 <module>']
```

## 2. How it works

We add "MICROPY_PY_SYS_SETTRACE" to rpi pico w configuration. This will increase memory usage.
This is done via "build_via_docker.sh"

Then we use a separate module "userlibs/excprinter/" to provide functions to extract both current callstack and exception callback.
If current callstack is not needed then the SETTRACE is not useful either.

## 3. How to build:

Init environment:
```
git submodule update --init --recursive
```

Build via docker
```
./build_via_docker.sh
```
And result will be a "firmware.uf2" for raspberry pi pico W in the main folder.

That image is the official micropython with "MICROPY_PY_SYS_SETTRACE" enabled and "userlibs/excprinter/" module built in.

## 4. Sample users

This is used by: https://github.com/AdrianCX/pico_irrigation/blob/main/src/base/logger.py


