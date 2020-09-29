
# Aritificial Intelligence Project
## A Study of Three Vacuum Cleaner Agents: 
## Simple Deterministic Reflex Agent, Random Reflex Agent, and Deterministic Model-based Reflex Agent  
&nbsp;

### Abstract

Three different vacuum cleaning agents, a simple memory-less deterministic reflex agent, a randomized reflex agent, and a deterministic model-based agent with memory, were designed and implemented for an experiment to compare their performance and learn their characteristic differences. A simple memory-less deterministic reflex agent decides its action only by its percepts from sensors such as a wall sensor and a dirt sensor. However, a randomized reflex agent decides its action just randomly. With internal state, a deterministic model-based reflex agent maintains 3 bits of memory, and the memory helps the agent chooses an action. They were tested in two different environments, and their total action count is 2000, and each agent ran 50 times. As the result, a deterministic model-based reflex agent showed the best performance followed by a simple memory-less deterministic reflex agent. We found that the performance of deterministic agents is better than a randomized reflex agent in a single-agent environment. Based on these findings, we discussed the future design of a deterministic model-based reflex agent for more complex environments.

&nbsp;
### Results & Discussion
Please see the [report](https://github.com/csdankim/Vacuum_Agent/blob/master/PA1_vauum_agents.pdf).

&nbsp;
### Code

The vacuum cleaner is comprised of 2 files.

************ Code ************ 
1. 4room.cpp<br>
input file: input_wall.txt
2. default.cpp<br>
input file: input_no_wall.txt

********** Execution **********<br>
g++ 4room.cpp<br>
```
./a.out
```

g++ default.cpp<br>
```
./a.out
```