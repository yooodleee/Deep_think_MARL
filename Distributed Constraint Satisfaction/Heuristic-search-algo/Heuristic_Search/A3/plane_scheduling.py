import csp_problems
import backtracking
import argparse


# Class to structure the data of a single instance of the plane scheduling problem
# test sequence--one solution (only one legal sequencing)
p1 = csp_problems.PlaneProblem(
    # planes
    ['AC-1'],
    # the flights
    ['AC01', 'AC02', 'AC03', 'AC04', 'AC05', 'AC06'],
    # flights each plane can fly
    [['AC-1', 'AC01', 'AC02', 'AC03', 'AC04', 'AC05', 'AC06']],
    # flights each plane can start with
    [['AC-1', 'AC01', 'AC02', 'AC03', 'AC04', 'AC05', 'AC06']],
    # flights that can follow each other
    [
        ('AC01', 'AC02'), ('AC02', 'AC03'),
        ('AC03', 'AC04'), ('AC04', 'AC05'),
        ('AC05', 'AC06'),
    ],
    # maintenance depots at end of these flights
    [],
    # min maintenance frequency
    7
)