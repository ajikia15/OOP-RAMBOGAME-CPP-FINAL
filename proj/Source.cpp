#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string> 

#define E_H 65             // ENTITY HEIGHT
#define E_W 55             // ENTITY WIDTH
#define EN_W 65
#define E_SP 300.0f        // ENTITY SPEED
#define BLT_SP 10.0f       // BULLET SPEED
#define E_VEL 1.0f         // ENTITY VELOCITY

#define P_HP 5             // PLAYER STARTING HP

#define EN_HP 1            // ENEMY STARTING HP 
#define EN_SPWN  600          // ENEMY SPAWN INTERVAL (in seconds)

#define STARTX 0
#define STARTY 0
#define MAX_X 1440
#define MAX_Y 860

#define FPS 120

#define MAINMENU 0
#define TITLE "RAMBO"
#define GAME 1
#define GUIDE 2
#define END 3
#define LOST 4

#define MENUTEXTSTART 300
#define MAINMENUTEXTCOUNT 3
#define GAMEOVERTEXTCOUNT 1
#define GUIDETEXTCOUNT 0
#define TEXTCOUNT MENUTEXTSTART+MAINMENUTEXTCOUNT+GAMEOVERTEXTCOUNT+GUIDETEXTCOUNT
class Entity : public sf::Sprite
{
protected:
    int m_health;
    int m_width;
    int m_height;
    float gravity;         // 
    sf::Vector2f velocity;  // Velocity of the entity
public:
    Entity(int health, int width, int height)
        : m_health(health), m_width(width), m_height(height), velocity(0.0f, 0.0f), gravity(0.5f) {}

    int getHealth() const { return m_health; }
    void setHealth(int health) { m_health = health; }

    int getWidth() const { return m_width; }
    void setWidth(int width) { m_width = width; }

    int getHeight() const { return m_height; }
    void setHeight(int height) { m_height = height; }

    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }

    /* for testing only */
    sf::RectangleShape m_rectangle;

    bool isColliding(const Entity& other) const {
        if (typeid(*this) == typeid(other)) {
            // Entities of the same type don't cause damage
            return false;
        }
        sf::FloatRect thisBounds = getGlobalBounds();
        sf::FloatRect otherBounds = other.getGlobalBounds();
        return thisBounds.intersects(otherBounds);
    }
    bool isEnemyColliding(const Entity& other) const {
        sf::FloatRect thisBounds = getGlobalBounds();
        sf::FloatRect otherBounds = other.getGlobalBounds();
        return thisBounds.intersects(otherBounds);
    }
    void update(float deltaTime) {
        sf::Vector2f position = getPosition();

        // Update the entity's position based on velocity
        position += velocity * deltaTime * E_SP;  // Adjust the multiplier to control movement speed

        // Check if the entity is within the window boundaries
        if (position.x < 0) {
            position.x = 0;
        }
        if (position.x + m_width > MAX_X) {
            position.x = MAX_X - m_width;
        }
        if (position.y < 0) {
            position.y = 0;
        }
        if (position.y + m_height > MAX_Y) {
            position.y = MAX_Y - m_height;
        }
        setPosition(position);
    }
};

class Bullet : public Entity
{
private:
    static sf::Texture bulletSheet;
    float playerScale;
public:
    Bullet(int x, int y, float scale)
        : Entity(1, 5, 10) {
        // Bullets have 1 HP and size 5x10
        setPosition(x, y + E_H / 3); // adding to y because the bullet spawns a bit higher than the player's gun
        playerScale = scale;
        if (!bulletSheet.loadFromFile("./player/weapon_bullet_level2.png")) {
            std::cout << "balls" << std::endl;
        }
        Bullet::setTexture(bulletSheet);
        setTextureRect(sf::IntRect(0, 0, 18, 16));
        setScale(E_W / 18.0f / 2, E_H / 16.0f / 2);
    }

