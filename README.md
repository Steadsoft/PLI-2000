# PLI-2000
## A PL/I compiler for Windows NT
This project began as an academic exercise while learning the 'C' language during the summer of 1990, I wanted something non-trivial to use as real goal when studying the 'C' language and decided to write a lexical analyzer for the PL/I language because that was a language I knew well, I had no intention to do more than just write the analyzer.

As the project progressed and my knowledge of C solidified, I began to develop a deeper interest into compiler theory including recursive descent parsing, again because I knew PL/I very well from my professional work PL/I presented no problems as the langauge to use for these experiments.

Eventually the project grew and I became more confident that I was understanding the subject and that I could perhapse implement a true compiler, one that could generate 32bit DLLs in COFF format that could be linked with the MS linker. There was an earlier version too that ran on DOS and built 16-bit DOS object files for the 286.

Here are some of the resources I used as I worked on this:

[ANSI Standard 74-1987 (R1998)](https://webstore.ansi.org/standards/incits/ansiincits741987r1998)

[The Multics PL/I Compiler](https://multicians.org/pl1-raf.html)

[Understanding and Writing Compilers](https://www.amazon.com/Understanding-Writing-Compilers-Yourself-Guide/dp/0333217314)

[Compilers: Principles, Techniques, and Tools](https://www.amazon.com/Compilers-Principles-Techniques-Alfred-Aho/dp/0201100886/ref=sr_1_4?keywords=dragon+book+compiler&qid=1636226821&s=books&sr=1-4)

[Compiler Construction: Theory and Practice](https://www.amazon.com/Compiler-Construction-Practice-Revised-Hardcover/dp/B011DBDVNC/ref=sr_1_2?keywords=compiler+construction+theory+and+practice&qid=1636226942&s=books&sr=1-2)

[Compiler Design In C](https://www.biblio.com/book/compiler-design-c-i-holub/d/1375125420)

[Intel 386 Programmer's Reference Manual](https://css.csail.mit.edu/6.858/2013/readings/i386.pdf)

[Microsoft COFF Specification](https://courses.cs.washington.edu/courses/cse378/03wi/lectures/LinkerFiles/coff.pdf)

The project was more or less shelved but did see a few sales as a tool that could be used on a PC to verify PL/I source code without needing to run on a mainframe or minicomputer, a kind of pre-validation for people using the language on mainframes and minicomputers who may not have had access to those systems yet needed to work on PL/I.

At the time it was shelved a decent subset of the language could be used to write code that would compile, link and run on Windows NT. I/O was supported by a support library written in C but callable from PL/I, that's how the language was able to interface to the OS, via that C layer.

A number of test source files were used for validating and testing the compiler when changes were made to it, these can be found here https://github.com/Steadsoft/PLI-2000/tree/main/pl1

The code was moved to Visual Studio after a few years (it was originally built using Borland tools on DOS) and is now a Visual Studio solution and project. Though somewhat dated now and not a language that's in use very much, this is a real working compiler that goes all the way from raw source code to Microsoft COFF DLL files. There was no source code control in place at the time I developed this either.

You are welcome to contact me or post issues here if you have questions or find the project useful in any way.

The solution (PLI2000.sln) builds (with some warnings) fine on Windows 10 with Visual Studio 2019, I'll check with Visual Studio 2022 once that's released.

## How to run

The compiler leverages a powerful command line argument menuing system, sadly the source for this has been lost (but the .LIB and DLL are still here!).

You can invoke the compiler from a console window as simply `PLI<Enter>` or `PLI ; <Enter>` (this latter will force the interactive menu system to run)

It expects the path to a source file and in the menu you can tab around and use left-arrow or right-arrow to cycle the various options.

![image](https://user-images.githubusercontent.com/12262952/140656440-382fd045-e8e3-47d1-8dd5-d19bdfa044e0.png)

running that elicits:

![image](https://user-images.githubusercontent.com/12262952/140656462-e42a2ca1-1abe-45e5-97f7-46e0d096a9b1.png)

Because we specified the listing option you'll see a .LST file generated that shows epxanded include files with per-file line numbering as well as nesting depths and so on.

![image](https://user-images.githubusercontent.com/12262952/140656410-2c42eb3b-4d98-4e8b-ad7d-aee382ef1af3.png)

Specifying the `+system` option will generate assembly code too, also in the listing file:

![image](https://user-images.githubusercontent.com/12262952/140656618-8deb1308-aebd-4f0c-bc79-e252ffbbd7af.png)



