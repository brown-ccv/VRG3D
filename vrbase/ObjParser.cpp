///@file ObjParser.cpp
///@author Marcus Henry Ewert/Vamsi Konchada
///@date 2009-07-20


#include "ObjParser.H"

using namespace G3D;

int
ObjParser::tokenizeLine(const std::string& str,
                      vector<std::string>& tokens,
                      const std::string& delimiters)
{
	int delimitNum = 0;
	int len = strlen(str.c_str());	
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	if(lastPos != 0)
		delimitNum += lastPos;	
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
		if(lastPos!=pos) {
			if(lastPos == string::npos)
				delimitNum+= (len-pos);
			else
				delimitNum+= (lastPos-pos);			
		}		
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
	return delimitNum;	
}

 // Currently only loads materials and diffuse color from a .mtl file
Table<std::string, Color3>
ObjParser::loadMaterialsFromFile(const std::string &filename)
{
    Table<std::string, Color3> map;
    TextFileReader tf(filename);
    std::string line;
    std::string miscText;
    std::string key;
    float r, g, b;
    bool notDone = true;
    while(notDone) {
	notDone = tf.getTextUntilToken("newmtl", miscText, false);
	if (notDone) {
	    tf.getNextLine(line);
	    key = line.substr(7);
	    tf.getTextUntilToken("Kd", miscText);
	    tf.getNextLine(line);
	    sscanf(line.c_str(), " %f %f %f", &r, &g, &b);
	    Color3 color(r,g,b);
	    map.set(key, color);
	    cout << "Adding new material color: "<<key<<" "<<color<<endl;
	}
    }
    return map;
}