    void update(float deltaTime) {
        sf::Vector2f position = getPosition();

        // Update the bullet's position based on the player's direction
        float movementDirection = playerScale < 0 ? -1.0f : 1.0f;  // negative/positive scale indicates which way the player is facing. 
        position.x += BLT_SP * movementDirection * deltaTime * E_SP;
        setPosition(position);
        if (position.x < STARTX || position.x > MAX_X || position.y < STARTY || position.y > MAX_Y) {
            // Remove the bullet    
            setHealth(0);
        }
        if (getHealth() <= 0) {
            // Remove the bullet
            return;
        }
    }
};
sf::Texture Bullet::bulletSheet;
class Player : public Entity
{
private:
    //..........................................................................................
    sf::Texture idleSpriteSheetTexture;
    sf::Texture moveSpriteSheetTexture;
    sf::Texture jumpSpriteSheetTexture;
    //..........................................................................................

    bool isJumping;        // Flag to track if the player is jumping
    float jumpVelocity;    // Velocity of the player during jumping
    int fireDelay;
    std::vector<Bullet>* bullets;


    //..........................................................................................
    int numFramesIdle;
    int numFramesMove;
    int numFramesJump;
    int currentFrame;
    int frameWidth;
    int frameHeight;
    float animationSpeed;
    sf::Clock animationClock;

    sf::Sound sound;
    sf::SoundBuffer fire_sound;
    //..........................................................................................

public:
    Player()
        : Entity(P_HP, E_W, E_H), isJumping(false), jumpVelocity(-10.0f), fireDelay(0),
        numFramesIdle(4), numFramesMove(8), currentFrame(0), frameWidth(26),
        frameHeight(22), animationSpeed(0.1f), numFramesJump(6)
    {
        //..........................................................................................
        if (!idleSpriteSheetTexture.loadFromFile("./player/john_idle.png"));
        if (!moveSpriteSheetTexture.loadFromFile("./player/john_run.png"));
        if (!jumpSpriteSheetTexture.loadFromFile("./player/john_jump.png"));
        //..........................................................................................
        if (!fire_sound.loadFromFile("./sounds/fire.ogg"));
        int tileSize = 22;
        bullets = new std::vector<Bullet>();

        // Set the initial texture and position
        setTexture(idleSpriteSheetTexture);
        setTextureRect(sf::IntRect(0, 0, frameWidth, tileSize));
        setPosition(MAX_X / 2, MAX_Y - E_H);
        setScale(E_W / 26.0f, E_H / 22.0f);
    }
    ~Player()
    {
        delete bullets;  // Deallocate the bullets vector in the destructor
    }
    void fire() {
        if (fireDelay >= 30) { // If enough time has passed since the last shot
            sound.setBuffer(fire_sound);
            // Calculate the bullet's initial position based on the player's direction
            sf::Vector2f bulletPosition;
            if (getScale().x > 0) {
                //std::cout << "facing right" << std::endl
                bulletPosition = sf::Vector2f(getPosition().x + getWidth(), getPosition().y);
            }
            else {
                //std::cout << "facing left" << std::endl;
                bulletPosition = sf::Vector2f(getPosition().x, getPosition().y);
            }

            // Fire a bullet from the calculated position
            bullets->push_back(Bullet(bulletPosition.x, bulletPosition.y, getScale().x));
            //std::cout << "firedelay" << std::endl;
            fireDelay = 0;
            sound.play();
        }
    }
    void clearBullets()
    {
        bullets->clear();
    }
    void respawn()
    {
        setHealth(P_HP);
    }
    void jump() {
        //..........................................................................................
        if (!isJumping)
        {
            velocity.y = jumpVelocity;
            isJumping = true;
            currentFrame = 0;  // Reset the animation frame for jump
            setTexture(jumpSpriteSheetTexture);  // Switch to jump sprite sheet
            animationClock.restart();  // Restart the animation clock for jump
        }
        //..........................................................................................
    }

