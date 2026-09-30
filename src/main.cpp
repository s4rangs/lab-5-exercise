#include <iostream>
#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;

using Point2D = sf::Vector2f;

// TODO: (Part 1) Define a function that samples a cubic Bezier curve at t in [0, 1].
Point2D getPoint(const std::vector<sf::Vector2f>& pts, float t) { 
    const float x = 1.0f - t;

    // returning calculated point instead of empty pt
    // exactly 4 points
    return x * x * x * pts[0] + 3.0f * x * x * t * pts[1] + 3.0f * x * t * t * pts[2]+ t * t * t * pts[3];  // 
}
    
// TODO: (Part 2) Define a function that returns the curve's slope at t in [0, 1].
Point2D getSlope(const std::vector<sf::Vector2f>& pts, float t) { 
    const float x = 1.0f - t;

    return 3.0f * x * x * (pts[1] - pts[0]) + 6.0f * x * t * (pts[2] - pts[1]) + 3.0f * t * t * (pts[3] - pts[2]);
 }

// TODO: (Part 1) Store four control points for the curve.
std::vector<sf::Vector2f> pts = {
    {100.0f, 300.0f},
    {200.0f, 100.0f},
    {300.0f, 100.0f},
    {400.0f, 300.0f}
};
// TODO: (Part 2) Track animation time for the square moving along the curve.
float animationTime = 0.0f;
// TODO: (Part 3) Track the index of the control point being dragged.
int selectedPoint = -1;

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            // TODO: (Part 3) On left-click, select the closest control point
            // using mouse->position and start dragging it.
            if (mouse->button == sf::Mouse::Button::Left) {
                selectedPoint = -1;

                float closestDistance = 20.0f * 20.0f;

                for (int i = 0; i < static_cast<int>(pts.size()); ++i) {
                    float dx = pts[i].x - static_cast<float>(mouse->position.x);
                    float dy = pts[i].y - static_cast<float>(mouse->position.y);

                    float distanceSquared = dx * dx + dy * dy;

                    if (distanceSquared < closestDistance) {
                        closestDistance = distanceSquared;
                        selectedPoint = i;
                    }
                }
            }    
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            // TODO: (Part 3) On left-button release, stop dragging.
            if (mouse->button == sf::Mouse::Button::Left) {
                selectedPoint = -1;
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            // TODO: (Part 3) Move the selected control point to mouse->position.
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
            if (selectedPoint != -1) {
                pts[selectedPoint] = {static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y)};

                // maintains smoothness when point 3 moves.
                if (selectedPoint >= 2 && selectedPoint % 3 == 2 && selectedPoint + 2 < static_cast<int>(pts.size())) {

                    Point2D direction = pts[selectedPoint + 1] - pts[selectedPoint];

                    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                    Point2D oldDirection = pts[selectedPoint + 2] - pts[selectedPoint + 1];

                    float oldLength = std::sqrt(oldDirection.x * oldDirection.x + oldDirection.y * oldDirection.y);

                    if (length > 0.0f) {
                        direction /= length;
                        pts[selectedPoint + 2] = pts[selectedPoint + 1] + direction * oldLength;
                    }
                }
            }
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // TODO: (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
            // keeping at least four points.
            if (key->code == sf::Keyboard::Key::Equal || key->code == sf::Keyboard::Key::Add){
                // adds three points
                Point2D last = pts.back();

                float step = 80.0f;

                // reduce step size if there is not enough room for 3 new points to the right
                if (last.x + 3.0f * step > WINDOW_WIDTH) {
                    step = (WINDOW_WIDTH - last.x) / 3.0f;
                }

                pts.push_back({last.x + step, last.y - 100.0f});

                pts.push_back({last.x + 2.0f * step, last.y - 100.0f});

                pts.push_back({last.x + 3.0f * step, last.y});

            }
            else if (key->code == sf::Keyboard::Key::Hyphen || key->code == sf::Keyboard::Key::Subtract){
                if (pts.size() >= 7) {
                    pts.pop_back();
                    pts.pop_back();
                    pts.pop_back();
                }
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Part 1) Sample GetPoint over t in [0, 1] and connect samples using the line-drawing
    // code from your project. Draw all four control points as circles after drawing the curve.
    // ====== ====== ======
    sf::VertexArray curve(sf::PrimitiveType::LineStrip);

    for (std::size_t start = 0;
        start + 3 < pts.size();
        start += 3) {

        std::vector<sf::Vector2f> segment = {pts[start], pts[start + 1], pts[start + 2], pts[start + 3]};

        for (int i = 0; i <= 100; ++i) {
            float t = static_cast<float>(i) / 100.0f;

            curve.append(sf::Vertex(getPoint(segment, t)));
        }
    }

    window.draw(curve);

    for (const auto& point : pts) {
        sf::CircleShape circle(8.0f);
        circle.setOrigin({8.0f, 8.0f});
        circle.setPosition(point);

        window.draw(circle);
    }
    // ====== ====== ======
    // TODO: (Part 2) Draw a small square moving repeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======
    animationTime += 0.01f;

    if (animationTime >= 1.0f) {
        animationTime = 0.0f;
    }

    std::size_t segmentCount = (pts.size() - 1) / 3;

    //OPENAI generated to help square travel through newly generated 3 points
    float segmentTime = animationTime * static_cast<float>(segmentCount);

    std::size_t segment = static_cast<std::size_t>(segmentTime);

    if (segment >= segmentCount) {
        segment = segmentCount - 1;
    }

    float t = segmentTime - static_cast<float>(segment);

    std::vector<Point2D> currentSegment = {
        pts[segment * 3],
        pts[segment * 3 + 1],
        pts[segment * 3 + 2],
        pts[segment * 3 + 3]
    };
    //GENERATION ENDS

    Point2D position = getPoint(currentSegment, t);
    Point2D slope = getSlope(currentSegment, t);

    float angle =
        std::atan2(slope.y, slope.x) * (180.0f / 3.14159265359f);

    sf::RectangleShape square({20.0f, 20.0f});
    square.setPosition(position);
    square.setRotation(sf::degrees(angle));
    square.setOrigin({10.0f, 10.0f});

    window.draw(square);

    // ====== ====== ======
    // TODO: (Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======
    sf::VertexArray handles(sf::PrimitiveType::Lines);

    for (std::size_t start = 0;
        start + 3 < pts.size();
        start += 3) {

        handles.append(sf::Vertex(pts[start]));
        handles.append(sf::Vertex(pts[start + 1]));

        handles.append(sf::Vertex(pts[start + 2]));
        handles.append(sf::Vertex(pts[start + 3]));
    }

    window.draw(handles);
    // ====== ====== ======
    // TODO: (Bonus) Support multiple curves, a Galaga screen overlay at a 1:2 ratio, and exporting
    // curve points as C++ code for Project 1b.
    // ====== ====== ======

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Bezier Curve Editor");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
