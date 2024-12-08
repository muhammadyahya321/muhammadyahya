#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
using namespace std;
using namespace sf;

const int resolutionX = 960;
const int resolutionY = 640;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionY / boxPixelsY; 
const int gameColumns = resolutionX / boxPixelsX; 

const int maxSprays = 56; 
const int spraysPerCan = 1; 
const int maxSprayCans = 3; 
int sprayLeft = 56;
int sprayCansRemaining[maxSprayCans] = { maxSprays, maxSprays, maxSprays };
bool sprayCanActive[maxSprayCans] = { true, false, false }; 
int activeSprayCan = 0; // Index for the currently active spray can
int spraysCount = 0;   // Count of sprays made

void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite);
void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock);
void movePlayer(float& player_x);
void drawBullet(RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite);
void drawSprayCans(RenderWindow& window, Sprite& sprayCanSprite);
void spawnBee();
void moveBees(RenderWindow& window, Font& font);
void drawBees(RenderWindow& window, Sprite& beeSprite, Sprite& fastbeeSprite);
void handleBeeBulletCollision(RenderWindow& window, float bullet_x, float bullet_y, Sprite& honeycombSprite, Music& hit);
void drawHoneycomb(RenderWindow& window, Sprite& honeycombSprite, Sprite& redhoneycombSprite);
void moveBird();
void renderBird(RenderWindow& window);
void handleBulletBirdCollision(float bullet_x, float bullet_y);
void updateFlowersForBee(RenderWindow& window, Font& font, int beeIndex);
void renderFlowers(RenderWindow& window);
void handleHoneyCombBulletCollision(RenderWindow& window, float bullet_x, float bullet_y, Music& hit);
void drawBar(RectangleShape& bar, RenderWindow& window, float& barX, float& barY, int steps, float& player_x, float& player_y, int& goDown);
void drawHives(RenderWindow& window, Sprite& hiveSprite);

// Worker bee variables
const int maxBees = 9; // Maximum number of bees
const int maxFastBees = 3; // Maximum number of bees

bool beeAlive[maxBees + maxFastBees] = {}; // Track if a bee is alive
float beeX[maxBees + maxFastBees], beeY[maxBees + maxFastBees]; // Track the position of each bee
float hcx[maxBees * 3 + maxFastBees * 3] = {};
float hcy[maxBees * 3 + maxFastBees * 3] = {};
bool hc[maxBees * 3 + maxFastBees * 3] = {};
bool beeDirection[maxBees + maxFastBees]; // Track the direction of each bee (true = right, false = left)
Clock beeClock[maxBees + maxFastBees]; // Timer for each bee's movement
Clock spawnClock; // Timer to spawn bees
int beeSpeed = 15; // Initial bee speed
int FastbeeSpeed = 25;
int beeSpawnInterval = 3; // Interval between bee spawns in seconds
int score = 0;
int level = 1;
float player_x = resolutionX / 2 - boxPixelsX / 2; // Centered horizontally
float player_y = resolutionY - (3 * boxPixelsY) - 65; // Near bottom of screen
float hiveX[maxBees * 3 + maxFastBees * 3], hiveY[maxBees * 3 + maxFastBees * 3];
// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};






void showGameOverScreen(sf::RenderWindow& window, sf::Font& font) {
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over\nPress Enter to Exit");
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(200.0f, 250.0f);

    bool exitGame = false;

    while (window.isOpen() && !exitGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                exitGame = true;
            }
        }

        window.clear(sf::Color::Black);
        window.draw(gameOverText);
        window.display();
    }

    window.close();
}

Sprite birdSprite;             // Sprite for the bird
Texture birdTexture;           // Texture for the bird
float birdX, birdY;            // Position of the bird (x, y)
int birdIndex = -1;            // Index of the bird's target (corresponding to hcx and hcy)
Clock birdTimer;               // Timer to keep track of the bird staying at coordinates
float birdSpeed = 0.1;             // Speed at which the bird moves towards its target
int birdHitCount = 0;          // To track number of times the bird is hit by bullets
int beeRespawnCount = 0;
int FastbeeRespawnCount = 0;
bool birdVisible = false;
bool isAtTarget = false;
Clock collisionCheckClock;
bool bullet_exists = false;
float flowerX[17]; // Store X positions for each flower
Sprite flowerSprite; // Flower sprite
Texture flowerTexture; // Flower texture
bool game = false;
/////////////////////////// LEVEL 4 ///////////////////////////////////////////////////////
// Power-up variables
const int maxPowerUps = 4; // Maximum number of power-ups on screen
bool powerUpActive[maxPowerUps] = {}; // Tracks whether each power-up is active
float powerUpX[maxPowerUps];          // X position of the power-up
float powerUpY[maxPowerUps];          // Y position of the power-up
int powerUpType[maxPowerUps];         // Type of power-up (0: Height Increase, 1: Height Decrease, 2: Speed Increase, 3: Speed Decrease)
Texture powerUpTextures[maxPowerUps]; // Textures for each power-up
Sprite powerUpSprites[maxPowerUps];   // Sprites for each power-up
float powerUpSpeed = 0.5;             // Speed of falling power-ups
float player_speed = 0.5;
int stepsRemaining = 7;
int bullet_count = 8;
int goDown = 0;
string userName = "";
const int maxHighscores = 10;
bool isSpeedPowerUpActive = false;
float speedTimer = 0.0f;    // Timer for speed power-up effects

std::clock_t powerUpStartTime;


//////////// Infant Bee
float* infantBeeX;     // X positions of Infant Bees
float* infantBeeY;     // Y positions of Infant Bees
float* infantBeeSpeed; // Speeds of Infant Bees
bool* infantBeeActive; // Whether each Infant Bee is active
bool* infantBeeMatured; // Whether each Infant Bee has matured
bool* infantBeeDirection; // Direction (true = right, false = left)

int maxInfantBees = 5; // Maximum number of Infant Bees
int activeInfantBees = 0; // Number of currently active Infant Bees

