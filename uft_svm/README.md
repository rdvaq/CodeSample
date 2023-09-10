Simple Virtual Machines and Language Translation.

----------------------------------------------------------------------------------

In the repository, you'll find these directories:

  - `bin` holds compiled binaries and also scripts. 

  - `build` is a holding area for compiled files (`.o` and that ilk).
    If you are having trouble compiling, it is safe to remove
    everything from it and start over. (git ignored)

  - `src` contains sources that I provide and that you'll edit:

    - `src/svm` contains source code for the Simple Virtual Machine.
    - `src/uft` contains source code for the Universal Forward Translator.


----------------------------------------------------------------------------------

Overview:

![image](./overviewppt.jpg)


To run the uft:

    - uft <inlang>-<outlang> <file> 
    i.e. uft ho-vo sum.scm
    Type just uft into terminal for usage

To run the svm:

    - svm <file>

Together:

    - uft ho-vo sum.scm | svm
