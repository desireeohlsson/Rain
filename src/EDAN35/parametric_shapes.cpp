#include "parametric_shapes.hpp"
#include "core/Log.h"

#include <glm/glm.hpp>

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

bonobo::mesh_data
parametric_shapes::createQuad(float const width, float const height,
                              unsigned int const horizontal_split_count,
                              unsigned int const vertical_split_count)
{
	
	auto const horizontal_slice_edges_count = horizontal_split_count + 1u;
	auto const vertical_slice_edges_count = vertical_split_count + 1u;
	auto const horizontal_slice_vertices_count = horizontal_slice_edges_count + 1u;
	auto const vertical_slice_vertices_count = vertical_slice_edges_count + 1u;
	auto const vertices_nb = horizontal_slice_vertices_count * vertical_slice_vertices_count;
	
	//auto const vertices = std::array<glm::vec3, 4>{
	//	glm::vec3(0.0f,  0.0f,   0.0f),
	//	glm::vec3(width, 0.0f,   0.0f),
	//	glm::vec3(width, height, 0.0f),
	//	glm::vec3(0.0f,  height, 0.0f)
	//};
	//size_t vertices_size = sizeof(glm::vec3) * vertices.size();
    //
	//auto const index_sets = std::array<glm::uvec3, 2>{
	//	glm::uvec3(0u, 1u, 2u),
	//	glm::uvec3(0u, 2u, 3u)
	//};
	//size_t index_size = sizeof(glm::uvec3) * index_sets.size();
	
	auto vertices  = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	
	size_t index = 0u;
	for (unsigned int i = 0u; i < horizontal_slice_vertices_count; ++i) {
		for (unsigned int j = 0u; j < vertical_slice_vertices_count; ++j) {
			vertices[index] = glm::vec3(height / horizontal_slice_edges_count * i,
										0,
										width / vertical_slice_edges_count * j);
			//printf("index (%d,%d): (%f, %f, %f)\n", i, j, vertices[index].x, vertices[index].y, vertices[index].z);
			// texture coordinates
			texcoords[index] = glm::vec3((float)i / horizontal_slice_edges_count,
										(float)j / vertical_slice_edges_count,
										0);
			++index;
		}
	}
	
	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * vertices_nb);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < horizontal_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < vertical_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(vertical_slice_vertices_count * (i + 0u) + (j + 0u),
			                               vertical_slice_vertices_count * (i + 0u) + (j + 1u),
			                               vertical_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;

			index_sets[index] = glm::uvec3(vertical_slice_vertices_count * (i + 0u) + (j + 1u),
			                               vertical_slice_vertices_count * (i + 1u) + (j + 1u),
			                               vertical_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);
	
	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const texcoords_offset = vertices_offset + vertices_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));

	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	                                            +texcoords_size
	                                            );
	
	
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = index_sets.size() * 3u;
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
	return data;
}