    void handleInput() {
        //..........................................................................................
        velocity.x = 0.0f;
        if (velocity.x != 0.0f && currentFrame != 0) {
            currentFrame = 0;
            int frameX = currentFrame * frameWidth;
            setTextureRect(sf::IntRect(frameX, 0, frameWidth, frameHeight));
        }
        //..........................................................................................

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -E_VEL - 0.1f;
            setScale(-E_W / 26.0f, E_H / 22.0f);
            setOrigin(E_W / 2 - E_W / 26.0f, 0); // karoche marjvniv tu midiodi da marcxniv gauxvdevdi an piriqit ucnaurad iyo dzaan da bevri kombinacia vcade
            // sabolood aseti gamoiyureba yvelaze kargad rato ar vici ar sheexo

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = E_VEL - 0.1f;
            setScale(E_W / 26.0f, E_H / 22.0f);
            setOrigin(E_W / 26.0f - E_W / 4, 0); // esec

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            jump();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            fire();
        }
    }
    bool isDead()
    {
        return (getHealth() < 1);
    }

    void update(float deltaTime) {
        // Apply gravity to the player
        velocity.y += gravity;

        // Update the player's position based on velocity
        Entity::update(deltaTime);

        // Check if the player is jumping and reached the peak
        if (isJumping && velocity.y >= 0.0f) {
            velocity.y = 0.0f;
            isJumping = false;
        }
        fireDelay++; // Increment the fire delay

        // Update the bullets
        for (auto& bullet : *bullets) {
            bullet.update(deltaTime);
        }
        //..........................................................................................
        // Update the animation frame if enough time has passed
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            if (velocity.x != 0.0f)
            {
                currentFrame++;  // Move to the next frame
                if (currentFrame >= numFramesMove)
                {
                    currentFrame = 0;  // Reset to the first frame if the last frame is reached
                }
                setTexture(moveSpriteSheetTexture);  // Switch to move sprite sheet
            }
            else if (isJumping)
            {
                currentFrame++;  // Move to the next frame
                if (currentFrame >= numFramesJump)
                {
                    currentFrame = 0;  // Reset to the first frame if the last frame is reached
                }
                setTexture(jumpSpriteSheetTexture);  // Switch to jump sprite sheet
            }
            else
            {
                currentFrame++;  // Move to the next frame
                if (currentFrame >= numFramesIdle)
                {
                    currentFrame = 0;  // Reset to the first frame if the last frame is reached
                }
                setTexture(idleSpriteSheetTexture);  // Switch to idle sprite sheet
            }

            int frameX = currentFrame * frameWidth;
            setTextureRect(sf::IntRect(frameX, 0, frameWidth, frameHeight));

            animationClock.restart();  // Restart the animation clock
        }
        //..........................................................................................

    }
    std::vector<Bullet>& getBullets()
    {
        return *bullets;
    }

};
class Enemy : public Entity
{
private:
    sf::Vector2f lastValidPosition;
    static sf::Texture enemySheetIdle;
    static sf::Texture enemySheetMove;
    int numFrames = 4;              // Number of animation frames
    int currentFrame = 0;           // Current frame index
    int frameWidth = 170;           // Width of each frame in pixels
    int frameHeight = 96;           // Height of each frame in pixels
    float animationSpeed = 0.1f;    // Speed of the animation (adjust as needed)
    sf::Clock animationClock;       // Clock to track the time for animation frame update

public:
    Enemy(int health, int width, int height, const sf::Vector2f& spawnPosition)
        : Entity(health, width, height) {
        setPosition(spawnPosition);
        if (!enemySheetIdle.loadFromFile("./enemy_sheet_idle.png")) {
            std::cout << "Failed to load enemy idle texture!" << std::endl;
        }

        if (!enemySheetMove.loadFromFile("./enemy_sheet_move.png")) {
            std::cout << "Failed to load enemy move texture!" << std::endl;
        }
        // Define the coordinates and size of the desired part of the tile sheet
        int tileX = 0;  // X coordinate of the tile within the tile sheet
        int tileY = 0;  // Y coordinate of the tile within the tile sheet

        // Set the texture rectangle of the sprite to display the desired part of the tile sheet

        setTextureRect(sf::IntRect(tileX, tileY, 102, 102));

    }

