#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>


enum class Controls {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    SPEED_UP,
    ROLL_R,
    ROLL_L,
    POS1,
    POS2,
    POS3,
    POS4,
    POS5,
    POS6
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ROLL = 0.0f;
const float SPEED = 8.0f;
const float TILT_SPEED = 2.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 50.0f;
const float ZOOM_SPEED = 0.3f;

class Camera
{
public:
    // world space coordinates
    glm::vec3 camera_position;
    glm::vec3 camera_front;
    glm::vec3 camera_up;
    glm::vec3 camera_right;
    
    glm::vec3 world_up;

    Camera(glm::vec3 position);

    // euler angle
    glm::vec3 euler_angle;

    // quat rot
    glm::quat camera_orientation;
    
    // camera options
    float zoom_speed;
    float movement_speed;
    float roll_speed;
    float mouse_sensitivity;
    float perspective_zoom;

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 view();

    glm::vec3 getEuler();
    void keysMove(Controls control, float dt);
    void moveAndLookAt(glm::vec3 pos, glm::vec3 lookAt);
    void setSpeed(float speed);
    void mouseLook(float xoffset, float yoffset);
    void mouseZoom(float yoffset);
    void bringTo(glm::vec3 point, float d);
    void rotatePoint(glm::vec3 point, float xoffset, float yoffset, float d);
private:
    int cameraMode;
    void rotatePoint(glm::vec3 point, float d);
    void update();
    void Otherupdate();
};
#endif