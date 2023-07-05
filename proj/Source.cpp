#include <SFML/Graphics.hpp>
#include <iostream>

#define E_H 65             // ENTITY HEIGHT
#define E_W 55             // ENTITY WIDTH
#define E_SP 200.0f        // ENTITY SPEED
#define BLT_SP 20.0f       // BULLET SPEED

#define P_HP 5             // PLAYER STARTING HP
#define EN_HP 1            // ENEMY STARTING HP 

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
    sf::Vector2f velocity;  // Velocity of the entity

public:
    Entity(int health, int width, int height)
        : m_health(health), m_width(width), m_height(height), velocity(0.0f, 0.0f) {}

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
    sf::Texture spriteSheetTexture;
    float playerScale;
public:
    Bullet(int x, int y, float scale)
        : Entity(1, 5, 10) { 
        // Bullets have 1 HP and size 5x10
        setPosition(x, y + E_H/2); // adding to y because the bullet spawns a bit higher than the player's gun
        playerScale = scale;
        if (!spriteSheetTexture.loadFromFile("./player/john_idle.png"));
        setTexture(spriteSheetTexture);
        setTextureRect(sf::IntRect(0, 0, 26, 22));
    }

    void update(float deltaTime) {
        sf::Vector2f position = getPosition();

        // Update the bullet's position based on the player's direction
        float movementDirection = playerScale  < 0 ? -1.0f : 1.0f;  // negative/positive scale indicates which way the player is facing. 
        position.x += BLT_SP * movementDirection * deltaTime * E_SP;
        setPosition(position);
        if (position.x < STARTX || position.x > MAX_X || position.y < STARTY || position.y > MAX_Y) {
            // Remove the bullet    
            setHealth(0);
        }
    }
};

class Player : public Entity
{
private:
    sf::Texture spriteSheetTexture;
    bool isJumping;        // Flag to track if the player is jumping
    float jumpVelocity;    // Velocity of the player during jumping
    float gravity;         // Gravity affecting the player
    int fireDelay;
    std::vector<Bullet> bullets;
public:
    Player()
        : Entity(P_HP, E_W, E_H), isJumping(false), jumpVelocity(-10.0f), gravity(0.5f), fireDelay(0) {
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
        if (fireDelay >= 15) { // If enough time has passed since the last shot
            // Calculate the bullet's initial position based on the player's direction
            sf::Vector2f bulletPosition;
            if (getScale().x > 0) {
                // Player is facing right
                std::cout << "facing right" << std::endl;

                bulletPosition = sf::Vector2f(getPosition().x + getWidth(), getPosition().y);
            }
            else {
                // Player is facing left
                std::cout << "facing left" << std::endl;

                bulletPosition = sf::Vector2f(getPosition().x, getPosition().y);
            }

            // Fire a bullet from the calculated position
            bullets.push_back(Bullet(bulletPosition.x, bulletPosition.y, getScale().x));
            std::cout << "firedelay" << std::endl;
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


        // Remove bullets that are off the screen
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet) {
            return bullet.getHealth() == 0;
            }), bullets.end());

    }

    std::vector<Bullet>& getBullets() { return bullets; }

};

class Enemy : public Entity
{
public:
    Enemy(int health, int width, int height)
        : Entity(health, width, height) {}

    void aiBehavior() {
        // Implement AI behavior here
    }
};

class Game
{
public:
    Game() : window(sf::VideoMode(MAX_X, MAX_Y), "SEX") {
        window.setFramerateLimit(FPS);
    }

    void run() {
        Player player;

        sf::Clock clock;
        while (window.isOpen()) {
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

            window.clear();
            window.draw(player);
            for (const auto& bullet : player.getBullets()) {
                window.draw(bullet);
            }
            window.display();
        }
    }

private:
    sf::RenderWindow window;
};

int main() {
    Game game;
    game.run();
    return 0;
}