    void update(float deltaTime, const std::vector<Enemy>& enemies, const sf::Vector2f& playerPosition) {
        // Store the current position as the last valid position
        lastValidPosition = getPosition();

        // Apply gravity to the enemy
        velocity.y += gravity;

        // Calculate the direction towards the player
        sf::Vector2f direction = playerPosition - getPosition();

        // Check for collisions with other enemies
        for (auto it = enemies.begin(); it != enemies.end(); ++it) {
            auto& otherEnemy = *it;
            if (&otherEnemy != this && isEnemyColliding(otherEnemy)) {
                // Collision occurred with another enemy, revert to last known valid position
                setPosition(lastValidPosition);
                break;
            }
        }

        // Adjust the horizontal velocity based on the direction towards the player
        if (direction.x > 0) {
            velocity.x = E_VEL;  // Move right
            setTexture(enemySheetMove);  // Set the moving animation texture
            setScale(1, 1);
        }
        else if (direction.x < 0) {
            velocity.x = -E_VEL; // Move left
            setTexture(enemySheetMove);  // Set the moving animation texture
            setScale(-1, 1);
        }
        else {
            velocity.x = 0.0f;  // Stop moving horizontally if player is in line
            setTexture(enemySheetIdle);  // Set the idle animation texture
        }

        // Update the animation frame if enough time has passed
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed) {
            currentFrame++;  // Move to the next frame
            if (currentFrame >= numFrames) {
                currentFrame = 0;  // Reset to the first frame if the last frame is reached
            }

            // Calculate the x-coordinate of the current frame in the sprite sheet
            int frameX = currentFrame * frameWidth;

            // Set the texture rectangle of the sprite to display the current frame
            setTextureRect(sf::IntRect(frameX, 0, frameWidth, frameHeight));

            animationClock.restart();  // Restart the animation clock
        }

        // Update the enemy's position based on velocity
        Entity::update(deltaTime);
    }
};
sf::Texture Enemy::enemySheetIdle;
sf::Texture Enemy::enemySheetMove;

class Map :public Entity
{
private:
    sf::Texture tileSheetTexture;
    sf::Texture backgroundTexture;
    sf::Texture backgroundTexture1;
    sf::Texture jungleTreeTexture;
    sf::Sprite sprite;
    sf::Sprite backgroundSprite;
    sf::Sprite backgroundSprite1;
    sf::Sprite jungleTreeSprite;
    int TileW;
    int TileH;
    std::vector<sf::Sprite> sprites;
    int TotalW;
    int TotalH;
    Player player;

public:
    Map() :
        TileW(26), TileH(22), TotalW(0), TotalH(0), Entity(1, 5, 10)
    {
    }
    void Initialize() {};

    void Load() {
        if (tileSheetTexture.loadFromFile("./tileset.png")) {
            TotalW = tileSheetTexture.getSize().x / TileW;
            TotalH = tileSheetTexture.getSize().y / TileH;

            sprite.setTexture(tileSheetTexture);
            sprite.setTextureRect(sf::IntRect(26, 0, TileW, TileH));

            sprite.setScale(sf::Vector2f(3, 3));
            sprite.setPosition(1, MAX_Y - 50);
        }

        if (backgroundTexture.loadFromFile("./background/background.jpg")) {

            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setTextureRect(sf::IntRect(0, 0, MAX_X, MAX_Y));
        }


        //load jungle trees 

        if (jungleTreeTexture.loadFromFile("./background/jungle_tree2.png")) {
            jungleTreeSprite.setTexture(jungleTreeTexture);
            jungleTreeSprite.setScale(3, 3);
        }
    }


    void Update(float deltatime, Player& player) {
        // Check for collision between player and sprites
        sf::Vector2f playerPosition = getPlayerPositionOnSprite(player);

        if (playerPosition.y != -1.f) {
            // Collision detected, update player's position
            player.setPosition(playerPosition);
        }
    }


    void addJungleTree(sf::RenderWindow& window, int x, int y) {
        sf::Vector2f treePosition1(x, y);  // Adjust the position as needed
        jungleTreeSprite.setPosition(treePosition1);
        jungleTreeSprite.setScale(5, 5);
        window.draw(jungleTreeSprite);
    }


