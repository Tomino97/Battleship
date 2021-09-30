class Plan {                                                      //class for the board representation //<>//
  Box[] boxes;
  int shipsAlive;
  int planSize = 5;
  int moves = 0;
  IntList notOpened = new IntList();
  ArrayList<Ship> ships = new ArrayList<Ship>();
  boolean lastHitShip;

  Plan() {                                                        //constructor with randomized ships placement
    ships.add(new Ship(3));
    ships.add(new Ship(2));
    boxes = new Box[25];
    lastHitShip = false;
    shipsAlive = 2;
    for (int i = 0; i < planSize * planSize; i++) {
      notOpened.append(i);
    }
    randomize();
  }

  Plan(boolean noRandom) {                                        //constuctor without the random ships placement
    ships.add(new Ship(3));
    ships.add(new Ship(2));
    boxes = new Box[25];
    lastHitShip = false;
    shipsAlive = 2;
    for (int i = 0; i < planSize * planSize; i++) {
      notOpened.append(i);
    }
  }

  Plan clone() {                                                      //copy function
    Plan result = new Plan(true);
    for (int i = 0; i < boxes.length; i++) {
      result.boxes[i] = boxes[i].clone();
      if (result.boxes[i].isPartOfShip() && !result.ships.contains(result.boxes[i].partOf)) {
        boolean place = true;
        for (Ship ship : ships) {
          if (ship == result.boxes[i].partOf) {
            place = false;
          }
        }
        if(place) {
          result.ships.add(result.boxes[i].partOf);
        }
      }
    }
    return result;
  }

  void show() {                                                    //method for the board rendering
    for (int i = 0; i < boxes.length; i++) {
      boxes[i].show();
    }
  }

  void randomize() {                                               //method for random placement of the ships
    IntList freeSpace = new IntList();
    for (int i = 0; i < planSize * planSize; i++) {
      freeSpace.append(i);
    }
    for (int i = 0; i < shipsAlive; i++) {

      boolean valid = false;
      Ship currentShip = ships.get(i);

      while (!valid) {
        int randomNumber = int(random(freeSpace.size()));
        int randomPos = freeSpace.get(randomNumber);
        PVector index = new PVector(randomPos / planSize, randomPos % planSize);
        PVector direction = getRandomDirection();

        if (validShip(index, direction, currentShip.size, freeSpace)) {

          for (int j = 0; j < currentShip.size; j++) {
            boxes[getIndex(index)] = new Box(new PVector(index.x, index.y), 1, currentShip);
            freeSpace.remove(removeValue(getIndex(index), freeSpace));
            index.add(direction);
          }

          for (int j = 0; j < currentShip.size; j++) {
            index.add(oppositeDirection(direction));

            for (PVector near : boxes[getIndex(index)].close) {
              if (freeSpace.hasValue(getIndex(near))) {
                boxes[getIndex(near)] = new Box(near, -1, null);
                currentShip.addNearBox(boxes[getIndex(near)]);
                freeSpace.remove(removeValue(getIndex(near), freeSpace));
              }
            }
          }

          valid = true;
        }
      }
    }
    for (int place : freeSpace) {
      boxes[place] = new Box(new PVector(place / planSize, place % planSize), -1, null);
    }
  }

  PVector getRandomDirection() {
    int rand = int(random(4));
    switch(rand) {
    case 0:
      return new PVector(1, 0);
    case 1:
      return new PVector(0, 1);
    case 2:
      return new PVector(-1, 0);
    case 3:
      return new PVector(0, -1);
    }
    return new PVector(1, 0);
  }

  int getIndex(PVector index) {
    return int(index.x * planSize + index.y);
  }

  int removeValue(int value, IntList list) {
    for (int i = 0; i < list.size(); i++) {
      int val = list.get(i);
      if (val == value) {
        return i;
      }
    }
    return -1;
  } 

  PVector oppositeDirection(PVector direction) {
    return new PVector(direction.x * -1, direction.y * -1);
  }

  boolean validShip(PVector indexReal, PVector direction, int length, IntList freeSpace) {        //check if a ship placement is valid
    PVector index = indexReal.copy();
    for (int i = 0; i < length; i++) {
      if (index.x >= planSize || index.y >= planSize || index.x < 0 || index.y < 0 || !freeSpace.hasValue(getIndex(index))) {
        return false;
      }
      index.add(direction);
    }
    return true;
  }

  boolean allBoxesOpened() {                                                                      //game ended function
    for (int i = 0; i < boxes.length; i++) {
      if (!boxes[i].opened) { 
        if (shipsAlive == 0) {
          boxes[i].openBox();
        } else {
          return false;
        }
      }
    }
    return true;
  }

  Box getBox(PVector index) {
    for (int i = 0; i < boxes.length; i++) {
      if (boxes[i].getIndex().x == index.x && boxes[i].getIndex().y == index.y) {
        return boxes[i];
      }
    }
    return null;
  }

  void openBox(int index) {
    if (!boxes[index].opened) {
      moves++;
      lastHitShip = boxes[index].openBox();
      notOpened.remove(removeValue(index, notOpened));
      for (int i = 0; i < ships.size(); i++) {
        Ship ship = ships.get(i);
        if (ship.found()) {
          ship.reveal();
          for (Box box : ship.boxesNear) {
            int toRemove = removeValue(getIndex(box.index), notOpened);
            if (toRemove != -1) {
              notOpened.remove(toRemove);
            }
          }
          ships.remove(i);
          shipsAlive--;
        }
      }
    }
  }

  IntList getNotOpened() {
    return notOpened;
  }

  int getMoves() {
    return moves;
  }
}
