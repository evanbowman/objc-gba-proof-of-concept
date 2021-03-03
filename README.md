# Objective-C GBA proof of concept

The ARM compiler toolchain can compile Objective-C just fine, we're just missing an implementation of the Obj-c runtime for the gameboy advance. In this very-hacky example (see main.m), I show that the code compiles and links just fine, and even demonstrate a message send. Message sends, are, however, very much still broken, as the runtime implementation is super minimal and does not register selectors correctly. The selector dispatch is sort of hard-coded to bootstrap this specific example.
