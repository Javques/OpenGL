#include "Camera.hpp"

Camera::Camera(glm::vec3 camera_position)
{
    camera_position = camera_position;
    cameraMode = 0;
    euler_angle = glm::vec3(YAW, PITCH, 0.0f);
    camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    movement_speed = SPEED;
    zoom_speed = ZOOM_SPEED;
    perspective_zoom = ZOOM;
    mouse_sensitivity = SENSITIVITY;
    update();
}

void Camera::moveAndLookAt(glm::vec3 pos, glm::vec3 lookAt)
{
    camera_position = pos;
    camera_front = glm::normalize(lookAt - pos);


    Otherupdate();
}

glm::vec3 Camera::getEuler()
{
        return euler_angle;
}
// return the camera's view matrix
glm::mat4 Camera::view()
{
    return glm::lookAt(camera_position, camera_position + camera_front, camera_up);
}

// all used in response to input
void Camera::keysMove(Controls control, float dt)
{
    // Movements
    float velocity = movement_speed * dt;
    float angVelocity = roll_speed * dt;

    switch (control) {
    case Controls::FORWARD:
        camera_position += camera_front * velocity;
        break;
    case Controls::BACKWARD:
        camera_position -= camera_front * velocity;
        break;
    case Controls::LEFT:
        camera_position -= camera_right * velocity;
        break;
    case Controls::RIGHT:
        camera_position += camera_right * velocity;
        break;
    }
}
// updates the camera axes on any sort of input
void Camera::Otherupdate()
{

    camera_front = glm::normalize(camera_front);
    camera_right = glm::normalize(glm::cross(camera_front, world_up));
    camera_up = glm::normalize(glm::cross(camera_right, camera_front));
}
void Camera::mouseLook(float xoffset, float yoffset)
{

    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    if (cameraMode == 1)
    {
    }

    euler_angle.x += xoffset;
    euler_angle.y += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    // bad things happen at -90 and 90 degrees
        if (euler_angle.y > 89.9f)
            euler_angle.y = 89.9f;
        if (euler_angle.y < -89.9f)
            euler_angle.y = -89.9f;

    // update Front, Right and Up Vectors using the updated Euler angles
    update();
}

void Camera::mouseZoom(float yoffset)
{
    perspective_zoom -= (float)yoffset * zoom_speed;
    if (perspective_zoom < 1.0f)
        perspective_zoom = 1.0f;
    if (perspective_zoom > 50.0f)
        perspective_zoom = 50.0f;
}

void Camera::bringTo(glm::vec3 point, float d)
{
    camera_position = (glm::distance(camera_position, point) - d) * glm::normalize(camera_position - point);
    camera_front = glm::normalize(camera_position - point);
}

void Camera::rotatePoint(glm::vec3 point, float xoffset, float yoffset, float d)
{

}

// updates the camera axes on any sort of input
void Camera::update()
{
    // add roll without screwing up orthogonality?
    // calculate front vector
    float angleX = euler_angle.x;
    float angleY = euler_angle.y;

    glm::vec3 newfront(cos(glm::radians(angleX)) * cos(glm::radians(angleY)), sin(glm::radians(angleY)), sin(glm::radians(angleX)) * cos(glm::radians(angleY)));
    // derive the rest from the front vector and world_up
    
    // only works as an FPS camera, a free camera with roll cannot derive both camera axes front front vector
    // you can calculate the forward vector with a spherical-to-rectangular coordinate transformation

    // 1. cross right and world_up to get a forward vector that is correct in yaw but not in pitch
    // 2. rotate forward around right through pitch
    // 3. cross forward and right to get a camera up

    // IF CAMERA FRONT IS PARALLEL TO WORLD_UP REST IN PEACE
    camera_front = glm::normalize(newfront);
    camera_right = glm::normalize(glm::cross(camera_front, world_up));
    camera_up = glm::normalize(glm::cross(camera_right, camera_front));
}