bonobo::mesh_data
parametric_shapes::createCube(float const side,
                              unsigned int const horizontal_split_count,
                              unsigned int const vertical_split_count)
{
	
	auto const horizontal_slice_edges_count = horizontal_split_count + 1u;
	auto const vertical_slice_edges_count = vertical_split_count + 1u;
	auto const horizontal_slice_vertices_count = horizontal_slice_edges_count + 1u;
	auto const vertical_slice_vertices_count = vertical_slice_edges_count + 1u;
	auto const face_vertices_nb = horizontal_slice_vertices_count * vertical_slice_vertices_count;
	auto const vertices_nb = face_vertices_nb * 6;
	
	
	auto vertices  = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto normals   = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);
	auto tangents  = std::vector<glm::vec3>(vertices_nb);
	
	size_t index = 0u;
	for (unsigned int i = 0u; i < horizontal_slice_vertices_count; ++i) {
		for (unsigned int j = 0u; j < vertical_slice_vertices_count; ++j) {
			float u = (float)i / horizontal_slice_edges_count;
			float v = (float)j / vertical_slice_edges_count;
			float uu = u - 0.5f;
			float vv = v - 0.5f;
			// Front
			vertices[index] = glm::vec3(vv, uu, 0) * side;
			texcoords[index] = glm::vec3(v, u, 0);
			tangents[index] =  glm::vec3(0, 1, 0);;
			binormals[index] = glm::vec3(-1, 0, 0);;
			normals[index] = glm::vec3(0, 0, 1);;
			
			// Right
			vertices[index + face_vertices_nb] = glm::vec3(0.5f, uu, vv) * side;
			texcoords[index + face_vertices_nb] = glm::vec3(u, v, 0);
			tangents[index + face_vertices_nb] =  glm::vec3(0, 1, 0);
			binormals[index + face_vertices_nb] = glm::vec3(0, 0, 1);
			normals[index + face_vertices_nb] = glm::vec3(1, 0, 0);
			
			// Back
			vertices[index + face_vertices_nb * 2] = glm::vec3(vv, uu, 0.5f) * side;
			texcoords[index + face_vertices_nb * 2] = glm::vec3(u, v, 0);
			tangents[index + face_vertices_nb * 2] =  glm::vec3(0, 1, 0);
			binormals[index + face_vertices_nb * 2] = glm::vec3(1, 0, 0);
			normals[index + face_vertices_nb * 2] = glm::vec3(0, 0, 1);
			
			// Left
			vertices[index + face_vertices_nb * 3] = glm::vec3(-0.5, uu, vv) * side;
			texcoords[index + face_vertices_nb * 3] = glm::vec3(u, v, 0);
			tangents[index + face_vertices_nb * 3] =  glm::vec3(0, 1, 0);
			binormals[index + face_vertices_nb * 3] = glm::vec3(0, 0, -1);
			normals[index + face_vertices_nb * 3] = glm::vec3(-1, 0, 0);
			
			// Up
			vertices[index + face_vertices_nb * 4] = glm::vec3(vv, 0.5f, uu) * side;
			texcoords[index + face_vertices_nb * 4] = glm::vec3(u, v, 0);
			tangents[index + face_vertices_nb * 4] =  glm::vec3(0, 0, 1);
			binormals[index + face_vertices_nb * 4] = glm::vec3(1, 0, 0);
			normals[index + face_vertices_nb * 4] = glm::vec3(0, 1, 0);
			
			// Down
			vertices[index + face_vertices_nb * 5] = glm::vec3(vv, -0.5f, uu) * side;
			texcoords[index + face_vertices_nb * 5] = glm::vec3(u, v, 0);
			tangents[index + face_vertices_nb * 5] =  glm::vec3(0, 0, 1);
			binormals[index + face_vertices_nb * 5] = glm::vec3(1, 0, 0);
			normals[index + face_vertices_nb * 5] = glm::vec3(0, -1, 0);
			
			++index;
		}
	}
	
	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * vertices_nb);

	// generate indices iteratively
	index = 0u;
	for (unsigned int k = 0u; k < 6; ++k) {
		for (unsigned int i = 0u; i < horizontal_slice_edges_count; ++i)
		{
			for (unsigned int j = 0u; j < vertical_slice_edges_count; ++j)
			{
				index_sets[index] = glm::uvec3(vertical_slice_vertices_count * (i + 0u) + (j + 0u),
				                               vertical_slice_vertices_count * (i + 0u) + (j + 1u),
				                               vertical_slice_vertices_count * (i + 1u) + (j + 0u));
				index_sets[index] += k * face_vertices_nb;
				++index;

				index_sets[index] = glm::uvec3(vertical_slice_vertices_count * (i + 0u) + (j + 1u),
				                               vertical_slice_vertices_count * (i + 1u) + (j + 1u),
				                               vertical_slice_vertices_count * (i + 1u) + (j + 0u));
				index_sets[index] += k * face_vertices_nb;
				++index;
			}
		}
	}
	

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);
	
	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	                                            +normals_size
	                                            +texcoords_size
	                                            +tangents_size
	                                            +binormals_size
	                                            );
	
	
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = index_sets.size() * 3u;
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
	return data;
}