void checkFlowerPlayerDetection(RenderWindow& window, Font& font) {
    for (int i = 0; i < 17; i++) {
        if (player_x >= flowerX[i] && player_x <= flowerX[i] + 55) {
            for (int j = 0; j < 17; j++) {
                flowerX[j] = -100;
            }
            sprayCansRemaining[activeSprayCan] = 0;
            activeSprayCan++;
            if (activeSprayCan > 2) {
                showGameOverScreen(window,font);
            }
        }
    }
}

void updatePowerUpEffect(float& playerSpeed, RenderWindow& window, Font& font) {
    if (isSpeedPowerUpActive) {
        double elapsed = (std::clock() - powerUpStartTime) / (double)CLOCKS_PER_SEC;

        if (elapsed >= 5.0) { // 5 seconds have passed
            playerSpeed = 0.5; // Revert to original speed
            isSpeedPowerUpActive = false;
            cout << "Power-up effect ended\n";
        }
        else {
            Text time;
            time.setFillColor(Color::White);
            time.setFont(font);
            time.setString("Bonus: " + to_string(elapsed)); // Set the text content
            time.setCharacterSize(24);
            time.setPosition(450.0f, 550.0f);
            window.draw(time);
        }
    }
}
void handlePowerUpCollection(float player_x, float player_y, float& playerSpeed, Sprite& playerSprite) {
    for (int i = 0; i < maxPowerUps; ++i) {
        if (powerUpActive[i] &&
            player_x + 32 >= powerUpX[i] - 16 && player_x <= powerUpX[i] + 48 &&
            player_y + 32 >= powerUpY[i] - 16 && player_y <= powerUpY[i] + 48) {

            powerUpActive[i] = false; // Deactivate the power-up

            switch (powerUpType[i]) {
            case 0: // Height Increase
                if (sprayLeft + 8 > 56) {
                    sprayLeft = 56;
                }
                else {
                    sprayLeft += 8;
                }
                stepsRemaining = 7;
                bullet_count = 8;
                goDown = 0;
                break;
            case 1: // Height Decrease
                if (sprayLeft - 8 < 0) {
                    sprayLeft = 0;
                }
                else {
                    sprayLeft -= 8;
                }
                stepsRemaining = 7;
                bullet_count = 8;
                goDown += 8;
                break;
            case 2: // Speed Increase
                playerSpeed += 0.5f; // Adjust as necessary
                isSpeedPowerUpActive = true;
                powerUpStartTime = std::clock(); // Record start time
                cout << "Speed increased\n";
                break;
            case 3: // Speed Decrease
                playerSpeed = max(0.1f, playerSpeed - 0.5f); // Prevent speed from going negative
                isSpeedPowerUpActive = true;
                powerUpStartTime = std::clock(); // Record start time
                cout << "Speed decreased\n";
                break;
            }
        }
    }
}

void drawPowerUps(RenderWindow& window) {
    for (int i = 0; i < maxPowerUps; ++i) {
        if (powerUpActive[i]) {
            powerUpSprites[powerUpType[i]].setPosition(powerUpX[i], powerUpY[i]);
            window.draw(powerUpSprites[powerUpType[i]]);
        }
    }
}
void movePowerUps() {
    for (int i = 0; i < maxPowerUps; ++i) {
        if (powerUpActive[i]) {
            powerUpY[i] += powerUpSpeed; // Move the power-up down

            // Deactivate the power-up if it goes off-screen
            if (powerUpY[i] > resolutionY) {
                powerUpActive[i] = false;
            }
        }
    }
}

void readHighscores(const std::string& filename, std::string names[], int scores[], int& count) {
    std::ifstream file(filename);
    count = 0;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line) && count < maxHighscores) {
            std::size_t commaPos = line.find(',');
            if (commaPos != std::string::npos) {
                names[count] = line.substr(0, commaPos);
                scores[count] = std::stoi(line.substr(commaPos + 1));
                count++;
            }
        }
        file.close();
    }
    else {
        std::cerr << "Failed to open " << filename << std::endl;
    }
}


// Function to write updated highscores to the file
void writeHighscores(const std::string& filename, std::string names[], int scores[], int count) {
    // Sort the highscores in descending order of scores
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j] > scores[j + 1]) { // Compare scores
                // Swap scores
                int tempScore = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = tempScore;

                // Swap corresponding names
                std::string tempName = names[j];
                names[j] = names[j + 1];
                names[j + 1] = tempName;
            }
        }
    }

    // Write sorted highscores to the file
    std::ofstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < count; i++) {
            file << names[i] << "," << scores[i] << "\n";
        }
        file.close();
    }
    else {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
    }
}
    

// Function to update highscores if the current score is greater than any in the list
void updateHighscores(std::string names[], int scores[], int& count, const std::string& playerName, int currentScore) {
    for (int i = 0; i < count; i++) {
        if (currentScore > scores[i]) {
            names[i] = playerName; // Replace the name
            scores[i] = currentScore; // Replace the score
            return;
        }
    }

    // If there is room for a new entry and no lower score is found, add it
    if (count < maxHighscores) {
        names[count] = playerName;
        scores[count] = currentScore;
        count++;
    }
}


void showHighscores(sf::RenderWindow& window, sf::Font& font, std::string names[], int scores[], int count) {
    bool returnToMenu = false;

    sf::Text title;
    title.setFont(font);
    title.setString("Highscores");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(300.0f, 50.0f);

    sf::Text scoreTexts[maxHighscores];
    for (int i = 0; i < count; i++) {
        std::ostringstream oss;
        oss << names[i] << ": " << scores[i];

        scoreTexts[i].setFont(font);
        scoreTexts[i].setString(oss.str());
        scoreTexts[i].setCharacterSize(24);
        scoreTexts[i].setFillColor(sf::Color::White);
        scoreTexts[i].setPosition(300.0f, 120.0f + i * 30.0f); // Adjust vertical spacing
    }

    sf::Text backText;
    backText.setFont(font);
    backText.setString("Press B to return to the menu");
    backText.setCharacterSize(20);
    backText.setFillColor(sf::Color::Green);
    backText.setPosition(250.0f, 500.0f);

    while (window.isOpen() && !returnToMenu) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
                returnToMenu = true;
            }
        }

        window.clear();
        window.draw(title);
        for (int i = 0; i < count; i++) {
            window.draw(scoreTexts[i]);
        }
        window.draw(backText);
        window.display();
    }
}

