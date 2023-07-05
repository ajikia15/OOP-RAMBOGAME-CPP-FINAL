#include <SFML/Graphics.hpp>
#include <iostream>

#define E_H 65             // ENTITY HEIGHT
#define E_W 55             // ENTITY WIDTH
#define E_SP 300.0f        // ENTITY SPEED
#define BLT_SP 20.0f       // BULLET SPEED

#define P_HP 5             // PLAYER STARTING HP

#define EN_HP 1            // ENEMY STARTING HP 
#define EN_SPWN  600          // ENEMY SPAWN INTERVAL (in seconds)

#define STARTX 0
#define STARTY 0
#define MAX_X 1024
#define MAX_Y 768

#define FPS 120

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

class Enemy : public Entity
{
private:
    sf::Vector2f lastValidPosition;
    static sf::Texture enemySheet;
public:
    Enemy(int health, int width, int height, const sf::Vector2f& spawnPosition)
        : Entity(health, width, height) {
        setPosition(spawnPosition);
        if (!enemySheet.loadFromFile("./player/john_idle.png"))
        {
            std::cout << "pic not found" << std::endl;
        }
        // Define the coordinates and size of the desired part of the tile sheet
        int tileX = 0;  // X coordinate of the tile within the tile sheet
        int tileY = 0;  // Y coordinate of the tile within the tile sheet

        // Set the texture rectangle of the sprite to display the desired part of the tile sheet
        setTexture(enemySheet);
        setTextureRect(sf::IntRect(tileX, tileY, 26, 22));
        setScale(E_W / 26.0f, E_H / 22.0f);

    }

    void update(float deltaTime, const std::vector<Enemy>& enemies, const sf::Vector2f& playerPosition) {
        // Apply gravity to the the enemy

        for (const auto& otherEnemy : enemies) {
            if (isEnemyColliding(otherEnemy) && this != &otherEnemy) {
                // Collision occurred with another enemy
                setPosition(lastValidPosition); // Revert to the last known valid position, needs improvement i guess
                if (isEnemyColliding(otherEnemy) && this != &otherEnemy)
                {

                }
                std::cout << "enemys havig sex" << std::endl;
                break; 
            }
            else {
                lastValidPosition = getPosition();
                velocity.y += gravity;

            }
        }
        sf::Vector2f direction = playerPosition - getPosition();
        if (direction.x > 0) {
            velocity.x = 1.0f;  // Move right
        }
        else if (direction.x < 0) {
            velocity.x = -1.0f; // Move left
        }
        else {
            velocity.x = 0.0f;  // Stop moving horizontally if player is in line
        }
        // Update the enemys's position based on velocity
        Entity::update(deltaTime);
    }
};
sf::Texture Enemy::enemySheet;

class Player : public Entity
{
private:
    sf::Texture spriteSheetTexture;
    bool isJumping;        // Flag to track if the player is jumping
    float jumpVelocity;    // Velocity of the player during jumping
    int fireDelay;
    std::vector<Bullet> bullets;
public:
    Player()
        : Entity(P_HP, E_W, E_H), isJumping(false), jumpVelocity(-10.0f), fireDelay(0) {
        //------------------------------------------------------------------------------------   
        //add player sprite
        if (!spriteSheetTexture.loadFromFile("./player/john_idle.png"))
        {
            //error accessing sprite location
        }
        // Define the coordinates and size of the desired part of the tile sheet
        int tileX = 0;  // X coordinate of the tile within the tile sheet
        int tileY = 0;  // Y coordinate of the tile within the tile sheet
        int tileSize = 22;  // Size of each tile

        // Set the texture rectangle of the sprite to display the desired part of the tile sheet
        setTexture(spriteSheetTexture);
        setTextureRect(sf::IntRect(tileX, tileY, 26, tileSize));
        // Set the initial position, scale, or any other properties of the sprite
        setPosition(MAX_X / 2, MAX_Y - E_H);
        setScale(E_W / 26.0f, E_H / 22.0f);
        //------------------------------------------------------------------------------------
    }
    void fire() {
        if (fireDelay >= 30) { // If enough time has passed since the last shot
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
            bullets.push_back(Bullet(bulletPosition.x, bulletPosition.y, getScale().x));
            //std::cout << "firedelay" << std::endl;
            fireDelay = 0;
        }
    }

    void jump() {
        if (!isJumping) {
            velocity.y = jumpVelocity;
            isJumping = true;
        }
    }

    void handleInput() {
        velocity.x = 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -1.0f;
            setScale(-E_W / 26.0f, E_H / 22.0f);
            setOrigin(E_W / 2 - E_W / 26.0f, 0); // karoche marjvniv tu midiodi da marcxniv gauxvdevdi an piriqit ucnaurad iyo dzaan da bevri kombinacia vcade
            // sabolood aseti gamoiyureba yvelaze kargad rato ar vici ar sheexo

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = 1.0f;
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
        for (auto& bullet : bullets) {
            bullet.update(deltaTime);
        }


    }

