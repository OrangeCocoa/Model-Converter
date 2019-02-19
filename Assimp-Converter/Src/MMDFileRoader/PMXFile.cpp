
#include<iostream>
#include<fstream>
#include<filesystem>

#include"PMXFile.h"

template<typename _To, typename _From>
inline constexpr _To s_cast(_From from) { return static_cast<_To>(from); }

//const std::string MODEL_DIR = "../../../Resource/Model/";

template <typename Type>
void Read(Type* value, std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(value), sizeof(*value));
}

template <typename Type>
void Read(Type* buffer, unsigned int size, std::ifstream& file)
{// string read
	file.read(reinterpret_cast<char*>(buffer), size);
}

bool ReadString(PMXFile* pmx_file, std::string* value, std::ifstream& file)
{
	int buffer_size;
	Read(&buffer_size, file);

	if (buffer_size > 0)
	{
		if (pmx_file->header.encode == 0)
		{// UTF-16
			std::u16string str(buffer_size / 2, u'\0');
			Read(&str[0], buffer_size, file);
			// u16->u8
			//ToDo : filesystem non use
			*value = std::filesystem::path(str).string();
		}
		else if (pmx_file->header.encode == 1)
		{// UTF-8
			value->resize(buffer_size);
			Read(value, buffer_size, file);
		}

		std::cout << *value << std::endl;
		return true;
	}
	else return false;
}

void ReadIndex(int* out_idx, unsigned int size, std::ifstream& file)
{// pmx index is 1 or 2 or 4
	switch (size)
	{
	case 1:
	{
		char idx;
		Read(&idx, file);
		if (idx < 0xFF)
		{
			*out_idx = static_cast<int>(idx);
		}
		else
		{
			*out_idx = -1;
		}
	}
	return;
	case 2:
	{
		short idx;
		Read(&idx, file);
		if (idx < 0xFFFF)
		{
			*out_idx = static_cast<int>(idx);
		}
		else
		{
			*out_idx = -1;
		}
	}
	return;
	case 4:
	{
		int idx;
		Read(&idx, file);
		if (idx < 0xFF)
		{
			*out_idx = static_cast<int>(idx);
		}
		else
		{
			*out_idx = -1;
		}
	}
	return;
	}
}

bool ReadHeader(PMXFile* pmx_file, std::ifstream& file)
{
	auto& header = pmx_file->header;

	Read(&header.magic, file);
	Read(&header.version, file);
	Read(&header.data_size, file);

	Read(&header.encode, file);
	Read(&header.add_UV_num, file);
	Read(&header.vertex_index_size, file);
	Read(&header.texture_index_size, file);
	Read(&header.material_index_size, file);
	Read(&header.bone_index_size, file);
	Read(&header.morph_index_size, file);
	Read(&header.rigidbody_index_size, file);

	return true;
}

bool ReadInfo(PMXFile* pmx_file, std::ifstream& file)
{
	auto& info = pmx_file->info;
	ReadString(pmx_file, &info.model_name, file);
	ReadString(pmx_file, &info.english_model_name, file);
	ReadString(pmx_file, &info.comment, file);
	ReadString(pmx_file, &info.english_comment, file);

	return true;
}

bool ReadVertex(PMXFile* pmx_file, std::ifstream& file)
{
	int vertex_count;
	Read(&vertex_count, file);

	auto& vertices = pmx_file->vertices;
	vertices.resize(vertex_count);

	for (int i = 0; i < vertex_count; ++i)
	{
		Read(&vertices[i].position, file);
		Read(&vertices[i].normal, file);
		Read(&vertices[i].uv, file);

		for (int i = 0; i < pmx_file->header.add_UV_num; ++i)
		{
			Read(&vertices[i].add_UV[i], file);
		}

		Read(&vertices[i].weight_type, file);

		switch (vertices[i].weight_type)
		{
		case PMXVertex::VertexWeight::BDEF1:
			ReadIndex(&vertices[i].bone_indices[0], pmx_file->header.bone_index_size, file);
			break;
		case PMXVertex::VertexWeight::BDEF2:
			ReadIndex(&vertices[i].bone_indices[0], pmx_file->header.bone_index_size, file);
			ReadIndex(&vertices[i].bone_indices[1], pmx_file->header.bone_index_size, file);
			Read(&vertices[i].bone_weights[0], file);
			break;
		case PMXVertex::VertexWeight::BDEF4:
		case PMXVertex::VertexWeight::QDEF: // same BDEF4
			for (int i = 0; i < 4; ++i)
			{
				ReadIndex(&vertices[i].bone_indices[i], pmx_file->header.bone_index_size, file);
				Read(&vertices[i].bone_weights[i], file);
			}
			break;
		case PMXVertex::VertexWeight::SDEF:
			ReadIndex(&vertices[i].bone_indices[0], pmx_file->header.bone_index_size, file);
			ReadIndex(&vertices[i].bone_indices[1], pmx_file->header.bone_index_size, file);
			Read(&vertices[i].bone_weights[0], file);
			Read(&vertices[i].sdef_C, file);
			Read(&vertices[i].sdef_R0, file);
			Read(&vertices[i].sdef_R1, file);
			break;
		}

		Read(&vertices[i].edge_mag, file);
	}

	return true;
}

