# Porting layer

ImmortalThreads is designed to be portable, although admittedly not so many
devices with builtin FRAM exist in the market.

Every port contains at least one header file named `port.h`, where some
platform specific macros need to be provided.
In particular the following macros are required:

* `_ei()`: enable interrupt
* `_di()`: disable interrupt