//infant bee
void spawnInfantBee() {
    // Check if there's room for another Infant Bee
    if (activeInfantBees >= maxInfantBees) return;

    // Find the first inactive slot
    for (int i = 0; i < maxInfantBees; ++i) {
        if (!infantBeeActive[i]) {
            // Initialize the Infant Bee
            infantBeeX[i] = hiveX[i];  // Random X position within screen width
            infantBeeY[i] = hiveY[i];         // Start at the top
            infantBeeActive[i] = true;
            infantBeeMatured[i] = false;
            infantBeeDirection[i] = false; // Default direction (left)

            // Increment the count of active Infant Bees
            activeInfantBees++;
            break;
        }
    }
}void moveInfantBees() {
    for (int i = 0; i < maxInfantBees; ++i) {
        if (infantBeeActive[i]) {
            bool obstacleAbove = false;
            bool obstacleLeft = false;
            bool obstacleRight = false;

            // Check if there is an obstacle above (e.g., a honeycomb or other bees)
            for (int j = 0; j < maxBees * 3 + maxFastBees * 3; ++j) {
                if (hcy[j] == infantBeeY[i] - 32 && abs(hcx[j] - infantBeeX[i]) < 32) {
                    obstacleAbove = true;
                    break;
                }
            }

            // Check if there is an obstacle to the left
            for (int j = 0; j < maxBees * 3 + maxFastBees * 3; ++j) {
                if (hcx[j] == infantBeeX[i] - 32 && hcy[j] >= infantBeeY[i] - 32 && hcy[j] <= infantBeeY[i]) {
                    obstacleLeft = true;
                    break;
                }
            }

            // Check if there is an obstacle to the right
            for (int j = 0; j < maxBees * 3 + maxFastBees * 3; ++j) {
                if (hcx[j] == infantBeeX[i] + 32 && hcy[j] >= infantBeeY[i] - 32 && hcy[j] <= infantBeeY[i]) {
                    obstacleRight = true;
                    break;
                }
            }

            // If the Infant Bee is blocked in all directions, turn it into a Bee Hive
            if (!obstacleAbove && obstacleLeft && obstacleRight) {
                // Transform into a Bee Hive at the current position
                for (int j = 0; j < maxBees * 3 + maxFastBees * 3; ++j) {
                    if (hiveX[j] == 0 && (hiveY[j] == 0 || hiveY[j] == -100)) {  // Find an empty spot for a new hive
                        hiveX[j] = infantBeeX[i];
                        hiveY[j] = infantBeeY[i];
                        break;
                    }
                }

                // Deactivate the Infant Bee and reset its state
                infantBeeActive[i] = false;
                activeInfantBees--; // Decrease the active count of Infant Bees
                cout << "Infant Bee is stuck and turned into a Bee Hive!" << endl;
            }

            // If not blocked above, continue moving upwards or left/right
            else if (!obstacleAbove) {
                // Move upward if no obstacle
                infantBeeY[i] -= infantBeeSpeed[i];

                // Check if the Infant Bee has reached the top (y <= 0)
                if (infantBeeY[i] <= 0) {
                    // Mark the Infant Bee as matured
                    infantBeeMatured[i] = true;
                    infantBeeActive[i] = false;   // Deactivate the Infant Bee
                    activeInfantBees--;           // Decrease the active Infant Bees count

                    // Now, treat this bee as a regular Hunter Bee
                    for (int j = 0; j < maxBees + maxFastBees; ++j) {
                        if (!beeAlive[j]) { // Find the first empty slot for a regular bee
                            beeAlive[j] = true;
                            beeX[j] = infantBeeX[i];
                            beeY[j] = 0;  // Start at the top
                            beeDirection[j] = true; // Default direction (right)
                            break;
                        }
                    }

                    cout << "Infant Bee has matured into a Hunter Bee!" << endl;
                }
            }
            else {
                // Move left or right if blocked above
                if (infantBeeDirection[i]) {
                    infantBeeX[i] += infantBeeSpeed[i];
                    if (infantBeeX[i] >= 960 - 32) infantBeeDirection[i] = false; // Boundary check
                }
                else {
                    infantBeeX[i] -= infantBeeSpeed[i];
                    if (infantBeeX[i] <= 0) infantBeeDirection[i] = true; // Boundary check
                }
            }
        }
    }
}



void renderInfantBees(RenderWindow& window, Sprite& infantBeeSprite) {
    for (int i = 0; i < maxInfantBees; ++i) {
        if (infantBeeActive[i]) {
            infantBeeSprite.setPosition(infantBeeX[i], infantBeeY[i]);
            window.draw(infantBeeSprite);
        }
    }
}
///////////////////////////////// LEVEL 4 ///////////////////////////////////////////////


