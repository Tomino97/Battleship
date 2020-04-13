Plan planToCopy = new Plan();
RandomPlayer player;
int planSize = 5;
int isPlaying = 2;    //0 = player clicking, 1 = randomPlayer, 2 = NN, 3 = ...
boolean gameEnded;

int nextConnectionNo = 1000;
Population pop = new Population(10);
int speed = 60;

int gen = 0;
int total = 0;
Player genPlayerTemp;

void setup() {
  background(#FFFFFF); 
  size(412, 412);
  gameEnded = false;
  planToCopy = new Plan();
  strokeWeight(2);
  for (int i = 0; i < planSize + 1; i++) {
    stroke(#030000);
    line(0, 82*i, (planSize + 1) * 2 + planSize * 80, 82*i);
    line(82*i, 0, 82*i, (planSize + 1) * 2 + planSize * 80);
  }
  if (isPlaying == 1) {
    player = new RandomPlayer(planToCopy);
    if(gen % 100 == 0) {
      if(gen != 0) {
        print("AVERAGE MOVES AFTER " + gen + " GENERATIONS: " + floor(total / gen) + "\n");
      }
      noLoop();
    }
  }
  if (isPlaying == 2) {
    pop.pop.get(0).plan = planToCopy;
    for (int i = 1; i< pop.pop.size(); i++) {//generate plan for each of the children
      pop.pop.get(i).plan = planToCopy.clone();
    }
    if(gen != 0) {
      print("AVERAGE MOVES AFTER " + gen + " GENERATIONS: " + (total / 10) + "\n");
      total = 0;
    }
    noLoop();
  }
}

void draw() {
  if (gameEnded) {
    print("GAME ENDED IN " + planToCopy.getMoves() + " TURNS\n");
    //delay(2000);
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
  
  if (isPlaying == 2) {//if current gen player is not dead then update it
    if(!pop.done()) {
      pop.updateAlive();
    } else {
      gen++;
      for (int i = 0; i< pop.pop.size(); i++) {
        total += pop.pop.get(i).plan.getMoves();
        println(pop.pop.get(i).plan.getMoves());
      }
      pop.naturalSelection();
      setup();
    }
  }
}

void mousePressed() {
  if (isPlaying == 0) {
    int x = (mouseX - 2) / 82; 
    int y = (mouseY - 2) / 82;
    planToCopy.openBox(5*x + y);
  }
  if (isPlaying == 1 || isPlaying == 2) {
    //redraw();
    loop();
  }
}

/*void mouseReleased() {
 if (isPlaying == 1) {
 noLoop();  // Releasing the mouse stops looping draw()
 }
 }*/
