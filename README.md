# 3DLakeSceneOpenGL
1. Subject specification: 
The implementation of a scene simulation by using openGL taking into consideration the 
next key features: 
● Creating the 3D scene: generating a realist environment using geometrical object, 
as well as applying transformations to the objects such as rotation 
● Integration of Viewing system: implementing a camera to navigate each point of 
the scene 
● Texture Mapping: applying texture to enhance the realism of object, an using it to 
create the transparency of the water 
● Lighting and Shading Techniques:use directional and point light sources to 
simulate realistic lighting, using the Phong lighting model to compute ambient, 
diffuse, and specular components, implementing shadow mapping for dynamic 
shadows. 
● Animation: implement the animation of a object 
● User Control: allowing the user to control the animation and the camera 
movement by using the keyboard and mouse 
2. Scenario: 
2.1 Scene and Objects: 
Lake object:the main object, it contains the bigger part of the scene, the landscape, trees, 
houses, and the unmoving crocodile 
Crocodile object:the crocodile that is moving 
LakeWater object:the water of the lake in scene, has a lower transparence; 
LightCube object: Object that corresponds with the directional light source
![LScene](https://github.com/user-attachments/assets/eefb611d-688a-4afa-a51f-5a0a9ea9b265)
![image](https://github.com/user-attachments/assets/3aaef4df-d8df-402e-9ddc-f86bc0696f43)
![image](https://github.com/user-attachments/assets/e9cb0d93-882e-4ddb-bbfe-e3649cc64a27)

4. Implementation: 
3.1 Camera Implementation: 
● constructor: creates the camera using its position, target and up vectors, it also 
calculates the front and right directions 
● getViewMatrix(): calls the lookAt function that returns the viewing matrix using 
the camera position, up, and target(position + front direction); 
● move(): moves the camera around its the 3 dimensions, on the directions of the 
front, right vectors and there cross product; 
● rotate(): rotates the camera, by calculating the new front direction using the pitch 
and yaw, the rotations on x and y 
● printLocation()and newLocation(): functions us to register an animation(the 
camera movement, its location and target direction) and the to redo the movement 
when the second function is called 
3.2 Main Implementation: 
● keyboardCallback(): hendels the keyboard key, to remember if a key is holed, as 
well as switching between modes and enabling animation 
● mouseCallback(): calculate the pitch and yaw that will be passed to the camera 
rotate function, using the mouse positions on the screen 
● processMovement():if a key is being pressed the camera is moved(detailed 
explanation on user interface) , updating the camera obj and passing the values to 
the shader 
● initModels() and initShaders(): this functions load the 3D models and the shaders 
used by the program 
● initUniform(): sends the uniforms for: model,view,normalMatrix,projection, 
lightDir and color and point light to the shaders, 
● initFBO(): sets up the framebuffer for shadow mapping, creates the textures that 
is attached to it, at the end the framer is unbound 
● computeLightSpaceMatrix(): compute the scene from the perspective of the light 
source by calling the lookAt function, is also uses an orthographic projection, 
because the light rays are parallel 
● viewMode(): changes between solid, wireform and point modes 
● drawObject():drows the 3 objects, the landscape, the crocodile and lakewater, all 
3 can rotate around the x axes, the crocodile can move and the water hai a lower 
transparency; 
● renderScene(): draws and updates the objects in the 3D scene, it  renders the 
shadow map, than the shadow map if the function is activated, but it maine draws 
the the objects of the scene using the basic shader, and  creates the movement of 
the light and the binding of shadows 
3.3 Shader Implementation 
● computeLightComponents(): computes the ambient diffuse and specular for the 
direction light. Ambient Light ensures the obj are visible even in shadow areas, 
diffuse simultes light scattering on the object, while the specular light makes the 
surfaces appear glossy 
● computeShadows(): determines whether a fragment is in shadow or illuminated 
by the light source by comparing the depth of the fragment with the shadow map 
to determine occlusion, 
● computePointLight():the ambient,diffuse and specular are centred around the 
point light and the attention make the light diminish by distance 
● main(): calls the previous functions, and calculates the color 
5. User Manual 
Move mouse: changes the direction of the camera 
Keys: 
● 1:starts the animation 
● 2: turn on/off light 1 
● 3: turn on/off light 2 
● 4: shows the wireframe model 
● 5: shows the point frame model 
● 6: show fog 
● W: moves the camera forward 
● S: moves the camera backward 
● A: moves the camera left 
● D: moves the camera right 
● R: moves the camera up 
● F: moves the camera down 
● L: rotates the light source 
● Q: rotates the scene to the left 
● E: Rotates the scene to the right 
6. Conclusions and further developments 
5.1 Conclusion: 
The implementation of the lake scene project, showcases a an effective integration of 
advanced OpenGL concepts such ass:  
● shadow mapping 
● dynamic lighting 
● custom shaders 
The scene showcases a visually engaging environment with realistic lighting and 
reflection effects, brought to life by a moving crocodile, translucent lake water, and a moving 
camera system.  
5.2 Extend: 
● implementation of the rain effect 
● adding more light sources, primari a spot light 
● improve the animation by adding more moving object 
● make the surface of the water reflective 
● implement the wind animation  
7. References 
https://moodle.cs.utcluj.ro/course/view.php?id=669 
https://stackoverflow.com/questions/18544089/opengl-3-with-orthographic-projection-of-directi
 onal-light 
https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping 
https://www.turbosquid.com/Search/Index.cfm?keyword=crocodile+LOW+POLI&media_typeid
 =2&max_price=0 
https://www.ogldev.org/www/tutorial20/tutorial20.html
