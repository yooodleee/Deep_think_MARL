### **Domain-pruning algorithms**
---

Under domain-pruning algorithms, nodes communicate with their neighbors in order to eliminate values from their domains. We consider two such algorithms. In the first, the filtering algorithm, each node communicates its domain to its neighbors, eliminates from its domain the values that are not consistent with the values received from the neighbors, and the process repeats. Specifically, each node xi, with domain Di repeatdly executes the procedure **Revise**(xi, xj) for each neighbor xj.

![alt text](../scripts/filtering_algorithm.png)

The process, known also under the general term **arc consistency**, terminates when no further elimination takes place, or when one of the domains becomes empty (in which case the problem has no solution). If the process terminates with one value in each domain, that set of values consistutes a solution. If it terminates with multiple values in each domain, the result is inconclusive; the problem might or might not have a solution.


Clearly, the algorithm is guaranteed to terminate, and furthermore it is sound (in that if it announces a solution, or announces that no solution exists, it is correct), but it is not complete (i.e., it may fail to pronounce a verdict). Consider, for example, the family of very simple graph-coloring problems shown in Figure 1.4. (Note tat problem (d) is identical to the problem in Figure 1.3.)

![alt text](../scripts/Figure_1_4.png)

In this family of CSPs the three variables (i.e., nodes) are fixed, as are the "not-equal" constraints between them. What are not fixed are the domains of the variables. Consider the four instances of Figure 1.4.

(a) Initially, as the nodes communicate with one another, only x1's messages result in any change. Specifically, when either x2 or x3 recieve x1's message they remove red from their domains, ending up with D2 = {blue} and D3 = {blue, green}. The, when x2 communicates his new domain to x3, x3 further reduces his domains to {green}. At this point no further changes take place and the algorithm terminates with a correct solution.

(b)