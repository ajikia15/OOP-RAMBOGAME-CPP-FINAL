#include <SFML/Graphics.hpp>


#define E_H 55             // ENTITY HEIGHT
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
        : m_health(health), m_damage(damage), m_width(width), m_height(height) {}

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

    /* for testing only */
    sf::RectangleShape m_rectangle;



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
    
public:
    Player(int health, int x, int y, int width, int height, int moveSpeed, int damage)
        : Entity(health, speed, damage, width, height) 
    {     
    //------------------------------------------------------------------------------------   
        //add player sprite
        if (!spriteSheetTexture.loadFromFile("./player/john_idle.png"))
        {
            //error accesing sprite location
        } 
        // Define the coordinates and size of the desired part of the tile sheet
        int tileX = 0;  // X coordinate of the tile within the tile sheet
        int tileY = 0;  // Y coordinate of the tile within the tile sheet
        int tileSize = 22;  // Size of each tile

        // Set the texture rectangle of the sprite to display the desired part of the tile sheet
        playerSprite.setTexture(spriteSheetTexture);
        playerSprite.setTextureRect(sf::IntRect(tileX, tileY, tileSize, tileSize));

        playerSprite.setPosition(x, y);

        float PH = E_H / 22.0f;
        float PW = E_W / 26.0f;
        playerSprite.setScale(PW, PH);
    }
    sf::Sprite& getPlayerSprite() {
        return playerSprite;
    }
    //------------------------------------------------------------------------------------

    void move(sf::Vector2f direction) {
        // Calculate the new position of the player sprite
        sf::Vector2f newPosition = playerSprite.getPosition() + sf::Vector2f(direction.x * speed, direction.y * speed);

        
        // Make sure the player sprite stays within the bounds of the screen
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

        // Update the position of the player sprite
        playerSprite.setPosition(newPosition);
    }


    void jump() {
        
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
    Game() : window(sf::VideoMode(MAX_X, MAX_Y), "SEX") {
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

            // Update game state here

            window.clear();

            window.draw(player.getPlayerSprite());
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