bool ReadFace(PMXFile* pmx_file, std::ifstream& file)
{
	int face_count;
	Read(&face_count, file);
	face_count = face_count / 3;

	auto& faces = pmx_file->faces;
	faces.resize(face_count);

	for (int i = 0; i < face_count; ++i)
	{
		ReadIndex(&faces[i].indices[0], pmx_file->header.vertex_index_size, file);
		ReadIndex(&faces[i].indices[1], pmx_file->header.vertex_index_size, file);
		ReadIndex(&faces[i].indices[2], pmx_file->header.vertex_index_size, file);
	}

	return true;
}

bool ReadTexture(PMXFile* pmx_file, std::ifstream& file)
{
	int texture_count;
	Read(&texture_count, file);

	auto& textures = pmx_file->textures;
	textures.resize(texture_count);

	for (int i = 0; i < texture_count; ++i)
	{
		ReadString(pmx_file, &textures[i].texture_name, file);
	}

	return true;
}

bool ReadMaterial(PMXFile* pmx_file, std::ifstream& file)
{
	int material_count;
	Read(&material_count, file);

	auto& materials = pmx_file->materials;
	materials.resize(material_count);

	for (int i = 0; i < material_count; ++i)
	{
		ReadString(pmx_file, &materials[i].name, file);
		ReadString(pmx_file, &materials[i].english_name, file);

		Read(&materials[i].diffuse, file);
		Read(&materials[i].specular, file);
		Read(&materials[i].specular_power, file);
		Read(&materials[i].ambient, file);
		Read(&materials[i].draw_mode, file);
		Read(&materials[i].edge_color, file);
		Read(&materials[i].edge_size, file);

		ReadIndex(&materials[i].texture_index, pmx_file->header.texture_index_size, file);
		ReadIndex(&materials[i].sphere_texture_index, pmx_file->header.texture_index_size, file);
		Read(&materials[i].sphere_mode, file);
		Read(&materials[i].toon_mode, file);

		switch (materials[i].toon_mode)
		{
		case PMXMaterial::ToonMode::Separate:
			ReadIndex(&materials[i].toon_texture_index, pmx_file->header.texture_index_size, file);
			break;
		case PMXMaterial::ToonMode::Common:
			uint8_t common_idx;
			Read(&common_idx, file);
			materials[i].toon_texture_index = common_idx;
			break;
		}

		ReadString(pmx_file, &materials[i].memo, file);
		Read(&materials[i].num_face_vertices, file);
	}

	return true;
}

