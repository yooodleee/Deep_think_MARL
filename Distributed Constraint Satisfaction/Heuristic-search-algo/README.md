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


#### **A simple example**
---

In Section 1.3.3 we give a more elaborate example, but here is a brief illustration of the operation of the ABT algorithm on one of the simple problems encountered earlier. Consider again the instance (c) of the CSP in Figure 1.4, and assume the agents are ordered alphabetically: x1, x2, x3. They initially select values at random; suppose they all select blue. x1 notifies x2 and x3 of his choice, and x2 notifies x3. x2's local view is thus {x1 = blue}, and x3's local view is {x1 = blue, x2 = blue}. x2 and x3 must check for consistency of their local views with their own values. x2 detects the conflict, changes for consistency and similarly changes his x3. In the meantime, x3 also checks for consistency and similarly  chagnes his value to red; he, however, notifies no one. Then x3 receives a second message from x2, and updates his local view to {x1 = blue, x2 = red}. At this point he cannot find a value from his domain consistent with his local view, and, using hyper resolution, generates the Nogood {x1 = blue, x2 = red}. He communicates this Nogood to x2, the lowest ranked agent participating in the Nogood. x2 now cannot find a value consistent with his local view, generates the Nogood {x1 = blue}, and communicates it to x1. x1 detects the inconsistency with his current value, changes his value to red, and communicates the new value to x2 and x3. The process now continuous as before; x2 changes his value back to blue, x3 finds no consistent value and generate the Nogood {x1 = red, x2 = blue}, and then x2 generates the Nogood {x1 = red}. At this point x1 has the Nogood {x1 = blue} as well as the Nogood {x1 = red}, and using hyper-resolution he generates the Nogood {}, and the algorithm terminates having determined that the problem has no solution.

The need for the addition of new edges is seen in a slightly modified example, show in Figure 1.5.

As in the previous example, here too x3 generates the Nogood {x1 = blue, x2 = red} and notifies x2. x2 is not able to regain consistency by changing his own value. However, x1 is not a neighbor of x2, and so x2 does not have the value x1 = blue in his local view and is not able to send the Nogood {x1 = blue} to x1. So x2 sends a request to x1 to add x2 to his list of neighbors and to send x2 his current value. From there onward the algorithm proceeds as before.

![alt text](../scripts/Figure_1_5.png)


#### **An extended example: the four queens problem**
---

