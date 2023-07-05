#include <SFML/Graphics.hpp>

#define E_H 50             // ENTITY HEIGHT
#define E_W 60             // ENTITY WIDTH
#define P_HP 5             // PLAYER STARTING HP
#define EN_HP 1            // ENEMY STARTING HP 
#define speed 5
#define STARTX 0
#define STARTY MAX_Y-E_H
#define MAX_X 1024
#define MAX_Y 768

#define FPS 90

class Entity : public sf::Transformable
{
public:
    Entity(int health, int moveSpeed, int damage, int width, int height)
        : m_health(health), m_damage(damage), m_width(width), m_height(height), isJumping(false), velocity(0, 0), acceleration(0) {}

    int getHealth() const { return m_health; }
    void setHealth(int health) { m_health = health; }


    int getDamage() const { return m_damage; }
    void setDamage(int damage) { m_damage = damage; }

    int getWidth() const { return m_width; }
    void setWidth(int width) { m_width = width; }

    int getHeight() const { return m_height; }
    void setHeight(int height) { m_height = height; }
    
    int getx() { return x; }
    void setx(int xs) { x = xs; }
    
    int gety() { return y; }
    void sety(int ys) { y = ys; }

    bool getIsJumping() const { return isJumping; }
    void setIsJumping(bool jumping) { isJumping = jumping; }

    void jump() {
        if (!getIsJumping()) {
            isJumping = true;
            velocity.y = -50;  // Adjust this value as needed
            acceleration = 1.0;  // Adjust this value as needed
        }
    }

    /* for testing only */
    sf::RectangleShape m_rectangle;

protected:
    sf::Vector2f velocity;  // New member variable for velocity
    float acceleration;    // New member variable for acceleration
    bool isJumping;         // New member variable for jumping state

private:
    int m_health;
    int m_damage;
    int m_width;
    int m_height;
    int x, y;
};

class Player : public Entity
{
private:
    sf::Texture spriteSheetTexture;
    sf::Sprite playerSprite;
    sf::Vector2f direction;

public:
    Player(int health, int x, int y, int width, int height, int moveSpeed, int damage)
        : Entity(health, speed, damage, width, height), direction(1, 0)
    {
        if (!spriteSheetTexture.loadFromFile("./player/john_idle.png"))
        {
            // Error accessing sprite location
        }

        int tileX = 0;
        int tileY = 0;
        int tileSize = 22;

        playerSprite.setTexture(spriteSheetTexture);
        playerSprite.setTextureRect(sf::IntRect(tileX, tileY, tileSize, tileSize));

        playerSprite.setPosition(x, y);
        playerSprite.setScale(E_W / 26.0f, E_H / 22.0f);
    }

    sf::Sprite& getPlayerSprite() {
        return playerSprite;
    }

    void move(sf::Vector2f direction) {
        sf::Vector2f newPosition = playerSprite.getPosition() + sf::Vector2f(direction.x * speed, direction.y * speed);

        if (newPosition.x < STARTX) {
            newPosition.x = STARTX;
        }
        if (newPosition.x + getWidth() > MAX_X) {
            newPosition.x = MAX_X - getWidth();
        }
        if (newPosition.y < STARTY) {
            newPosition.y = STARTY;
        }
        if (newPosition.y + getHeight() > MAX_Y) {
            newPosition.y = MAX_Y - getHeight();
        }

        if (isJumping) {
            velocity.y += acceleration;
            newPosition.y += velocity.y;

            if (newPosition.y >= STARTY) {
                newPosition.y = STARTY;
                isJumping = false;
                velocity.y = 0;
                acceleration = 0;
            }
        }

        this->direction = direction;
        playerSprite.setPosition(newPosition);
    }

    

    void shoot() {
        // Implement shooting logic here
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            move(sf::Vector2f(-1, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            move(sf::Vector2f(1, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            jump();
        }
    }

    void draw(sf::RenderWindow& window)
    {

        if (direction.x != 0) {
            if (direction.x > 0) {
                playerSprite.setScale(E_W / 26.0f, E_H / 22.0f);
            }
            else if (direction.x < 0) {
                playerSprite.setScale(-E_W / 26.0f, E_H / 22.0f);
            }
        }

        window.draw(playerSprite);
    }
};

class Enemy : public Entity
{
public:
    Enemy(int health, int moveSpeed, int damage, int width, int height)
        : Entity(health, moveSpeed, damage, width, height) {}

    void aiBehavior() {
        // Implement AI behavior here
    }
};

class Game
{
public:
    Game() : window(sf::VideoMode(MAX_X, MAX_Y), "Game Window") {
        window.setFramerateLimit(FPS);
    }

    void run() {
        Player player(P_HP, STARTX, STARTY, E_W, E_H, speed, EN_HP);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                player.handleInput();
            }

            window.clear();

            player.move(sf::Vector2f(0, 0));  // Update player position only if not jumping

            player.draw(window);
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
