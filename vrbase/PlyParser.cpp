	///@file PlyParser.cpp
///@author Dane Coffey
///@date 2010-01-08

#include "PlyParser.H"

using namespace G3D;

char *elem_names[] = { /* list of the kinds of elements in the user's object */
  "vertex", "face", "texture"
};

PlyProperty vert_props[] = { /* list of property information for a vertex */
  {"x", PLY_FLOAT, PLY_FLOAT, offsetof(Vector3,x), 0, 0, 0, 0},
  {"y", PLY_FLOAT, PLY_FLOAT, offsetof(Vector3,y), 0, 0, 0, 0},
  {"z", PLY_FLOAT, PLY_FLOAT, offsetof(Vector3,z), 0, 0, 0, 0},
};

PlyProperty face_props[] = { /* list of property information for a vertex */
  {"v1", PLY_INT, PLY_INT, offsetof(MeshAlg::Face,vertexIndex[0]), 0, 0, 0, 0},
  {"v2", PLY_INT, PLY_INT, offsetof(MeshAlg::Face,vertexIndex[1]), 0, 0, 0, 0},
  {"v3", PLY_INT, PLY_INT, offsetof(MeshAlg::Face,vertexIndex[2]), 0, 0, 0, 0},
};

PlyProperty texture_props[] = { /* list of property information for a vertex */
  {"u", PLY_FLOAT, PLY_FLOAT, offsetof(Vector2,x), 0, 0, 0, 0},
  {"v", PLY_FLOAT, PLY_FLOAT, offsetof(Vector2,y), 0, 0, 0, 0},
};

bool
PlyParser::loadFromFile(const std::string &filename, 
			Array<Vector3> &vertices, 
			Array<int> &indices,
			Array<Vector2> &textCoord,
			double uniformScale)
{
	int nElems, nProps;
	char **elemNames;
	int fileType;
	float version;

	char *fname = new char[filename.size()+1];
	fname[filename.size()]=0;
	memcpy(fname,filename.c_str(),filename.size());
	PlyFile *plyFile = ply_open_for_reading(fname, &nElems, &elemNames, &fileType, &version);
	//expects three elements, "vertex", "face", and "texture"
	bool bVertex = false;
	bool bFace = false;
	bool bTexture = false;
	for(int i = 0; i< nElems; i++) {
		if(strcmp(elemNames[i], "vertex") == 0) {
			bVertex = true;
		}
		else if(strcmp(elemNames[i], "face") == 0) {
			bFace = true;
		}
		else if(strcmp(elemNames[i], "texture") == 0) {
			bTexture = true;
		}
	}
	if(!(bVertex && bFace && bTexture)) {
		return false;
	}

	PlyProperty **plyProps = ply_get_element_description(plyFile, "vertex", &nElems, &nProps);
	ply_get_property(plyFile, "vertex", &vert_props[0]);
	ply_get_property(plyFile, "vertex", &vert_props[1]);
	ply_get_property(plyFile, "vertex", &vert_props[2]);

	for(int i = 0; i < nElems; i++) {
		Vector3 v;
		ply_get_element(plyFile, (void*) &v);
		vertices.append(uniformScale*v);
	}

	plyProps = ply_get_element_description(plyFile, "face", &nElems, &nProps);
	ply_get_property(plyFile, "face", &face_props[0]);
	ply_get_property(plyFile, "face", &face_props[1]);
	ply_get_property(plyFile, "face", &face_props[2]);

	for(int i = 0; i < nElems; i++) {
	  MeshAlg::Face face;
	  ply_get_element(plyFile, (void*) &face);
	  if ((face.vertexIndex[0] < vertices.size()) && 
	      (face.vertexIndex[1] < vertices.size()) && 
	      (face.vertexIndex[2] < vertices.size()) &&
	      (face.vertexIndex[0] >= 0) && 
	      (face.vertexIndex[1] >= 0) && 
	      (face.vertexIndex[2] >= 0)) {

	    indices.append(face.vertexIndex[0]);
	    indices.append(face.vertexIndex[1]);
	    indices.append(face.vertexIndex[2]);
	  }
	}

	plyProps = ply_get_element_description(plyFile, "texture", &nElems, &nProps);
	ply_get_property(plyFile, "texture", &texture_props[0]);
	ply_get_property(plyFile, "texture", &texture_props[1]);

	for(int i = 0; i < nElems; i++) {
		Vector2 tc;
		ply_get_element(plyFile, (void*) &tc);
		textCoord.append(tc);
	}

	ply_close(plyFile);
	return true;
}

SMeshRef 
PlyParser::createSMeshFromFile(const std::string &filename,
                                      double uniformScale)
{
	MeshAlg::Geometry geom;
	Array<int> indices;
	Array<Vector2> textureCoord;
	if (!loadFromFile(filename, geom.vertexArray, indices, textureCoord, uniformScale)) {
		return NULL;
	}

	// Remove redundant vertices via G3D's weld routine
	Array<Vector3> newVerts;
	Array<int> toNew, toOld;
	MeshAlg::computeWeld(geom.vertexArray, newVerts, toNew, toOld);
	Array<int> newIndices;
	for (int i=0;i<indices.size();i++) {
		newIndices.append(toNew[indices[i]]);
	}
	geom.vertexArray = newVerts;
	indices = newIndices;

	MeshAlg::computeNormals(geom, indices);

	//cout << "# Vertices " << newVerts.size() << endl;
	//cout << "# Indices " << newIndices.size() << endl;

	SMeshRef mesh = new SMesh(geom.vertexArray, geom.normalArray, indices, textureCoord);
	return mesh;
}
