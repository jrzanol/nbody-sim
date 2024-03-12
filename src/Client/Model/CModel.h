// by jrzanol
//

#pragma once

#include "CMesh.h"

const int MAX_OBJECT = 4096;

class CModel
{
public:
	CModel();

	void Draw(GLuint, const glm::mat4&) const;

	glm::mat4& GetModelPos() const;
	glm::vec3* GetPosition() { return &m_Position; }

	int m_Id;
	char m_Name[12];
	float m_Mass;

	std::string m_ObjName;
	std::string m_ObjDir;
	std::vector<CMesh*> m_Meshes;
	
	glm::vec3 m_Scale;
	float m_Angle;

	int m_ModelType;

	bool m_DecisionOp;
	float m_DecisionTimer;
	float m_LastAttackTimer;

	static CModel* GetModel(int);
	static CModel* GetModelFromId(int);
	static CModel* LoadModel(std::string, bool = true);
	static void DeleteModel(CModel*);
	static void DeleteAllModel();

private:
	bool m_Atived;
	glm::vec3 m_Position;

	void ProcessModelNode(aiNode*, const aiScene*);
	CMesh* ProcessModelMesh(aiMesh*, const aiScene*);

	static int listCounter;
	static CModel g_List[MAX_OBJECT];
};