    void Draw(sf::RenderWindow& window) {

        window.draw(backgroundSprite);
        window.draw(backgroundSprite1);





        int numSprites = 30; // Number of sprites to print
        int spacing = 50; // Spacing between each sprite
        int i = 0;
        int spacingBetweenP = 200;
        int spacingheight = 270;
        sprites.clear(); // Clear the vector before adding new sprites

        //loop for trees 
        do {
            if (i == 0 || i == 10 || i == 29)
                addJungleTree(window, i * spacing, sprite.getPosition().y - spacingheight);
            i++;
        } while (i < numSprites);


        i = 0;
        numSprites = MAX_X / 20;
        //lower ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            // Adjust the position of the current sprite
            int posX = sprite.getPosition().x + (i * spacing);
            int posY = sprite.getPosition().y;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);
            sprites.push_back(currentSprite);
            i++;

        } while (i < numSprites);
        i = 0;
        numSprites = 5;
        //top left ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            // Adjust the position of the current sprite
            int posX = sprite.getPosition().x + (i * spacing);
            int posY = MAX_Y / 2;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);
            sprites.push_back(currentSprite);
            i++;

        } while (i < numSprites);

        i = 0;
        numSprites = 3;
        //middle right ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            int posX = MAX_X;
            // Adjust the position of the current sprite
            posX = posX - (i * spacing);
            int posY = MAX_Y - spacingBetweenP - spacing;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);
            sprites.push_back(currentSprite);
            i++;
        } while (i < numSprites);

        i = 0;
        numSprites = 5;
        //top right ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            int posX = MAX_X;
            // Adjust the position of the current sprite
            posX = posX - (i * spacing);
            int posY = MAX_Y / 2 - spacingBetweenP;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);
            sprites.push_back(currentSprite);
            i++;
        } while (i < numSprites);
    }

    sf::Vector2f getPlayerPositionOnSprite(const Player& player) const {
        for (const sf::Sprite& currentSprite : sprites) {
            if (currentSprite.getGlobalBounds().intersects(player.getGlobalBounds())) {
                sf::FloatRect intersection;
                currentSprite.getGlobalBounds().intersects(player.getGlobalBounds(), intersection);
                float posY = intersection.top - player.getGlobalBounds().height;
                return sf::Vector2f(player.getPosition().x, posY);
            }
        }

        // If there is no collision, return an empty position
        return sf::Vector2f(-1.f, -1.f);
    }

    sf::Vector2f getEnemyPositionOnSprite(const Enemy& enemy) const {
        for (const sf::Sprite& currentSprite : sprites) {
            if (currentSprite.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                sf::FloatRect intersection;
                currentSprite.getGlobalBounds().intersects(enemy.getGlobalBounds(), intersection);
                float posY = intersection.top - enemy.getGlobalBounds().height;
                return sf::Vector2f(enemy.getPosition().x, posY);
            }
        }

        // If there is no collision, return an empty position
        return sf::Vector2f(-1.f, -1.f);
    }
};
class Game
{
private:
    sf::RenderWindow window;
    sf::Clock enemySpawnClock;
    sf::Time enemySpawnTimer;
    sf::Time enemySpawnInterval = sf::milliseconds(EN_SPWN);
    std::vector<Enemy> enemies;
    std::vector<sf::Vector2f> spawnPoints;
    int state;

    sf::Text menuOptions[TEXTCOUNT];
    sf::Text guide[7];
    sf::Text timeToDisplay;

    int selectedOption;
    sf::Font titleFont;
    sf::Font mainFont;

    sf::Music mainmenutheme;
    sf::SoundBuffer death_sound;
    sf::SoundBuffer game_over_sound;
    sf::SoundBuffer menuselect_sound;
    sf::SoundBuffer menuswitch_sound;
    sf::SoundBuffer damage_sound;
    sf::SoundBuffer hit_sound;

