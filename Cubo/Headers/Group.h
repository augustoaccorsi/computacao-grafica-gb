#ifndef OPENGL_CG_GROUP_H
#define OPENGL_CG_GROUP_H

#include "../Face.cpp"

class Group {
public:
	string name;
	string material;
	vector<Face*> faces;
	GLuint vao;

	Group(string n, string m);
};

#endif