bonobo::mesh_data
parametric_shapes::createSphere(float const radius,
                                unsigned int const longitude_split_count,
                                unsigned int const latitude_split_count)
{

	auto const latitude_slice_edges_count = latitude_split_count + 1u;
	auto const longitude_slice_edges_count = longitude_split_count + 1u;
	auto const latitude_slice_vertices_count = latitude_slice_edges_count + 1u;
	auto const longitude_slice_vertices_count = longitude_slice_edges_count + 1u;
	auto const vertices_nb = latitude_slice_vertices_count * longitude_slice_vertices_count;

	auto vertices  = std::vector<glm::vec3>(vertices_nb);
	auto normals   = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto tangents  = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);

	float const d_theta = glm::two_pi<float>() / (static_cast<float>(longitude_slice_edges_count));
	float const d_phi = glm::pi<float>() / (static_cast<float>(latitude_slice_edges_count));
	
	// generate vertices iteratively
	size_t index = 0u;
	
	float phi = 0.0f;
	for (unsigned int i = 0u; i < latitude_slice_vertices_count; ++i) {
		float const cos_phi = std::cos(phi);
		float const sin_phi = std::sin(phi);
		float theta = 0.0f;

		for (unsigned int j = 0u; j < longitude_slice_vertices_count; ++j) {
			float const cos_theta = std::cos(theta);
			float const sin_theta = std::sin(theta);
			// vertex
			vertices[index] = glm::vec3(radius * sin_theta * sin_phi,
			                            -radius * cos_phi,
			                            radius * cos_theta * sin_phi);

			// texture coordinates
			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(longitude_slice_vertices_count)),
			                             static_cast<float>(i) / (static_cast<float>(latitude_slice_vertices_count)),
			                             0.0f);

			// tangent
			auto const t = glm::vec3(cos_theta, 0.0f, -sin_theta);
			tangents[index] = t;

			// binormal
			auto const b = glm::vec3(sin_theta * cos_phi, sin_phi, cos_theta * cos_phi);
			binormals[index] = b;

			// normal
			auto const n = glm::cross(t, b);
			normals[index] = n;
			//printf("theta = %g, phi = %g: (%g, %g, %g)\n", theta, phi, n.x, n.y, n.z);

			++index; 
			theta += d_theta;
		}

		phi += d_phi;
	}
	
	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * vertices_nb);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < latitude_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < longitude_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(longitude_slice_vertices_count * (i + 0u) + (j + 0u),
			                               longitude_slice_vertices_count * (i + 0u) + (j + 1u),
			                               longitude_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;

			index_sets[index] = glm::uvec3(longitude_slice_vertices_count * (i + 0u) + (j + 1u),
			                               longitude_slice_vertices_count * (i + 1u) + (j + 1u),
			                               longitude_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}
	

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);
	
	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	                                            +normals_size
	                                            +texcoords_size
	                                            +tangents_size
	                                            +binormals_size
	                                            );
	
	
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = index_sets.size() * 3u;
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
	return data;
}

