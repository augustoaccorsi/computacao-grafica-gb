#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

//GLEW
#define GLEW_STATIC

//GL
#include <GL\glew.h>

//GLFW
#include <GLFW\glfw3.h>

//GLM
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <vec2.hpp>

//Headers
#include "Shader.h"
#include "MTLWriter.h"
#include "TXTWriter.h"
#include "OBJWriter.h"