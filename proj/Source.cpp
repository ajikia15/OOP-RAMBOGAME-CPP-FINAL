#include <SFML/Graphics.hpp>

#define E_H 50             // ENTITY HEIGHT
#define E_W 50             // ENTITY WIDTH
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

public:
    Entity(int health, int width, int height)
        : m_health(health), m_width(width), m_height(height) {}

    int getHealth() const { return m_health; }
    void setHealth(int health) { m_health = health; }

    int getWidth() const { return m_width; }
    void setWidth(int width) { m_width = width; }

    int getHeight() const { return m_height; }
    void setHeight(int height) { m_height = height; }

    /* for testing only */
    sf::RectangleShape m_rectangle;
};

class Player : public Entity
{
private:
    sf::Texture spriteSheetTexture;
    sf::Vector2f velocity;  // Velocity of the player
    bool isJumping;        // Flag to track if the player is jumping
    float jumpVelocity;    // Velocity of the player during jumping
    float gravity;         // Gravity affecting the player

public:
    Player()
        : Entity(P_HP, E_W, E_H), velocity(0.0f, 0.0f), isJumping(false), jumpVelocity(-10.0f), gravity(0.5f) {
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
        setTextureRect(sf::IntRect(tileX, tileY, tileSize, tileSize));
        // Set the initial position, scale, or any other properties of the sprite
        setPosition(MAX_X / 2, MAX_Y - E_H);
        setScale(1.0f, 1.0f);
        //------------------------------------------------------------------------------------
    }

    void jump() {
        if (!isJumping) {
            velocity.y = jumpVelocity;
            isJumping = true;
        }
    }

    void shoot() {
        // Implement shooting logic here
    }

    void handleInput() {
        velocity.x = 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            jump();
        }
    }

    void update(float deltaTime) {
        // Apply gravity to the player
        velocity.y += gravity;

        // Update the player's position based on velocity
        sf::Vector2f position = getPosition();
        position += velocity * deltaTime * 100.0f;  // Adjust the multiplier to control movement speed

        // Check if the player is within the window boundaries
        if (position.x < 0) {
            position.x = 0;
        }
        if (position.x + E_W > MAX_X) {
            position.x = MAX_X - E_W;
        }
        if (position.y < 0) {
            position.y = 0;
        }
        if (position.y + E_H > MAX_Y) {
            position.y = MAX_Y - E_H;
            velocity.y = 0.0f;
            isJumping = false;
        }

        setPosition(position);

        // Check if the player is jumping and reached the peak
        if (isJumping && velocity.y >= 0.0f) {
            velocity.y = 0.0f;
            isJumping = false;
        }
    }

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
