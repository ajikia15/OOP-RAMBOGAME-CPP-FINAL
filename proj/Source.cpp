#include <SFML/Graphics.hpp>


#define E_H 50             // ENTITY HEIGHT
#define E_W 50             // ENTITY WIDTH
#define P_HP 5             // PLAYER STARTING HP
#define EN_HP 1            // ENEMY STARTING HP 

#define STARTX 0
#define STARTY 0
#define MAX_X 1680
#define MAX_Y 920

#define FPS 120



class Entity : public sf::Transformable
{
public:
    Entity(int health, int moveSpeed, int damage, int width, int height)
        : m_health(health), m_moveSpeed(moveSpeed), m_damage(damage), m_width(width), m_height(height) {}

    int getHealth() const { return m_health; }
    void setHealth(int health) { m_health = health; }

    int getMoveSpeed() const { return m_moveSpeed; }
    void setMoveSpeed(int moveSpeed) { m_moveSpeed = moveSpeed; }

    int getDamage() const { return m_damage; }
    void setDamage(int damage) { m_damage = damage; }

    sf::Vector2f getPosition() const { return this->getPosition(); }
    void setPosition(const sf::Vector2f& position) { this->setPosition(position); }

    int getWidth() const { return m_width; }
    void setWidth(int width) { m_width = width; }

    int getHeight() const { return m_height; }
    void setHeight(int height) { m_height = height; }

    /* for testing only */
    sf::RectangleShape m_rectangle;

private:
    int m_health;
    int m_moveSpeed;
    int m_damage;
    int m_width;
    int m_height;
};

class Player : public Entity
{
public:
    Player(int health, int moveSpeed, int damage, int width, int height)
        : Entity(health, moveSpeed, damage, height, width) {
        /* for testing only */
        m_rectangle.setSize(sf::Vector2f(E_W, E_H));
        m_rectangle.setFillColor(sf::Color::Blue);
    }

    void move(sf::Vector2f direction) {
        // Implement movement logic here
    }

    void jump() {
        // Implement jumping logic here
    }

    void shoot() {
        // Implement shooting logic here
    }
    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            move(sf::Vector2f(0, -getMoveSpeed()));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            move(sf::Vector2f(0, getMoveSpeed()));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            move(sf::Vector2f(-getMoveSpeed(), 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            move(sf::Vector2f(getMoveSpeed(), 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            jump();
        }
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
    Game() : window(sf::VideoMode(MAX_X, MAX_X), "SEX") {
        window.setFramerateLimit(FPS);
    }

    void run() {
        Player player(P_HP, STARTX, STARTY, E_W, E_H);
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
            // Draw game state here
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
