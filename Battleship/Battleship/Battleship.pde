// ConnectionGene, ConnectionHistory, Genome, Node, Player, Population, Species are classes mainly constructed and commented by a guy under pseudonym of Code-Bullet
// https://github.com/Code-Bullet/NEAT_Template/tree/master/TemplateNeat - contains the NEAT template, made by him

Plan planToCopy;
RandomPlayer player;
int planSize = 5;     //initial board size
int isPlaying = 2;    //0 = player clicking, 1 = randomPlayer, 2 = NN
boolean gameEnded;

int nextConnectionNo = 1000;
Population pop = new Population(100);                              //initial size of 100 agents
int speed = 60;

int gen = 0;
int total = 0;
Player genPlayerTemp;

void setup() {
  background(#FFFFFF);                                             //construction of the board
  size(412, 412);
  gameEnded = false;
  planToCopy = new Plan();
  strokeWeight(2);
  for (int i = 0; i < planSize + 1; i++) {
    stroke(#030000);
    line(0, 82*i, (planSize + 1) * 2 + planSize * 80, 82*i);
    line(82*i, 0, 82*i, (planSize + 1) * 2 + planSize * 80);
  }                                                                
  if (isPlaying == 1) {                                            //Random player playing
    player = new RandomPlayer(planToCopy);
    if(gen % 20 == 0) {                                            //every 20 games, print the average moves
      if(gen != 0) {
        print("AVERAGE MOVES AFTER " + gen + " GENERATIONS: " + floor(total / 20) + "\n");
        total = 0;
      }
    }
  }
  if (isPlaying == 2) {                                            //Neural network - new generation
    pop.pop.get(0).plan = planToCopy;
    for (int i = 1; i< pop.pop.size(); i++) {                      //generate plan for each of the children
      pop.pop.get(i).plan = new Plan();
    }
    if(gen % 20 == 0) {                                            //every 20 games, print the average moves
      if(gen != 0) {
        print("AVERAGE MOVES AFTER " + gen + " GENERATIONS: " + ((float)total / 2000) + "\n");
        total = 0;
      } //<>//
    }
  }
}

void draw() {
  if (gameEnded) {
    print("GAME ENDED IN " + planToCopy.getMoves() + " TURNS\n");
    //delay(2000);                                                //uncomment to have an extra 2 seconds to observe
    if (isPlaying == 1) {
      gen++;
      total += planToCopy.getMoves();
    }
    setup();
  }
  
  if (isPlaying != 2) {
    if(planToCopy.allBoxesOpened()) {
      gameEnded = true;
    }
    planToCopy.show();
  }
  
  if (isPlaying == 1 && !gameEnded) {
    player.makeMove();
  }
  
  if (isPlaying == 2) {                                            //if current gen players are not done then update it
    if(!pop.done()) {
      pop.updateAlive();
    } else {
      gen++;
      for (int i = 0; i< pop.pop.size(); i++) {
        total += pop.pop.get(i).plan.getMoves();
      }
      pop.naturalSelection();                                      //remove non-efficient agents
      setup();
    }
  }
}

void mousePressed() {                                              //coordinates of the mouse processed, when pressed
  if (isPlaying == 0) {
    int x = (mouseX - 2) / 82; 
    int y = (mouseY - 2) / 82;
    planToCopy.openBox(5*x + y);
  }
  if (isPlaying == 1 || isPlaying == 2) {
    loop();
  }
}
