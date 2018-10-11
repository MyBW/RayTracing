#include "Object.h"
#include <vector>
#include "..\Math\BWPrimitive.h"
#include "..\RealTimeRenderer\RealTimeRenderer.h"
#include "..\Test\HelpFunc.h"

Object::Object():Position(0.0 , 0.0, 0.0), Scale(1.0, 1.0, 1.0)
{
	Orientation.w = 1.0;
	Orientation.x = 0;
	Orientation.y = 0;
	Orientation.z = 0;
}

bool Object::LoadObjModel(const char* FileName,const std::string &Name)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices; //��ʱ�洢��
	std::vector<BWVector3D> temp_vertices;
	std::vector<BWPoint2DD> temp_uvs;
	std::vector<BWVector3D> temp_normals;


	FILE * file = fopen(FileName, "r"); //��obj�ļ�
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	this->Name = Name;
	while (1) { //����ִ��

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
				   //�ж�ÿ�е�����ĸ�����֣���Ӧ�洢����ͬ��������ȥ
		if (strcmp(lineHeader, "v") == 0) {
			BWVector3D vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			BWPoint2DD uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			BWVector3D normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			char sLineWord[200];
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			fscanf(file, "%[^\n]%*c", sLineWord);

			int matchNum = sscanf(sLineWord, "%d %d %d", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matchNum == 3)
			{
				vertexIndices.push_back(vertexIndex[0] - 1);
				vertexIndices.push_back(vertexIndex[1] - 1);
				vertexIndices.push_back(vertexIndex[2] - 1);
				continue;
			}
			matchNum = sscanf(sLineWord, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matchNum == 9)
			{
				vertexIndices.push_back(vertexIndex[0] - 1);
				vertexIndices.push_back(vertexIndex[1] - 1);
				vertexIndices.push_back(vertexIndex[2] - 1);
				uvIndices.push_back(uvIndex[0] - 1);
				uvIndices.push_back(uvIndex[1] - 1);
				uvIndices.push_back(uvIndex[2] - 1);
				normalIndices.push_back(normalIndex[0] - 1);
				normalIndices.push_back(normalIndex[1] - 1);
				normalIndices.push_back(normalIndex[2] - 1);
				continue;
			}
			matchNum = sscanf(sLineWord, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
			if (matchNum == 6)
			{
				vertexIndices.push_back(vertexIndex[0] - 1);
				vertexIndices.push_back(vertexIndex[1] - 1);
				vertexIndices.push_back(vertexIndex[2] - 1);
				normalIndices.push_back(normalIndex[0] - 1);
				normalIndices.push_back(normalIndex[1] - 1);
				normalIndices.push_back(normalIndex[2] - 1);
				continue;
			}
			matchNum = sscanf(sLineWord, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
			if (matchNum == 6)
			{
				vertexIndices.push_back(vertexIndex[0] - 1);
				vertexIndices.push_back(vertexIndex[1] - 1);
				vertexIndices.push_back(vertexIndex[2] - 1);
				uvIndices.push_back(uvIndex[0] - 1);
				uvIndices.push_back(uvIndex[1] - 1);
				uvIndices.push_back(uvIndex[2] - 1);
				continue;
			}
			printf("File can't be read by our simple parser :-( Try exporting with other options\n");
			return false;
		}
		else {
			// Probably a comment, eat up the rest of the line����������δ֪���ڴ˴���Ҳ���ܻ����������������Ϊ��Сֻ��1000
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle������������ݴ洢����Ӧ��VAO��
	PosData.clear();
	UVData.clear();
	NormalData.clear();
	IndexData = vertexIndices;
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		PosData.push_back(temp_vertices[vertexIndex]);

		if (temp_uvs.size() != 0 && uvIndices.size() != 0)
		{
			unsigned int uvIndex = uvIndices[i];
			UVData.push_back(temp_uvs[uvIndex]);
		}
		if (normalIndices.size() != 0 && temp_normals.size() != 0)
		{
			unsigned int normalIndex = normalIndices[i];
			NormalData.push_back(temp_normals[normalIndex]);
		}
	}
	ModelMatrix = BWMatrix4::IDENTITY;
	GetLightMapData("uvs1-export.buf", LightmapUV);
	return true;

}

const std::vector<BWVector3D>& Object::GetPosData() const
{
	return PosData;
}

const std::vector<BWPoint2DD>& Object::GetUVData() const
{
	return UVData;
}

const std::vector<BWVector3D>& Object::GetNormalData() const
{
	return NormalData;
}

const std::vector<unsigned int>& Object::GetIndexData() const
{
	return IndexData;
}

const std::vector<BWPoint2DD>& Object::GetLightmapUVData() const
{
	return LightmapUV;
}

void Object::SetPosition(float X, float Y, float Z)
{
	Position.x = X;
	Position.y = Y;
	Position.z = Z;
	UpdateModelMatrix();
}

void Object::SetRotation(float w, float x, float y, float z)
{
	Orientation.set(w, x, y, z);
	UpdateModelMatrix();
}

void Object::SetRoataion(const BWVector3D& axis, const Radian &angle)
{
	BWQuaternion q;
	q.fromAngleAxis(angle, axis);
	q.normalize();
	Orientation = Orientation * q;
	UpdateModelMatrix();
}

void Object::SetScale(float x, float y, float z)
{
	Scale.x = x;
	Scale.y = y;
	Scale.z = z;
	UpdateModelMatrix();
}

int Object::GetTriangleNum()
{
	return IndexData.size() / 3;
}

bool Object::GetTriangleInfoByIndex(int Index, TriangleInfo &Reslut)
{
	if (Index >= GetTriangleNum())
	{
		return false;
	}
	for (int i = 0 ;i < 3; i++)
	{
		Reslut.P.push_back(ModelMatrix * PosData[Index * 3 + i]);
		Reslut.N.push_back(ModelMatrix *NormalData[Index * 3 + i]);
		Reslut.UV.push_back(UVData[Index * 3 + i]);
	}
}

bool Object::GetTriangleVertexIndex(int Index, int &P1, int &P2, int &P3)
{
	if (Index >= GetTriangleNum())
	{
		return false;
	}
	P1 = IndexData[Index];
	P2 = IndexData[Index + 1];
	P3 = IndexData[Index + 2];
}

const BWMatrix4& Object::GetModelMatrix() const
{
	return ModelMatrix;
}


void Object::UpdateModelMatrix()
{
	ModelMatrix.makeTransform(Position, Scale, Orientation);
}
