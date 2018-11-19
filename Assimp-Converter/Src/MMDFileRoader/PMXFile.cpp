
#include<iostream>
#include<fstream>
#include<filesystem>

#include"PMXFile.h"

namespace MMDModel
{
	namespace PMX
	{
		template<typename _To, typename _From>
		constexpr _To s_cast(_From from) { return static_cast<_To>(from); }

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
				break;
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
				break;
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
				break;
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

			for (auto& vertex : vertices)
			{
				Read(&vertex.position, file);
				Read(&vertex.normal, file);
				Read(&vertex.uv, file);
				
				for (int i = 0; i < pmx_file->header.add_UV_num; ++i)
				{
					Read(&vertex.add_UV[i], file);
				}

				Read(&vertex.weight_type, file);

				switch (vertex.weight_type)
				{
				case PMXVertexWeight::BDEF1:
					ReadIndex(&vertex.bone_indices[0], pmx_file->header.bone_index_size, file);
					break;
				case PMXVertexWeight::BDEF2:
					ReadIndex(&vertex.bone_indices[0], pmx_file->header.bone_index_size, file);
					ReadIndex(&vertex.bone_indices[1], pmx_file->header.bone_index_size, file);
					Read(&vertex.bone_weights[0], file);
					break;
				case PMXVertexWeight::BDEF4:
				case PMXVertexWeight::QDEF: // same BDEF4
					for (int i = 0; i < 4; ++i)
					{
						ReadIndex(&vertex.bone_indices[i], pmx_file->header.bone_index_size, file);
						Read(&vertex.bone_weights[i], file);
					}
					break;
				case PMXVertexWeight::SDEF:
					ReadIndex(&vertex.bone_indices[0], pmx_file->header.bone_index_size, file);
					ReadIndex(&vertex.bone_indices[1], pmx_file->header.bone_index_size, file);
					Read(&vertex.bone_weights[0], file);
					Read(&vertex.sdef_C, file);
					Read(&vertex.sdef_R0, file);
					Read(&vertex.sdef_R1, file);
					break;
				}

				Read(&vertex.edge_mag, file);
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

			for (auto& face : faces)
			{
				ReadIndex(&face.indices[0], pmx_file->header.vertex_index_size, file);
				ReadIndex(&face.indices[1], pmx_file->header.vertex_index_size, file);
				ReadIndex(&face.indices[2], pmx_file->header.vertex_index_size, file);
			}

			return true;
		}

		bool ReadTexture(PMXFile* pmx_file, std::ifstream& file)
		{
			int texture_count;
			Read(&texture_count, file);

			auto& textures = pmx_file->textures;
			textures.resize(texture_count);

			for (auto& texture : textures)
			{
				ReadString(pmx_file, &texture.texture_name, file);
			}

			return true;
		}

		bool ReadMaterial(PMXFile* pmx_file, std::ifstream& file)
		{
			int material_count;
			Read(&material_count, file);

			auto& materials = pmx_file->materials;
			materials.resize(material_count);

			for (auto& material : materials)
			{
				ReadString(pmx_file, &material.name, file);
				ReadString(pmx_file, &material.english_name, file);

				Read(&material.diffuse, file);
				Read(&material.specular, file);
				Read(&material.specular_power, file);
				Read(&material.ambient, file);
				Read(&material.draw_mode, file);
				Read(&material.edge_color, file);
				Read(&material.edge_size, file);

				ReadIndex(&material.texture_index, pmx_file->header.texture_index_size, file);
				ReadIndex(&material.sphere_texture_index, pmx_file->header.texture_index_size, file);
				Read(&material.sphere_mode, file);
				Read(&material.toon_mode, file);

				switch(material.toon_mode)
				{
				case PMXToonMode::Separate:
					ReadIndex(&material.toon_texture_index, pmx_file->header.texture_index_size, file);
					break;
				case PMXToonMode::Common:
					uint8_t common_idx;
					Read(&common_idx, file);
					material.toon_texture_index = common_idx;
					break;
				}

				ReadString(pmx_file, &material.memo, file);
				Read(&material.num_face_vertices, file);
			}

			return true;
		}