bool
ObjParser::loadFromFileWithNormals(const std::string &filename, 
                          Array<Vector3> &vertices, 
						  Array<Vector3> &normals,
                          Array<int> &indices,
						  Array<Vector2> &textCoord,
                          Array<Color3> &vertColors,
                          double uniformScale)
{
	//instead of sscanf, the tokens can be verified if they are int/float 
	//wherever required will impose stricter condition checks in the Parser	
	TextFileReader tf(filename);
	vector<std::string> lineTokens;
	int lastPerc = 0;
	std::string line;
	Array<Vector3> tmpVertices;
	Array<Vector3> tmpNormals;
	Array<Vector2> tempTexCoords;
	Array<Vector2> tmpVertColors;
	Table<std::string, Color3> materials;
	std::string curMtlKey = "";
	while (tf.getNextLine(line)) {
		tokenizeLine(line,lineTokens, " ");
		if(lineTokens.size() == 0 || lineTokens[0][0] == '#') {
			cout<<"Empty line"<<endl;
		} else if (lineTokens[0] == "mtllib") {
		    	materials = loadMaterialsFromFile(FilePath::concat(FilePath::parent(filename), lineTokens[1]));
		} else if(lineTokens[0] == "v") {
			float n1, n2, n3, wt; 
			char c;
			wt = 1.0;			
			if(lineTokens.size() == 4) {								
				sscanf(line.c_str(), "%c %f %f %f", &c, &n1, &n2, &n3);				
			} else if(lineTokens.size() == 5) {
				sscanf(line.c_str(), "%c %f %f %f %f", &c, &n1, &n2, &n3, &wt);
				if(wt<= 0.0) {
					cout<<"ObjParser: Illegal syntax for vertex information!"<<endl;
					return false;
				}				
			} else {
				cout<<"ObjParser: Illegal syntax for vertex information!"<<endl;
				return false;
			}
			Vector3 v(n1,n2,n3);
			tmpVertices.append(uniformScale*v);
		} else if(lineTokens[0] == "vn") {
			float n1, n2, n3; 
			char c;
			if(lineTokens.size() == 4) {
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &n1, &n2, &n3);
			} else {
				cout<<"ObjParser: Illegal syntax for normal information!"<<endl;
				return false;
			}
			Vector3 n(n1,n2,n3);
			tmpNormals.append(n);
		} else if(lineTokens[0] == "vt") {
			float u, v, w; 
			char c;
			v = w = 0.0;
			if(lineTokens.size() == 2) {
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &u);
			} else if(lineTokens.size() == 3) {	
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &u, &v);
			} else if(lineTokens.size() == 4) {
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &u, &v, &w);
			} else {
				cout<<"ObjParser: Illegal syntax for texture coordinates information!"<<endl;
				return false;
			}
			Vector2 t(u,v);
			tempTexCoords.append(t);
		} else if(lineTokens[0] == "usemtl") {
			std:string mtl = lineTokens[1];
			if (materials.containsKey(mtl)) {
				curMtlKey = mtl;
				cout << "Setting current material: "<<curMtlKey<<endl;
			}
			else {
				curMtlKey = "";
			}
		} else if(lineTokens[0] == "f") {
			//cout<<line<<" - ";
			int i1, i2, i3;
			int arrsize = lineTokens.size() - 1;
			//int i_arr[arrsize];  // TODO: NOT LEGAL C++
			//int t_arr[arrsize];
			//int n_arr[arrsize];
			Array<int> i_arr;
			Array<int> t_arr;
			Array<int> n_arr;
			i_arr.resize(arrsize);
			t_arr.resize(arrsize);
			n_arr.resize(arrsize);
			char c;
			
			if(lineTokens.size() >= 4) {
		
				vector<string> fInfoTokens;
				for(int i = 1; i < lineTokens.size(); i++) {
					int delimNum = tokenizeLine(lineTokens[i], fInfoTokens, "/");
					if(delimNum > 2) {
						cout<<"ObjParser: Illegal syntax for face information!"<<endl;
						return false;						
					}		
					if(fInfoTokens.size()== 1) {
						//valid possibilites x, x/, x// 
						//others /x/, //x
						sscanf(fInfoTokens[0].c_str(), "%d", &i_arr[i-1]);
					} else if(fInfoTokens.size()== 2) {
						//valid possibilites x/y, x/y/, x//y
						//others /x/y 
						sscanf(fInfoTokens[0].c_str(), "%d", &i_arr[i-1]);	
						sscanf(fInfoTokens[1].c_str(), "%d", &t_arr[i-1]);
					} else if(fInfoTokens.size()== 3) {
						//possibilities x/y/z
						sscanf(fInfoTokens[0].c_str(), "%d", &i_arr[i-1]);	
						sscanf(fInfoTokens[1].c_str(), "%d", &t_arr[i-1]);
						sscanf(fInfoTokens[2].c_str(), "%d", &n_arr[i-1]);
					} else {
						cout<<"ObjParser: Illegal syntax for face information!"<<endl;
						return false;
					}
					fInfoTokens.clear();
				}
				vertices.append(tmpVertices[i_arr[0]-1]);
				textCoord.append(tempTexCoords[t_arr[0]-1]);
				normals.append(tmpNormals[n_arr[0]-1]);
				indices.append(vertices.size()-1);
				vertices.append(tmpVertices[i_arr[1]-1]);
				textCoord.append(tempTexCoords[t_arr[1]-1]);
				normals.append(tmpNormals[n_arr[1]-1]);
				indices.append(vertices.size()-1);
				vertices.append(tmpVertices[i_arr[2]-1]);
				textCoord.append(tempTexCoords[t_arr[2]-1]);
				normals.append(tmpNormals[n_arr[2]-1]);
				indices.append(vertices.size()-1);

				if (curMtlKey != "") {
				   	vertColors.append(materials[curMtlKey]);
				   	vertColors.append(materials[curMtlKey]);
				   	vertColors.append(materials[curMtlKey]);
				}	
			} else {
				cout<<"ObjParser: Illegal syntax for face information!"<<endl;
				return false;				
			}
		}
		lineTokens.clear();		
	}
	return true;
}

