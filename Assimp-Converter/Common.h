#pragma once

#include"DirectXTK\SimpleMath.h"
#include"assimp\scene.h"

DirectX::SimpleMath::Vector3 FromAssimp(const aiVector3D& v)
{
	return DirectX::SimpleMath::Vector3(v.x, v.y, v.z);
}

DirectX::SimpleMath::Color FromAssimp(const aiColor3D& col)
{
	return DirectX::SimpleMath::Color(col.r, col.g, col.b);
}

DirectX::SimpleMath::Color FromAssimp(const aiColor4D& col)
{
	return DirectX::SimpleMath::Color(col.r, col.g, col.b, col.a);
}

DirectX::SimpleMath::Matrix FromAssimp(const aiMatrix4x4& mtx)
{
	return DirectX::SimpleMath::Matrix(
		DirectX::SimpleMath::Vector4(mtx.a1, mtx.a2, mtx.a3, mtx.a4),
		DirectX::SimpleMath::Vector4(mtx.b1, mtx.b2, mtx.b3, mtx.b4),
		DirectX::SimpleMath::Vector4(mtx.c1, mtx.c2, mtx.c3, mtx.c4),
		DirectX::SimpleMath::Vector4(mtx.d1, mtx.d2, mtx.d3, mtx.d4));
}