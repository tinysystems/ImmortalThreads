## Immortal Threads

Welcome to the homepage of ImmortalThreads! 

Immortal Threads is a novel programming model that brings pseudo-stackful multithreaded processing to intermittent computing. Programmers using Immortal Threads are oblivious to intermittent execution and write their applications in a multithreaded fashion using common event-driven multithreading primitives. Our compiler fronted transforms the stackful threads into stackless threads that waste a minimum amount of computational progress upon power failures. Our runtime implements fair scheduling to switch between threads efficiently.

You can use access the source code of ImmortalThreads [on GitHub](https://github.com/tinysystems/ImmortalThreads)

## How to Cite This Work

The results of this project have been published in a peer-reviewed academic publication (from which all technical figures in this file originate). Details of the publication are as follows.

* **Authors and the project team:** [Eren Yildiz](https://erenyildiz33.github.io/), [Lijun Chen](https://github.com/chenlijun99), [Kasim Sinan Yildirim](https://sinanyil81.github.io/)
* **Publication title:** _Immortal Threads: Multithreaded Event-driven Intermittent Computing on Ultra-Low-Power Microcontrollers_
* **Pulication venue:** [16th USENIX Symposium on Operating Systems Design and Implementation (OSDI 22)](.)
* **Link to publication:** 

To cite this publication please use the following BiBTeX entry.

```
@inproceedings{yildiz:osdi:2022:immortal,
  title = {Immortal Threads: Multithreaded Event-driven Intermittent Computing on Ultra-Low-Power Microcontrollers},
  author={Yildiz, Eren and Chen, Lijun and Yildirim, Kasim Sinan},
  booktitle={16th USENIX Symposium on Operating Systems Design and Implementation (OSDI 22)},
  year={2022}
}
```