    sf::Texture heart;
    sf::Sprite heartSprite;
public:
    Game() : window(sf::VideoMode(MAX_X, MAX_Y), TITLE) {
        window.setFramerateLimit(FPS);

        state = MAINMENU;
        loadFiles();
        createText();

        // spawn points on the left side
        spawnPoints.push_back(sf::Vector2f(E_W, MAX_Y - E_H));   // bottom
        spawnPoints.push_back(sf::Vector2f(E_W * 2, MAX_Y / 2 - 100));

        //spawn point on the right sid

        spawnPoints.push_back(sf::Vector2f(MAX_X - E_W, MAX_Y - E_H)); // bottom
        spawnPoints.push_back(sf::Vector2f(MAX_X - E_W, MAX_Y - 350));
        spawnPoints.push_back(sf::Vector2f(MAX_X - E_W, MAX_Y / 2 - 300));


    }
    void loadFiles()
    {
        titleFont.loadFromFile("./fonts/28 Days Later.ttf");
        mainFont.loadFromFile("./fonts/PressStart2P-Regular.ttf");
        mainmenutheme.openFromFile("./sounds/mainmenutheme.ogg");
        heart.loadFromFile("./player/heart.png");
        death_sound.loadFromFile("./sounds/death.ogg");
        game_over_sound.loadFromFile("./sounds/game_over.ogg");
        menuselect_sound.loadFromFile("./sounds/menuselect.ogg");
        menuswitch_sound.loadFromFile("./sounds/menuswitch.ogg");
        damage_sound.loadFromFile("./sounds/damage.ogg");
        hit_sound.loadFromFile("./sounds/hit.ogg");
    }
    void drawGuideText()
    {
        sf::Text title(TITLE, titleFont, 100);
        title.setPosition((MAX_X - title.getGlobalBounds().width) / 2, MENUTEXTSTART - 100);
        for (int i = 0; i < 7; i++)
        {
            window.draw(guide[i]);
        }
    }
    void createText()
    {
        selectedOption = -1;
        menuOptions[0].setString("Play");
        menuOptions[1].setString("Guide");
        menuOptions[2].setString("Exit");
        menuOptions[3].setString("You've Lost! Press X");
        menuOptions[4].setString("Press X");
        guide[0].setString("Rambo's helicopter has crashed on a remote island!");
        guide[1].setString("Try to survive as long as you can!");
        guide[2].setString("Use Arrow Keys to move");
        guide[3].setString("Use Spacebar to jump");
        guide[4].setString("Click 'Z' to shoot");
        guide[5].setString("Don't stay too long on the platforms, it's slippery!");
        guide[6].setString("Press X");
        timeToDisplay.setFont(mainFont);
        timeToDisplay.setCharacterSize(50);
        timeToDisplay.setPosition(MAX_X - 100, 10);
        for (int i = 0; i < TEXTCOUNT; i++)
        {
            menuOptions[i].setFont(mainFont);
            menuOptions[i].setCharacterSize(50);
            menuOptions[i].setPosition((MAX_X - menuOptions[i].getGlobalBounds().width) / 2, 300 + i * 100);
        }
        for (int i = 0; i < 7; i++)
        {
            guide[i].setFont(mainFont);
            guide[i].setCharacterSize(30);
            guide[i].setPosition((MAX_X - guide[i].getGlobalBounds().width) / 2, 200 + i * 100);
        }
    }
    void drawMainMenuText() {
        sf::Text title(TITLE, titleFont, 100);
        title.setPosition((MAX_X - title.getGlobalBounds().width) / 2, MENUTEXTSTART - 150);
        window.draw(title);
        for (int i = 0; i < MAINMENUTEXTCOUNT; i++)
        {
            if (i == selectedOption)
            {
                menuOptions[i].setFillColor(sf::Color::Red);
            }
            else
            {
                menuOptions[i].setFillColor(sf::Color::White);
            }
            window.draw(menuOptions[i]);
        }
    }
    void drawGameOverText()
    {
        for (int i = MAINMENUTEXTCOUNT; i < MAINMENUTEXTCOUNT + GAMEOVERTEXTCOUNT; i++)
        {
            window.draw(menuOptions[i]);
        }
    }
    void setGameState(int s)
    {
        state = s;
    }
    int getGameState()
    {
        return state;
    }
    void drawHealth(int health)
    {
        heartSprite.setTexture(heart);
        heartSprite.setScale(1.5, 1.5);
        for (int i = 0; i < health; i++)
        {
            heartSprite.setPosition(12 + i * E_W ,5);
            window.draw(heartSprite);
        }
    }
    void drawTime(const sf::Clock& clock)
    {
        sf::Time elapsedTime = clock.getElapsedTime();
        std::string elapsedTimeString = std::to_string(elapsedTime.asSeconds());
        timeToDisplay.setString(elapsedTimeString);
        window.draw(timeToDisplay);
    }
    void run() {
        sf::Sound sound;
        Player player;
        Map map;
        map.Load();
        sf::Clock clock;
        sf::Clock time;
        mainmenutheme.play();
        while (window.isOpen() && getGameState() != END) {
            while (getGameState() == MAINMENU)
            {
                sf::Event event;
                //std::cout << selectedOption << std::endl;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                        sound.setBuffer(menuselect_sound);
                        selectedOption == -1 ? selectedOption = 0 : selectedOption != 2 ? selectedOption++ : 0; // prosta lamazi sintaqsebia tu ver gaige mitxari
                        sound.play();
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                        sound.setBuffer(menuselect_sound);
                        selectedOption == -1 ? selectedOption = 0 : selectedOption != 0 ? selectedOption-- : 2;  // kai araa lamazi mara asworebs
                        sound.play();
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && selectedOption > -1) {
                        sound.setBuffer(menuswitch_sound);
                        setGameState(selectedOption + 1);
                        sound.play();
                        window.clear();
                        break;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                        window.close();
                    }
                }

                window.clear();
                map.Draw(window);
                drawMainMenuText();
                window.display();
            }
            while (getGameState() == GAME)
            {
                mainmenutheme.stop();
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                    player.handleInput();
                }