int main()
{

    srand(time(0));

    // Declaring RenderWindow.
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Buzz Bombers", Style::Close | Style::Titlebar);
    window.setPosition(Vector2i(500, 200));
    sf::Font font;

    // Load a font
    if (!font.loadFromFile("Fonts/Poppins.ttf")) { // Replace with a valid font path
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(490.0f, 300.0f);

    Text promptText;
    promptText.setFont(font);
    promptText.setString("Enter your name:");
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::Yellow);
    promptText.setPosition(270.0f, 300.0f);

    bool nameEntered = false;

    while (window.isOpen() && !nameEntered) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !userName.empty()) { // Handle backspace
                    userName.pop_back();
                }
                else if (event.text.unicode == '\r') { // Handle Enter key
                    if (!userName.empty())
                        nameEntered = true;
                }
                else if (event.text.unicode < 128 && event.text.unicode != '\b') { // Add valid characters
                    userName += static_cast<char>(event.text.unicode);
                }
            }
        }

        inputText.setString(userName);

        window.clear();
        window.draw(promptText);
        window.draw(inputText);
        window.display();
    }

    while (game != true) {
        
        // Define button properties
        string names[maxHighscores];
        int scores[maxHighscores];
        int scoreCount = 0;
        readHighscores("highscores.txt", names, scores, scoreCount);
        const int buttonCount = 4;
        sf::RectangleShape buttons[buttonCount];
        sf::Text buttonTexts[buttonCount];
        std::string labels[] = { "Start Game", "Boss Level", "Highscores", "Exit Game" };

        float buttonWidth = 300.0f;
        float buttonHeight = 50.0f;
        float startX = (800.0f - buttonWidth/2) / 2.0f; // Center buttons horizontally
        float startY = 200.0f;
        float spacing = 20.0f;

        // Initialize buttons
        for (int i = 0; i < buttonCount; i++) {
            buttons[i].setSize(sf::Vector2f(buttonWidth, buttonHeight));
            buttons[i].setPosition(startX, startY + i * (buttonHeight + spacing));
            buttons[i].setFillColor(sf::Color::Blue);

            buttonTexts[i].setFont(font);
            buttonTexts[i].setString(labels[i]);
            buttonTexts[i].setCharacterSize(24);
            buttonTexts[i].setFillColor(sf::Color::White);

            // Center text within the button
            sf::FloatRect textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition(
                buttons[i].getPosition().x + (buttonWidth - textBounds.width) / 2.0f,
                buttons[i].getPosition().y + (buttonHeight - textBounds.height) / 2.0f - textBounds.top
            );
        }

        while (window.isOpen() && game == false) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                    // Check which button was clicked
                    for (int i = 0; i < buttonCount; i++) {
                        if (buttons[i].getGlobalBounds().contains(mousePos)) {
                            if (i == 0) {
                                cout << "Start Game clicked!" << std::endl;
                                game = true;
                                
                            }
                            else if (i == 1) {
                                cout << "Boss Level clicked!" << std::endl;
                                level = 4;
                                game = true;
                               
                            }
                            else if (i == 2) {
                                cout << "Highscores clicked!" << std::endl;
                                showHighscores(window, font, names, scores, scoreCount);
                            }
                            else if (i == 3) {
                                std::cout << "Exit Game clicked!" << std::endl;
                                
                            }
                        }
                    }
                }
            }

            window.clear();

            // Draw buttons
            for (int i = 0; i < buttonCount; i++) {
                window.draw(buttons[i]);
                window.draw(buttonTexts[i]);
            }

            window.display();
        }
        game = true;
    }
    // Initializing Background Music.
    Music bgMusic;
    if (!bgMusic.openFromFile("Music/Music3.ogg")) {
        cout << "Error: Could not load music file!" << endl;
    }
    Music hit;
    if (!hit.openFromFile("Sound Effects/pop.wav")) {
        cout << "Error: Could not load music file!" << endl;
    }
    hit.setVolume(100);
    bgMusic.setVolume(5);
    bgMusic.setLoop(true);
    bgMusic.play();
    // Initializing Player and Player Sprites.

  
        //////////// phase 1 variables //////////////

    Texture playerTexture;
    Sprite playerSprite;
    playerTexture.loadFromFile("Textures/spray.png");
    playerSprite.setTexture(playerTexture);
    playerSprite.setScale(2, 2); // Scaling player for better visibility

    // Initializing Bullet and Bullet Sprites
    float bullet_x = player_x;
    float bullet_y = player_y;

    Clock bulletClock;
    Texture bulletTexture;
    Sprite bulletSprite;
    bulletTexture.loadFromFile("Textures/bullet.png");
    bulletSprite.setTexture(bulletTexture);
    bulletSprite.setScale(3, 3);

    // Initializing Spray Can Sprite
    Texture sprayCanTexture;
    Sprite sprayCanSprite;
    sprayCanTexture.loadFromFile("Textures/spray.png");
    sprayCanSprite.setTexture(sprayCanTexture);
    sprayCanSprite.setScale(.5, .5); // Scaling spray can for better visibility

    // Initializing Bee Sprite
    Texture beeTexture;
    Sprite beeSprite;
    beeTexture.loadFromFile("Textures/Regular_bee.png");
    beeSprite.setTexture(beeTexture);
    beeSprite.setScale(1, 1); // Scaling bee for better visibility

    Texture FastbeeTexture;
    Sprite FastbeeSprite;
    FastbeeTexture.loadFromFile("Textures/Fast_bee.png");
    FastbeeSprite.setTexture(FastbeeTexture);
    FastbeeSprite.setScale(1, 1); // Scaling bee for better visibility

    Texture honeycombTexture;
    Sprite honeycombSprite;
    honeycombTexture.loadFromFile("Textures/honeycomb.png");
    honeycombSprite.setTexture(honeycombTexture);
    honeycombSprite.setScale(0.75, 0.75); // Scaling player for better visibility

    Texture RedhoneycombTexture;
    Sprite RedhoneycombSprite;
    RedhoneycombTexture.loadFromFile("Textures/honeycomb_red.png");
    RedhoneycombSprite.setTexture(RedhoneycombTexture);
    RedhoneycombSprite.setScale(0.75, 0.75); // Scaling player for better visibility

    Texture HiveTexture;
    Sprite HiveSprite;
    HiveTexture.loadFromFile("Textures/hive.png");
    HiveSprite.setTexture(HiveTexture);
    HiveSprite.setScale(0.75, 0.75); // Scaling player for better visibility

    //infant bee
    Clock infantBeeSpawnClock;
    Texture infantBeeTexture;
    Sprite infantBeeSprite;
    infantBeeTexture.loadFromFile("Sprites/infantBeeSprite.png");
    infantBeeSprite.setTexture(infantBeeTexture);
    infantBeeSprite.setScale(1, 1);

    // Create a text element
    sf::Text scoreText;
    sf::Text spraysText;
    sf::Text levelText;
   

    //////////////////////////////Level 4 ///////////////////////////////////////////

    if (!powerUpTextures[0].loadFromFile("Sprites/Height_Inc.png") ||
        !powerUpTextures[1].loadFromFile("Sprites/Height_Dec.png") ||
        !powerUpTextures[2].loadFromFile("Sprites/Speed_Inc.png") ||
        !powerUpTextures[3].loadFromFile("Sprites/Speed_Dec.png")) {
        cout << "Error: Could not load power-up textures!" << endl;
    }

    // Set up sprites
    for (int i = 0; i < maxPowerUps; ++i) {
        powerUpSprites[i].setTexture(powerUpTextures[i]);
        powerUpSprites[i].setScale(1.5, 1.5); // Adjust as necessary for visibility
    }

    /////////////////////////////Level 4 ////////////////////////////////////////////////


    // Load flower texture
    if (!flowerTexture.loadFromFile("Textures/obstacles.png")) {
        cout << "Error: Could not load flower texture!" << endl;
    }
    flowerSprite.setTexture(flowerTexture);
    flowerSprite.setScale(2, 2); // Set scale to 2x

    if (!birdTexture.loadFromFile("Textures/flying.png")) {
        cout << "Error: Could not load bird texture!" << endl;
    }
    birdSprite.setTexture(birdTexture);
    birdSprite.setScale(2, 2); // Scale the bird sprite for better visibility

   

    if (!birdTexture.loadFromFile("Textures/flying.png")) {
        cout << "Error: Could not load bird texture!" << endl;
    }
    birdSprite.setTexture(birdTexture);
    birdSprite.setScale(2, 2); // Scale the bird sprite for better visibility

    //infant bee
    // Allocate memory for Infant Bee properties
    infantBeeX = new float[maxInfantBees];
    infantBeeY = new float[maxInfantBees];
    infantBeeSpeed = new float[maxInfantBees];
    infantBeeActive = new bool[maxInfantBees];
    infantBeeMatured = new bool[maxInfantBees];
    infantBeeDirection = new bool[maxInfantBees];

    // Initialize arrays
    for (int i = 0; i < maxInfantBees; ++i) {
        infantBeeX[i] = 0.0f;
        infantBeeY[i] = 0.0f;
        infantBeeSpeed[i] = 0.1f; // Default speed
        infantBeeActive[i] = false;
        infantBeeMatured[i] = false;
        infantBeeDirection[i] = true; // Default direction
    }

    // The ground on which player moves
    RectangleShape groundRectangle(Vector2f(resolutionX, boxPixelsY));
    groundRectangle.setPosition(0, resolutionY - boxPixelsY);
    groundRectangle.setFillColor(Color::Green);

    float barX = 40.0;
    float barY = 59.5;
    
    RectangleShape bar(Vector2f(barX, barY));

    bool spacePressed = false;
    int beeSpawnTimer = 0; // Timer to spawn new bees
    int FastbeeSpawnTimer = 0; // Timer to spawn new bees
    for (int i = 0; i < maxBees * 3 + maxFastBees * 3; ++i) {
        hcy[i] = -100;
    }

    for (int i = 0; i < maxBees * 3 + maxFastBees * 3; ++i) {
        hc[i] = false;
    }

    for (int i = 0; i < maxBees * 3 + maxFastBees * 3; ++i) {
        hiveY[i] = -100;
    }
    hiveX[0] = 400;
    hiveY[0] = 400;

    for (int i = 0; i < 17; ++i) {
        flowerX[i] = -100;
    }

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                return 0;
            }
        }
        //infant bee
        if (infantBeeSpawnClock.getElapsedTime().asSeconds() >= 3) {
            spawnInfantBee();
            infantBeeSpawnClock.restart();
        }

        // Update player position
        movePlayer(player_x);

        // Bullet handling
        if (bullet_exists) {
            moveBullet(bullet_y, bullet_exists, bulletClock);
        }
        else {
            bullet_x = player_x + 40; // Adjust for bullet position relative to player
            bullet_y = player_y;
        }

        // Handle space bar press
        if (Keyboard::isKeyPressed(Keyboard::Space) && !spacePressed && sprayCansRemaining[activeSprayCan] > 0 && !bullet_exists) {
            spacePressed = true;  // Prevent continuous firing while holding space
            bullet_count--;
            sprayLeft--;
            if (bullet_count == 0) {
                stepsRemaining--;
               
                bullet_count = 8;
                goDown += 8;
            }
            // Fire a bullet and reduce spray in the active can
            bullet_exists = true;
            // Reduce spray in active can after firing
            if (sprayCansRemaining[activeSprayCan] > 0) {
                sprayCansRemaining[activeSprayCan] -= spraysPerCan; // Decrease sprays in the active can
                if (sprayCansRemaining[activeSprayCan] <= 0) {
                    bool foundNextCan = false;
                    cout << "Hi" << endl;
                    sprayCanActive[activeSprayCan] = false;
                    // Move to the next can
                    for (int i = activeSprayCan + 1; i < maxSprayCans; ++i) {
                        if (!sprayCanActive[i]) {
                            stepsRemaining = 7;
                            goDown = 0;
                            sprayCanActive[i] = true;
                            activeSprayCan = i; // Update the active can
                            sprayLeft = 56;
                            stepsRemaining = 7;
                            foundNextCan = true;
                            break;
                        }
                    }
                    if (!foundNextCan || activeSprayCan > 2) {

                        // Read highscores
                        std::string names[maxHighscores];
                        int scores[maxHighscores];
                        int scoreCount = 0;
                        readHighscores("highscores.txt", names, scores, scoreCount);

                        // Update highscores
                        updateHighscores(names, scores, scoreCount,userName, score);

                        // Write updated highscores to the file
                        writeHighscores("highscores.txt", names, scores, scoreCount);

                        // Show "Game Over" screen
                        showGameOverScreen(window, font);
                        window.close();
                    }
                }
            }
        }
        else if (!Keyboard::isKeyPressed(Keyboard::Space)) {
            spacePressed = false; // Reset flag when space is released
        }
        // Bee spawn logic
        if (beeSpawnTimer == 0) {
            spawnBee();
            beeSpawnTimer = 100; // Reset timer
        }
        else {
            beeSpawnTimer--;
        }

      

        // Move and draw bees
        moveBees(window, font);
        //infant bee
        moveInfantBees();
        // Draw everything
        window.clear();
        drawPlayer(window, player_x, player_y, playerSprite);
        if (bullet_exists) {
            drawBullet(window, bullet_x, bullet_y, bulletSprite);
        }
       
        // Draw spray cans left
        drawBar(bar, window, barX, barY, stepsRemaining, player_x, player_y, goDown);
        window.draw(groundRectangle);
        drawSprayCans(window, sprayCanSprite);

        // Draw bees
        drawBees(window, beeSprite, FastbeeSprite);

        // Handle bee collision with bullet
        handleBeeBulletCollision(window,bullet_x, bullet_y, honeycombSprite, hit);



        drawHoneycomb(window, honeycombSprite,RedhoneycombSprite);
        handleHoneyCombBulletCollision(window, bullet_x, bullet_y, hit);

        renderBird(window);
        moveBird();
        renderFlowers(window);

       
        handleBulletBirdCollision(bullet_x, bullet_y);
        drawHives(window, HiveSprite);
        checkFlowerPlayerDetection(window, font);
        scoreText.setFont(font); // Set the font
        scoreText.setString("score: " + to_string(score)); // Set the text content
        scoreText.setCharacterSize(24); // Set text size
        scoreText.setFillColor(sf::Color::White); // Set text color
        scoreText.setPosition(240.0f, 610.0f); // Set position on the screen
        window.draw(scoreText);

        spraysText.setFont(font); // Set the font
        spraysText.setString("sprays left: " + to_string(sprayLeft)); // Set the text content
        spraysText.setCharacterSize(24); // Set text size
        spraysText.setFillColor(sf::Color::White); // Set text color
        spraysText.setPosition(700.0f, 610.0f); // Set position on the screen
        window.draw(spraysText);

        levelText.setFont(font); // Set the font
        levelText.setString("level: " + to_string(level)); // Set the text content
        levelText.setCharacterSize(24); // Set text size
        levelText.setFillColor(sf::Color::White); // Set text color
        levelText.setPosition(450.0f, 610.0f); // Set position on the screen
        window.draw(levelText);

        ///////////////////////////////////////// LEVEL 4 ////////////////////////////////////////////////////
        if (level == 4) {
        movePowerUps();
        drawPowerUps(window);
        handlePowerUpCollection(player_x, player_y, player_speed, playerSprite);
        updatePowerUpEffect(player_speed, window, font);
        }
        //infant bee
        renderInfantBees(window, infantBeeSprite);
        //player_speed = 0.5;
        
        ////////////////////////////////////// LEVEL 4 ///////////////////////////////////////////////////////
        window.display();

    }
}

