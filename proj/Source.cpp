#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include <memory>

class MovableEntity {
public:
    MovableEntity(float startX, float startY) :
        position(startX, startY),
        velocity(0.0f, 0.0f),
        isJumping(false),
        jumpForce(300.0f),
        gravity(800.0f),
        movementSpeed(100.0f),
        isAlive(true) {
        // Set up any other properties or resources for the movable entity here
    }

    virtual void update(float deltaTime, const std::vector<std::shared_ptr<MovableEntity>>& entities) {
        if (!isAlive) {
            // ...
            return; // Skip the rest of the update logic for inactive entities
        }

        // Apply gravity
        velocity.y += gravity * deltaTime;

        // Move the entity horizontally
        position.x += velocity.x * deltaTime;

        // Move the entity vertically
        position.y += velocity.y * deltaTime;

        // Check for collisions with other entities
        for (const auto& entity : entities) {
            if (entity.get() != this) {
                if (isIntersecting(entity)) {
                    handleCollision(entity);
                }
            }
        }

        // Update any other entity-specific logic here
        // Temporary bottom bounds
        if (position.y > 1200.0f - 50.0f) {
            // Adjust the position and velocity to prevent falling off the screen
            position.y = 1200.0f - 50.0f;
            velocity.y = 0.0f;
            isJumping = false;
        }
    }

    void moveLeft() {
        velocity.x = -movementSpeed;
    }

    void moveRight() {
        velocity.x = movementSpeed;
    }

    void jump() {
        if (!isJumping) {
            isJumping = true;
            velocity.y = -jumpForce; // Instantly apply jump force
        }
    }

    sf::Vector2f getPosition() const {
        return position;
    }

protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    bool isJumping;
    bool isAlive;
    float jumpForce;
    float gravity;
    float movementSpeed;

    bool isIntersecting(const std::shared_ptr<MovableEntity>& otherEntity) const {
        sf::FloatRect thisBounds(position.x, position.y, 50.0f, 50.0f);
        sf::FloatRect otherBounds(otherEntity->position.x, otherEntity->position.y, 50.0f, 50.0f);
        return thisBounds.intersects(otherBounds);
    }

    void handleCollision(const std::shared_ptr<MovableEntity>& otherEntity) {
        // Adjust the positions/velocities of the colliding entities to resolve the collision
        // Here, we'll simply stop the current entity from moving in the collided direction

        sf::FloatRect thisBounds(position.x, position.y, 50.0f, 50.0f);
        sf::FloatRect otherBounds(otherEntity->position.x, otherEntity->position.y, 50.0f, 50.0f);
        sf::FloatRect intersection;
        thisBounds.intersects(otherBounds, intersection);

        // Determine the side of collision
        if (intersection.width < intersection.height) {
            // Horizontal collision
            if (position.x < otherEntity->position.x) {
                // Collided from the left
                position.x = otherEntity->position.x - 50.0f;
            }
            else {
                // Collided from the right
                position.x = otherEntity->position.x + 50.0f;
            }
            velocity.x = 0.0f;
        }
        else {
            // Vertical collision
            if (position.y < otherEntity->position.y) {
                // Collided from above
                position.y = otherEntity->position.y - 50.0f;
                isJumping = false; // Reset jumping state
            }
            else {
                // Collided from below
                position.y = otherEntity->position.y + 50.0f;
                velocity.y = 0.0f;
            }
        }
    }
};

class Player : public MovableEntity {
public:
    Player(float startX, float startY) : MovableEntity(startX, startY) {
        // Set up any other properties or resources specific to the player here
    }

    void update(float deltaTime, const std::vector<std::shared_ptr<MovableEntity>>& entities) {
        // Update the player's movement based on the input
        handleInput();

        // Call the base class update function to handle movement and physics
        MovableEntity::update(deltaTime, entities);

        // Add any other player-specific logic here
    }

private:
    void handleInput() {
        // Check the input and move the player accordingly

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            moveLeft();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            moveRight();
        }
        else {
            // Stop moving if no input is detected
            velocity.x = 0.0f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            jump();
        }
    }
};

class Enemy : public MovableEntity {
public:
    Enemy(float startX, float startY) : MovableEntity(startX, startY) {
        // Set up any other properties or resources specific to the enemy here
    }

    void update(float deltaTime, const std::vector<std::shared_ptr<MovableEntity>>& entities) {
        // Update the enemy's movement and behavior here
        // For example, you could make it move in a specific pattern or track the player

        // Call the base class update function to handle movement and physics
        MovableEntity::update(deltaTime, entities);

        // Add any other enemy-specific logic here
    }
};

class Game {
public:
    Game() : window(sf::VideoMode(800, 600), "Game"), camera(window.getDefaultView()) {
        // Set up any initial game variables or resources here
        window.setFramerateLimit(60);

        // Create and add player entity
        player = std::make_shared<Player>(100.0f, 100.0f);
        // Create and add enemy entities
        std::shared_ptr<Enemy> enemy1 = std::make_shared<Enemy>(300.0f, 200.0f);
        std::shared_ptr<Enemy> enemy2 = std::make_shared<Enemy>(500.0f, 300.0f);
        entities.push_back(enemy1);
        entities.push_back(enemy2);
        entities.push_back(player);
    }

    void run() {
        sf::Clock clock;
        while (window.isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            processEvents();
            update(deltaTime);
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::View camera;
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<MovableEntity>> entities;

    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    void update(float deltaTime) {
        // Update all entities
        for (const auto& entity : entities) {
            entity->update(deltaTime, entities);
        }

        // Update the camera's position based on the player's position
        camera.setCenter(player->getPosition());

        // Set the camera view for rendering
        window.setView(camera);
    }
    /* void render() { // normal rendering
        window.clear(sf::Color::White);
        // Draw game objects here
        for (const auto& entity : entities) {
            sf::RectangleShape entityShape(sf::Vector2f(50.0f, 50.0f));
            entityShape.setPosition(entity->getPosition());
            entityShape.setFillColor(sf::Color::Green);
            window.draw(entityShape);
        }
        window.display();
    }

*/
    void render() {
        //window.clear();
        window.clear(sf::Color::White);

        // Render all entities within the camera view
        for (const auto& entity : entities) {
            if (isEntityInCameraView(entity)) {
                // Render the entity within the camera view
                sf::RectangleShape entityShape(sf::Vector2f(50.0f, 50.0f));
                entityShape.setPosition(entity->getPosition());
                if (dynamic_cast<Enemy*>(entity.get())) {
                    entityShape.setFillColor(sf::Color::Red);
                }
                else {
                    entityShape.setFillColor(sf::Color::Green);
                }
                window.draw(entityShape);
            }
        }

        window.display();
    }

    bool isEntityInCameraView(const std::shared_ptr<MovableEntity>& entity) const {
        sf::FloatRect entityBounds(entity->getPosition().x, entity->getPosition().y, 50.0f, 50.0f);
        sf::FloatRect cameraBounds(camera.getCenter() - camera.getSize() / 2.0f, camera.getSize());
        return cameraBounds.intersects(entityBounds);
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