bonobo::mesh_data
parametric_shapes::createTorus(float const major_radius,
                               float const minor_radius,
                               unsigned int const major_split_count,
                               unsigned int const minor_split_count)
{
	//! \todo (Optional) Implement this function
	auto const major_slice_edges_count = major_split_count + 1u;
	auto const minor_slice_edges_count = minor_split_count + 1u;
	auto const major_slice_vertices_count = major_slice_edges_count + 1u;
	auto const minor_slice_vertices_count = minor_slice_edges_count + 1u;
	auto const vertices_nb = major_slice_vertices_count * minor_slice_vertices_count;

	auto vertices  = std::vector<glm::vec3>(vertices_nb);
	auto normals   = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto tangents  = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);
	
	float const d_theta = glm::two_pi<float>() / (static_cast<float>(minor_slice_edges_count));
	float const d_phi = glm::two_pi<float>() / (static_cast<float>(major_slice_edges_count));
	
	// generate vertices iteratively
	size_t index = 0u;
	
	float phi = 0.0f;
	
	for (unsigned int i = 0u; i < major_slice_vertices_count; ++i) {
		float const cos_phi = std::cos(phi);
		float const sin_phi = std::sin(phi);
		float theta = 0.0f;

		for (unsigned int j = 0u; j < minor_slice_vertices_count; ++j) {
			float const cos_theta = std::cos(theta);
			float const sin_theta = std::sin(theta);
			// vertex
			vertices[index] = glm::vec3((major_radius + minor_radius * cos_theta) * cos_phi,
			                            -minor_radius * sin_theta,
			                            (major_radius + minor_radius * cos_theta) * sin_phi);

			// texture coordinates
			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(minor_slice_vertices_count)),
			                             static_cast<float>(i) / (static_cast<float>(major_slice_vertices_count)),
			                             0.0f);

			// tangent
			auto const t = glm::vec3(-sin_theta * cos_phi, -cos_theta, -sin_theta * sin_phi);
			tangents[index] = t;

			// binormal
			auto const b = glm::vec3(-(major_radius + minor_radius * cos_theta) * sin_phi, 0.0f, (major_radius + minor_radius * cos_theta) * cos_phi);
			binormals[index] = glm::normalize(b);

			// normal
			auto const n = glm::cross(b, t);
			normals[index] = n;
			//printf("theta = %g, phi = %g: (%g, %g, %g)\n", theta, phi, n.x, n.y, n.z);

			++index; 
			theta += d_theta;
		}
		
		phi += d_phi;
	}
	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * vertices_nb);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < major_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < minor_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(minor_slice_vertices_count * (i + 0u) + (j + 0u),
			                               minor_slice_vertices_count * (i + 0u) + (j + 1u),
			                               minor_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;

			index_sets[index] = glm::uvec3(minor_slice_vertices_count * (i + 0u) + (j + 1u),
			                               minor_slice_vertices_count * (i + 1u) + (j + 1u),
			                               minor_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}
	

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);
	
	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	                                            +normals_size
	                                            +texcoords_size
	                                            +tangents_size
	                                            +binormals_size
	                                            );
	
	
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = index_sets.size() * 3u;
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	
	return data;
}

bonobo::mesh_data
parametric_shapes::createCircleRing(float const radius,
                                    float const spread_length,
                                    unsigned int const circle_split_count,
                                    unsigned int const spread_split_count)
{
	auto const circle_slice_edges_count = circle_split_count + 1u;
	auto const spread_slice_edges_count = spread_split_count + 1u;
	auto const circle_slice_vertices_count = circle_slice_edges_count + 1u;
	auto const spread_slice_vertices_count = spread_slice_edges_count + 1u;
	auto const vertices_nb = circle_slice_vertices_count * spread_slice_vertices_count;

	auto vertices  = std::vector<glm::vec3>(vertices_nb);
	auto normals   = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto tangents  = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);

	float const spread_start = radius - 0.5f * spread_length;
	float const d_theta = glm::two_pi<float>() / (static_cast<float>(circle_slice_edges_count));
	float const d_spread = spread_length / (static_cast<float>(spread_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float theta = 0.0f;
	for (unsigned int i = 0u; i < circle_slice_vertices_count; ++i) {
		float const cos_theta = std::cos(theta);
		float const sin_theta = std::sin(theta);

		float distance_to_centre = spread_start;
		for (unsigned int j = 0u; j < spread_slice_vertices_count; ++j) {
			// vertex
			vertices[index] = glm::vec3(distance_to_centre * cos_theta,
			                            distance_to_centre * sin_theta,
			                            0.0f);

			// texture coordinates
			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(spread_slice_vertices_count)),
			                             static_cast<float>(i) / (static_cast<float>(circle_slice_vertices_count)),
			                             0.0f);

			// tangent
			auto const t = glm::vec3(cos_theta, sin_theta, 0.0f);
			tangents[index] = t;

			// binormal
			auto const b = glm::vec3(-sin_theta, cos_theta, 0.0f);
			binormals[index] = b;

			// normal
			auto const n = glm::cross(t, b);
			normals[index] = n;

			distance_to_centre += d_spread;
			++index;
		}

		theta += d_theta;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * circle_slice_edges_count * spread_slice_edges_count);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < circle_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < spread_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
			                               spread_slice_vertices_count * (i + 0u) + (j + 1u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 1u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	                                            +normals_size
	                                            +texcoords_size
	                                            +tangents_size
	                                            +binormals_size
	                                            );
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = index_sets.size() * 3u;
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}
