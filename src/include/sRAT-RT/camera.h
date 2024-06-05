/*
/   Based on the work by Joey de Vries in his tutorials from learnopengl
/   Code has been modified to fit the needs for this project
*/
#ifndef CAMERA_CLASS_H_
#define CAMERA_CLASS_H_

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

/// Default camera values
/// TODO: Change and put this in the .ini?
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float WIDTH = 800.0f;
const float HEIGHT = 600.0f;
const float _NEAR = 0.1f;
const float _FAR = 100.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float cam_width;
    float cam_height;
    float near_plane;
    float far_plane;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float width = WIDTH, float height = HEIGHT, float near = _NEAR, float far = _FAR)
     : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), near_plane(near), far_plane(far)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        cam_width = width;
        cam_height = height;

        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch,
                        float width = WIDTH, float height = HEIGHT, float near = _NEAR, float far = _FAR) 
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), near_plane(near), far_plane(far)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        cam_width = width;
        cam_height = height;

        updateCameraVectors();
    }
    
    glm::mat4 get_view_matrix();
    glm::mat4 get_projection_matrix();
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
    void ProcessMouseScroll(float yoffset);

    float get_near();
    float get_far();
    
    void updateCameraVectors();
    
};
#endif