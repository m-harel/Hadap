#include <OctoWS2811.h>
#include <FastLED.h>

const int ledsPerStrip = 600;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

class Star {

  public:

  static const int totalPixels = 1;

  CHSV leds[totalPixels];

  void clear() {
    for(int i=0; i<totalPixels; i++) {
      leds[i].val = 0;
    }
  }
};

class FlyingSaucer {

 public:

  // top
  static const int top_start = 0;
  static const int top_total = 16;
  static const int top_end = top_start + top_total;
  
  // side
  static const int side_start = top_end;
  static const int side_total = 45;
  static const int side_end = side_start + side_total;
  
  // bottom
  static const int bottom_start = side_end;
  static const int bottom_total = 8;
  static const int bottom_end = bottom_start + bottom_total;
  
  static const int totalPixels = top_total + side_total + bottom_total;
 
  CHSV leds[totalPixels]; 

  void clear() {
    for(int i=0; i<totalPixels; i++) {
      leds[i].val = 0;
    }
  }
  
};

const int numberOfFS = 12;
FlyingSaucer flyingSaucers[numberOfFS];

const int numberOfStars = 27;
Star stars[numberOfStars];

class Animation {

  public:
    unsigned long startTime;
    virtual void render() = 0;
    virtual void startStory()
    {
      startTime = millis();
    }
    
    virtual bool endStory()
    {
      return millis() - startTime > 30000;
    }
  
};

class BreathAnimation : public Animation {

  public:

  BreathAnimation() {
    for(int i=0; i<(numberOfFS / 4 +1); i++) {
      StartNewFS();
    }

    for(int i=0; i<numberOfFS; i++) {
      maxBrightness[i] = random(64, 255);
    }

    for(int i=0; i<(numberOfStars / 2 +1); i++) {
      StartNewStar();
    }

    for(int i=0; i<numberOfStars; i++) {
      maxStarBrightness[i] = random(254, 255);
    }
  }

  bool onFS[numberOfFS] = { false };
  bool isFSGoingBack[numberOfFS] = {false};
  uint8_t maxBrightness[numberOfFS] = { 0 };
  
  bool onStars[numberOfStars] = { false };
  bool isStarGoingBack[numberOfStars] = {false};
  uint8_t maxStarBrightness[numberOfStars] = { 0 };
  
  void render() {

    for (int s=0; s<numberOfStars; s++) {
      if(!onStars[s]) {
          continue;
      }

      Star &star = stars[s];
      uint8_t hue = (s * 256) / numberOfStars + beatsin8(1);

      uint8_t newValue = 0;
        if (!isStarGoingBack[s]) {
          int newRandomValue = random8() % 6;
          newValue = min(255, star.leds[0].val + newRandomValue);
        } else {
          int newRandomValue = random8() % 3;
          newValue = max(0, star.leds[0].val - newRandomValue);
        }

        if (newValue >= maxStarBrightness[s]) {
          isStarGoingBack[s] = true;
        }

        CHSV &pixel = star.leds[0];
        pixel.val = newValue;
        pixel.hue = hue;
        pixel.sat = 255;

        
        if(newValue <= 15 && isStarGoingBack[s]) {
          star.clear();
          StartNewStar();
          onStars[s] = false;
          isStarGoingBack[s] = false;
        }
    }
    
    for(int s=0; s<numberOfFS; s++) {

        if(!onFS[s]) {
          continue;
        }

        FlyingSaucer &fs = flyingSaucers[s];
        uint8_t hue = (s * 256) / numberOfFS + beatsin8(1);
        uint8_t newValue = 0;
        if (!isFSGoingBack[s]) {
          int newRandomValue = random8() % 6;
          newValue = min(255, fs.leds[0].val + newRandomValue);
        } else {
          int newRandomValue = random8() % 3;
          newValue = max(0, fs.leds[0].val - newRandomValue);
        }

        if (newValue >= maxBrightness[s]) {
          isFSGoingBack[s] = true;
        }

        for(int i=0; i < fs.totalPixels; i++) {
          
          
          CHSV &pixel = fs.leds[i];
          pixel.val = newValue;
          pixel.hue = hue;
          pixel.sat = 255;
        }

        if(newValue <= 15 && isFSGoingBack[s]) {
          fs.clear();
          StartNewFS();
          onFS[s] = false;
          isFSGoingBack[s] = false;
        }
      
    }
  }

  void StartNewFS() {
    while(1) {
      int randIndex = random(0, numberOfFS);
      if(onFS[randIndex] == false) {
        onFS[randIndex] = true;
        return;
      }
    }
  }