bool ReadBone(PMXFile* pmx_file, std::ifstream& file)
{
	int bone_count;
	Read(&bone_count, file);

	auto& bones = pmx_file->bones;
	bones.resize(bone_count);

	for (int i = 0; i < bone_count; ++i)
	{
		ReadString(pmx_file, &bones[i].name, file);
		ReadString(pmx_file, &bones[i].english_name, file);

		Read(&bones[i].position, file);
		ReadIndex(&bones[i].parent_bone_index, pmx_file->header.bone_index_size, file);
		Read(&bones[i].deform_depth, file);

		Read(&bones[i].bone_flag, file);

		if ((static_cast<unsigned short>(bones[i].bone_flag) &
			static_cast<unsigned short>(PMXBone::BoneFlags::TargetShowMode)) == 0)
		{
			Read(&bones[i].position_offset, file);
		}
		else
		{
			ReadIndex(&bones[i].link_bone_index, pmx_file->header.bone_index_size, file);
		}

		if ((static_cast<unsigned short>(bones[i].bone_flag) &
			static_cast<unsigned short>(PMXBone::BoneFlags::AppendRotate)) != 0 ||
			(static_cast<unsigned short>(bones[i].bone_flag) &
				static_cast<unsigned short>(PMXBone::BoneFlags::AppendTranslate)) != 0)
		{
			ReadIndex(&bones[i].append_bone_index, pmx_file->header.bone_index_size, file);
			Read(&bones[i].append_weight, file);
		}

		if ((static_cast<unsigned short>(bones[i].bone_flag) &
			static_cast<unsigned short>(PMXBone::BoneFlags::FixedAxis)) != 0)
		{
			Read(&bones[i].fixed_axis, file);
		}

		if ((static_cast<unsigned short>(bones[i].bone_flag) &
			static_cast<unsigned short>(PMXBone::BoneFlags::LocalAxis)) != 0)
		{
			Read(&bones[i].local_X_axis, file);
			Read(&bones[i].local_Z_axis, file);
		}

		if ((static_cast<unsigned short>(bones[i].bone_flag) &
			static_cast<unsigned short>(PMXBone::BoneFlags::DeformOuterParent)) != 0)
		{
			Read(&bones[i].key_value, file);
		}

		if ((static_cast<unsigned short>(bones[i].bone_flag) &
			static_cast<unsigned short>(PMXBone::BoneFlags::IK)) != 0)
		{
			ReadIndex(&bones[i].ik_target_bone_index, pmx_file->header.bone_index_size, file);
			Read(&bones[i].ik_iteration_count, file);
			Read(&bones[i].ik_limit, file);

			int ik_link_count;
			Read(&ik_link_count, file);

			auto& ik_links = bones[i].ik_links;
			ik_links.resize(ik_link_count);

			for (auto& ik_link : ik_links)
			{
				ReadIndex(&ik_link.ik_bone_index, pmx_file->header.bone_index_size, file);
				Read(&ik_link.enable_limit, file);

				if (ik_link.enable_limit == 1)
				{
					Read(&ik_link.limit_min, file);
					Read(&ik_link.limit_max, file);
				}
			}
		}
	}

	return true;
}

bool ReadMorph(PMXFile* pmx_file, std::ifstream& file)
{
	int morph_count;
	Read(&morph_count, file);

	auto& morphs = pmx_file->morphs;
	morphs.resize(morph_count);

	for (int i = 0; i < morph_count; ++i)
	{
		ReadString(pmx_file, &morphs[i].name, file);
		ReadString(pmx_file, &morphs[i].english_name, file);

		Read(&morphs[i].control_panel, file);
		Read(&morphs[i].morph_type, file);

		int offset_count;
		Read(&offset_count, file);

		switch (morphs[i].morph_type)
		{
		case PMXMorph::MorphType::Position:
		{
			auto& pos_morphs = morphs[i].position_morph;
			pos_morphs.resize(offset_count);

			for (auto& pm : pos_morphs)
			{
				ReadIndex(&pm.vertex_index, pmx_file->header.vertex_index_size, file);
				Read(&pm.position, file);
			}

			break;
		}
		case PMXMorph::MorphType::UV:
		{
			auto& uv_morphs = morphs[i].uv_morph;
			uv_morphs.resize(offset_count);

			for (auto& um : uv_morphs)
			{
				ReadIndex(&um.vertex_index, pmx_file->header.vertex_index_size, file);
				Read(&um.uv, file);
			}

			break;
		}
		case PMXMorph::MorphType::Bone:
		{
			auto& bone_morphs = morphs[i].bone_morph;
			bone_morphs.resize(offset_count);

			for (auto& bm : bone_morphs)
			{
				ReadIndex(&bm.bone_index, pmx_file->header.bone_index_size, file);
				Read(&bm.position, file);
				Read(&bm.quaternion, file);
			}

			break;
		}
		case PMXMorph::MorphType::Material:
		{
			auto& mat_morphs = morphs[i].material_morph;
			mat_morphs.resize(offset_count);

			for (auto& mm : mat_morphs)
			{
				ReadIndex(&mm.material_index, pmx_file->header.material_index_size, file);
				Read(&mm.op_type, file);
				Read(&mm.diffuse, file);
				Read(&mm.specular, file);
				Read(&mm.specular_power, file);
				Read(&mm.ambient, file);
				Read(&mm.edge_color, file);
				Read(&mm.edge_size, file);
				Read(&mm.texture_factor, file);
				Read(&mm.sphere_texture_factor, file);
				Read(&mm.toon_texture_factor, file);
			}

			break;
		}
		case PMXMorph::MorphType::Group:
		{
			auto& group_morphs = morphs[i].group_morph;
			group_morphs.resize(offset_count);

			for (auto& gm : group_morphs)
			{
				ReadIndex(&gm.morph_index, pmx_file->header.morph_index_size, file);
				Read(&gm.weight, file);
			}

			break;
		}
		case PMXMorph::MorphType::Flip:
		{
			auto& flip_morphs = morphs[i].flip_morph;
			flip_morphs.resize(offset_count);

			for (auto& fm : flip_morphs)
			{
				ReadIndex(&fm.morph_index, pmx_file->header.morph_index_size, file);
				Read(&fm.weight, file);
			}

			break;
		}
		case PMXMorph::MorphType::Impulse:
		{
			auto& impulse_morphs = morphs[i].impulse_morph;
			impulse_morphs.resize(offset_count);

			for (auto& im : impulse_morphs)
			{
				ReadIndex(&im.rigidbody_index, pmx_file->header.rigidbody_index_size, file);
				Read(&im.local_flag, file);
				Read(&im.translate_velocity, file);
				Read(&im.rotate_torque, file);
			}

			break;
		}
		}
	}

	return true;
}

