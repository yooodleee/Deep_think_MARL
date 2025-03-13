### **Heuristic search algorithms**
---

A straightforward centralized trail-and-error solution to a CSP is to first order the variables (e.g., alphabetically).
Then, given the ordering x1, x2, ..., xn, invoke the procedure ChooseValue(x1, {}). The procedure ChooseValue is defined 
recursively as follows, where {v1, v2, ..., vn-1} is the set of values assigned to variables x1, ...., xi-1.

```
procedure ChooseValue(xi, {v1, v2, ..., vi-1})
vi <- value from the domain of xi, that is consistent with {v1, v2, ..., vi-1}

if no such value exists then
 |  backtrack^1
else if i = n then
 |  stop
else
    ChooseValue(xi+1, {v1, v2, ..., vi})
```

This exhausive search of the space of assignments has the advantage of completeness. But it is "distributed" only in the 
uninteresting sence that the different agents execute sequentially, mimicking the execution of a centralized algorithm.

This following attempt at a distributed algorithm has the opposite properties: it allows the agents to execute in parallel and asynchronously, is sound, but is not complete. Consider the following naive procedure, executed by all agents in parallel and asynchronously.

```
select a value from your domain
repeat
|   if your current value is consistent with the current value of your
|   neighbors, or if none of the values in your domain are consistent with
|   them then
|    |  do nothing
|    else
|    |  select a value in your domain that is consistent with those of your
|    |  neighbors and notify your neighbors of your new value
until there is no change in your value
```

Clearly, when the algorithm terminates because no constraint violations have occurred, a solution has been found. But in all 
other cases, all bets are off. If the algorithm terminates because no agent can find a value consistent with those of his 
neighbors, there might still be a consistent global assignment. And the algorithm may never terminate even if there is a solution. For example, consider example (d) of Figure 1.4: if every agent cycles sequentially between red, green, and blue, the alrogithm will never terminate.

We have given these two straw-man algorithms for two reasons. Our first reason is to show that reconciling true parallelism and asynchrony with soundness and completeness is likely to require somewhat complex alrogithms. And second, the fundamental heuristic algorithm for distributed CSPs-the asynchronous  backtracking(or ABT) algorithm-shares much with the two alrogithms.
From the first alrogithm it borrows the notion of a global total ordering on the agents. From the second it borrows a message-passing protocol, albeit a more complex one, which relies on the global ordering. We will describe the ABT in its simplest form. After demonstrating it on an extended example, we will point to ways in which it can be improved upon.


#### **The asynchronous backtracking alrogithm**
---

As we said, the asynchronous backtracking (ABT) alrogithm assumes a total ordering (the "priority order") on the agents. Each binary constraint is known to both the constrained agents and is checked in the algorithm by the agent with the lower priority between the two. A link in the constraint network is always directed from an agent with higher priority to an agent with lower priority.

Agents instantiate their variables concurrently and send their assigned values to the agents that are connected to them by outgoing links. All agents wait for and respond to messages. After each update of his assignment, an agent sends his new assignment along all outgoing links. An agent who recieves an assignment (from the higher-priority agent of the link), tries to find an assignment for his variable that does not violate a constraint with the assignment his received.

ok? messages are messages carrying an agent's variable assignment. When an agent Ai receives an ok? message from agent Aj, Ai places the received assignment in a data structure called agent_view, which holds the last assignment Ai received from higher-priority neighbors such as Aj. Next, Ai checks if his current assignment is still consistent with his agent_view. If it is consistent, Ai does nothing. If not, Ai searches his domain for a new consistent value. If he finds one, he assigns his variable that value and sends ok? messages to all lower-priority. Otherwise, Ai backtracks.

The backtrack operation is executed by sending a Nogood message. Recall that a Nogood is simply an inconsistent partial assigment, that is, assignments, that is, assignments of specific values to some of the variables that together violate the constraints on those variables. In this case, the Nogood consistent of Ai's agent_view.^2 The Nogood is sent to the agent with the lowest priority among the agents whose assignments are included in the consistent tuple in the Nogood. Agent Ai whe sends a Nogood message to agent Aj assumes that Aj will change his assignment. Therefore, Ai removes from his agent_view the assignment of Aj and makes an attempt to find an assignment for Aj's variable that is consistent with the updated agent_view.

Because of its reliance on building up a set of Nogoods, the ABT algorithm can be seen as a greedy version of the hyper-resolution algorithm of the previous section. In the letter, all possible Nogoods are generated by each agent and communicated to all neighbors, even though the vast majority of these messages are not useful. Here, agents make tentative choices of a value for their variables, only generate Nogoods that incorporate values already generated by the agents above them in the order, and -importantly-communicate new values only to some agents ane new Nogoods to only one agent.

Below is the pseudocode of the ABT algorithm, specifying the protocol for agent Ai.

![alt text](../scripts/ABT_algorithm.png)

Notice a certain wrinkle in the pseudocode, having to do with the addition of edges. Since the Nogood can include assignments of some agent Aj, which Ai was not previously constrained with, after adding Aj's assignment to its agent_view Ai sends a message to Aj asking it to add Ai to its list of outgoing links. Furthermore, after adding the link, Aj sends an ok? message to Ai each time it reassigns its variable. After storing the Nogood, Ai checks if its assignment is still consistent. If it is, a message is sent to the agent the Nogood was received from. This resending of the assignment is crucial since, as mentioned earlier, the agent sending a Nogood assumes that the receiver of the Nogood replaces its assignment. Therefore it needs to know that the assignment is still valid. If the old assignment that was forbidden by the Nogood is inconsistent, Ai tries to find a new assignment to the case when an ok? message is received.