Custom C code that runs a smart oven for baking polymer clay. 

Instead of relying on a standard timer, thE program safely heats the oven up, holding it at the exact right baking temperature so the clay doesn't burn, and cooling it down. 

The code fakes the oven's heat mechanics so you can run and test the whole baking cycle inside a software simulator.

## How to run it

1. `make` (compiles the code)
2. `renode renode/run.resc` (boots up the simulated oven)

*If you need to debug the temperature logic line-by-line, hook GDB up to port 3333.*