void drawBar(RectangleShape& bar, RenderWindow& window, float& barX, float& barY, int steps, float& player_x, float& player_y, int& goDown) {
   
    bar.setSize(Vector2f(40, 8.5 * steps));
    bar.setPosition(player_x + 45, player_y + 65 + goDown);
    bar.setFillColor(Color::White);
    window.draw(bar);
}

void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite) {
    playerSprite.setPosition(player_x, player_y);
    window.draw(playerSprite);
}

void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock) {
    if (bulletClock.getElapsedTime().asMilliseconds() < 20)
        return;

    bulletClock.restart();
    bullet_y -= 20;
    if (bullet_y < -32)
        bullet_exists = false;
}

void drawBullet(sf::RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite) {
    bulletSprite.setPosition(bullet_x, bullet_y);
    window.draw(bulletSprite);
}

void movePlayer(float& player_x) {

    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        player_x -= player_speed; // Move left
        if (player_x < -15) {
            player_x = -15; // Prevent moving out of bounds
        }
        else {
            for (int i = 0; i < 17; i++) {
                if (player_x >= flowerX[i] && player_x <= flowerX[i] + 85) {
                    player_x += player_speed;
                }
            }
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        player_x += player_speed; // Move right
        if (player_x > 840) {
            player_x = 840; // Prevent moving out of bounds
        }
        else {
            for (int i = 0; i < 17; i++) {
                if (player_x >= flowerX[i] - 100 && player_x <= flowerX[i] + 100) {
                    player_x -= player_speed;
                }
            }
        }
    }

}
void drawSprayCans(RenderWindow& window, Sprite& sprayCanSprite) {
    int xOffset = 10; // Start position on the left side of the screen
    int yOffset = resolutionY - 33; // Place cans a little above the bottom
    for (int i = 0; i < maxSprayCans; ++i) {
        if (sprayCansRemaining[i] > 0) { // Only draw the can if it has sprays left
            sprayCanSprite.setPosition(xOffset + i * (boxPixelsX * 2 - 30), yOffset);
            window.draw(sprayCanSprite);
        }
    }
}

