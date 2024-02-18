# Operating System Course
This repository contains the `Operating System` course projects.

### [Socket](https://github.com/ShahnamFeyzian/Operating-System-Course/tree/main/socket/src)
In this project we used the POSIX socket APIs to communication between restaurant, customer and supplier programs.  
For running this programs you should give them the same port address for UDP connections, the 8080 port is recommended for it.   
Also, I wrote some test codes for playing with socket and find out how they work. You can find them [here](https://github.com/ShahnamFeyzian/Operating-System-Course/tree/main/socket/test).

Just run the `make` command in the root and execute the *.out files to run the programs and see its functionality.

### [Pipeline](https://github.com/ShahnamFeyzian/Operating-System-Course/tree/main/pipeline/work%20space)
At this project we used the POSIX named and unnamed pipelines to transfer data between processes. At first, we run the `company.out` as a main program, and it will create some other processes such as buildings, financial, meter, etc.   
The test codes are like the previous part, and you can find them [here](https://github.com/ShahnamFeyzian/Operating-System-Course/tree/main/pipeline/test%20code).

For running the project simply run `make` command at `work space` directory, next run this command.
``` bash
make run ARGS="../../buildings"
```

### [Multi-Thread](https://github.com/ShahnamFeyzian/Operating-System-Course/tree/main/multi-thread)
At this project we implemented a simple image processing program with two approaches, single thread and multi-thread. You can compare the executing time between these two programs and see the multi-thread is faster.  

For running the project first run `make` command in `Parallel` or `Serial` directory and next, run the command below.
``` bash
make run ARGS="first.bmp first_out.bmp"
```
---

> Note: the make file template that I used was implemented by [Misagh Mohaghegh](https://github.com/MisaghM), you can find the template [here](https://github.com/MisaghM/Makefile-Project-Template) if you interested.
