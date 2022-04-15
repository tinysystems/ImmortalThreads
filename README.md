[ImmortalThreads](https://tinysystems.github.io/ImmortalThreads/) is a novel programming model that brings pseudo-stackful multithreaded processing to intermittent computing. Programmers using Immortal Threads are oblivious to intermittent execution and write their applications in a multithreaded fashion using common event-driven multithreading primitives. Our compiler fronted transforms the stackful threads into stackless threads that waste a minimum amount of computational progress upon power failures. Our runtime implements fair scheduling to switch between threads efficiently.

Please check [OSDI 22' Artifact Evaluation](ARTIFACT_EVALUATION.md) page for the details.

## Acknowledgments

<a href="https://www.unitn.it/"><img src="./doc-images/unitn_logo.png" width="300px"></a> <a href="https://ege.edu.tr/"><img src="./doc-images/ege_logo.png" width="100px"></a>

## Copyright

MIT Licence or otherwise specified. See [license](./LICENSE.txt) file for details.