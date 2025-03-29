from util import manhattanDistance
from game import Directions
from game import Agent
import random, util 
import math



class ReflexAgent(Agent):
    """ A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.
    
    The code below is provided as a guide. You are welcome to change it in 
    any way you see fit, so long as you don't touch your method headers.
    """

    def getAction(self, gameState):
        """getAction chooses among the best options according to the evaluation function.
        Just like in the previous projext, getAction takes a GameState and returns
        some Directions.X for sme X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        """

        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices)    # Pick randomly among the best

        return legalMoves[chosenIndex]
    
    def evaluationFunction(self, currentGameState, action):
        """Design a better evalution function here.
        The evalutaion function take sin the current and propsed successor
        GameStates (pacman.py) and returns a number, where higher numbers 
        are better.
        
        The code below extracts some useful information from the state, like
        the remaining food (newFood) and Pacman position after moving (newPosition).
        newScaredTimes holds the number of moves that each ghost will remain scared
        because of Pacma having eaten a power pellet.
        
        Print out these variables to see what you're getting, then combine them
        to create a masterfull evaluation function.
        """

        # Useful info you can extract from a GameState (pacman.py)
        nextState = currentGameState.generatePacmanSuccessor(action)
        newPosition, foodList = nextState.getPacmanPosition(), nextState.getFood().asList()
        closestFood = min(
            [
                manhattanDistance(newPosition, food)
                for food in foodList
            ] + [float("inf")]
        )
        minGDist = min(
            [
                manhattanDistance(newPosition, ghost)
                for ghost in nextState.getGhostPositions()
            ]
        )

        if minGDist <= 1:
            return -float("inf")
        
        return nextState.getScore() + 1/closestFood - 1/minGDist


def scoreEvaluationFunction(currentGameState):
    """This default eval func just returns the score of the state.
    The score is the smae one displayed in the Pacma GUI.
    
    This eval func is meant for use with adversarial search agents
    (not reflex agents).
    """
    return currentGameState.getScore()


class MultiAgentSearchAgent(Agent):
    """This class provides some elements to all of your multi-agent searchers.
    Any methods defined here will be available to the MinmaxPacmanAgent,
    AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.
    
    "do not" need to make any changes here, but if want to add funtionality to
    all adversarial search agents add here. however, do not remove anything here.
    
    Note: this is an abstract class: one that should not be instantiated. It's 
    only partially specified, and designed to be extended. Agent (game.py)
    is another abstract class.
    """

    def __init__(self, evalFn='scoreEvaluationFunction', depth='2'):
        self.index = 0  # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)


class MinmaxAgent(MultiAgentSearchAgent):
    """minmax agent (question 2)"""

    def getAction(self, gameState):
        """Returns the minmax act from the current gameState using self.depth
        and self.evaluationFunction.
        
        Here are some method calls that might be useful when implementing minmax.
        
        gameState.getLegalActions(Index):
            returns a list fof legal acts for an agent
            Index=0 means Pacman, ghosts are >= 1
            
        gameState.generateSuccessor(Index, action):
            returns the successor game state after an agent takes an action
            
        gameState.getNumAgents():
            returns the total num of agents in the game
            
        gameState.isWin():
            returns whether or not the game state is a winning state
            
        gameState.isLose():
            returns whether or not the game state is a losing state
        """

        return self.MinMax(gameState, 0, 0)[1]
    
    def MinMax(self, gameState, Index, depth):
        if depth == self.depth * gameState.getNumAgents() \
                or gameState.isLose() or gameState.isWin():     # Base Case
            
            return (self.evaluationFunction(gameState), None)
        
        Actions = [(self.MinMax(gameState.generateSuccessor(Index, action), (depth + 1)
                                % gameState.getNumAgents(), depth + 1)[0], action)
                    for action in gameState.getLegalActions(Index)]
        
        if Index == 0:
            return max(Actions) # Max Agent
        else:
            return min(Actions) # Min Agent


