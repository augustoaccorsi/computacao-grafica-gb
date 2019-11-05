#ifndef OPENGL_CG_OBJ3D_H
#define OPENGL_CG_OBJ3D_H

#include "Mesh.h"

class Obj3D {
public:
	Mesh* mesh;
	vector<Material*> materials;
	glm::mat4 ModelMatrix;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	void Inicializar() {
		this->position = glm::vec3(0.1f, 0.1f, 0.1f);
		this->rotation = glm::vec3(0.0f);
		this->scale = glm::vec3(1.f);
	}

	void transform() {
		this->ModelMatrix = glm::mat4(1.0f);
		this->ModelMatrix = glm::translate(this->ModelMatrix, this->position);
		this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
		this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
		this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
		this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);

	}

	Mesh* Obj3D::processObj(const string filename) {
		Mesh* mesh = readOBJ(filename);
		loadVertices(mesh);
		return mesh;
	}

	vector<Material*> getMat() {
		return materials;
	}

private:

	GLuint loadTexture(const char* filename) {
		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

		int texWidth, texHeight, nrChannels;
		unsigned char* data;

		GLuint texture;

		// Loading image with filename from parameter
		data = stbi_load(filename, &texWidth, &texHeight, &nrChannels, 0);

		glGenTextures(1, &texture);

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, texture);

		if (data) {
			// The image I chose has no alpha channel, so using GL_RGB to process it
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);

		return texture;
	}

	void readMTL(const string filename) {
		Material* m = nullptr;

		ifstream arq(filename);

		while (!arq.eof()) {
			string line;
			getline(arq, line);
			stringstream sline;
			sline << line;
			string temp;
			sline >> temp;

			if (temp == "newmtl") {
				if (m != nullptr) {
					materials.push_back(m);
				}
				string mtlName;
				sline >> mtlName;
				m = new Material(mtlName);
			}
			else if (temp == "Ka") {
				float r, g, b;
				sline >> r >> g >> b;
				m->ka = new glm::vec3(r, g, b);
			}
			else if (temp == "Kd") {
				float r, g, b;
				sline >> r >> g >> b;
				m->kd = new glm::vec3(r, g, b);
			}
			else if (temp == "Ks") {
				float r, g, b;
				sline >> r >> g >> b;
				m->ks = new glm::vec3(r, g, b);
			}
			else if (temp == "Ns") {
				float ns;
				sline >> ns;
				m->ns = ns;
			}
			else if (temp == "map_Kd") {
				string textureFile;
				sline >> textureFile;
				m->texture = loadTexture(textureFile.c_str());
			}
		}
		materials.push_back(m);
	}

	Mesh* readOBJ(const string filename) {
		auto mesh = new Mesh;
		Group* g = nullptr;

		ifstream arq(filename);

		while (!arq.eof()) {
			string line;
			getline(arq, line);
			stringstream sline;
			sline << line;
			string temp;
			sline >> temp;
			if (temp == "mtllib") {
				string mtlFile;
				sline >> mtlFile;
				readMTL(mtlFile);
			}
			else if (temp == "v") {
				float x, y, z;
				sline >> x >> y >> z;
				auto* v = new glm::vec3(x, y, z);
				mesh->vertex.push_back(v);
			}
			else if (temp == "vt") {
				float x, y;
				sline >> x >> y;
				auto* v = new glm::vec2(x, y);
				mesh->mappings.push_back(v);
			}
			else if (temp == "vn") {
				float x, y, z;
				sline >> x >> y >> z;
				auto* v = new glm::vec3(x, y, z);
				mesh->normals.push_back(v);
			}
			else if (temp == "g") {
				if (g != nullptr) {
					mesh->groups.push_back(g);
				}
				string inName;
				sline >> inName;
				g = new Group(inName, "default");
			}
			else if (temp == "usemtl") {
				if (g == nullptr) {
					g = new Group("default", "default");
				}
				string inMaterial;
				sline >> inMaterial;
				g->material = inMaterial;
			}
			else if (temp == "f") {
				if (g == nullptr) {
					g = new Group("default", "default");
				}
				auto* f = new Face();
				while (!sline.eof()) {
					string token;
					sline >> token;
					if (token.empty()) {
						continue;
					}
					stringstream stoken;
					stoken << token;
					string aux[3];
					int countParam = -1;
					do {
						countParam = countParam + 1;
						getline(stoken, aux[countParam], '/');
					} while (!stoken.eof());
					for (int i = 0; i < 3; i = i + 1) {
						switch (i) {
						case 0:
							if (aux[i].empty()) {
								f->verts.push_back(-1);
							}
							else {
								f->verts.push_back(stoi(aux[i]) - 1);
							}
							break;
						case 1:
							if (aux[i].empty()) {
								f->texts.push_back(-1);
							}
							else {
								f->texts.push_back(stoi(aux[i]) - 1);
							}
							break;
						case 2:
							if (aux[i].empty()) {
								f->norms.push_back(-1);
							}
							else {
								f->norms.push_back(stoi(aux[i]) - 1);
							}
							break;
						default:
							break;
						}
					}
				}
				g->faces.push_back(f);
			}
		}
		mesh->groups.push_back(g);
		return mesh;
	}

	void loadVertices(Mesh* mesh) {

		for (Group* g : mesh->groups) {
			vector<float> vs;
			vector<float> vn;
			vector<float> vt;

			for (Face* f : g->faces) {
				for (int i = 0; i < f->verts.size(); i = i + 1) {
					int vi = f->verts[i];
					glm::vec3* v = mesh->vertex[vi];
					vs.push_back(v->x);
					vs.push_back(v->y);
					vs.push_back(v->z);
				}

				for (int i = 0; i < f->verts.size(); i = i + 1) {
					int vi = f->norms[i];
					glm::vec3* v = mesh->normals[vi];
					vn.push_back(v->x);
					vn.push_back(v->y);
					vn.push_back(v->z);
				}

				for (int i = 0; i < f->verts.size(); i = i + 1) {
					int vi = f->texts[i];
					glm::vec2* v = mesh->mappings[vi];
					vt.push_back(v->x);
					vt.push_back(v->y);
				}
			}

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// VBO for vertices
			GLuint vboVerts;
			glGenBuffers(1, &vboVerts);
			glBindBuffer(GL_ARRAY_BUFFER, vboVerts);
			glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(float), vs.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			// VBO for normals
			GLuint vboNorms;
			glGenBuffers(1, &vboNorms);
			glBindBuffer(GL_ARRAY_BUFFER, vboNorms);
			glBufferData(GL_ARRAY_BUFFER, vn.size() * sizeof(float), vn.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			// VBO for texture mappings
			GLuint vboTexts;
			glGenBuffers(1, &vboTexts);
			glBindBuffer(GL_ARRAY_BUFFER, vboTexts);
			glBufferData(GL_ARRAY_BUFFER, vt.size() * sizeof(float), vt.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

			// Store VAO for group
			g->vao = vao;
		}
	}
};

#endif //OPENGL_CG_OBJ3D_H