bool ReadDisplayFrame(PMXFile* pmx_file, std::ifstream& file)
{
	int frame_count;
	Read(&frame_count, file);

	auto& frames = pmx_file->display_frames;
	frames.resize(frame_count);

	for (int i = 0; i < frame_count; ++i)
	{
		ReadString(pmx_file, &frames[i].name, file);
		ReadString(pmx_file, &frames[i].english_name, file);

		Read(&frames[i].flag, file);

		int element_count;
		Read(&element_count, file);
		frames[i].targets.resize(element_count);

		for (auto& target : frames[i].targets)
		{
			Read(&target.type, file);

			if (target.type == PMXDispalyFrame::TargetType::BoneIndex)
			{
				ReadIndex(&target.index, pmx_file->header.bone_index_size, file);
			}
			else if (target.type == PMXDispalyFrame::TargetType::MorphIndex)
			{
				ReadIndex(&target.index, pmx_file->header.morph_index_size, file);
			}
		}
	}
	return true;
}

bool ReadRigidbody(PMXFile* pmx_file, std::ifstream& file)
{
	int rb_count;
	Read(&rb_count, file);

	auto& rigidbodies = pmx_file->rigidbodies;
	rigidbodies.resize(rb_count);

	for (int i = 0; i < rb_count; ++i)
	{
		ReadString(pmx_file, &rigidbodies[i].name, file);
		ReadString(pmx_file, &rigidbodies[i].english_name, file);

		ReadIndex(&rigidbodies[i].bone_index, pmx_file->header.bone_index_size, file);

		Read(&rigidbodies[i].group, file);
		Read(&rigidbodies[i].collision_group, file);

		Read(&rigidbodies[i].shape, file);
		Read(&rigidbodies[i].shape_size, file);

		Read(&rigidbodies[i].translate, file);
		Read(&rigidbodies[i].rotate, file);

		Read(&rigidbodies[i].mass, file);
		Read(&rigidbodies[i].translate_dimmer, file);
		Read(&rigidbodies[i].rotate_dimmer, file);
		Read(&rigidbodies[i].repulsion, file);
		Read(&rigidbodies[i].friction, file);
		Read(&rigidbodies[i].op, file);
	}

	return true;
}

bool ReadJoint(PMXFile* pmx_file, std::ifstream& file)
{
	int joint_count;
	Read(&joint_count, file);

	auto& joints = pmx_file->joints;
	joints.resize(joint_count);

	for (auto& joint : joints)
	{
		ReadString(pmx_file, &joint.name, file);
		ReadString(pmx_file, &joint.english_name, file);

		Read(&joint.type, file);
		ReadIndex(&joint.rigidbody_A_index, pmx_file->header.rigidbody_index_size, file);
		ReadIndex(&joint.rigidbody_B_index, pmx_file->header.rigidbody_index_size, file);

		Read(&joint.translate, file);
		Read(&joint.rotate, file);
		Read(&joint.translate_lower_limit, file);
		Read(&joint.translate_upper_limit, file);
		Read(&joint.rotate_lower_limit, file);
		Read(&joint.rotate_upper_limit, file);

		Read(&joint.spring_translate_factor, file);
		Read(&joint.spring_rotate_factor, file);
	}

	return true;
}