    std::vector<Bullet>& getBullets() { return bullets; }

};

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

    int TotalW;
    int TotalH;

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


        if (backgroundTexture.loadFromFile("./background/background_color.png")) {

            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setTextureRect(sf::IntRect(0, 0, MAX_X, MAX_Y));

            if (backgroundTexture1.loadFromFile("./background/jungle_paralax_bg2.png")) {

                backgroundTexture1.setRepeated(true); // Set the texture repeat mode to true

                backgroundSprite1.setTexture(backgroundTexture1);
                backgroundSprite1.setTextureRect(sf::IntRect(0, 0, MAX_X, MAX_Y));
                //Set the position of the background sprite to the bottom of the window
                backgroundSprite1.setPosition(0, MAX_Y - backgroundTexture1.getSize().y - 250);
                backgroundSprite1.setScale(2, 2);
            }
        }



        //load jungle trees 

        if (jungleTreeTexture.loadFromFile("./background/jungle_tree2.png")) {
            jungleTreeSprite.setTexture(jungleTreeTexture);
            jungleTreeSprite.setScale(3, 3);
        }
    }


    void Update(float deltatime) {

    }
    void Draw(sf::RenderWindow& window) {

        window.draw(backgroundSprite);
        window.draw(backgroundSprite1);


        int numSprites = 20; // Number of sprites to print
        int spacing = 50; // Spacing between each sprite
        int i = 0;
        //lower ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            // Adjust the position of the current sprite
            int posX = sprite.getPosition().x + (i * spacing);
            int posY = sprite.getPosition().y;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);


            i++;

        } while (i < numSprites);


        i = 0;
        numSprites = 5;
        //top left ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            // Adjust the position of the current sprite
            int posX = sprite.getPosition().x + (i * spacing);
            int posY = 300;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);
            i++;

        } while (i < numSprites);


        i = 0;
        numSprites = 9;
        //middle right ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            int posX = MAX_X;
            // Adjust the position of the current sprite
            posX = posX - (i * spacing);
            int posY = 500;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);
            i++;
        } while (i < numSprites);

        i = 0;
        numSprites = 6;
        //top right ground
        do {
            sf::Sprite currentSprite = sprite; // Create a copy of the sprite

            int posX = MAX_X;
            // Adjust the position of the current sprite
            posX = posX - (i * spacing);
            int posY = 200;

            currentSprite.setPosition(posX, posY);
            window.draw(currentSprite);
            i++;
        } while (i < numSprites);
    }

    bool isColliding(const Player& player) const {
        // Check if the player's bounding box intersects with the map sprite's bounding box
        return backgroundSprite.getGlobalBounds().intersects(player.getGlobalBounds());
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

public:
    Game() : window(sf::VideoMode(MAX_X, MAX_Y), "SEX") {
        window.setFramerateLimit(FPS);

        // spawn points on the right side
        spawnPoints.push_back(sf::Vector2f(E_W, MAX_Y - E_H));   // bottom
        spawnPoints.push_back(sf::Vector2f(E_W, MAX_Y - E_H - 200));

        //spawn points on the left side
        spawnPoints.push_back(sf::Vector2f(MAX_X - E_W, MAX_Y - E_H)); // bottom
        spawnPoints.push_back(sf::Vector2f(MAX_X - E_W, MAX_Y - E_H - 200));

    }

    void run() {
        Player player;
        Map map;
        map.Load();
        sf::Clock clock;
        while (window.isOpen()) {
            while (player.getHealth() > 0)
            {
                // Check for collision between the map sprite and the player sprite
                if (map.isColliding(player)) {
                    // Collision occurred
                    std::cout << "Collision between map and player" << std::endl;
                    // Handle the collision as desired (e.g., reduce player health, stop player movement, etc.)
                }


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
                map.Update(deltaTime);

                if (enemySpawnClock.getElapsedTime() >= enemySpawnInterval) {
                    int spawnIndex = std::rand() % spawnPoints.size();
                    sf::Vector2f spawnPosition = spawnPoints[spawnIndex];
                    enemies.push_back(Enemy(EN_HP, E_W, E_H, spawnPosition));
                    enemySpawnClock.restart();
                }

                // Update enemies
                for (auto it = enemies.begin(); it != enemies.end(); ) {
                    auto& enemy = *it;
                    enemy.update(deltaTime, enemies, player.getPosition());

                    if (enemy.isColliding(player)) {
                        std::cout << "gay" << std::endl;
                        player.setHealth(player.getHealth() - 1);
                        enemy.setHealth(enemy.getHealth() - 1);
                    }

                    for (auto bulletIt = player.getBullets().begin(); bulletIt != player.getBullets().end(); ) {
                        auto& bullet = *bulletIt;
                        if (bullet.isColliding(enemy)) {
                            std::cout << "Collision with enemy" << std::endl;
                            bullet.setHealth(0);
                            enemy.setHealth(enemy.getHealth() - 1);
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
                window.clear();
                map.Draw(window);
                window.draw(player);
                for (const auto& enemy : enemies) {
                    window.draw(enemy);
                }
                for (const auto& bullet : player.getBullets()) {
                    window.draw(bullet);
                }
                window.display();
            }
            if (player.getHealth() < 1)
            {
                std::cout << "YOU'VE LOST" << std::endl;
            }
        }
    }

};

int main() {
    Game game;
    game.run();
    return 0;
}
