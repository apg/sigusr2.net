% A Language by Any Other Name
% thought, programming, #pinned
% 2014-02-10


Some programs are configurable, and they expose a so called "configuration"
language. These tend to range from key value pairs to full-fledged, [Turing-
complete][turing], programming languages.

A "programming" language is also a "configuration" language, but only in the
abstract. A programming language (somehow, by compiler, or virtual machine, or
direct execution of an AST) produces, let's call it an image[^1], in
memory which evolves the CPU and memory _over time_ while executing.

But, then there's an interesting realization. A "configuration" language
doesn't configure the CPU and its attached memory in the typical sense. It
configures the image. Essentially, a "configuration" language _writes_
specialized programs on the fly, just like a JIT, or a macro-processor in
Lisp.

 [^1]: An image is just an executable and associated data residing in memory.

   [turing]: https://en.wikipedia.org/wiki/Turing_completeness


