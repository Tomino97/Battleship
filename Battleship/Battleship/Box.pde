class Box {                                                    //class for representation of a single tile //<>//
  PVector index;
  int scheme;
  boolean opened;
  int planSize = 5;
  ArrayList<PVector> close = new ArrayList<PVector>();
  Ship partOf;

  Box(PVector index, int scheme, Ship partOf) {
    this.index = index;
    opened = false;
    this.scheme = scheme;
    this.partOf = partOf;
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        if (index.x + i < planSize && index.x + i >= 0 && index.y + j < planSize && index.y + j >= 0 && (i != 0 || j != 0)) {
          close.add(new PVector(index.x + i, index.y + j));
        }
      }
    }
  }
  
  Box clone() {                                                //copy function
    Box result;
    if(partOf != null) {
      result = new Box(new PVector(index.x, index.y), scheme, partOf.clone()); 
    } else {
      result = new Box(new PVector(index.x, index.y), scheme, null);
    }
    return result;
  }

  void show() {                                                //method for rendering the tile
    if (!opened) {
      fill(255, 255, 255);
      noStroke();
      rect(2 + index.x * 2 + index.x * 80, 2 + index.y * 2 + index.y * 80, 80, 80);
    } else {
      switch(scheme) {
      case -1:
        fill(#2A31E5);
        break;
      case 1:
        fill(#FF3434);
        break;
      }
      noStroke();
      rect(2 + index.x * 2 + index.x * 80, 2 + index.y * 2 + index.y * 80, 80, 80);
    }
  }

  boolean openBox() {                                        //function for uncovering the tile
    opened = true;
    if(partOf != null) { //<>//
      partOf.open(); 
      return true;
    } 
    return false;
  }

  boolean isPartOfShip() {
    return scheme == 1;
  }

  PVector getIndex() {
    return index;
  }
}
