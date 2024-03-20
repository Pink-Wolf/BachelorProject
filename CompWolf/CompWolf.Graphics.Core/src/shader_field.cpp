#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"
#include "shader_field_type.hpp"

namespace CompWolf::Graphics
{
	auto shader_field_info<float>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32_SFLOAT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<float2d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32G32_SFLOAT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<float3d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32G32B32_SFLOAT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<float4d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32G32B32A32_SFLOAT
		};
		return Private::from_private(&field_info);
	}

	auto shader_field_info<double>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R64_SFLOAT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<double2d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R64G64_SFLOAT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<double3d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R64G64B64_SFLOAT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<double4d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R64G64B64A64_SFLOAT
		};
		return Private::from_private(&field_info);
	}

	auto shader_field_info<int>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32_SINT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<int2d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32G32_SINT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<int3d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32G32B32_SINT
		};
		return Private::from_private(&field_info);
	}
	auto shader_field_info<int4d>::type_info() noexcept -> Private::shader_field_info_handle
	{
		static Private::shader_field_info field_info{
			.format = VK_FORMAT_R32G32B32A32_SINT
		};
		return Private::from_private(&field_info);
	}
}
