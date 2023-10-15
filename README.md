# compartmentalisation-helloworld
"Hello World" compartmentalisation example.

### Compile
To compile them, we can first clone the repo and then run ``gmake``. 

There will be two executables, ``hellolb`` and ``hellolpb``, and as the name suggests, they use LB- and LPB-sealed cap respectively. We can run them and see the output.

```bash
$ gmake
cc -march=morello+c64 -mabi=purecap -Xclang -morello-vararg=new  -Iutil -DUSE_LB_SEALED_CAP util/capprint.c util/morello.c src/lb.S main.c -o hellolb
cc -march=morello+c64 -mabi=purecap -Xclang -morello-vararg=new  -Iutil util/capprint.c util/morello.c src/lpb.S main.c -o hellolpb

$ ./hellolb
using LB-sealed capability
before...
csp: 0000fffffff7fe70 1 [0000ffffbff80000:0000fffffff80000) GrRMwWL-----I-V-23 none 1073741424 of 1073741824
inside...
csp: 0000000040a1ff40 1 [0000000040a1c000:0000000040a20000) GrRMwWL----------- none 16192 of 16384
after...
csp: 0000fffffff7fe70 1 [0000ffffbff80000:0000fffffff80000) GrRMwWL-----I-V-23 none 1073741424 of 1073741824
result: 2 + 3 = 5

$ ./hellolpb
using LPB-sealed capability
before...
csp: 0000fffffff7fe70 1 [0000ffffbff80000:0000fffffff80000) GrRMwWL-----I-V-23 none 1073741424 of 1073741824
inside...
csp: 0000000040a1ff40 1 [0000000040a1c000:0000000040a20000) GrRMwWL----------- none 16192 of 16384
after...
csp: 0000fffffff7fe70 1 [0000ffffbff80000:0000fffffff80000) GrRMwWL-----I-V-23 none 1073741424 of 1073741824
result: 2 + 3 = 5
```
