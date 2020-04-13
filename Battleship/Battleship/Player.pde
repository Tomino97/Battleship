class Player {
  float fitness;
  Genome brain;
  
  int genomeInputs = 26;
  int genomeOutputs = 25;
  float[] vision = new float[genomeInputs];//the input array fed into the neuralNet 
  float[] decision = new float[genomeOutputs]; //the out put of the NN 
  
  float unadjustedFitness;
  int lifespan = 0;//how long the player lived for fitness
  int bestScore =0;//stores the score achieved used for replay
  boolean dead;
  int score = 25*100;
  int gen = 0;
  
  Plan plan;

  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  //constructor

  Player() {
    brain = new Genome(genomeInputs, genomeOutputs);
  }

  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  void show() {
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<replace
  }
  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  void move() {
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<replace
  }
  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  void update() {
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<replace
  }
  //----------------------------------------------------------------------------------------------------------------------------------------------------------

  void look() {
    int i = 0;
    for (Box box : plan.boxes) {
      if(!box.opened) {
       vision[i] = 0.01; 
      } else {
       if(box.isPartOfShip()) {
        vision[i] = 1; 
       } else {
        vision[i] = -1; 
       }
      }
      i++;
    }
    if(plan.lastHitShip) {
      vision[i] = 1;
    } else {
      vision[i] = 0; 
    }
  }






  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  //gets the output of the brain then converts them to actions
  void think() {

    float max = 0;
    int maxIndex = 0;
    //get the output of the neural network
    decision = brain.feedForward(vision);
    
    for (int i = 0; i < decision.length; i++) {
      if (decision[i] > max) {
        max = decision[i];
        maxIndex = i;
      }
    }
    boolean successfull = false;
    while(!successfull) {
      if(plan.boxes[maxIndex].opened) {
        maxIndex = floor(random(decision.length));
        continue;
      }
      successfull = true;
      boolean consecutiveHit = plan.lastHitShip;
      plan.openBox(maxIndex);
      if(plan.allBoxesOpened()) {
        dead = true;
      }
      score -= 100;
      if(plan.lastHitShip && consecutiveHit) {
        score += 500; 
      }
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<replace
  }
  //---------------------------------------------------------------------------------------------------------------------------------------------------------  
  //returns a clone of this player with the same brian
  Player clone() {
    Player clone = new Player();
    clone.brain = brain.clone();
    clone.fitness = fitness;
    clone.brain.generateNetwork(); 
    clone.gen = gen;
    clone.bestScore = score;
    return clone;
  }

  //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  //since there is some randomness in games sometimes when we want to replay the game we need to remove that randomness
  //this fuction does that

  Player cloneForReplay() {
    Player clone = new Player();
    clone.brain = brain.clone();
    clone.fitness = fitness;
    clone.brain.generateNetwork();
    clone.gen = gen;
    clone.bestScore = score;
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<replace
    return clone;
  }

  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  //fot Genetic algorithm
  void calculateFitness() {
    fitness = score * score;
  }

  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  Player crossover(Player parent2) {
    Player child = new Player();
    child.brain = brain.crossover(parent2.brain);
    child.brain.generateNetwork();
    return child;
  }
}
