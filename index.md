## Immortal Threads

Welcome to the homepage of ImmortalThreads! 

Immortal Threads is a novel programming model that brings pseudo-stackful multithreaded processing to intermittent computing. Programmers using Immortal Threads are oblivious to intermittent execution and write their applications in a multithreaded fashion using common event-driven multithreading primitives. Our compiler fronted transforms the stackful threads into stackless threads that waste a minimum amount of computational progress upon power failures. Our runtime implements fair scheduling to switch between threads efficiently.

## How to Cite This Work

The results of this project have been published in a peer-reviewed academic publication (from which all technical figures in this file originate). Details of the publication are as follows.

* **Authors and the project team:** [Eren Yildiz](https://jasperdewinkel.com/), [Lijun Chen](https://www.vitokortbeek.com/), [Kasim Sinan Yildirim](https://josiahhester.com/)
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
}
```



You can use access the source code of ImmortalThreads [on GitHub](https://github.com/tinysystems/ImmortalThreads) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [Basic writing and formatting syntax](https://docs.github.com/en/github/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/tinysystems/ImmortalThreads/settings/pages). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://support.github.com/contact) and we’ll help you sort it out.
