./Generator input.gene out.stk > genout
./../dart/bin/xrate -g genestructure.eg out.stk -ar > xrateout
./DifferenceFinder xrateout genout
rm out.stk