void spawnBee() {
    // Only spawn new bees if there is space for them
    for (int i = 0; i < maxBees + maxFastBees; ++i) {
        if (!beeAlive[i] && spawnClock.getElapsedTime().asSeconds() >= beeSpawnInterval) {
            beeAlive[i] = true;
            // Randomly set the bee's starting position
            beeX[i] = (rand() % 2 == 0) ? 0 : resolutionX; // Randomly choose left or right side
            beeY[i] = 0; // Start at the top
            beeDirection[i] = (beeX[i] == 0) ? true : false; // True for moving right, false for left
            beeClock[i].restart();
            spawnClock.restart(); // Reset the spawn timer
            break;
        }
    }
}

void moveBees(RenderWindow& window, Font& font) {
    //448
    for (int i = 0; i < maxBees + maxFastBees; ++i) {
        if (beeAlive[i]) {
            if (beeClock[i].getElapsedTime().asMilliseconds() > (20 / beeSpeed)) { // Increase speed of bees
                beeClock[i].restart();
                // Move bee in zigzag pattern
                if (beeDirection[i]) {
                    beeX[i] += 1; // Move right
                    if (beeX[i] >= resolutionX - 32) { // Hit the right side
                        beeDirection[i] = false; // Change direction to left
                        for (int j = 0; j < maxBees * 3 + maxFastBees * 3; ++j) {  // Loop through all possible targets
                            if (hcy[j] != -100) {  // Ensure there's a valid target
                                if (abs(beeX[i] - hcx[j]) < 32 && abs(beeY[i] +32 - hcy[j]) < 32) {
                                    // Collision with target found
                                    beeAlive[i] = false;
                                    hiveX[j] = beeX[i];
                                    hiveY[j] = beeY[i];
                                }
                            }
                        }
                        if (beeAlive[i]) {
                            beeY[i] += 32; // Move down
                        }

                    }
                }
                else {
                    beeX[i] -= 1; // Move left
                    if (beeX[i] <= 0) { // Hit the left side
                        beeDirection[i] = true; // Change direction to right
                        for (int j = 0; j < maxBees * 3 + maxFastBees * 3; ++j) {  // Loop through all possible targets
                            if (hcy[j] != -100) {  // Ensure there's a valid target
                                if (abs(beeX[i] - hcx[j]) < 32 && abs(beeY[i] + 32 - hcy[j]) < 32) {
                                    // Collision with target found
                                    beeAlive[i] = false;
                                    hiveX[j] = beeX[i];
                                    hiveY[j] = beeY[i];
                                }
                            }
                        }
                        if (beeAlive[i]) {
                            beeY[i] += 32; // Move down
                        }
                    }
                }
            }

            for (int j = 0; j < maxBees * 3 + maxFastBees * 3; ++j) {  // Loop through all possible targets
                if (hcy[j] != -100 && i%3!=0) {  // Ensure there's a valid target
                    if (abs(beeX[i] - hcx[j]) < 32 && abs(beeY[i] - hcy[j]) < 32) {
                        // Collision with target found
                        beeY[i] += 32; // Move 
                        beeDirection[i] = !beeDirection[i]; // Change direction
                        break;  // Exit the loop after the first collision
                    }
                }
            }

            // Check if beeY is greater than 447, reset position to (0, 0)
            if (beeY[i] > 448) {
                for (int j = 0; j < maxBees + maxFastBees; ++j) {
                    if (beeAlive[j]) {
                        updateFlowersForBee(window, font, j); // Update flowers based on the bee's position
                    }
                }
                beeX[i] = 0;
                beeY[i] = 0;
            }
        }
    }
}


