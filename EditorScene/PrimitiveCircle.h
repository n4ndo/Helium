#pragma once

#include "PrimitiveRadius.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveCircle : public PrimitiveRadius
		{
		public:
			typedef PrimitiveRadius Base;

			bool m_HackyRotateFlag;

			PrimitiveCircle();

			virtual void Update() HELIUM_OVERRIDE;
			virtual void Draw(
				BufferedDrawer* drawInterface,
				Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY,
				const bool* solid = NULL,
				const bool* transparent = NULL ) const HELIUM_OVERRIDE;
			virtual void DrawFill(
				BufferedDrawer* drawInterface,
				Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY ) const;
			virtual void DrawHiddenBack( const Editor::Camera* camera, const Matrix4& m ) const;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
		};
	}
}