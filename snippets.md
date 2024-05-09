---
layout: default
---

[leland.zip](https://leland.zip/)

## Snippets

![artistic](./images/snippets.png)


- [Tracing a program](https://leland.zip/snippets.html#trace-program)

## Trace Program

gcc has a flag ```-finstrument-functions``` which instruments the resultant binary:
- with a call upon function entry ```__cyg_profile_func_enter```
- with another call upon function exit ```__cyg_profile_func_exit```


The below images show the same local function without and with ```-finstrument-functions``` in arm64:

### Local arm64 function without -finstrument-functions
![pre-instrument](./images/pre-function-instrument.png)

### Local arm64 function with -finstrument-functions
![post-instrument](./images/post-function-instrument.png)


### Hook
```
#include  <stdio.h>

void __cyg_profile_func_enter(void *funcAddr, void *callSite) {
  printf("ENTER: %p, from %p\n", funcAddr, callSite);
}

void __cyg_profile_func_exit(void *funcAddr, void *callSite) {
  printf("EXIT: %p, from %p\n", funcAddr, callSite);
}
```

Can be compiled: ```gcc -shared -g hook.c -o hook.so```

### Compile program of interest

```gcc -fno-pie -no-pie -g -finstrument-functions main.c -lm -o main```

### LD_PRELOAD
``` LD_PRELOAD=/home/leland/function_instrumentation/hook.so ./main``` 
