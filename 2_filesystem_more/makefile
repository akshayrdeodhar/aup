111703013_a2.pdf: a2.md ctval.png tac.png
	pandoc -o 111703013_a2.pdf a2.md -f markdown-implicit_figures


clean:
	rm 111703013_a2.pdf

hole.out: hole.c 
	clang hole.c -Wall -o hole.out