  void StartNewStar() {
    while(1) {
      int randIndex = random(0, numberOfStars);
      if(onStars[randIndex] == false) {
        onStars[randIndex] = true;
        return;
      }
    }
  }
   
};

class RandomConffetiAnimation : public Animation {

  public:

  bool onFS[numberOfFS] = { false };
  uint8_t currentOnFS = 0;
  
  void render() {
    
    for(int s=0; s<numberOfFS; s++) {

        uint8_t hue = (s * 256) / numberOfFS + beatsin8(10);

        FlyingSaucer &fs = flyingSaucers[s];
        for(int i=0; i < fs.totalPixels; i++) {
          
          
          CHSV &pixel = fs.leds[i];
          uint8_t newVal = pixel.val + (random8() % 30);
          if (newVal > 140) {
            pixel.val = 0;
          } else {
            pixel.val = newVal;
          }
          pixel.hue = hue;
          pixel.sat = 255;
          
        }
      
    }
  }
   
};


class OneToFourFastAnimation : public Animation {

  public:
  
  void render() {
    
    for(int s=0; s<numberOfFS; s++) {

        uint8_t hue = (s * 256) / numberOfFS + beatsin8(10);

        FlyingSaucer &fs = flyingSaucers[s];
        for(int i=fs.side_start; i < fs.side_end; i++) {

          CHSV &pixel = fs.leds[i];
          if (i % 5 != beat16(8) % 5) {
            pixel.val = 0;
            pixel.hue = 0;
            pixel.sat = 0;
            continue;
          }
          pixel.hue = hue;
          pixel.sat = 255;
          pixel.val = 150;         
        }
        for(int i=fs.top_start; i < fs.top_end; i++) {
          
          CHSV &pixel = fs.leds[i];
          pixel.hue = hue;
          pixel.sat = 255;
          pixel.val = 150;         
        }
        for(int i=fs.bottom_start; i < fs.bottom_end; i++) {
          
          CHSV &pixel = fs.leds[i];
          pixel.hue = hue;
          pixel.sat = 255;
          pixel.val = 150;         
        }
    }
  }
   
};

class OneToFourAnimation : public Animation {

  public:
  
  void render() {

    uint8_t bottomValue = beatsin8(35, 40, 128);
    for(int s=0; s<numberOfFS; s++) {

        uint8_t hue = (s * 256) / numberOfFS + beatsin8(10);

        FlyingSaucer &fs = flyingSaucers[s];
        for(int i=fs.side_start; i < fs.side_end; i++) {

          CHSV &pixel = fs.leds[i];
          if (i % 5 != beat8(2) % 5) {
            pixel.val = 0;
            pixel.hue = 0;
            pixel.sat = 0;
            continue;
          }
          pixel.hue = hue;
          pixel.sat = 255;
          pixel.val = 255;      
        }
        for(int i=fs.top_start; i < fs.top_end; i++) {
          
          CHSV &pixel = fs.leds[i];
          if (i % 4 != beat8(1) % 4) {
            pixel.val = 0;
            pixel.hue = 0;
            pixel.sat = 0;
            continue;
          }
          pixel.hue = hue;
          pixel.sat = 255;
          pixel.val = 255;        
        }
        for(int i=fs.bottom_start; i < fs.bottom_end; i++) {
          
          CHSV &pixel = fs.leds[i];
          pixel.hue = 0;
          pixel.sat = 215;
          pixel.val = bottomValue;         
        }
    }
  }
   
};

class ZionOneToFourAnimation : public Animation {

  public:

  ZionOneToFourAnimation() {
    

    for(int i=0;i<numberOfStars; i++) {
      beatsinForStar[i] = random(10,20);
    }
  }

  int beatsinForStar[numberOfStars] = {0};
  
