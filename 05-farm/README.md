Implementation of a farm using OpenMP. Tasks to be computed have to be provided through a for loop with iterations producing 
one of the input tasks and then awaiting for Ta time (parameter) before executing next iteration. The parallelism degree of 
the farm should be a parameter. Each task should spent some parametric amount of time (possibly using the active_wait 
function) to produce the final result.