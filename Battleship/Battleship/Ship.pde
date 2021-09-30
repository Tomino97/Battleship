class Ship {                                                          //class for a single ship representation
  int size;
  ArrayList<Box> boxesNear = new ArrayList<Box>();
  int openedBoxes;

  Ship(int size) {
    this.size = size;  
    openedBoxes = 0;
  }

  void addNearBox(Box toAdd) {
    boxesNear.add(toAdd);
  }

  void open() {
    openedBoxes++;
  }

  boolean found() {
    return openedBoxes == size;
  }

  void reveal() {
    for (Box box : boxesNear) {
      if (!box.opened) {
        box.openBox();
      }
    }
  }

  Ship clone() {
    Ship clone = new Ship(size);
    for (Box box : boxesNear) {
      clone.boxesNear.add(box.clone());
    }
    return clone;
  } 
}