  void render() {
        int hue1 = 130, hue2 = 191;
    for (int s=0; s<numberOfStars; s++) {

      Star &star = stars[s];
      CHSV &pixel = star.leds[0];
      uint8_t bottomValue = beatsin8(beatsinForStar[s], 20, 255);
      pixel.hue = hue2;
      pixel.sat = beatsin8(25,40,255);
      pixel.val = bottomValue;
    }
    for(int s=0; s<numberOfFS; s++) {
    uint8_t breath = beatsin8(4+s, 50,200);
        FlyingSaucer &fs = flyingSaucers[s];
        for(int i=fs.side_start; i < fs.side_end; i++) {
          int tick = beat8(2);int myBreath = breath;
          
          CHSV &pixel = fs.leds[i];
          if (i % 5 != tick % 5) {
          tick++;
            if (i % 5 == tick % 5) {
            pixel.val = 30;
            continue;
            }
            tick++;
            if (i % 5 == tick % 5) {
            pixel.val = 10;
            continue;
            }
            
                        pixel.val = 30;
            continue;

            
          }
          if (i % 5== tick % 5) {
            pixel.sat =255 ;
            pixel.hue = hue1;
            pixel.val = myBreath;  
            continue;
          }
          pixel.sat = 0;
          pixel.hue = hue2;
          pixel.val = myBreath;      
        }
        for(int i=fs.top_start; i < fs.top_end; i++) {
          
          CHSV &pixel = fs.leds[i];
          if (i % 4 != beat8(1) % 4) {
          pixel.hue = 0;
          pixel.sat = 0;
          pixel.val = breath; 
            continue;
          }
          if (i % 8!= beat8(2) % 4) {
        pixel.hue = 0;
          pixel.sat = 0;
          pixel.val = breath;  
            continue;
          }
          pixel.hue = 0;
           pixel.sat = 0;
          pixel.val = breath;     
        }
        for(int i=fs.bottom_start; i < fs.bottom_end; i++) {
          CHSV &pixel = fs.leds[i];
          if (i % 2 != beat8(1) % 2) {
            pixel.val = 40;
            pixel.hue = hue2;
            pixel.sat = breath;
            continue;
          }
          if (i % 4!= beat8(2) % 2) {
            pixel.val = 40;
            pixel.hue = hue2;
            pixel.sat = breath;
            continue;
          }
            pixel.val = 40;
            pixel.hue = hue2;
            pixel.sat = breath;
        }
    }
  }
   
};

class SnakeAnimation : public Animation {

  public:
  
  void render() {

    uint8_t bottomValue = beatsin8(35, 40, 128);
    for(int s=0; s<numberOfFS; s++) {

        uint8_t hue = (s * 256) / numberOfFS;

        FlyingSaucer &fs = flyingSaucers[s];
        uint8_t headPos = ( ((unsigned long)beat16(30)) * fs.side_total) / 65535 % fs.side_total;
        for(int snake_i = 0; snake_i < 3; snake_i++) {
          for(int i=0; i < fs.side_total / 3; i++) {
  
            int currIndex = fs.side_start + ((headPos + snake_i * (fs.side_total / 3) - i + fs.side_total) % fs.side_total);
            
            CHSV &pixel = fs.leds[currIndex];
            pixel.hue = hue + snake_i * (256 / 3);
            pixel.sat = 255;
            pixel.val = max(255 - i*25, 0);
          }
        }
        
        for(int i=fs.bottom_start; i < fs.bottom_end; i++) {
          
          CHSV &pixel = fs.leds[i];
          pixel.hue = 0;
          pixel.sat = 0;
          pixel.val = bottomValue;
        }

        for(int i=fs.top_start; i < fs.top_end; i++) {
          
          CHSV &pixel = fs.leds[i];
          pixel.hue = 0;
          pixel.sat = 0;
          pixel.val = bottomValue;
        }
      
    }
  }
   
};

class OnlyStarsAnimation : public Animation {
    public:

  OnlyStarsAnimation() {
    

    for(int i=0;i<numberOfStars; i++) {
      beatsinForStar[i] = random(10,20);
    }
  }

  int beatsinForStar[numberOfStars] = {0};


  void render() {
    
    for (int s=0; s<numberOfStars; s++) {

      uint8_t bottomValue = beatsin8(beatsinForStar[s], 20, 255);

      Star &star = stars[s];
      CHSV &pixel = star.leds[0];
      pixel.hue = beatsin8(beatsinForStar[s] / 4);
      pixel.sat = 128;
      pixel.val = bottomValue;
    }

    for(int s=0; s<numberOfFS; s++) {

        FlyingSaucer &fs = flyingSaucers[s];

        for(int i=0; i < fs.totalPixels; i += 2) {
          
          
          CHSV &pixel = fs.leds[i];
          pixel.val = 1;
          pixel.hue = 20;
          pixel.sat = 0;
        }
    }
    
  }
};

class SnakeColorfullFadingAnimation : public Animation {

  public:

  int beatsinPerFS[numberOfFS];

  SnakeColorfullFadingAnimation() {
    for(int i=0; i<(numberOfFS / 3 +1); i++) {
      StartNewFS();
    }

    for(int i=0; i<numberOfFS; i++) {
      beatsinPerFS[i] = random(5, 20);
    }
  }

  bool onFS[numberOfFS] = { false };
  