bool ReadSoftbody(PMXFile* pmx_file, std::ifstream& file)
{
	int sb_count;
	Read(&sb_count, file);

	auto& softbodies = pmx_file->softbodies;
	softbodies.resize(sb_count);

	for (int i = 0; i < sb_count; ++i)
	{
		ReadString(pmx_file, &softbodies[i].name, file);
		ReadString(pmx_file, &softbodies[i].english_name, file);

		Read(&softbodies[i].type, file);
		ReadIndex(&softbodies[i].material_index, pmx_file->header.material_index_size, file);

		Read(&softbodies[i].group, file);
		Read(&softbodies[i].collision_group, file);

		Read(&softbodies[i].flag, file);
		Read(&softbodies[i].B_link_length, file);
		Read(&softbodies[i].num_clusters, file);
		Read(&softbodies[i].total_mass, file);
		Read(&softbodies[i].collision_margin, file);

		Read(&softbodies[i].aero_model, file);

		// config
		Read(&softbodies[i].VCF, file);
		Read(&softbodies[i].DP, file);
		Read(&softbodies[i].DG, file);
		Read(&softbodies[i].LF, file);
		Read(&softbodies[i].PR, file);
		Read(&softbodies[i].VC, file);
		Read(&softbodies[i].DF, file);
		Read(&softbodies[i].MT, file);
		Read(&softbodies[i].CHR, file);
		Read(&softbodies[i].KHR, file);
		Read(&softbodies[i].SHR, file);
		Read(&softbodies[i].AHR, file);

		// cluster
		Read(&softbodies[i].SRHR_CL, file);
		Read(&softbodies[i].SKHR_CL, file);
		Read(&softbodies[i].SSHR_CL, file);
		Read(&softbodies[i].SR_SPLT_CL, file);
		Read(&softbodies[i].SK_SPLT_CL, file);
		Read(&softbodies[i].SS_SPLT_CL, file);

		//interation
		Read(&softbodies[i].V_IT, file);
		Read(&softbodies[i].P_IT, file);
		Read(&softbodies[i].D_IT, file);
		Read(&softbodies[i].C_IT, file);

		//material
		Read(&softbodies[i].LST, file);
		Read(&softbodies[i].AST, file);
		Read(&softbodies[i].VST, file);

		int anchor_count;
		Read(&anchor_count, file);
		softbodies[i].anchor_rigidbodies.resize(anchor_count);

		for (auto& anchor : softbodies[i].anchor_rigidbodies)
		{
			ReadIndex(&anchor.rigid_body_index, pmx_file->header.rigidbody_index_size, file);
			ReadIndex(&anchor.vertex_index, pmx_file->header.vertex_index_size, file);
			Read(&anchor.near_mode, file);
		}

		int pin_count;
		Read(&pin_count, file);
		softbodies[i].pin_vertex_indices.resize(pin_count);

		for (auto& pin : softbodies[i].pin_vertex_indices)
		{
			ReadIndex(&pin, pmx_file->header.vertex_index_size, file);
		}
	}

	return true;
}

bool ReadPMXFile(PMXFile* pmx_file, const std::string& file_name)
{
	std::string file_path = /*MODEL_DIR +*/ file_name;

	std::ifstream file;
	file.open(file_path, std::ios::in | std::ios::binary);

	if (!file)
	{
		std::cout << "ファイル名 : " << file_path.c_str() << std::endl << "が開けません" << std::endl;
		return false;
	}

	// binary size calculate
	file.seekg(0, std::ios::end);
	std::streampos binary_size = file.tellg();
	file.clear();
	file.seekg(0, std::ios::beg);

	if (!ReadHeader(pmx_file, file)) return false;
	if (!ReadInfo(pmx_file, file)) return false;
	if (!ReadVertex(pmx_file, file)) return false;
	if (!ReadFace(pmx_file, file)) return false;
	if (!ReadTexture(pmx_file, file)) return false;
	if (!ReadMaterial(pmx_file, file)) return false;
	if (!ReadBone(pmx_file, file)) return false;
	if (!ReadMorph(pmx_file, file)) return false;
	if (!ReadDisplayFrame(pmx_file, file)) return false;
	if (!ReadRigidbody(pmx_file, file)) return false;
	if (!ReadJoint(pmx_file, file)) return false;

	if (file.tellg() != binary_size)
	{// soft body included only ver 2.1 read
		if (!ReadSoftbody(pmx_file, file)) return false;
	}

	file.close();

	return true;
}