#include "Camera.hpp"
#include <iostream>
#include <vector>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {

        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraUpDirection, this->cameraFrontDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {

        return glm::lookAt(this->cameraPosition, this->cameraTarget, this->cameraUpDirection);
    }

    bool inStatue(glm::vec3 toBe) {
        if (toBe.x > -724.0f)
            if (toBe.x < -706.0f)
                if (toBe.z > 233.0f)
                    if (toBe.z < 258.0f)
                        return true;
        return false;
    }

    bool inBuilding(glm::vec3 toBe) {
        if (toBe.x > -486.f)
            if (toBe.x < -317.f)
                if (toBe.z > -113.f)
                    if (toBe.z < 75.f)
                        return true;
        return false;
    }

    bool inTiribomba(glm::vec3 toBe) {
        float dist = sqrt((toBe.x + 782.f) * (toBe.x + 782.f) + (toBe.z + 137.f) * (toBe.z + 137.f));
        if (dist <= 89.f)
            return true;
        else
            return false;
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {

        glm::vec3 frontBack;

        switch (direction) {
        case MOVE_FORWARD:
            frontBack = this->cameraPosition + this->cameraFrontDirection * speed;
            if (!inStatue(frontBack) && !inBuilding(frontBack) && !inTiribomba(frontBack)) {
                if (frontBack.y < 0.f)
                    frontBack.y = 0;
                this->cameraPosition = frontBack;
            }
            break;
        case MOVE_BACKWARD:
            frontBack = this->cameraPosition - this->cameraFrontDirection * speed;
            if (!inStatue(frontBack) && !inBuilding(frontBack) && !inTiribomba(frontBack)) {
                if (frontBack.y < 0.f)
                    frontBack.y = 0;
                this->cameraPosition = frontBack;
            }
            break;
        case MOVE_LEFT:
            frontBack = this->cameraPosition + this->cameraRightDirection * speed;
            if (!inStatue(frontBack) && !inBuilding(frontBack) && !inTiribomba(frontBack)) {
                if (frontBack.y < 0.f)
                    frontBack.y = 0;
                this->cameraPosition = frontBack;
            }
            break;
        case MOVE_RIGHT:
            frontBack = this->cameraPosition - this->cameraRightDirection * speed;
            if (!inStatue(frontBack) && !inBuilding(frontBack) && !inTiribomba(frontBack)) {
                if (frontBack.y < 0.f)
                    frontBack.y = 0;
                this->cameraPosition = frontBack;
            }
            break;
        case MOVE_UP:  
            frontBack = this->cameraPosition + this->cameraUpDirection * speed;
            if (frontBack.y < 0.f)
                frontBack.y = 0;
            this->cameraPosition = frontBack;
            break;
        case MOVE_DOWN:
            frontBack = this->cameraPosition - this->cameraUpDirection * speed;
            if (frontBack.y < 0.f)
                frontBack.y = 0;
            this->cameraPosition = frontBack;
            break;
        }

        //cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraTarget = this->cameraFrontDirection + this->cameraPosition;
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraUpDirection, this->cameraFrontDirection));
        /*std::cout << this->cameraPosition.x<<" ";
        std::cout << this->cameraPosition.y<<" ";
        std::cout << this->cameraPosition.z << std::endl;*/
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        this->cameraFrontDirection = glm::normalize(direction);
        this->cameraTarget = this->cameraFrontDirection + this->cameraPosition;
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraUpDirection, this->cameraFrontDirection));
    }

    void Camera::animate(int i, glm::vec3 points[], float t) {
       
        if (i % 2 == 0) {
            if (i == 0) {
                glm::vec3 start = points[i];
                glm::vec3 end = points[i + 1];
                this->cameraTarget = end;
                this->cameraPosition = start * (1 - t) + end * t;
            }
            else {
                glm::vec3 start = points[i - 1];
                glm::vec3 end = points[i + 1];
                this->cameraTarget = end;
                this->cameraPosition = start * (1 - t) + end * t;
            }
        }
        else {
            std::cout << this->cameraTarget.x << " " << this->cameraTarget.y << " " << this->cameraTarget.z << std::endl;
            glm::vec3 start = points[i];
            glm::vec3 end = points[i + 1];
            if (this->cameraTarget == end)
                return;
            this->cameraTarget = start * (1 - t) + end * t;
        }
    }


    
}