		bool ReadBone(PMXFile* pmx_file, std::ifstream& file)
		{
			int bone_count;
			Read(&bone_count, file);

			auto& bones = pmx_file->bones;
			bones.resize(bone_count);

			for (auto& bone : bones)
			{
				ReadString(pmx_file, &bone.name, file);
				ReadString(pmx_file, &bone.english_name, file);

				Read(&bone.position, file);
				ReadIndex(&bone.parent_bone_index, pmx_file->header.bone_index_size, file);
				Read(&bone.deform_depth, file);
				
				Read(&bone.bone_flag, file);

				if ((static_cast<unsigned short>(bone.bone_flag) & static_cast<unsigned short>(PMXBoneFlags::TargetShowMode)) == 0)
				{
					Read(&bone.position_offset, file);
				}
				else
				{
					ReadIndex(&bone.link_bone_index, pmx_file->header.bone_index_size, file);
				}

				if ((static_cast<unsigned short>(bone.bone_flag) & static_cast<unsigned short>(PMXBoneFlags::AppendRotate)) == 1 ||
					(static_cast<unsigned short>(bone.bone_flag) & static_cast<unsigned short>(PMXBoneFlags::AppendTranslate)) == 1)
				{
					ReadIndex(&bone.append_bone_index, pmx_file->header.bone_index_size, file);
					Read(&bone.append_weight, file);
				}

				if ((static_cast<unsigned short>(bone.bone_flag) & static_cast<unsigned short>(PMXBoneFlags::FixedAxis)) == 1)
				{
					Read(&bone.fixed_axis, file);
				}

				if ((static_cast<unsigned short>(bone.bone_flag) & static_cast<unsigned short>(PMXBoneFlags::LocalAxis)) == 1)
				{
					Read(&bone.local_X_axis, file);
					Read(&bone.local_Z_axis, file);
				}

				if ((static_cast<unsigned short>(bone.bone_flag) & static_cast<unsigned short>(PMXBoneFlags::DeformOuterParent)) == 1)
				{
					Read(&bone.key_value, file);
				}

				if ((static_cast<unsigned short>(bone.bone_flag) & static_cast<unsigned short>(PMXBoneFlags::IK)) == 1)
				{
					ReadIndex(&bone.ik_target_bone_index, pmx_file->header.bone_index_size, file);
					Read(&bone.ik_iteration_count, file);
					Read(&bone.ik_limit, file);

					int ik_link_count;
					Read(&ik_link_count, file);

					auto& ik_links = bone.ik_links;
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

			for (auto& morph : morphs)
			{
				ReadString(pmx_file, &morph.name, file);
				ReadString(pmx_file, &morph.english_name, file);

				Read(&morph.control_panel, file);
				Read(&morph.morph_type, file);

				int offset_count;
				Read(&offset_count, file);

				switch (morph.morph_type)
				{
				case PMXMorphType::Position:
				{
					auto& pos_morphs = morph.position_morph;
					pos_morphs.resize(offset_count);

					for (auto& pm : pos_morphs)
					{
						ReadIndex(&pm.vertex_index, pmx_file->header.vertex_index_size, file);
						Read(&pm.position, file);
					}

					break;
				}
				case PMXMorphType::UV:
				{
					auto& uv_morphs = morph.uv_morph;
					uv_morphs.resize(offset_count);

					for (auto& um : uv_morphs)
					{
						ReadIndex(&um.vertex_index, pmx_file->header.vertex_index_size, file);
						Read(&um.uv, file);
					}

					break;
				}
				case PMXMorphType::Bone:
				{
					auto& bone_morphs = morph.bone_morph;
					bone_morphs.resize(offset_count);

					for (auto& bm : bone_morphs)
					{
						ReadIndex(&bm.bone_index, pmx_file->header.bone_index_size, file);
						Read(&bm.position, file);
						Read(&bm.quaternion, file);
					}

					break;
				}
				case PMXMorphType::Material:
				{
					auto& mat_morphs = morph.material_morph;
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
				case PMXMorphType::Group:
				{
					auto& group_morphs = morph.group_morph;
					group_morphs.resize(offset_count);

					for (auto& gm : group_morphs)
					{
						ReadIndex(&gm.morph_index, pmx_file->header.morph_index_size, file);
						Read(&gm.weight, file);
					}

					break;
				}
				case PMXMorphType::Flip:
				{
					auto& flip_morphs = morph.flip_morph;
					flip_morphs.resize(offset_count);

					for (auto& fm : flip_morphs)
					{
						ReadIndex(&fm.morph_index, pmx_file->header.morph_index_size, file);
						Read(&fm.weight, file);
					}

					break;
				}
				case PMXMorphType::Impulse:
				{
					auto& impulse_morphs = morph.impulse_morph;
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

			for (auto& frame : frames)
			{
				ReadString(pmx_file, &frame.name, file);
				ReadString(pmx_file, &frame.english_name, file);

				Read(&frame.flag, file);

				int element_count;
				Read(&element_count, file);
				frame.targets.resize(element_count);

				for (auto& target : frame.targets)
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

			for (auto& rb : rigidbodies)
			{
				ReadString(pmx_file, &rb.name, file);
				ReadString(pmx_file, &rb.english_name, file);

				ReadIndex(&rb.bone_index, pmx_file->header.bone_index_size, file);

				Read(&rb.group, file);
				Read(&rb.collision_group, file);

				Read(&rb.shape, file);
				Read(&rb.shape_size, file);

				Read(&rb.translate, file);
				Read(&rb.rotate, file);

				Read(&rb.mass, file);
				Read(&rb.translate_dimmer, file);
				Read(&rb.rotate_dimmer, file);
				Read(&rb.repulsion, file);
				Read(&rb.friction, file);
				Read(&rb.op, file);

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

			for (auto& sb : softbodies)
			{
				ReadString(pmx_file, &sb.name, file);
				ReadString(pmx_file, &sb.english_name, file);

				Read(&sb.type, file);
				ReadIndex(&sb.material_index, pmx_file->header.material_index_size, file);

				Read(&sb.group, file);
				Read(&sb.collision_group, file);

				Read(&sb.flag, file);
				Read(&sb.B_link_length, file);
				Read(&sb.num_clusters, file);
				Read(&sb.total_mass, file);
				Read(&sb.collision_margin, file);

				Read(&sb.aero_model, file);

				// config
				Read(&sb.VCF, file);
				Read(&sb.DP, file);
				Read(&sb.DG, file);
				Read(&sb.LF, file);
				Read(&sb.PR, file);
				Read(&sb.VC, file);
				Read(&sb.DF, file);
				Read(&sb.MT, file);
				Read(&sb.CHR, file);
				Read(&sb.KHR, file);
				Read(&sb.SHR, file);
				Read(&sb.AHR, file);

				// cluster
				Read(&sb.SRHR_CL, file);
				Read(&sb.SKHR_CL, file);
				Read(&sb.SSHR_CL, file);
				Read(&sb.SR_SPLT_CL, file);
				Read(&sb.SK_SPLT_CL, file);
				Read(&sb.SS_SPLT_CL, file);

				//interation
				Read(&sb.V_IT, file);
				Read(&sb.P_IT, file);
				Read(&sb.D_IT, file);
				Read(&sb.C_IT, file);

				//material
				Read(&sb.LST, file);
				Read(&sb.AST, file);
				Read(&sb.VST, file);

				int anchor_count;
				Read(&anchor_count, file);
				sb.anchor_rigidbodies.resize(anchor_count);

				for (auto& anchor : sb.anchor_rigidbodies)
				{
					ReadIndex(&anchor.rigid_body_index, pmx_file->header.rigidbody_index_size, file);
					ReadIndex(&anchor.vertex_index, pmx_file->header.vertex_index_size, file);
					Read(&anchor.near_mode, file);
				}
				
				int pin_count;
				Read(&pin_count, file);
				sb.pin_vertex_indices.resize(pin_count);

				for (auto& pin : sb.pin_vertex_indices)
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
			file.open(file_path, std::ios::in | std::ios::binary | std::ios::end);

			if (!file)
			{
				std::cout << "ファイル名 : " << file_path.c_str() << std::endl << "が開けません" << std::endl;
				return false;
			}

			// binary size calculate
			std::streampos binary_size = file.tellg();
			file.seekg(std::ios::beg);

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
			{// soft body included only read
				if (!ReadSoftbody(pmx_file, file)) return false;
			}

			file.close();

			return true;
		}
	}
}