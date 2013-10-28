#pragma once

#include "Primitive.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveRadius : public PrimitiveTemplate< Helium::SimpleVertex >
		{
		public:
			typedef PrimitiveTemplate< Helium::SimpleVertex > Base;

			float m_Radius;
			int m_RadiusSteps;

		public:
			PrimitiveRadius();

			virtual void Update() HELIUM_OVERRIDE;
			virtual void Draw(
				BufferedDrawer* drawInterface,
				Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY,
				const bool* solid = NULL,
				const bool* transparent = NULL ) const HELIUM_OVERRIDE = 0;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE = 0;
		};
	}
}