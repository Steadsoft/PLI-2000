# PLI-2000
## A PL/I compiler for Windows NT
This project began as an academic exercise while learning the 'C' language during the summer of 1990, I wanted something non-trivial to use as real goal when studying the 'C' language and decided to write a lexical analyzer for the PL/I language because that was a language I knew well, I had no intention to do more than just write the analyzer.

As the project progressed and my knowledge of C solidified, I began to develop a deeper interest into compiler theory including recursive descent parsing, again because I knew PL/I very well from my professional work PL/I presented no problems as the langauge to use for these experiments.

Eventually the project grew and I became more confident that I was understanding the subject and that I could perhapse implement a true compiler, one that could generate 32bit DLLs in COFF format that could be linked with the MS linker. There was an earlier version too that ran on DOS and built DOS object files for the 286.

Here are some of the resources I used as I worked on this:

[ANSI Standard 74-1987 (R1998)](https://webstore.ansi.org/standards/incits/ansiincits741987r1998)

[The Multics PL/I Compiler](https://multicians.org/pl1-raf.html)

[Understanding and Writing Compilers](https://www.amazon.com/Understanding-Writing-Compilers-Yourself-Guide/dp/0333217314)

[Compilers: Principles, Techniques, and Tools](https://www.amazon.com/Compilers-Principles-Techniques-Alfred-Aho/dp/0201100886/ref=sr_1_4?keywords=dragon+book+compiler&qid=1636226821&s=books&sr=1-4)

[Compiler Construction: Theory and Practice](https://www.amazon.com/Compiler-Construction-Practice-Revised-Hardcover/dp/B011DBDVNC/ref=sr_1_2?keywords=compiler+construction+theory+and+practice&qid=1636226942&s=books&sr=1-2)

[Compiler Design In C](https://www.biblio.com/book/compiler-design-c-i-holub/d/1375125420)

[Intel 386 Programmer's Reference Manual](https://css.csail.mit.edu/6.858/2013/readings/i386.pdf)

The project was more or less shelved but did see a few sales as a tool that could be used on a PC to verify PL/I source code without needing to run on a mainframe or minicomputer, a kind of pre-validation for people using the language on mainframes and minicomputers who may not have had access to those systems yet needed to work on PL/I.

At the time it was shelved a decent subset of the language could be used to write code that would compile, link and run on Windows NT. I/O was supported by a support library written in C but callable from PL/I, that's how the language was able to interface to the OS, via that C layer.

A number of test source files were used for validating and testing the compiler when changes were made to it, these can be found here https://github.com/Steadsoft/PLI-2000/tree/main/pl1

The code was moved to Visual Studio after a few years (it was originally built using Borland tools on DOS) and is now a Visual Studio solution and project.

