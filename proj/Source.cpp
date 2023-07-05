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

class Entity
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

class Player : public sf::Sprite, public Entity
{
private:
    sf::Texture spriteSheetTexture;
    float movementSpeed;  // Constant movement speed

public:
    Player()
        : Entity(P_HP, E_W, E_H), movementSpeed(5.0f) {
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
        setPosition(0, 0);
        //------------------------------------------------------------------------------------
    }

    void move(sf::Vector2f direction) {
        sf::Vector2f newPosition = getPosition() + (direction * movementSpeed);
        setPosition(newPosition);
    }

    void jump() {
        // Implement jumping logic here
    }

    void shoot() {
        // Implement shooting logic here
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            move(sf::Vector2f(0, -1));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            move(sf::Vector2f(0, 1));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            move(sf::Vector2f(-1, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            move(sf::Vector2f(1, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            jump();
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

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                player.handleInput();
            }

            // Update game state here

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
