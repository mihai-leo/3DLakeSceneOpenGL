#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {

        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)); // Right direction
     this->lastReadPos = 0;

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {

      // cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        if (direction == MOVE_FORWARD) {
            cameraPosition += cameraFrontDirection * speed;
        }
        else if (direction == MOVE_BACKWARD) {
            cameraPosition -= cameraFrontDirection * speed;
        }
        else if (direction == MOVE_LEFT) {
            cameraPosition -= cameraRightDirection * speed;
        }
        else if (direction == MOVE_RIGHT) {
            cameraPosition += cameraRightDirection * speed;
        }
        else if (direction == MOVE_UP) {
            cameraPosition -= cross(cameraFrontDirection,cameraRightDirection)* speed;
        }
        else if (direction == MOVE_DOWN) {
            cameraPosition += cross(cameraFrontDirection, cameraRightDirection) * speed;
        }
       // cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

    }
    

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(front);
        //cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }
    void Camera::printLocation() {
        std::ofstream outFile("data.txt", std::ios::app); // Open file in append mode
        if (outFile.is_open()) {
            outFile << cameraPosition.x << "  "
                << cameraPosition.y << "  "
                << cameraPosition.z << " "
                << cameraFrontDirection.x << "  "
                << cameraFrontDirection.y << "  "
                << cameraFrontDirection.z <<" "
                << cameraRightDirection.x << "  "
                << cameraRightDirection.y << "  "
                << cameraRightDirection.z
                << std::endl;
            outFile.close(); // Close the file after writing
          
        }
        else {
            std::cerr << "Failed to open the file: data.txt\n";
        }
    }
    void Camera::newLocation()
    {
       
        std::ifstream inFile("data.txt");

        if (inFile.is_open()) {
            inFile.seekg(lastReadPos);
            float x, y, z, fx, fy, fz, rx, ry, rz;

            // Read camera position (x, y, z)
            if (inFile >> x >> y >> z) {
                cameraPosition = glm::vec3(x, y, z);  // Update the camera position
                printf("Camera Position: %f %f %f\n", x, y, z);

                // Read camera front direction (x1, y1, z1)
                if (inFile >> fx >> fy >> fz) {
                    cameraFrontDirection = glm::vec3(fx, fy, fz);  // Update the camera front direction
                    printf("Camera Front Direction: %f %f %f\n", fx, fy, fz);
                }

                // Read camera right direction (x2, y2, z2)
                if (inFile >> rx >> ry >> rz) {
                 //   cameraRightDirection = glm::vec3(rx, ry, rz);  // Update the camera right direction
                    printf("Camera Right Direction: %f %f %f\n", rx, ry, rz);
                }

                // Update the last read position in the file
                lastReadPos = inFile.tellg();
            }

            inFile.close(); // Close the file
            std::cout << "Camera positions read successfully from " << "data.txt" << ".\n";
        }
        else {
            std::cerr << "Failed to open the file: " << "data.txt" << "\n";
        }
    }

}
