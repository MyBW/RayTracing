#include "TestFBXLoad.h"
#include <iostream>
#include "../MeshLoad/FBXImporterImp.h"
#include "../MeshLoad/FBXAttributeTypeData.h"

void TestFBXLoad::ImportFBX(const std::string& FileName)
{
	auto PrintLog = [](std::string& Msg1, std::string& Msg2)
	{
		std::cout << Msg1 << "    " << Msg2 << std::endl;
	};
	FBXImporterImp FBXImp;
	FBXImp.InitFBX(PrintLog);
	if (!FBXImp.LoadFBX(FileName)) 	return;

	const std::vector<FBXAttributeBaseImport*>* MeshImports = (FBXImp.GetAttributeDataImport(FbxNodeAttribute::eMesh));
	for (int i = 0; i < MeshImports->size(); i++)
		//if (MeshImports && MeshImports->size() > 0)
	{
		const FBXMeshAttributeImport* MeshImport = dynamic_cast<const FBXMeshAttributeImport*>((*MeshImports)[i]);
		const std::vector<float>& Vertices = MeshImport->GetVertices();
		const std::vector<float>& UVs = MeshImport->GetUVs();
		const std::vector<float>& Normals = MeshImport->GetNormals();
		const std::vector<float>& Tangents = MeshImport->GetTangents();
		const std::vector<unsigned int>& Indices = MeshImport->GetIndices();

		const std::vector<FBXMeshAttributeImport::SubMesh*>& SubMeshs = MeshImport->GetSubMeshs();
		//Mesh->GetVertexData() = new VertexData();

		//������ͬ���������ݱ���Ϊһ��submesh  һ��submesh�������ݰ����� vertex UV normal indices ÿ��һ�ֶ���vertexelement
		// ��������Ҫ��submesh���洢��������
		for (int i = 0; i < SubMeshs.size(); i++)
		{
			//BWSubMesh* SubMesh = Mesh->createSubMesh();
			//IndexDataPrt SubMeshIndexData = SubMesh->getIndexData();
			//VertexDataPrt SubMeshVertexData = new VertexData();
			std::vector<float> BufferData;
			//SubMesh->SetIsUseSharedVertex(false);
			//SubMesh->SetVertexData(SubMeshVertexData);
			int Offset = SubMeshs[i]->IndexOffset;
			int TriangleNum = SubMeshs[i]->TriangleCount;
			int VertexNum = SubMeshs[i]->TriangleCount * TRIANGLE_VERTEX_COUNT;
			int VertexSize = MeshImport->GetVertexElementSize();
			int VertexByteSize = MeshImport->GetVertexElementSizeByByte();
			BufferData.resize(VertexSize * VertexNum);

			int PositionStartIndex = Offset *  VERTEX_STRIDE;
			int PositionDataNum = VertexNum * VERTEX_STRIDE;
			//SubMeshVertexData->mVertexDeclaration->addElement(i, 0, VET_FLOAT4, VES_POSITION);

			int TangentStartIndex = Offset * TANGENT_STRIDE;
			int TangentDataNum = VertexNum * TANGENT_STRIDE;
			//SubMeshVertexData->mVertexDeclaration->addElement(i, (VERTEX_STRIDE) * sizeof(float), VET_FLOAT4, VES_TANGENT);

			int UVStartIndex = Offset *  UV_STRIDE;
			int UVDataNum = VertexNum * UV_STRIDE;
			//SubMeshVertexData->mVertexDeclaration->addElement(i, (TANGENT_STRIDE + VERTEX_STRIDE) * sizeof(float), VET_FLOAT2, VES_TEXTURE_COORDINATES);
			int NormalStartIndex = Offset *  NORMAL_STRIDE;
			int NormalNum = VertexNum * NORMAL_STRIDE;
			//SubMeshVertexData->mVertexDeclaration->addElement(i, (TANGENT_STRIDE + VERTEX_STRIDE + UV_STRIDE) * sizeof(float), VET_FLOAT3, VES_NORMAL);

			for (int VertexNumIndex = 0; VertexNumIndex < VertexNum; VertexNumIndex++)
			{
				for (int PositionIndex = 0; PositionIndex < VERTEX_STRIDE; PositionIndex++)
				{
					BufferData[VertexNumIndex * VertexSize + PositionIndex]
						= Vertices[PositionStartIndex + VertexNumIndex * VERTEX_STRIDE + PositionIndex];
				}

				for (int TangentIndex = 0; TangentIndex < TANGENT_STRIDE; TangentIndex++)
				{
					BufferData[VertexNumIndex * VertexSize + VERTEX_STRIDE + TangentIndex]
						= Tangents[TangentStartIndex + VertexNumIndex * TANGENT_STRIDE + TangentIndex];
				}


				for (int UVIndex = 0; UVIndex < UV_STRIDE; UVIndex++)
				{
					BufferData[VertexNumIndex * VertexSize + VERTEX_STRIDE + TANGENT_STRIDE + UVIndex]
						= UVs[UVStartIndex + VertexNumIndex * UV_STRIDE + UVIndex];
				}
				for (int NormalIndex = 0; NormalIndex < NORMAL_STRIDE; NormalIndex++)
				{
					BufferData[VertexNumIndex * VertexSize + VERTEX_STRIDE + UV_STRIDE + TANGENT_STRIDE + NormalIndex]
						= Normals[NormalStartIndex + VertexNumIndex * NORMAL_STRIDE + NormalIndex];
				}
			}
			drawInfo.BufferData = BufferData;
			drawInfo.VertexByteSize = VertexByteSize;
			for (int j = 0 ; j < VertexNum ; j++)
			{
				drawInfo.IndexData.push_back(Indices[SubMeshs[i]->IndexOffset + j]);
			}
			//BWHardwareVertexBufferPtr vbuf;
			//vbuf = BWHardwareBufferManager::GetInstance()->createVertexBuffer(FileName,
			//	VertexByteSize, TriangleNum * TRIANGLE_VERTEX_COUNT,
			//	BWHardwareBuffer::Usage::HBU_DYNAMIC, false
			//);
			//void * buf = vbuf->lock(BWHardwareBuffer::HBL_DISCARD);
			//memcpy(buf, &BufferData[0], BufferData.size() * sizeof(float));
			//vbuf->unlock();
			//SubMeshVertexData->mVertexBufferBind->setBinding(i, vbuf);

			//BWHardwareIndexBufferPtr ibuf;
			//SubMesh->getIndexData()->mIndexStart = 0;
			//SubMesh->getIndexData()->mIndexCount = VertexNum;
			//ibuf = BWHardwareBufferManager::GetInstance()->createIndexBuffer(
			//	SubMesh->getParent()->getName(),
			//	BWHardwareIndexBuffer::IT_32BIT,
			//	SubMesh->getIndexData()->mIndexCount,
			//	BWHardwareBuffer::Usage::HBU_DYNAMIC,
			//	false);
			//unsigned int *pIdx = static_cast<unsigned int*>(ibuf->lock(BWHardwareBuffer::HBL_DISCARD));
			//memcpy(pIdx, (&Indices[SubMeshs[i]->IndexOffset]), SubMesh->getIndexData()->mIndexCount * sizeof(unsigned int));
			//ibuf->unlock();
			//SubMesh->getIndexData()->mIndexBuffer = ibuf;
		}
	}

	FBXImp.ReleseData();
}