void drawBees(RenderWindow& window, Sprite& beeSprite, Sprite& FastbeeSprite) {
    for (int i = 0; i < maxBees + maxFastBees; ++i) {
        if (beeAlive[i]) {
            if (i % 3 == 0 && i!=0 && level > 1) {
                FastbeeSprite.setPosition(beeX[i], beeY[i]);
                window.draw(FastbeeSprite);
            }
            else
            {
                beeSprite.setPosition(beeX[i], beeY[i]);
                window.draw(beeSprite);
            }
        }
    }
}

void handleBeeBulletCollision(RenderWindow& window, float bullet_x, float bullet_y, Sprite& honeycombSprite, Music& hit) {

    // Check collision with regular bees
    for (int i = 0; i < maxBees + maxFastBees; ++i) {
        if (beeAlive[i] && bullet_x >= beeX[i] - 30 && bullet_x <= beeX[i] + 30 && bullet_y >= beeY[i] - 15 && bullet_y <= beeY[i] + 15) {
            // If the bee is hit by the bullet, mark it as dead
            beeAlive[i] = false;
            hit.play();

            // Save the honeycomb position for this bee
            for (int j = 0; j < maxBees * 3 + maxFastBees * 3; j++) {
                if (hcx[j] == 0 && (hcy[j] == 0 || hcy[j] == -100)) {
                    if (i % 3 == 0 && i != 0 && level > 1) {
                        hc[j] = true;
                    }
                    hcx[j] = beeX[i];
                    hcy[j] = beeY[i];
                    break;
                }
            }

            // Optionally, handle score increase or other effects here
            score += 100; // Example of increasing score when a bee is hit
            beeRespawnCount += 1;
            cout << beeRespawnCount << endl;
            if (beeRespawnCount == 20) {
                level = 2;
                stepsRemaining = 7;
                bullet_count = 8;
                goDown = 0;
                sprayLeft = 56;
            }
            else if (beeRespawnCount == 40) {
                level = 3;
                stepsRemaining = 7;
                bullet_count = 8;
                goDown = 0;
                sprayLeft = 56;
            }
            else if (beeRespawnCount == 70) {
                level = 4;
                stepsRemaining = 7;
                bullet_count = 8;
                goDown = 0;
                sprayLeft = 56;
            }

            cout << "score: " << score << endl;
            bullet_exists = false; // Deactivate bullet after collision
        }
    }

    // Check collision with Infant Bees
    for (int i = 0; i < maxInfantBees; ++i) {
        if (infantBeeActive[i] && bullet_x >= infantBeeX[i] - 30 && bullet_x <= infantBeeX[i] + 30 && bullet_y >= infantBeeY[i] - 15 && bullet_y <= infantBeeY[i] + 15) {
            // If the Infant Bee is hit by the bullet
            if (!infantBeeMatured[i]) {
                // Penalty for hitting the Infant Bee (child form)
                score -= 500;  // Deduct score as punishment for hitting the Infant Bee
                hit.play();    // Play hit sound effect

                // Deactivate the Infant Bee
                infantBeeActive[i] = false;
                activeInfantBees--;  // Decrease the number of active Infant Bees

                cout << "Hit Infant Bee. Penalty applied." << endl;
            }
            else {
                // If the Infant Bee has matured, treat it like a regular bee
                score += 100;  // Reward for hitting a matured Infant Bee (now a Hunter Bee)
                hit.play();    // Play hit sound effect

                // Deactivate the matured Infant Bee
                infantBeeActive[i] = false;
                activeInfantBees--;  // Decrease the number of active Infant Bees

                cout << "Hit matured Infant Bee. Reward applied." << endl;
            }

            bullet_exists = false; // Deactivate bullet after collision
        }
    }
}

void handleHoneyCombBulletCollision(RenderWindow& window, float bullet_x, float bullet_y, Music& hit) {

    for (int i = 0; i < maxBees * 3 + maxFastBees * 3; ++i) {
        if (bullet_x >= hcx[i] - 30 && bullet_x <= hcx[i] + 30 && bullet_y >= hcy[i] - 15 && bullet_y <= hcy[i] + 15 && bullet_exists) {
            // If the bee is hit by the bullet, mark it as dead

            if (hc[i] == true)
            {
                for (int j = 0; j < maxPowerUps; ++j) {
                    if (!powerUpActive[j]) {
                        powerUpActive[j] = true;
                        powerUpX[j] = hcx[i]; // Position it at the honeycomb's location
                        powerUpY[j] = hcy[i];
                        powerUpType[j] = rand() % 4; // Randomly select a power-up type
                        break;
                    }
                }
            }
            hc[i] = false;
            hcx[i] = 0;
            hcy[i] = -100;
            bullet_exists = false;


            hit.play();
            // Save the honeycomb position for this 
        }

    }
}

