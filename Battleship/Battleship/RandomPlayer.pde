class RandomPlayer {
  Plan plan;
  int planSize = 5;

  RandomPlayer(Plan plan) {
    this.plan = plan;
  }

  void makeMove() { //<>//
    int randomNumber = int(random(plan.getNotOpened().size()));
    plan.openBox(plan.getNotOpened().get(randomNumber));
  }
}