  void render() {
    
    uint8_t bottomValue = beatsin8(35, 40, 128);
    for (int s=0; s<numberOfStars; s++) {
      Star &star = stars[s];
      CHSV &pixel = star.leds[0];
      pixel.hue = 0;
      pixel.sat = 0;
      pixel.val = (int)bottomValue;
    }
    
    for(int s=0; s<numberOfFS; s++) {

      if(!onFS[s]) {
        //continue;
      }

      FlyingSaucer &fs = flyingSaucers[s];
      uint8_t hue = (s * 256) / numberOfFS + beatsin8(1);

      int valueFactor = beatsin8(beatsinPerFS[s]);

      uint8_t headPos = ( ((unsigned long)beat16(30)) * fs.side_total) / 65535 % fs.side_total;
      int currIndex = 0;
      for(int i=0; i < fs.side_total; i++) {

        currIndex = fs.side_start + ((headPos - i + fs.side_total) % fs.side_total);
        
        CHSV &pixel = fs.leds[currIndex];
        pixel.hue = max(hue + i*25, 0);
        pixel.sat = 255;
        pixel.val = max(valueFactor - i*20, 0);
      }
      
      for(int i=fs.bottom_start; i < fs.bottom_end; i++) {
        
        CHSV &pixel = fs.leds[i];
        pixel.hue = 0;
        pixel.sat = 0;
        pixel.val = valueFactor * (int)bottomValue / 256;
      }

      for(int i=fs.top_start; i < fs.top_end; i++) {
        
        CHSV &pixel = fs.leds[i];
        pixel.hue = 0;
        pixel.sat = 0;
        pixel.val = bottomValue;
      }
      if (currIndex == fs.side_total) {
        //fs.clear();
        StartNewFS();
        onFS[s] = false;        
      }
    }
  }

  void StartNewFS() {
    while(1) {
      int randIndex = random(0, numberOfFS);
      if(onFS[randIndex] == false) {
        onFS[randIndex] = true;
        return;
      }
    }
  }
   
};

class PsyRainbowAnimation : public Animation {
    public:

  void render() {

    uint8_t topHue = beatsin8(3);
    
    for(int s=0; s<numberOfFS; s++) {

        FlyingSaucer &fs = flyingSaucers[s];
        fs.clear();

        uint8_t hueOffset = beatsin8(8) + beatsin8(17) + beatsin8(s * 4);

        int alternate = beatsin8(90, 0, 1);

        for(int i=fs.side_start + alternate; i < fs.side_end; i+=2 ) {
          uint8_t hue = hueOffset + i * 255 / fs.side_total;
          fs.leds[i] = CHSV(hue, 255, 128);
        }

        for(int i=fs.bottom_start; i < fs.bottom_end; i++ ) {
          uint8_t hue = hueOffset + i * 255 / fs.bottom_total;
          fs.leds[i] = CHSV(hue, 0, 16);
        }    

        for(int i=fs.top_start; i < fs.top_end; i++ ) {
          uint8_t hue = topHue;
          fs.leds[i] = CHSV(hue, 255, 255);
        }    

    }

    for(int i=0; i<numberOfStars; i++) {
      uint8_t hue = beatsin8(8) + beatsin8(5) + beatsin8(i * 2);
      stars[i].leds[0] = CHSV(hue, 255, 255);
    }
    
  }
};

class SolidColorAnimation : public Animation {
    public:

  void render() {

    uint8_t hue = beatsin8(3);
    
    for(int s=0; s<numberOfFS; s++) {

        FlyingSaucer &fs = flyingSaucers[s];
        for(int i=0; i < fs.totalPixels; i++ ) {
          fs.leds[i] = CHSV(hue, 255, 100);
        }

    }

    for(int i=0; i<numberOfStars; i++) {
      stars[i].leds[0] = CHSV(hue, 255, 100);
    }
    
  }
};

class PinkColorAnimation : public Animation {
    public:

  void render() {
    
    for(int s=0; s<numberOfFS; s++) {

        FlyingSaucer &fs = flyingSaucers[s];
        for(int i=0; i < fs.side_end; i++ ) {
          fs.leds[i] = CHSV(240, 220, 100);
        }

        for(int i=fs.bottom_start; i < fs.bottom_end; i++ ) {
          fs.leds[i] = CHSV(130, 255, beatsin8(25, 0, 200));
        }

    }

    for(int i=0; i<numberOfStars; i++) {
      stars[i].leds[0] = CHSV(230, beatsin8(25, 0, 220), 100);
    }
    
  }
};

class TravelingSnakesAnimation : public Animation {
  private:
    const int snakeSize = 10;

    uint8_t getCurrentHue(){
      return beatsin8(8) + beatsin8(17);
    }

    uint8_t getPieceActivation(int piece){
      return (uint8_t)((255 * piece * piece) / (snakeSize * snakeSize));
    }