void drawHoneycomb(RenderWindow& window, Sprite& honeycombSprite, Sprite& redhoneycombSprite) {
    for (int i = 0; i < maxBees * 3 + maxFastBees * 3; ++i) {
        if (hc[i] == true && level > 1) {
            redhoneycombSprite.setPosition(hcx[i], hcy[i]);
            window.draw(redhoneycombSprite);
        }
        else
        {
            honeycombSprite.setPosition(hcx[i], hcy[i]);
            window.draw(honeycombSprite);
        }
    }
}

void drawHives(RenderWindow& window, Sprite& hiveSprite) {
    for (int i = 0; i < maxBees * 3 + maxFastBees * 3; ++i) {
        hiveSprite.setPosition(hiveX[i], hiveY[i]);
        window.draw(hiveSprite);
    }
}

void renderBird(RenderWindow& window) {
    if (birdVisible) {
        birdSprite.setPosition(birdX, birdY);
        window.draw(birdSprite);
    }
}

void moveBird() {
    if (birdVisible) {
        // Check if the bird is at its target position (hcx, hcy)
        if (abs(birdX - hcx[birdIndex]) > 1 || abs(birdY - hcy[birdIndex]) > 1 && hcy[birdIndex]!=100) {
            // Calculate the difference in position
            float dx = hcx[birdIndex] - birdX;
            float dy = hcy[birdIndex] - birdY;
            float distance = sqrt(dx * dx + dy * dy);

            // Normalize the direction vector to get a unit direction
            dx /= distance;
            dy /= distance;

            // Move the bird towards the target by 'birdSpeed' distance
            birdX += dx * birdSpeed;
            birdY += dy * birdSpeed;
        }
        else {
            // Bird reached the target, start waiting for 2 seconds
            if (!isAtTarget) {
                birdTimer.restart();  // Start the timer to wait at the target
                isAtTarget = true;    // Mark that the bird has reached the target

            }

            // Bird has waited at the target for 2 seconds
            if (isAtTarget && birdTimer.getElapsedTime().asSeconds() > 5) {

                if (hcy[birdIndex] < 33 && hcy[birdIndex]!=100) {
                    score += 1000;
                }
                else if (hcy[birdIndex] < 129 && hcy[birdIndex] != 100) {
                    score += 800;
                }
                else if(hcy[birdIndex] != 100){
                    score += 500;
                }
                if (hc[birdIndex] == true && hcy[birdIndex] != 100) {
                    score += 1000;
                }
                hc[birdIndex] = false;
                hcx[birdIndex] = 0;  // Reset current target
                hcy[birdIndex] = -100;


                // Find the next target (next hcx, hcy)
                for (int i = 0; i < maxBees * 3 + maxFastBees * 3; i++) {
                    if (hcy[i] != -100) {
                        birdIndex = i;  // Set the target for the bird
                        break;
                    }
                }

                // Reset the target flag and timer for next movement
                isAtTarget = false;
                birdTimer.restart();  // Restart timer for the next target
            }
        }
    }

    else {
        // Check for the first target if bird is not visible
        for (int i = 0; i < maxBees * 3 + maxFastBees * 3; i++) {
            if (hcy[i] != -100) {
                birdIndex = i; // Set the target for the bird
                birdX = static_cast<float>(resolutionX) / 4;  // Start from the left side
                birdY = static_cast<float>(resolutionY) / 2;  // Start from the middle vertically
                birdVisible = true;        // Make bird visible
                birdTimer.restart();       // Start the timer for the target
                isAtTarget = false;        // Reset flag when looking for the next target
                break;
            }
        }
    }
}



void handleBirdHit() {
    if (birdHitCount >= 3) {
        birdVisible = false; // Hide bird
        birdHitCount = 0; // Reset hit count
        birdTimer.restart(); // Restart timer
    }
}

// Function to handle bullet collision with bird
void handleBulletBirdCollision(float bullet_x, float bullet_y) {
    if (birdVisible && bullet_exists && bullet_x >= birdX - 30 && bullet_x <= birdX + 30 && bullet_y >= birdY - 15 && bullet_y <= birdY + 15) {
        birdHitCount++; // Increment hit count
        bullet_exists = false;
        handleBirdHit(); // Check if bird should disappear
    }
}

void renderFlowers(RenderWindow& window) {
    for (int i = 0; i < 17; i++) {
        // Render the flower sprite at flowerX[i] and y = 550
        flowerSprite.setPosition(flowerX[i], 550);
        window.draw(flowerSprite); // Draw the flower
    }
}

void updateFlowersForBee(RenderWindow& window, Font& font, int beeIndex) {
    // Only update flowers if beeY[i] is greater than 448
    if (beeY[beeIndex] > 448) {
        // Map the bee's X position to the flowerX array
        int flowerIndex = static_cast<int>(beeX[beeIndex] / 55);

        if (flowerIndex == 0) {
            for (int i = 0; i < 17; i++) {
                if (flowerX[i] == -100) {
                    if (i == 0) {
                        flowerX[i] = 5;
                        flowerX[i + 1] = 55;
                        checkFlowerPlayerDetection(window, font);
                    }
                    else {
                    flowerX[i] = i * 55;
                    checkFlowerPlayerDetection(window, font);
                    }
                    break;
                }
            }
        }
        else if (flowerIndex == 16) {
            for (int i = 16; i >= 0; i--) {
                if (flowerX[i] == -100) {
                    if (i == 16) {
                        flowerX[i] = i * 55;
                        flowerX[i - 1] = (i - 1) * 55;
                        checkFlowerPlayerDetection(window, font);
                    }
                    else {
                        flowerX[i] = i * 55;
                        checkFlowerPlayerDetection(window, font);
                    }
                    break;
                }
            }
        }
    }
}