bool
ObjParser::loadFromFile(const std::string &filename, 
                          Array<Vector3> &vertices, 
                          Array<int> &indices,
			  Array<Vector2> &textCoord,
                          Array<Color3> &vertColors,
                          double uniformScale)
{
	//instead of sscanf, the tokens can be verified if they are int/float 
	//wherever required will impose stricter condition checks in the Parser	
	TextFileReader tf(filename);
	vector<std::string> lineTokens;
	int lastPerc = 0;
	std::string line;
	Array<Vector3> normals;
	Array<Vector2> tempTexCoords;
	Table<std::string, Color3> materials;
	std::string curMtlKey = "";
	while (tf.getNextLine(line)) {
		tokenizeLine(line,lineTokens, " ");
		if(lineTokens.size() == 0 || lineTokens[0][0] == '#') {
			cout<<"Empty line"<<endl;
		} else if (lineTokens[0] == "mtllib") {
		    	materials = loadMaterialsFromFile(FilePath::concat(FilePath::parent(filename), lineTokens[1]));
		} else if(lineTokens[0] == "v") {
			float n1, n2, n3, wt; 
			char c;
			wt = 1.0;			
			if(lineTokens.size() == 4) {								
				sscanf(line.c_str(), "%c %f %f %f", &c, &n1, &n2, &n3);				
			} else if(lineTokens.size() == 5) {
				sscanf(line.c_str(), "%c %f %f %f %f", &c, &n1, &n2, &n3, &wt);
				if(wt<= 0.0) {
					cout<<"ObjParser: Illegal syntax for vertex information!"<<endl;
					return false;
				}				
			} else {
				cout<<"ObjParser: Illegal syntax for vertex information!"<<endl;
				return false;
			}
			Vector3 v(n1,n2,n3);
			vertices.append(uniformScale*v);
			if (materials.size() != 0) {
				vertColors.resize(vertColors.size()+1);
			}			
		} else if(lineTokens[0] == "vn") {
			float n1, n2, n3; 
			char c;
			if(lineTokens.size() == 4) {
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &n1, &n2, &n3);
			} else {
				cout<<"ObjParser: Illegal syntax for normal information!"<<endl;
				return false;
			}
			Vector3 n(n1,n2,n3);
			normals.append(n);
		} else if(lineTokens[0] == "vt") {
			float u, v, w; 
			char c;
			v = w = 0.0;
			if(lineTokens.size() == 2) {
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &u);
			} else if(lineTokens.size() == 3) {	
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &u, &v);
			} else if(lineTokens.size() == 4) {
				sscanf(line.c_str(), "%c %c %f %f %f", &c, &c, &u, &v, &w);
			} else {
				cout<<"ObjParser: Illegal syntax for texture coordinates information!"<<endl;
				return false;
			}
			Vector2 t(u,v);
			//tempTexCoords.append(t);
			textCoord.append(t);
		} else if(lineTokens[0] == "usemtl") {
			std:string mtl = lineTokens[1];
			if (materials.containsKey(mtl)) {
				curMtlKey = mtl;
				cout << "Setting current material: "<<curMtlKey<<endl;
			}
			else {
				curMtlKey = "";
			}
		} else if(lineTokens[0] == "f") {
			//cout<<line<<" - ";
			int i1, i2, i3;
			int arrsize = lineTokens.size() - 1;
			//int i_arr[arrsize];  // TODO: NOT LEGAL C++
			//int t_arr[arrsize];
			//int n_arr[arrsize];
			Array<int> i_arr;
			Array<int> t_arr;
			Array<int> n_arr;
			i_arr.resize(arrsize);
			t_arr.resize(arrsize);
			n_arr.resize(arrsize);
			char c;
			
			if(lineTokens.size() >= 4) {
		
				vector<string> fInfoTokens;
				for(int i = 1; i < lineTokens.size(); i++) {
					int delimNum = tokenizeLine(lineTokens[i], fInfoTokens, "/");
					if(delimNum > 2) {
						cout<<"ObjParser: Illegal syntax for face information!"<<endl;
						return false;						
					}		
					if(fInfoTokens.size()== 1) {
						//valid possibilites x, x/, x// 
						//others /x/, //x
						sscanf(fInfoTokens[0].c_str(), "%d", &i_arr[i-1]);
					} else if(fInfoTokens.size()== 2) {
						//valid possibilites x/y, x/y/, x//y
						//others /x/y 
						sscanf(fInfoTokens[0].c_str(), "%d", &i_arr[i-1]);	
						sscanf(fInfoTokens[1].c_str(), "%d", &t_arr[i-1]);
					} else if(fInfoTokens.size()== 3) {
						//possibilities x/y/z
						sscanf(fInfoTokens[0].c_str(), "%d", &i_arr[i-1]);	
						sscanf(fInfoTokens[1].c_str(), "%d", &t_arr[i-1]);
						sscanf(fInfoTokens[2].c_str(), "%d", &n_arr[i-1]);
					} else {
						cout<<"ObjParser: Illegal syntax for face information!"<<endl;
						return false;
					}
					fInfoTokens.clear();
				}
				Array<int> face;
				face.append(i_arr[0]-1);
				face.append(i_arr[1]-1);
				face.append(i_arr[2]-1);
				//cout<<i_arr[0]-1<<" "<<i_arr[1]-1<<" "<<i_arr[2]-1<<endl;	
				if ((face[0] < vertices.size()) &&  (face[1] < vertices.size()) &&  (face[2] < vertices.size()) && (face[0] >= 0) &&  (face[1] >= 0) &&  (face[2] >= 0)) {
					indices.append(face);
				} else {
					cout<<"face info lost"<<endl;
				}
				if (curMtlKey != "") {
				    	vertColors[face[0]] = materials[curMtlKey];
					vertColors[face[1]] = materials[curMtlKey];
					vertColors[face[2]] = materials[curMtlKey];					
				}	
			} else {
				cout<<"ObjParser: Illegal syntax for face information!"<<endl;
				return false;				
			}
		}
		lineTokens.clear();		
	}
	return true;
}

                               
SMeshRef
ObjParser::createSMeshFromFile(const std::string &filename, double uniformScale)
{
  MeshAlg::Geometry geom;
  Array<int> indices;
  Array<Vector2> texCoords;
  Array<Color3> vertColors;
  if (!loadFromFile(filename, geom.vertexArray, indices, texCoords, vertColors, uniformScale)) {	  
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

  SMeshRef mesh;
  if (vertColors.size() != 0) {
      mesh = new SMesh(geom.vertexArray, geom.normalArray, vertColors, indices);
  }
  else {
      mesh = new SMesh(geom.vertexArray, geom.normalArray, indices, texCoords);
  }
  return mesh;
}

SMeshRef
ObjParser::createSMeshFromFileWithNormals(const std::string &filename, double uniformScale)
{
  MeshAlg::Geometry geom;
  Array<int> indices;
  Array<Vector2> texCoords;
  Array<Color3> vertColors;
  if (!loadFromFileWithNormals(filename, geom.vertexArray, geom.normalArray, indices, texCoords, vertColors, uniformScale)) {	  
    return NULL;
  }

  SMeshRef mesh;
  if (vertColors.size() != 0) {
      mesh = new SMesh(geom.vertexArray, geom.normalArray, vertColors, indices);
  }
  else {
      mesh = new SMesh(geom.vertexArray, geom.normalArray, indices, texCoords);
  }
  return mesh;
}

SMeshRef
ObjParser::createColoredSMeshFromFile(const std::string &filename, Vector3 critVertex, double uniformScale)
{
  MeshAlg::Geometry geom;
  Array<int> indices;
  Array<Vector2> texCoords;
  Array<Color3> vertColors;
  if (!loadFromFile(filename, geom.vertexArray, indices, texCoords, vertColors, uniformScale)) {
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

  SMeshRef mesh = new SMesh(geom.vertexArray, geom.normalArray, indices);

  Array<Color3> heartColors;
  for(int i = 0; i < geom.vertexArray.size(); i++) {
    double len = (geom.vertexArray[i] - critVertex).magnitude();
    if(len < 0.1) {
      heartColors.append(Color3::green());
    }
    else {
      heartColors.append(Color3(0.845, 0.586, 0.71));
    }
  }
  mesh->ApplyColoring(heartColors);
  
  return mesh;
}


