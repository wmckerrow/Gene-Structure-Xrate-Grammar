./Generator input.gene out.stk > genout
xrate -g genestructure.eg out.stk -ar > xrateout
./DifferenceFinder xrateout genout
rm out.stk
