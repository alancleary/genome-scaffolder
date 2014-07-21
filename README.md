genome-scaffolder
=================

###Compiling###
Set CPLEXDIR and CONCERTDIR to the directories where CPLEX and CONCERT are installed. Uncomment and set the variables in the MakeFile or set them in your .bashrc or .bash_profile file.

If you have installed CPLEX on a platform other than x86 linux, make sure to update the SYSTEM variable in the MakeFile.

You may need to add an include flag to the compiler commands in the MakeFile for the Boost Graph Library if it's not already in your path.

Eventually a config file will be added to automate these steps.


###Helpful links###

* [Markdown syntax](http://daringfireball.net/projects/markdown/syntax)
* [Boost Graph Library docs](http://www.boost.org/doc/libs/1_55_0/libs/graph/doc/table_of_contents.html)
* [CPLEX docs](http://pic.dhe.ibm.com/infocenter/cosinfoc/v12r6/index.jsp)
* [Google sparsehash](https://code.google.com/p/sparsehash/)
* [Valgrind](http://valgrind.org/)
* [SAM specification](http://samtools.github.io/hts-specs/SAMv1.pdf)
* [Bowtie read mapper](http://bowtie-bio.sourceforge.net/index.shtml)
* [BWA read mapper](http://bio-bwa.sourceforge.net/)
* [APG specification](http://www.ncbi.nlm.nih.gov/projects/genome/assembly/agp/AGP_Specification_v2.0.shtml)
* [GapFiller](http://www.baseclear.com/lab-products/bioinformatics-tools/gapfiller/)