                // Update game state here
                float deltaTime = clock.restart().asSeconds();
                player.update(deltaTime);
                map.Update(deltaTime, player);
                //-------------------------------------------------------------------------------for player to stand 
                sf::Vector2f playerPosition = map.getPlayerPositionOnSprite(player);
                if (playerPosition.x != -1.f && playerPosition.y != -1.f) {
                    // Set the player's position to the determined position on the sprite
                    player.setPosition(playerPosition);
                }
                //--------------------------------------------------------------------------------------------------
                // Update enemies
                for (auto it = enemies.begin(); it != enemies.end(); ) {
                    auto& enemy = *it;
                    enemy.update(deltaTime, enemies, player.getPosition());
                    sf::Vector2f enemyPosition = map.getEnemyPositionOnSprite(enemy);
                    if (enemyPosition.x != -1.f && enemyPosition.y != -1.f) {
                        enemy.setPosition(enemyPosition);
                    }
                    if (enemy.isColliding(player)) {
                        //std::cout << "gay" << std::endl;
                        sound.setBuffer(damage_sound);
                        player.setHealth(player.getHealth() - 1);
                        enemy.setHealth(enemy.getHealth() - 1);
                        sound.play();
                    }

                    for (auto bulletIt = player.getBullets().begin(); bulletIt != player.getBullets().end(); ) {
                        auto& bullet = *bulletIt;
                        if (bullet.isColliding(enemy)) {
                            //std::cout << "Collision with enemy" << std::endl;
                            sound.setBuffer(hit_sound);
                            bullet.setHealth(0);
                            enemy.setHealth(enemy.getHealth() - 1);
                            sound.play();
                        }

                        if (bullet.getHealth() <= 0) {
                            bulletIt = player.getBullets().erase(bulletIt);  // Remove the bullet
                        }
                        else {
                            ++bulletIt;
                        }
                    }
                    if (enemy.getHealth() <= 0) {
                        it = enemies.erase(it);  // Remove the enemy
                    }
                    else {
                        ++it;
                    }
                }
                if (enemySpawnClock.getElapsedTime() >= enemySpawnInterval) {
                    int spawnIndex = std::rand() % spawnPoints.size();
                    sf::Vector2f spawnPosition = spawnPoints[spawnIndex];
                    enemies.push_back(Enemy(EN_HP, E_W, E_H, spawnPosition));
                    enemySpawnClock.restart();
                }
                window.clear();
                map.Draw(window);
                drawTime(time);
                drawHealth(player.getHealth());
                window.draw(player);
                for (const auto& enemy : enemies) {

                    window.draw(enemy);
                }
                for (const auto& bullet : player.getBullets()) {
                    window.draw(bullet);
                }
                window.display();
                if (player.isDead()) {
                    sound.setBuffer(game_over_sound);
                    sound.play();
                    setGameState(LOST);
                    window.clear();
                    window.display();
                    break;
                }
            }
            while (getGameState() == LOST || getGameState() == GUIDE)
            {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                    {
                        sound.setBuffer(menuswitch_sound);
                        sound.play();
                        setGameState(MAINMENU);
                        enemies.clear();
                        player.clearBullets();
                        player.respawn();
                        clock.restart();
                        time.restart();
                        window.clear();
                        window.display();
                        mainmenutheme.play();
                        break;
                    }
                }
                window.clear();
                map.Draw(window);
                getGameState() == LOST ? drawGameOverText() : drawGuideText();
                window.display();
            }
            while (getGameState() == END)
            {
                mainmenutheme.stop();
                window.close();
            }
        }
    }

};

int main() {
    Game game;
    game.run();
    return 0;
}