  public:

  void render() {
    int MOD = numberOfFS * FlyingSaucer::side_total;
    uint8_t topHue = beatsin8(3);
    
    for(int s=0; s<numberOfFS; s++) {
      FlyingSaucer &fs = flyingSaucers[s];
      fs.clear();
    }

    int numberOfSnake = 2;
    for(int snakeNumber = 0; snakeNumber < numberOfSnake; snakeNumber++){
      int snakeOffset = snakeNumber * MOD / numberOfSnake;
      // The snakes current color
      int speed = 3;
      uint8_t snakeColor = getCurrentHue();
      int currentStartIndex = beatsin16(speed, 0, MOD);
      for(int pieceIndex = 0; pieceIndex < snakeSize; ++pieceIndex){
          int currentIndex = pieceIndex + currentStartIndex + snakeOffset;
          currentIndex %= MOD;
          uint8_t ativation = getPieceActivation(pieceIndex);
          flyingSaucers[currentIndex/FlyingSaucer::side_total].leds[FlyingSaucer::side_start + currentIndex % FlyingSaucer::side_total]
            = CHSV(snakeColor, 255, ativation);
      }
    }

    for(int i=0; i<numberOfStars; i++) {
      uint8_t activation = beatsin8(7) + beatsin8(3) + beatsin8(i * 4);
      uint8_t hue = beatsin8(8) + beatsin8(5) + beatsin8(i * 2);
      stars[i].leds[0] = CHSV(hue, 255, activation/5);
    }
  }
};


static BreathAnimation breath;
static RandomConffetiAnimation confftieColor;
static OneToFourAnimation oneToFourColor;
static ZionOneToFourAnimation zionOneToFourColor;
static OneToFourFastAnimation OneToFourFastColor;
static SnakeAnimation snakeColor;
static SnakeColorfullFadingAnimation snakeColfullFadingColor;
static PsyRainbowAnimation psyRainbow;
static SolidColorAnimation solidColor;
static PinkColorAnimation pinkColor;

static OnlyStarsAnimation onlyStartColor;
static TravelingSnakesAnimation tsa;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
Animation *animations[] = {&tsa, 
&solidColor, &pinkColor, &psyRainbow, &oneToFourColor, &onlyStartColor, &snakeColfullFadingColor, &breath, &OneToFourFastColor, &confftieColor, &snakeColor, &zionOneToFourColor};
//Animation *animations[] = {&confftieColor};
int currentBaseAnimation = 0;


void setup() {

  leds.begin();
  leds.show();
}

void loop() {

  render();

  for(int s=0; s<numberOfFS; s++) {

      const FlyingSaucer &fs = flyingSaucers[s];

      int stickStartIndex;
      if (s >= 6) {
        stickStartIndex = ((s % 6) * fs.totalPixels) + 600;
      }else{
        stickStartIndex = s * fs.totalPixels;
      }
      for(int i = fs.top_start; i < fs.side_end; i++) {
        CRGB fastLedRGB = (CRGB)fs.leds[i];
        int octowsColor = leds.color(fastLedRGB.r, fastLedRGB.g, fastLedRGB.b);
        leds.setPixel(stickStartIndex + i, octowsColor);
      }
      for(int i = fs.bottom_start; i < fs.bottom_end; i++) {
        CRGB fastLedRGB = (CRGB)fs.leds[i];
        int octowsColor = leds.color(fastLedRGB.g, fastLedRGB.r, fastLedRGB.b);
        leds.setPixel(stickStartIndex + i, octowsColor);
      }
    
  }

  for(int s=0; s<numberOfStars; s++) {
    const Star &star = stars[s];
    int starIndex;
    if (s < 13) {
      starIndex = 1200 + s;
    } else {
      starIndex = 1800 + s % 13;
    }
    CRGB fastLedRGB = (CRGB)star.leds[0];
    int octowsColor = leds.color(fastLedRGB.g, fastLedRGB.r, fastLedRGB.b);
    leds.setPixel(starIndex, octowsColor);    
  }
  
  leds.show();

}

void render() {
  if(animations[currentBaseAnimation]->endStory())
  {
    currentBaseAnimation = (currentBaseAnimation + 1) % ARRAY_SIZE(animations);
    animations[currentBaseAnimation]->startStory();
    for(int s=0; s<numberOfFS; s++) {
      FlyingSaucer &fs = flyingSaucers[s];
      fs.clear();
    }
    for(int s=0; s<numberOfStars; s++) {
      Star &star = stars[s];
      star.clear();
    }
  }

  animations[currentBaseAnimation]->render();
}
